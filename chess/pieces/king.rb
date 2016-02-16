##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

#  --

module Chess

  ##
  #  King

  class King < Chess::Piece

    ##
    #
    #  Gives all the possibles moves for this piece in game
    #
    #  @return Array
    #  @scope public
    #

    def possible_moves(game)

      res = [

        # Forward
        @col + (@line.to_i+forward).to_s,

        # Backward
        @col + (@line.to_i+backward).to_s,

        # Left + Backward
        (@col.chr.ord-1).chr + (@line.to_i+backward).to_s,

        # Left
        (@col.chr.ord-1).chr + @line.to_s,

        # Left+Forward
        (@col.chr.ord-1).chr + (@line.to_i+forward).to_s,

        # Right+Forward
        (@col.chr.ord+1).chr + (@line.to_i+forward).to_s,

        # Right
        (@col.chr.ord+1).chr + @line.to_s,

        # Right + Backward
        (@col.chr.ord+1).chr + (@line.to_i+backward).to_s

      ]

      moves = []
      res.each do |move|
        moves.push move if (move[0].chr.ord) >= 'a'.chr.ord && (move[0].chr.ord) <= 'h'.chr.ord && move[1].to_i >= 1 && move[1].to_i <= 8 && (game.chessboard[move].nil? || game.chessboard[move].color != @color)
      end

      moves

    end

  end

end
