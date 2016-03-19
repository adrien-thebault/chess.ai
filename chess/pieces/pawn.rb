module Chess::Pieces

  ##
  #
  #  Pawn
  #  @author Adrien Thébault <me@adrien-thebault.fr>
  #  @todo add "En passant"
  #

  module Pawn

    def self.possible_moves(game,pos)

      color, ennemy = game[:chessboard][pos[0]][pos[1]]&0b1000, Chess::Game.ennemy(game[:chessboard][pos[0]][pos[1]]&0b1000)
      forward, res = (color == Chess::Game::WHITE) ? 1 : -1, []

      res.push([pos[0]+1*forward, pos[1]]) unless pos[0]+1*forward < 0 || pos[0]+1*forward > 7 || !game[:chessboard][pos[0]+1*forward][pos[1]].nil?
      res.push([pos[0]+2*forward, pos[1]]) if ((color == Chess::Game::WHITE && pos[0] == 1)||(color == Chess::Game::BLACK && pos[0] == 6)) && game[:chessboard][pos[0]+1*forward][pos[1]].nil? && game[:chessboard][pos[0]+2*forward][pos[1]].nil?
      res.push([pos[0]+1*forward, pos[1]+1]) unless pos[0]+1*forward < 0 || pos[0]+1*forward > 7 || pos[1]+1 < 0 || pos[1]+1 > 7 || game[:chessboard][pos[0]+1*forward][pos[1]+1].nil? || game[:chessboard][pos[0]+1*forward][pos[1]+1]&0b1000 == color
      res.push([pos[0]+1*forward, pos[1]-1]) unless pos[0]+1*forward < 0 || pos[0]+1*forward > 7 || pos[1]-1 < 0 || pos[1]-1 > 7 || game[:chessboard][pos[0]+1*forward][pos[1]-1].nil? || game[:chessboard][pos[0]+1*forward][pos[1]-1]&0b1000 == color
      res.push(pos) if ((color == Chess::Game::WHITE && pos[0] == 7)||(color == Chess::Game::BLACK && pos[0] == 0))

      res

    end

  end

end
