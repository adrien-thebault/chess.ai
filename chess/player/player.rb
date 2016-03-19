##
#
#  @author Adrien Thébault <me@adrien-thebault.fr>
#  @brief Actual artifical intelligence
#

## --

module Chess

  module Player

    ##
    #  Constants

    MIN, MAX, INF = 0, 1, 999999
    MAX_DEPTH = 4

    ##
    #
    #  Tries to guess the best move
    #
    #  @return Array [from, to]
    #  @scope public
    #

    def self.play(game, player)

      ##
      # => Round data

      @@current, @@player = hash(game[:chessboard]), player
      @@finished, @@best_move, @@boards, @@time_left = false, nil, {
        @@current => {
          :hash => @@current,
          :game => game,
          :children => [],
          :valuation => Chess::Game.valuation(game, @@player),
          :valuation_min => nil,
          :valuation_min_depth => -1,
          :valuation_max => nil,
          :valuation_max_depth => -1
        }
      }, 0

      INFO << 'Actual valuation : ' + @@boards[@@current][:valuation].to_s

      ##
      # => Let's play!

      #thr = (Thread.new do
      #  begin
      #    iterative_deepening_alphabeta
      #  rescue Exception => e

      #    ERROR << e.class.name + ' : ' + e.message
      #    e.backtrace.each do |trace| ERROR << "|\t" + trace.to_s end

      #  end
      #end)

      #sleep 0.1 until ((@@time_left = (@@boards[@@current][:game][:round][:end_time].to_i - Time.now.to_i)) < 5) || thr.stop?

      #WARNING << '5 seconds left, sending actual best move' if ((@@boards[@@current][:game][:round][:end_time].to_i - Time.now.to_i) < 5)
      #SUCCESS << 'Iterative deepening alpha-beta finished' if thr.stop?

      iterative_deepening_alphabeta
      @@best_move

    end

    ##
    #
    #  Iterative deepening
    #
    #  @return nil
    #  @scope public
    #

    def self.iterative_deepening_alphabeta

      INFO << 'Starting iterative deepening alpha-beta with MAX_DEPTH = ' + MAX_DEPTH.to_s
      @@nb_children, depth = 0, 1

      until depth > MAX_DEPTH

        INFO << "|\ttime left : " + @@time_left.to_s + "s"
        INFO << "|\tdepth = " + depth.to_s

        alphabeta(depth)

        INFO << "|\tBest move has a valuation of " + @@boards[@@current][:valuation].to_s

        @@boards[@@current][:children].sort! do |x,y| @@boards[y.last][:valuation_min] <=> @@boards[x.last][:valuation_min] end
        move = @@boards[@@current][:children].first.first

        @@best_move = [(move[0][1]+97).chr + (move[0][0]+1).to_s, (move[1][1]+97).chr + (move[1][0]+1).to_s] unless move.nil?
        INFO << "|\t-> move = " + @@best_move.to_s + "\n"

        depth += 1

      end

    end

    ##
    #
    #  alpha-beta
    #
    #  @return Integer
    #  @scope public
    #

    def self.alphabeta(limit, depth = 1, node = nil, alpha = nil, beta = nil, min_or_max = nil)

      if node.nil? || alpha.nil? || beta.nil? || min_or_max.nil?

        INFO << "|\t|\talpha-beta with limit = " + limit.to_s

        @@boards[@@current][:valuation] = alphabeta(limit, depth, @@boards[@@current], -INF, INF, MAX)
        INFO << "|\t|\twent through " + @@nb_children.to_s + " possibilities"

      else

        if depth <= limit

          if node[:children].empty?

            children(node)
            @@nb_children += node[:children].length

          end

          if min_or_max == MIN && node[:valuation_min_depth] >= (limit-depth)
            node[:valuation_min]
          elsif min_or_max == MIN

            valuation = INF
            node[:children].each do |mv,child|

                valuation = [valuation, alphabeta(limit, depth+1, @@boards[child], alpha, beta, (min_or_max+1)%2)].min
                break if alpha >= valuation

                beta = [beta, valuation].min

            end

            node[:valuation_min_depth] = limit-depth
            (node[:valuation_min] = valuation)

          elsif min_or_max == MAX && node[:valuation_max_depth] >= (limit-depth)
            node[:valuation_max]
          else

            valuation = -INF
            node[:children].each do |mv,child|

              valuation = [valuation, alphabeta(limit, depth+1, @@boards[child], alpha, beta, (min_or_max+1)%2)].max
              break if beta <= valuation

              alpha = [alpha, valuation].max

            end

            node[:valuation_max_depth] = limit-depth
            (node[:valuation_max] = valuation)

          end

        else

          if min_or_max == MIN && node[:valuation_min_depth] == -1
            node[:valuation_min_depth] = limit-depth
            (node[:valuation_min] = Chess::Game.valuation(node[:game], @@player))

          elsif min_or_max == MAX && node[:valuation_max_depth] == -1
            node[:valuation_max_depth] = limit-depth
            (node[:valuation_max] = Chess::Game.valuation(node[:game], @@player))

          elsif min_or_max == MIN && node[:valuation_min_depth] > -1
            node[:valuation_min]

          elsif min_or_max == MAX && node[:valuation_max_depth] > -1
            node[:valuation_max]

          end

        end

      end

    end

    ##
    #
    #  Compute every children of a given parent
    #
    #  @return nil
    #  @scope public

    def self.children(parent)

      parent_hash = parent[:hash]
      if parent[:children].empty?

        parent_game = Chess::Game.clone(parent[:game])

        # Computes every possible move
        processes, pipes = [], {}
        parent_game[:chessboard].each_with_index do |row,line|
          row.each_with_index do |piece,col|
            if !piece.nil? && ((piece&0b1000) == parent_game[:round][:player])
              read, write = IO.pipe
              if (pid = fork).nil?
                read.close
                Chess::Game.possible_moves(parent_game, [line,col], true).each do |move|

                  child_hash = hash((game = Chess::Game.move(parent_game, [line,col], move))[:chessboard])
                  game[:valuation] = nil

                  write << (JSON.fast_generate([[line,col],move,{
                    :hash => child_hash,
                    :game => game,
                    :children => [],
                    :valuation => nil,
                    :valuation_min => nil,
                    :valuation_min_depth => -1,
                    :valuation_max => nil,
                    :valuation_max_depth => -1
                  }]) + "\n") unless @@boards.has_key?(child_hash)

                end
                write.close
                exit
              else

                processes.push(pid)

                write.close
                pipes[pid] = read

              end
            end
          end
        end

        processes.each do |process|

          Process.wait(process, 1)
          pipes[process].readlines.each do |data|

            from, to, child = JSON.parse(data, :symbolize_names => true)

            @@boards[parent_hash][:children].push([[from, to], child[:hash]])
            @@boards[child[:hash]] = child unless @@boards.has_key?(child[:hash])

          end

          pipes[process].close

        end

      end

    end

    ##
    #
    #  Update the valuations
    #
    #  @return nil
    #  @scope public
    #

    def self.update_valuations(from, min_or_max)
      unless from[:children].empty?

        threads = []
        from[:children].each do |mv,child| threads.push(Thread.new do update_valuations(@@boards[child], (min_or_max+1)%2) end) end
        threads.each do |thr| thr.join end

        from[:valuation] = (min_or_max == MAX) ? max_of_children(from) : min_of_children(from)

      end
    end

    ##
    #
    #  Give the min valuation of parent's direct children
    #
    #  @return Integer
    #  @scope public
    #

    def self.min_of_children(parent)
      (parent[:children].min do |x,y| x[:valuation] <=> y[:valuation] end)[:valuation]
    end

    ##
    #
    #  Give the max valuation of parent's direct children
    #
    #  @return Integer
    #  @scope public
    #

    def self.max_of_children(parent)
      (parent[:children].min do |x,y| x[:valuation] <=> y[:valuation] end)[:valuation]
    end

    ##
    #
    #  Hash the chessboard
    #
    #  @return String
    #  @scope public
    #

    def self.hash(chessboard)

      res = ''

      chessboard.each do |line|
        line.each do |piece| res << ((piece.nil?) ? '-' : piece.to_s) end
      end

      res

    end

  end

end
