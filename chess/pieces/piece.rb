##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

#  --

module Chess

  ##
  #  Piece

  class Piece

    ##
    #  Constants

    COLOR_WHITE = 0
    COLOR_BLACK = 1

    ##
    #
    #  initialize
    #
    #  @return nil
    #  @scope public
    #

    def initialize(position)
      @position = position
      @col = position[0]
      @line = position[1]
    end

    ##
    #
    #  Set piece as white
    #
    #  @return nil
    #  @scope public
    #

    def white!
      @color = COLOR_WHITE
    end

    ##
    #
    #  Set piece as black
    #
    #  @return nil
    #  @scope public
    #

    def black!
      @color = COLOR_BLACK
    end

    ##
    #
    #  Returns true if piece is in danger
    #
    #  @return Boolean
    #  @scope public
    #

    def in_danger?(game)

      in_danger = false

      game.chessboard.each do |k,piece|
        if !piece.nil? && piece.color != @color && (piece.possible_moves(game).include? @position)
          in_danger = true
          break
        end
      end

      in_danger

    end

    ##
    #
    #  Returns the number of pieces endangered by this one
    #
    #  @return Integer
    #  @scope public
    #

    def endangered_pieces(game)

      res = 0

      possible_moves(game).each do |move|
        res += 1 unless game.chessboard[move].nil? || game.chessboard[move].color == @color
      end

      res

    end

    ##
    #
    #  Returns line increment to go forward
    #
    #  @return Integer
    #  @scope public
    #

    def forward
      if @color == COLOR_WHITE
        +1
      else
        -1
      end
    end

    ##
    #
    #  Returns line increment to go backward
    #
    #  @return Integer
    #  @scope public
    #

    def backward
      -forward
    end

    ##
    #
    #  Move the piece
    #
    #  @return nil
    #  @scope public
    #

    def move(to)
      @position = to
      @col = to[0]
      @line = to[1]
    end

    ##
    #  Getters

    attr_reader :position, :line, :col, :color

  end

end
