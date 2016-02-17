##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

##
#  Dependencies

require 'time'

require './chess/pieces/piece'
require './chess/pieces/bishop'
require './chess/pieces/king'
require './chess/pieces/knight'
require './chess/pieces/pawn'
require './chess/pieces/rook'
require './chess/pieces/queen'

#  --

module Chess

  ##
  #  Game

  class Game

    ##
    #  Constants

    PLAYER_WHITE = 0
    PLAYER_BLACK = 1

    ##
    #
    #  Load game using hash
    #
    #  @return nil
    #  @scope public

    def from_hash(hash)

      @key = hash['id']

      @round_number = hash['round']['nb']
      @round_player = (hash['round']['to_play'] == 'white') ? PLAYER_WHITE : PLAYER_BLACK
      @round_time = hash['round']['time'].to_i
      @round_end_time = (hash['round']['end_time'] == -1) ? (Time.now+@round_time) : Time.parse(hash['round']['end_time'])

      @chessboard = {}

      for line in (1..8)
        for col in ('a'..'h')

          if hash['chessboard'][col + line.to_s]['type'].nil?

            piece = nil

          else

            piece = Object.const_get('Chess::' + hash['chessboard'][col + line.to_s]['type'].capitalize).new(col + line.to_s)

            piece.white! if hash['chessboard'][col + line.to_s]['color'] == 'white'
            piece.black! if hash['chessboard'][col + line.to_s]['color'] == 'black'

          end

          @chessboard[col + line.to_s] = piece

        end
      end

    end

    ##
    #
    #  Moves a piece
    #
    #  @return Boolean
    #  @scope public
    #

    def move(from, to, bypass_validity = false)
      if bypass_validity || valid_move?(from, to)

        @chessboard[from].move to
        @chessboard[to] = @chessboard[from].clone
        @chessboard[from] = nil

        unless bypass_validity
          @round_number += 1
          @round_player = (round_player == PLAYER_WHITE) ? PLAYER_BLACK : PLAYER_WHITE
          @round_end_time = Time.now + @round_time.to_i
        end

        @hash = nil
        true

      else
        false
      end
    end

    ##
    #
    #  Is the move valid?
    #
    #  @return Boolean
    #  @scope public
    #

    def valid_move?(from,to)
      !@checkmate && (!@chessboard[from].nil? && @chessboard[from].color == @round_player) && (@round_end_time == -1 || Time.now < @round_end_time) && (@chessboard[from].possible_moves(self).include? to) && !check?(from, to)
    end

    ##
    #
    #  Check if move applied ?
    #
    #  @return Boolean
    #  @scope public
    #

    def check?(from = nil, to = nil, color = nil)

      game = Game.new
      game.from_hash hash
      game.move(from, to, true) unless from.nil? && to.nil?
      color = @round_player if color.nil?

      res = false
      game.chessboard.each do |k,piece|

        if !piece.nil? && piece.color == color && piece.class == Chess::King
          res = piece.in_danger? game
          break
        end

      end

      res

    end

    ##
    #
    #  Checkmate?
    #
    #  @return Boolean
    #  @scope public
    #

    def checkmate?(color = nil)

      color = @round_player if color.nil?

      res = true
      @chessboard.each do |k,piece|

        if !piece.nil? && piece.color == color

          piece.possible_moves(self).each do |move|
            res = res && check?(k, move)
          end

          break if res == false

        end

      end

      res

    end

    ##
    #
    #  Give the valuation for the color c
    #
    #  @todo add bad trade penalty
    #  @todo add elephantiasis effect
    #
    #  @return Integer
    #  @scope public
    #

    CENTIPAWN = 1

    PIECE_VALUE = {
      Chess::King => 0*CENTIPAWN,
      Chess::Pawn => 100*CENTIPAWN,
      Chess::Bishop => 350*CENTIPAWN,
      Chess::Knight => 350*CENTIPAWN,
      Chess::Rook => 525*CENTIPAWN,
      Chess::Queen => 1000*CENTIPAWN
    }

    TRAPPED_PIECES = {
      Chess::Knight => {
        PLAYER_WHITE => ['a1', 'h1', 'a2', 'h2'],
        PLAYER_BLACK => ['a8', 'h8', 'a7', 'h7']
      }
    }

    TRAPPED_MALUS = 0.5*PIECE_VALUE[Chess::Pawn]
    BISHOP_PAIR_BONUS = 0.5*PIECE_VALUE[Chess::Pawn]
    ROOK_PAIR_MALUS = 0.5*PIECE_VALUE[Chess::Pawn]
    KNIGHT_PAIR_MALUS = 0.5*PIECE_VALUE[Chess::Pawn]
    RETURNING_BISHOP_MALUS = 0.5*PIECE_VALUE[Chess::Pawn]
    NO_PAWN_MALUS = 2*PIECE_VALUE[Chess::Pawn]

    LESS_PAWN_KNIGHT_VALUE_BONUS = 0.5
    LESS_PAWN_ROOK_VALUE_BONUS = 0.5

    PAWN_SHIELD_BONUS = 2*PIECE_VALUE[Chess::Pawn]

    PIECE_IN_DANGER = 3
    MOBILITY = 0.05

    CHECKMATE = 99999*CENTIPAWN
    CHECK = 500*CENTIPAWN

    def valuation(c)

      valuation = 0
      material_valuation = 0

      piece_number = {
        Chess::King => 0,
        Chess::Pawn => 0,
        Chess::Bishop => 0,
        Chess::Knight => 0,
        Chess::Rook => 0,
        Chess::Queen => 0
      }

      # Bonus if the opponent is check or checkmate
      # Malus if we are check or checkmate

      opponent_checkmate, opponent_check, we_checkmate, we_check = 0, 0, 0, 0
      check_checkmate_thread = Thread.new {

        opponent_checkmate += CHECKMATE if checkmate?((c+1)%2)
        opponent_check += CHECK if check?(nil, nil, (c+1)%2)

        we_checkmate -= CHECKMATE if checkmate?(c)
        we_check -= CHECK if check?(nil, nil, c)

      }

      pieces_threads = []
      @chessboard.each do |k,piece|
        if !piece.nil? && piece.color == c

          pieces_threads.push(Thread.new {

            # Count the number of each type of piece
            piece_number[piece.class] += 1

            # Malus if piece is in danger !
            valuation -= PIECE_IN_DANGER*PIECE_VALUE[piece.class] if piece.in_danger?(self)

            # Malus if piece is trapped
            valuation -= TRAPPED_MALUS if TRAPPED_PIECES.has_key?(piece.class) && TRAPPED_PIECES[piece.class].include?(k)

            # Bonus if piece (!= pawn) can move
            valuation += MOBILITY*piece.possible_moves(self).length*PIECE_VALUE[piece.class] if piece.class != Chess::Pawn

            # Decrease the value of "rook" pawns
            piece_number[piece.class] -= 0.5 if piece.class == Chess::Pawn && k[0] == 'a' || k[0] == 'h'

            # Increase the value of "central" pawns
            piece_number[piece.class] += 0.5 if piece.class == Chess::Pawn && k[0] == 'd' || k[0] == 'e'

            # Malus for a Bishop which is on its original position
            valuation -= RETURNING_BISHOP_MALUS if piece.class == Chess::Bishop && ((c == PLAYER_WHITE && ['c1','f1'].include?(k)) || (c == PLAYER_BLACK && ['c8','f8'].include?(k)))

            # Bonus if king has a pawn shield
            if piece.class == Chess::King

              pawn_shield = true
              pawn_positions = [
                [(k[0].chr.ord-1).chr + (k[1].to_i-1).to_s,
                (k[0].chr.ord-1).chr + (k[1].to_i-2).to_s],
                [k[0] + (k[1].to_i-1).to_s,
                k[0] + (k[1].to_i-2).to_s],
                [(k[0].chr.ord+1).chr + (k[1].to_i-1).to_s,
                (k[0].chr.ord+1).chr + (k[1].to_i-2).to_s],
                [(k[0].chr.ord-1).chr + (k[1].to_i+1).to_s,
                (k[0].chr.ord-1).chr + (k[1].to_i+2).to_s],
                [k[0] + (k[1].to_i+1).to_s,
                k[0] + (k[1].to_i+2).to_s],
                [(k[0].chr.ord+1).chr + (k[1].to_i+1).to_s,
                (k[0].chr.ord+1).chr + (k[1].to_i+2).to_s],
              ]

              pawn_positions.each do |pos|
                if ((pos[0][0].chr.ord) >= 'a'.chr.ord && (pos[0][0].chr.ord) <= 'h'.chr.ord && pos[0][1].to_i >= 1 && pos[0][1].to_i <= 8) && ((pos[1][0].chr.ord) >= 'a'.chr.ord && (pos[1][0].chr.ord) <= 'h'.chr.ord && pos[1][1].to_i >= 1 && pos[1][1].to_i <= 8)
                  pawn_shield = pawn_shield && ((!@chessboard[pos[0]].nil? && @chessboard[pos[0]].class == Chess::Pawn) || (!@chessboard[pos[1]].nil? && @chessboard[pos[1]].class == Chess::Pawn))
                end
              end

              valuation += PAWN_SHIELD_BONUS if pawn_shield

            end

          })

        end
      end

      pieces_threads.each do |thread|
        thread.join
      end

      # Knights gain value the less pawn there are
      piece_number[Chess::Knight] *= (1 + (1 - (piece_number[Chess::Pawn]/8)))*LESS_PAWN_KNIGHT_VALUE_BONUS

      # Rooks gain value the less pawn there are
      piece_number[Chess::Rook] *= (1 + (1 - (piece_number[Chess::Pawn]/8)))*LESS_PAWN_ROOK_VALUE_BONUS

      # Material valuation
      piece_number.each do |piece, number|
        material_valuation += number*PIECE_VALUE[piece]
      end

      # Bonus if we have a bishop pair
      valuation += BISHOP_PAIR_BONUS if piece_number[Chess::Bishop] == 2

      # Malus if we have a rook pair (?)
      valuation -= ROOK_PAIR_MALUS if piece_number[Chess::Rook] == 2

      # Malus if we have a knight pair (?)
      valuation -= KNIGHT_PAIR_MALUS if piece_number[Chess::Knight] == 2

      # Malus if we have no pawn
      valuation -= NO_PAWN_MALUS if piece_number[Chess::Pawn] == 0

      # Malus if "if one side has no pawns left, it needs the equivalent of +4 pawns more material to win" no respected
      valuation -= INSUFFICIENT_MATERIAL_MALUS if piece_number[Chess::Pawn] == 0 && material_valuation < 4*PIECE_VALUE[Chess::Pawn]

      valuation += material_valuation

      check_checkmate_thread.join
      (valuation + opponent_checkmate + opponent_check + we_checkmate + we_check)

    end

    ##
    #
    #  Gives the material balance for the color c
    #
    #  @return Integer
    #  @scope public
    #

    def material_valuation(c)

      material_balance = 0
      @chessboard.each do |k,piece|
        if !piece.nil? && piece.color == c
          material_balance += PIECE_VALUE[piece.class]
        end
      end

      material_balance

    end

    ##
    #
    #  Give a hash usable by from_hash
    #
    #  @return Hash
    #  @scope public
    #

    def hash

      if @hash.nil?

        chessboard = Hash.new
        @chessboard.each do |k,v|
          chessboard[k] = {
            'empty' => v.nil?,
            'color' => (v.nil?) ? nil:((v.color == Chess::Piece::COLOR_WHITE) ? 'white' : 'black'),
            'type' => (v.nil?) ? nil:v.class.name.downcase.split('::').last
          }
        end

        @hash = {
          'id' => @key,
          'round' => {
            'nb' => @round_number,
            'to_play' => (@round_player == PLAYER_WHITE) ? 'white' : 'black',
            'end_time' => @round_end_time.to_s,
            'time' => @round_time.to_s
          },
          'chessboard' => chessboard
        }

      end

      @hash

    end

    ##
    #  Getters

    attr_reader :key, :chessboard, :round_number, :round_time, :round_player, :round_end_time

  end

end
