##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

#  --

module Chess

  ##
  #  Queen

  class Queen < Chess::Piece

    ##
    #
    #  Gives all the possibles moves for this piece in game
    #
    #  @return Array
    #  @scope public
    #

    def possible_moves(game)

      rook = Chess::Rook.new @position
      bishop = Chess::Bishop.new @position

      if @color == COLOR_WHITE
        rook.white!
        bishop.white!
      else
        rook.black!
        bishop.black!
      end

      rook.possible_moves(game) + bishop.possible_moves(game)

    end

  end

end
