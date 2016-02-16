##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

#  --

module Chess

  ##
  #  Bishop

  class Bishop < Chess::Piece

    ##
    #
    #  Gives all the possibles moves for this piece in game
    #
    #  @return Array
    #  @scope public
    #

    def possible_moves(game)

      res = []

      # top-left
      for line in (@line.to_i+1..8)

        col = (@col.chr.ord - (line - @line.to_i)).chr
        if col.chr.ord >= 'a'.chr.ord && col.chr.ord <= 'h'.chr.ord
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color == @color
          res.push(col + line.to_s)
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color != @color
        end

      end

      # top-right
      for line in (@line.to_i+1..8)

        col = (@col.chr.ord + (line - @line.to_i)).chr
        if col.chr.ord >= 'a'.chr.ord && col.chr.ord <= 'h'.chr.ord
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color == @color
          res.push(col + line.to_s)
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color != @color
        end

      end

      # bottom-right
      for line in (1..@line.to_i-1).to_a.reverse

        col = (@col.chr.ord - (line - @line.to_i)).chr
        if col.chr.ord >= 'a'.chr.ord && col.chr.ord <= 'h'.chr.ord
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color == @color
          res.push(col + line.to_s)
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color != @color
        end

      end

      # bottom-left
      for line in (1..@line.to_i-1).to_a.reverse

        col = (@col.chr.ord + (line - @line.to_i)).chr
        if col.chr.ord >= 'a'.chr.ord && col.chr.ord <= 'h'.chr.ord
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color == @color
          res.push(col + line.to_s)
          break if !game.chessboard[col + line.to_s].nil? && game.chessboard[col + line.to_s].color != @color
        end

      end

      res

    end

  end

end
