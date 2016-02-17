##
#
#  @author Adrien Thébault <me@adrien-thebault.fr>
#  @brief Actual artifical intelligence
#

##
#  Dependencies

require 'singleton'

## --

module Chess

  class Player

    include Singleton

    ##
    #  Constants

    PLAYER_WHITE = 0
    PLAYER_BLACK = 1

    MINMAX = 0
      MIN = 0
      MAX = 1

    ALPHABETA = 1
      INF = 999999

    MAX_DEPTH = 2

    ##
    #
    #  Tries to guess the best move
    #
    #  @return Array [from, to]
    #  @scope public
    #

    def play(game, color, algo = MINMAX)

      ##
      # => Round data

      @game = game
      @color = color
      @boards = {}

      @actual_hash = chessboard_hash(@game.chessboard)
      @boards[@actual_hash] = {
        'chessboard_hash' => @actual_hash,
        'game_hash' => @game.hash,
        'children' => [],
        'valuation' => game.valuation(@color),
        'valuation_min' => nil,
        'valuation_min_depth' => -1,
        'valuation_max' => nil,
        'valuation_max_depth' => -1
      }

      ##
      # => Let's play!

      iterative_deepening algo

    end

    ##
    #
    #  Iterative deepening
    #
    #  @return Array [from, to]
    #  @scope private
    #

    def iterative_deepening(algo = MINMAX)

      print "\n" << '[INFO]' << "\t" << 'Starting iterative deepening'
      @nb_children = []

      depth = 0
      compute_time = 0

      until depth > MAX_DEPTH || (@game.round_end_time - Time.now).to_f < (compute_time.to_f*10)

        print "\n" << '[INFO]' << "\t|\t" << 'time left : ' + (@game.round_end_time - Time.now).to_s
        beginning = Time.now

        if algo == MINMAX
          minmax(depth)
        elsif algo == ALPHABETA
          alphabeta(depth)
        end

        print "\n" << '[INFO]' << "\t|\t" << 'Best move has a valuation of ' + @boards[@actual_hash]['valuation'].to_s

        move = nil
        @boards[@actual_hash]['children'].each do |mv,child|
          if (algo == MINMAX && @boards[child]['valuation'] == @boards[@actual_hash]['valuation']) || (algo == ALPHABETA && @boards[child]['valuation_min'] == @boards[@actual_hash]['valuation'])
            move = mv
            break
          end
        end

        print "\n" << '[INFO]' << "\t|\t" << 'move = ' << move.to_s + "\n"

        compute_time += (Time.now - beginning).to_f
        depth += 1

      end

      move

    end

    private :iterative_deepening

    ##
    #
    #  MinMax
    #
    #  @return Integer
    #  @scope private
    #

    def minmax(limit)

      print "\n" << '[INFO]' << "\t|\t" << 'minmax with limit = ' << limit.to_s

      depth = 0
      parents = [@boards[@actual_hash]]

      until depth > limit

        print "\n" << '[INFO]' << "\t|\t|\t" << 'minmax with depth = ' << depth.to_s
        pre = @nb_children.inject(0, :+)

        parents_threads = []
        parents.each do |parent|

          parents_threads.push(Thread.new {

            if parent['children'].empty?

              children(parent)
              @nb_children.push parent['children'].length

            end

          })

        end

        parents_threads.each do |thread|
          thread.join
        end

        sum = @nb_children.inject(0, :+)
        print "\n" << '[INFO]' << "\t|\t|\t" << 'Game has now ' + sum.to_s + ' children' if sum > pre

        new_parents = []
        parents.each do |parent|
          parent['children'].each do |mv,child|
            new_parents.push @boards[child]
          end
        end

        parents = new_parents
        depth += 1

      end

      update_valuations @boards[@actual_hash], MAX
      (@boards[@actual_hash]['valuation_max'] = @boards[@actual_hash]['valuation'])

    end

    private :minmax

    ##
    #
    #  alpha-beta
    #
    #  @return Integer
    #  @scope private
    #

    def alphabeta(limit, depth = 0, node = nil, alpha = nil, beta = nil, min_or_max = nil)

      if node.nil? || alpha.nil? || beta.nil?

        @nb_children = 0 if @nb_children == []

        print "\n" << '[INFO]' << "\t|\t" << 'alphabeta with limit = ' << limit.to_s
        @boards[@actual_hash]['valuation'] = alphabeta(limit, depth, @boards[@actual_hash], -INF, INF, MAX)

        print "\n" << '[INFO]' << "\t|\t|--> " << 'Went through ' + @nb_children.to_s + ' possibilities'

      else

        if depth <= limit

          if node['children'].empty?

            children node
            @nb_children += node['children'].length

          end

          if min_or_max == MIN && node['valuation_min_depth'] >= (limit-depth)
            node['valuation_min']
          elsif min_or_max == MIN

            valuation = INF
            node['children'].each do |mv,child|

                valuation = [valuation, alphabeta(limit, depth+1, @boards[child], alpha, beta, (min_or_max+1)%2)].min
                break if alpha >= valuation

                beta = [beta, valuation].min

            end

            node['valuation_min_depth'] = limit-depth
            (node['valuation_min'] = valuation)

          elsif min_or_max == MAX && node['valuation_max_depth'] >= (limit-depth)
            node['valuation_max']
          else

            valuation = -INF
            node['children'].each do |mv,child|

              valuation = [valuation, alphabeta(limit, depth+1, @boards[child], alpha, beta, (min_or_max+1)%2)].max
              break if beta <= valuation

              alpha = [alpha, valuation].max

            end

            node['valuation_max_depth'] = limit-depth
            (node['valuation_max'] = valuation)

          end

        else

          if min_or_max == MIN && node['valuation_min_depth'] == -1
            (node['valuation_min'] = node['valuation'])

          elsif min_or_max == MAX && node['valuation_max_depth'] == -1
            (node['valuation_max'] = node['valuation'])

          elsif min_or_max == MIN && node['valuation_min_depth'] > -1
            node['valuation_min']

          elsif min_or_max == MAX && node['valuation_man_depth'] > -1
            node['valuation_max']

          end

        end

      end

    end

    private :alphabeta

    ##
    #
    #  Compute every children of a given parent
    #
    #  @return nil
    #  @scope private

    def children(parent)

      parent_chessboard_hash = parent['chessboard_hash']
      if parent['children'].empty?

        parent_game = Game.new
        parent_game.from_hash parent['game_hash']

        # Computes every possible move

        possible_moves = {}
        possible_moves_threads = []

        parent_game.chessboard.each do |k,piece|
          if !piece.nil? && piece.color == parent_game.round_player
            possible_moves_threads.push(Thread.new {
              possible_moves[k] = piece.possible_moves(parent_game)
            })
          end
        end

        possible_moves_threads.each do |thread|
          thread.join
        end

        # Computes every child according to possible_moves
        children_threads = []
        possible_moves.each do |from,moves|
          moves.each do |to|

            children_threads.push(Thread.new {

              child = Chess::Game.new
              child.from_hash parent['game_hash']

              if child.move(from, to)

                child_chessboard_hash = chessboard_hash child.chessboard
                @boards[parent_chessboard_hash]['children'].push([[from,to], child_chessboard_hash])

                unless @boards.has_key? child_chessboard_hash

                  @boards[child_chessboard_hash] = {
                    'chessboard_hash' => child_chessboard_hash,
                    'game_hash' => child.hash,
                    'children' => [],
                    'valuation' => child.valuation(@color),
                    'valuation_min' => nil,
                    'valuation_min_depth' => -1,
                    'valuation_max' => nil,
                    'valuation_max_depth' => -1
                  }

                end

              end

            })

          end
        end

        children_threads.each do |thread|
          thread.join
        end

      end

    end

    private :children

    ##
    #
    #  Update the valuations
    #
    #  @return nil
    #  @scope private
    #

    def update_valuations(from, min_or_max)

      unless from['children'].empty?

        children_threads = []
        from['children'].each do |mv,child|
          update_valuations @boards[child], (min_or_max+1)%2
        end

        from['valuation'] = (min_or_max == MAX) ? max_of_children(from) : min_of_children(from)

      end

    end

    ##
    #
    #  Give the min valuation of parent's direct children
    #
    #  @return Integer
    #  @scope private
    #

    def min_of_children(parent)

      min = nil

      parent['children'].each do |mv,child|
        if min.nil? || @boards[child]['valuation'] < min
          min = @boards[child]['valuation']
        end
      end

      min

    end

    ##
    #
    #  Give the min valuation of parent's direct children
    #
    #  @return Integer
    #  @scope private
    #

    def max_of_children(parent)

      max = nil

      parent['children'].each do |mv,child|
        if max.nil? || @boards[child]['valuation'] > max
          max = @boards[child]['valuation']
        end
      end

      max

    end

    ##
    #
    #  Hash the chessboard
    #
    #  @return String
    #  @scope private
    #

    def chessboard_hash(chessboard)

      res = ''

      chessboard.each do |k,piece|
        res << ((piece.nil?) ? 'XX' : (piece.color.to_s << piece.class.name)) << '-'
      end

      res

    end

  end

end
