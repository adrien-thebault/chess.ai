module Chess::Pieces

  ##
  #
  #  Bishop
  #  @author Adrien Thébault <me@adrien-thebault.fr>
  #

  module Bishop

    def self.possible_moves(game,pos)

      color, ennemy = game[:chessboard][pos[0]][pos[1]]&0b1000, Chess::Game.ennemy(game[:chessboard][pos[0]][pos[1]]&0b1000)
      line, res, finished_left, finished_right, orientation = pos[0], [], false, false, -1

      loop do

        if line <= pos[0] then col_right, col_left = pos[1] - (line - pos[0]), pos[1] + (line - pos[0])
        else col_left, col_right = pos[1] - (line - pos[0]), pos[1] + (line - pos[0]) end

        unless finished_left

          res.push([line,col_left]) if game[:chessboard][line][col_left].nil? || game[:chessboard][line][col_left]&0b1000 == ennemy
          finished_left = true if (!game[:chessboard][line][col_left].nil? && line != pos[0]) || col_left == 0

        end

        unless finished_right

          res.push([line,col_right]) if game[:chessboard][line][col_right].nil? || game[:chessboard][line][col_right]&0b1000 == ennemy
          finished_right = true if (!game[:chessboard][line][col_right].nil? && line != pos[0]) || col_right == 7

        end

        line, finished_left, finished_right, orientation = pos[0]-1, false, false, 1 if (finished_left && finished_right && line <= pos[0]) || (line == 0 && orientation == -1)
        break if (finished_left && finished_right && line > pos[0]) || (line == 7 && orientation == 1)

        line += orientation

      end

      res

    end

  end

end
