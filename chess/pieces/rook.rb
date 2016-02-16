##
#  @author Adrien Thébault <me@adrien-thebault.fr>
#

#  --

module Chess

  ##
  #  Rook

  class Rook < Chess::Piece

    ##
    #
    #  Gives all the possibles moves for this piece in game
    #
    #  @return Array
    #  @scope public
    #

    def possible_moves(game)

      res = []

      # To the right!
      for col in ((@col.chr.ord+1).chr..'h')
        if game.chessboard[col + @line].nil?
          res.push(col + @line)
        elsif game.chessboard[col + @line].color != @color
          res.push(col + @line)
          break
        else
          break
        end
      end

      # To the left!
      for col in ('a'..(@col.chr.ord-1).chr).to_a.reverse
        if game.chessboard[col + @line].nil?
          res.push(col + @line)
        elsif game.chessboard[col + @line].color != @color
          res.push(col + @line)
          break
        else
          break
        end
      end

      # To the top!
      for line in (@line.to_i+1..8)
        if game.chessboard[@col + line.to_s].nil?
          res.push(@col + line.to_s)
        elsif game.chessboard[@col + line.to_s].color != @color
          res.push(@col + line.to_s)
          break
        else
          break
        end
      end

      # To the bottom!
      for line in (1..@line.to_i-1).to_a.reverse
        if game.chessboard[@col + line.to_s].nil?
          res.push(@col + line.to_s)
        elsif game.chessboard[@col + line.to_s].color != @color
          res.push(@col + line.to_s)
          break
        else
          break
        end
      end

      res

    end

  end

end
