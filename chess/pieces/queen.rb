module Chess::Pieces

  ##
  #
  #  Queen
  #  @author Adrien Thébault <me@adrien-thebault.fr>
  #

  module Queen

    def self.possible_moves(game,pos)
      Chess::Pieces::Bishop.possible_moves(game,pos) + Chess::Pieces::Rook.possible_moves(game,pos)
    end

  end

end
