##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

#  --

module Chess

  ##
  #  Knight

  class Knight < Chess::Piece

    ##
    #
    #  Gives all the possibles moves for this piece in game
    #
    #  @return Array
    #  @scope public
    #

    def possible_moves(game)

      res = []

      moves = [

        # Forward + Left*2
        [(@col.chr.ord-2).chr + (@line.to_i+forward).to_s, @col != 'a' && @col != 'b' && (@line.to_i+forward > 0 && @line.to_i+forward < 9)],
        # Forward + Right*2
        [(@col.chr.ord+2).chr + (@line.to_i+forward).to_s, @col != 'g' && @col != 'h' && (@line.to_i+forward > 0 && @line.to_i+forward < 9)],
        # Forward*2 + Left
        [(@col.chr.ord-1).chr + (@line.to_i+2*forward).to_s, @col != 'a' && (@line.to_i+2*forward > 0 && @line.to_i+2*forward < 9)],
        # Forward*2 + Right
        [(@col.chr.ord+1).chr + (@line.to_i+2*forward).to_s, @col != 'h' && (@line.to_i+2*forward > 0 && @line.to_i+2*forward < 9)],
        # Backward + Left*2
        [(@col.chr.ord-2).chr + (@line.to_i+backward).to_s, @col != 'a' && @col != 'b' && (@line.to_i+backward > 0 && @line.to_i+backward < 9)],
        # Backward + Right*2
        [(@col.chr.ord+2).chr + (@line.to_i+backward).to_s, @col != 'g' && @col != 'h' && (@line.to_i+backward > 0 && @line.to_i+backward < 9)],
        # Backward*2 + Left
        [(@col.chr.ord-1).chr + (@line.to_i+2*backward).to_s, @col != 'a' && (@line.to_i+2*backward > 0 && @line.to_i+2*backward < 9)],
        # Backward*2 + Right
        [(@col.chr.ord+1).chr + (@line.to_i+2*backward).to_s, @col != 'h' && (@line.to_i+2*backward > 0 && @line.to_i+2*backward < 9)]

      ]

      moves.each do |move|
        res.push move[0] if move[1] && (game.chessboard[move[0]].nil? || game.chessboard[move[0]].color != @color)
      end

      res

    end

  end

end
