##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

#  --

module Chess

  ##
  #  Pawn

  class Pawn < Chess::Piece

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

        # Forward
        [@col + (@line.to_i+forward).to_s,
          (((@line.to_i+forward) <= 8) && ((@line.to_i+forward) >= 1)) && game.chessboard[@col + (@line.to_i+forward).to_s].nil?],

        # Forward*2
        [@col + (@line.to_i+2*forward).to_s,
          ((@color == COLOR_WHITE && @line.to_i == 2) || (@color == COLOR_BLACK && @line.to_i == 7)) && game.chessboard[@col + (@line.to_i+2*forward).to_s].nil?],

        # Forward+Right
        [(@col.chr.ord+1).chr + (@line.to_i+forward).to_s,
          (((@line.to_i+forward) <= 8) & ((@line.to_i+forward) >= 1)) && @col != 'h' && !game.chessboard[(@col.chr.ord+1).chr + (@line.to_i+forward).to_s].nil? && game.chessboard[(@col.chr.ord+1).chr + (@line.to_i+forward).to_s].class.name != 'King' && game.chessboard[(@col.chr.ord+1).chr + (@line.to_i+forward).to_s].color != @color],

        # Forward+Left
        [(@col.chr.ord-1).chr + (@line.to_i+forward).to_s,
          (((@line.to_i+forward) <= 8) & ((@line.to_i+forward) >= 1)) && @col != 'a' && !game.chessboard[(@col.chr.ord-1).chr + (@line.to_i+forward).to_s].nil? && game.chessboard[(@col.chr.ord-1).chr + (@line.to_i+forward).to_s].class.name != 'King' && game.chessboard[(@col.chr.ord-1).chr + (@line.to_i+forward).to_s].color != @color],

        # @TODO : Add "En passant"

      ]

      moves.each do |move|
        res.push move[0] if move[1]
      end

      res

    end

  end

end
