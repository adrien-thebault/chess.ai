module Chess::Pieces

  ##
  #
  #  Rook
  #  @author Adrien Thébault <me@adrien-thebault.fr>
  #

  module Rook

    def self.possible_moves(game,pos)

      color, ennemy = game[:chessboard][pos[0]][pos[1]]&0b1000, Chess::Game.ennemy(game[:chessboard][pos[0]][pos[1]]&0b1000)
      offset, res, finished_top, finished_bottom, finished_left, finished_right = 0, [], false, false, false, false

      loop do

        unless finished_top

          res.push([pos[0]+offset, pos[1]]) if game[:chessboard][pos[0]+offset][pos[1]].nil? || game[:chessboard][pos[0]+offset][pos[1]]&0b1000 == ennemy
          finished_top = true if (!game[:chessboard][pos[0]+offset][pos[1]].nil? && offset > 0) || pos[0]+offset == 7

        end

        unless finished_bottom

          res.push([pos[0]-offset, pos[1]]) if game[:chessboard][pos[0]-offset][pos[1]].nil? || game[:chessboard][pos[0]-offset][pos[1]]&0b1000 == ennemy
          finished_bottom = true if (!game[:chessboard][pos[0]-offset][pos[1]].nil? && offset > 0) || pos[0]-offset == 0

        end

        unless finished_left

          res.push([pos[0], pos[1]-offset]) if game[:chessboard][pos[0]][pos[1]-offset].nil? || game[:chessboard][pos[0]][pos[1]-offset]&0b1000 == ennemy
          finished_left = true if (!game[:chessboard][pos[0]][pos[1]-offset].nil? && offset > 0) || pos[1]-offset == 0

        end

        unless finished_right

          res.push([pos[0], pos[1]+offset]) if game[:chessboard][pos[0]][pos[1]+offset].nil? || game[:chessboard][pos[0]][pos[1]+offset]&0b1000 == ennemy
          finished_right = true if (!game[:chessboard][pos[0]][pos[1]+offset].nil? && offset > 0) || pos[1]+offset == 7

        end

        break if finished_top && finished_bottom && finished_left && finished_right
        offset += 1

      end

      res

    end

  end

end
