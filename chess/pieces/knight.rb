module Chess::Pieces

  ##
  #
  #  Knight
  #  @author Adrien Thébault <me@adrien-thebault.fr>
  #

  module Knight

    def self.possible_moves(game,pos)

      color, ennemy = game[:chessboard][pos[0]][pos[1]]&0b1000, Chess::Game.ennemy(game[:chessboard][pos[0]][pos[1]]&0b1000)
      tmp, res = [
        [pos[0]+1,pos[1]+2],
        [pos[0]+1,pos[1]-2],
        [pos[0]-1,pos[1]+2],
        [pos[0]-1,pos[1]-2],
        [pos[0]+2,pos[1]+1],
        [pos[0]-2,pos[1]+1],
        [pos[0]+2,pos[1]-1],
        [pos[0]-2,pos[1]-1]
      ], []

      tmp.each do |move|
        res.push(move) unless (move[0] < 0 || move[0] > 7 || move[1] < 0 || move[1] > 7) || (!game[:chessboard][move[0]][move[1]].nil? && game[:chessboard][move[0]][move[1]]&0b1000 == color)
      end

      res

    end

  end

end
