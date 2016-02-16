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
    #  @return Integer
    #  @scope public
    #

    CENTIPAWN = 1

    PIECE_VALUE = {
      Chess::King => 0,
      Chess::Pawn => 100*CENTIPAWN,
      Chess::Bishop => 350*CENTIPAWN,
      Chess::Knight => 350*CENTIPAWN,
      Chess::Rook => 525*CENTIPAWN,
      Chess::Queen => 1000*CENTIPAWN
    }

    PIECE_IN_DANGER = 3
    MOBILITY = 0.05

    CHECKMATE = 99999
    CHECK = 300

    def valuation(c)

      valuation = 0
      material_balance = 0

      @chessboard.each do |k,piece|
        if !piece.nil? && piece.color == c

            # Material balance
            material_balance += PIECE_VALUE[piece.class]

            # Malus if piece is in danger !
            valuation -= PIECE_LOSS*piece.value if piece.in_danger?(self)

            # Bonus if piece (!= pawn) can move
            valuation += MOBILITY_GAIN*piece.possible_moves(self).length*piece.value if piece.class != Chess::Pawn

        end
      end

      # Bonus if the opponent is check or checkmate
      valuation += CHECKMATE if checkmate?((c+1)%2)
      valuation += CHECK if check?(nil, nil, (c+1)%2)

      # Malus if we are check or checkmate
      valuation -= CHECKMATE if checkmate?(c)
      valuation -= CHECK if check?(nil, nil, c)

      (valuation += material_balance)

    end

    ##
    #
    #  Gives the material balance for the color c
    #
    #  @return Integer
    #  @scope public
    #

    def material_balance(c)

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
