##
#  Dependencies

require 'json'
require 'time'

#  --

module Chess

  require_relative '../pieces/bishop'
  require_relative '../pieces/king'
  require_relative '../pieces/knight'
  require_relative '../pieces/pawn'
  require_relative '../pieces/queen'
  require_relative '../pieces/rook'

  ##
  #
  #  Game
  #  @author Adrien Thébault <me@adrien-thebault.fr>
  #

  module Game

    ##
    #  Constants

    WHITE, BLACK = 0b0000, 0b1000
    BISHOP, KING, KNIGHT, PAWN, QUEEN, ROOK = 0b0000, 0b0001, 0b0010, 0b0011, 0b0100, 0b0101

    ##
    #
    #  Returns other color
    #
    #  @return Integer
    #  @scope public
    #

    def self.ennemy(player)
      ~player & 0b1000
    end

    ##
    #
    #  Moves a piece
    #
    #  @return Game
    #  @scope public
    #

    def self.move(game, from, to)

      game[:chessboard][to[0]][to[1]] = game[:chessboard][from[0]][from[1]]
      game[:chessboard][from[0]][from[1]] = nil

      game[:round][:player] = ennemy(game[:round][:player])
      game[:round][:end_time] = Time.now.to_i+game[:round][:duration]
      game[:round][:number] += 1

      game

    end

    ##
    #
    #  Promote pawn to piece at pos
    #
    #  @return Game
    #  @scope public
    #

    def self.promote(game, pos, piece)

      game[:chessboard][pos[0]][pos[1]] = piece

      game[:round][:player] = ennemy(game[:round][:player])
      game[:round][:end_time] = Time.now.to_i+game[:round][:duration]
      game[:round][:number] += 1

      game

    end

    ##
    #
    #  Find piece in game
    #
    #  @return Array[line,col]
    #  @scope public
    #

    def self.find_piece(game, to_find)

      pos = nil
      game[:chessboard].each_with_index do |row, line|
        row.each_with_index do |piece, col|
          if piece == to_find
            pos = [line,col]
            break
          end
        end
        break unless pos.nil?
      end

      pos

    end

    ##
    #
    #  Check if piece is in danger
    #
    #  @return Boolean
    #  @scope public
    #

    def self.in_danger?(game, pos)

      in_danger = false
      game[:chessboard].each_with_index do |row, line|
        row.each_with_index do |piece, col|
          if (piece&0b1000) != (game[:chessboard][pos[0]][pos[1]]&0b1000)
            in_danger = possible_moves(game, [line,col]).include?(pos)
            break if in_danger
          end
        end
        break if in_danger
      end

      in_danger

    end

    ##
    #
    #  Possible moves of piece at pos in game
    #
    #  @return Array[Array]
    #  @scope public
    #

    def self.possible_moves(game, pos, reject = false)

      if game[:chessboard][pos[0]][pos[1]].nil? then []
      else

        piece = game[:chessboard][pos[0]][pos[1]]&0b0111

        if piece == Chess::Game::PAWN then moves = Chess::Pieces::Pawn.possible_moves(game,pos)
        elsif piece == Chess::Game::ROOK then moves = Chess::Pieces::Rook.possible_moves(game,pos)
        elsif piece == Chess::Game::QUEEN then moves = Chess::Pieces::Queen.possible_moves(game,pos)
        elsif piece == Chess::Game::KING then moves = Chess::Pieces::King.possible_moves(game,pos)
        elsif piece == Chess::Game::KNIGHT then moves = Chess::Pieces::Knight.possible_moves(game,pos)
        else moves = Chess::Pieces::Bishop.possible_moves(game,pos) end

        (reject) ? (moves.find_all do |mv| !check?(move(Chess::Game.clone(game), pos, mv), game[:chessboard][pos[0]][pos[1]]&0b1000) end) : moves

      end

    end

    ##
    #
    #  Check if a move is valid
    #
    #  @return Boolean
    #  @scope public
    #

    def self.valid_move?(game, from, to)
      possible_moves(game, from, true).include?(to)
    end

    ##
    #
    #  Check for player
    #
    #  @return Boolean
    #  @scope public
    #

    def self.check?(game, player)
      in_danger?(game, find_piece(game, player|Chess::Game::KING))
    end

    ##
    #
    #  Checkmate for player
    #
    #  @return Boolean
    #  @scope public
    #

    def self.checkmate?(game, player)
      if check?(game,player)

        no_possible_moves = true
        game[:chessboard].each_with_index do |row, line|
          row.each_with_index do |piece, col|
            if (piece&0b1000) == player
              no_possible_moves = possible_moves(game, [line, col], true).empty?
              break unless no_possible_moves
            end
          end
          break unless no_possible_moves
        end

        no_possible_moves

      else false end
    end

    ##
    #
    #  PAT for player
    #
    #  @return Boolean
    #  @scope public
    #

    def self.pat?(game, player)
      if !check?(game, player)

        no_possible_moves = true
        game[:chessboard].each_with_index do |row, line|
          row.each_with_index do |piece, col|
            if (piece&0b1000) == player
              no_possible_moves = possible_moves(game, [line, col], true).empty?
              break unless no_possible_moves
            end
          end
          break unless no_possible_moves
        end

        no_possible_moves

      else false end
    end

    ##
    #
    #  Check if draw
    #
    #  @return Boolean
    #  @scope public
    #

    def self.draw?(game, player)
      pat?(game, player)
    end

    ##
    #
    #  Give the valuation of game for player
    #
    #  @todo add bad trade penalty
    #  @todo add elephantiasis effect
    #
    #  @return Integer
    #  @scope public
    #

    CENTIPAWN = 1

    PIECE_VALUE = {
      Chess::Game::KING => 0*CENTIPAWN,
      Chess::Game::PAWN => 100*CENTIPAWN,
      Chess::Game::BISHOP => 350*CENTIPAWN,
      Chess::Game::KNIGHT => 350*CENTIPAWN,
      Chess::Game::ROOK => 525*CENTIPAWN,
      Chess::Game::QUEEN => 1000*CENTIPAWN
    }

    TRAPPED_PIECES = {
      Chess::Game::KNIGHT => {
        Chess::Game::WHITE => [[0,0], [0,7], [1,0], [1,7]],
        Chess::Game::BLACK => [[7,0], [7,7], [6,0], [6,7]]
      }
    }

    TRAPPED_MALUS = 0.5*PIECE_VALUE[Chess::Game::PAWN]
    BISHOP_PAIR_BONUS = 0.5*PIECE_VALUE[Chess::Game::PAWN]
    ROOK_PAIR_MALUS = 0.5*PIECE_VALUE[Chess::Game::PAWN]
    KNIGHT_PAIR_MALUS = 0.5*PIECE_VALUE[Chess::Game::PAWN]
    RETURNING_BISHOP_MALUS = 0.5*PIECE_VALUE[Chess::Game::PAWN]
    NO_PAWN_MALUS = 2*PIECE_VALUE[Chess::Game::PAWN]

    LESS_PAWN_KNIGHT_VALUE_BONUS = 0.5
    LESS_PAWN_ROOK_VALUE_BONUS = 0.5

    PAWN_SHIELD_BONUS = 2*PIECE_VALUE[Chess::Game::PAWN]

    PIECE_IN_DANGER = 3
    MOBILITY = 0.05

    CHECKMATE, CHECK = 99999*CENTIPAWN, 300*CENTIPAWN

    def self.valuation(game, player)

      if game[:valuation].nil?

        valuation, material, piece_number = 0, 0, {
          Chess::Game::KING => 0,
          Chess::Game::PAWN => 0,
          Chess::Game::BISHOP => 0,
          Chess::Game::KNIGHT => 0,
          Chess::Game::ROOK => 0,
          Chess::Game::QUEEN => 0
        }

        # Bonus if the opponent is check or checkmate
        # Malus if we are check or checkmate

        opponent_checkmate, opponent_check, we_checkmate, we_check = 0, 0, 0, 0
        check_checkmate_thread = Thread.new {

          opponent_checkmate += CHECKMATE if checkmate?(game, ennemy(player))
          opponent_check += CHECK if check?(game, ennemy(player))

          we_checkmate -= CHECKMATE if checkmate?(game, player)
          we_check -= CHECK if check?(game, player)

        }

        pieces_threads = []
        game[:chessboard].each_with_index do |row, l|
          row.each_with_index do |piece, c|

            if !piece.nil? && (piece & 0b1000) == player

              pieces_threads.push(Thread.new {

                piece &= 0b0111
                pos = [l,c]

                # Count the number of each type of piece + add its value to material valuation
                piece_number[piece] += 1
                material += PIECE_VALUE[piece]

                # Malus if piece is in danger !
                valuation -= PIECE_IN_DANGER*PIECE_VALUE[piece] if in_danger?(game, pos)

                # Malus if piece is trapped
                valuation -= TRAPPED_MALUS if TRAPPED_PIECES.has_key?(piece) && TRAPPED_PIECES[piece].include?(pos)

                # Bonus if piece (!= pawn) can move
                valuation += MOBILITY*possible_moves(game, piece).length*PIECE_VALUE[piece] if piece != Chess::Game::PAWN

                # Decrease the value of "rook" pawns
                piece_number[piece] -= 0.5 if piece == Chess::Game::PAWN && c == 0 || c == 7

                # Increase the value of "central" pawns
                piece_number[piece] += 0.5 if piece == Chess::Game::PAWN && c == 3 || c == 4

                # Malus for a Bishop which is on its original position
                valuation -= RETURNING_BISHOP_MALUS if piece == Chess::Game::BISHOP && ((player == Chess::Game::WHITE && [[0,2],[0,5]].include?(pos)) || (player == Chess::Game::BLACK && [[7,2],[7,5]].include?(pos)))

                # Bonus if king has a pawn shield
                #if piece.class == Chess::King
                #
                #  pawn_shield = true
                #  pawn_positions = [
                #    [(k[0].chr.ord-1).chr + (k[1].to_i-1).to_s,
                #    (k[0].chr.ord-1).chr + (k[1].to_i-2).to_s],
                #    [k[0] + (k[1].to_i-1).to_s,
                #    k[0] + (k[1].to_i-2).to_s],
                #    [(k[0].chr.ord+1).chr + (k[1].to_i-1).to_s,
                #    (k[0].chr.ord+1).chr + (k[1].to_i-2).to_s],
                #    [(k[0].chr.ord-1).chr + (k[1].to_i+1).to_s,
                #    (k[0].chr.ord-1).chr + (k[1].to_i+2).to_s],
                #    [k[0] + (k[1].to_i+1).to_s,
                #    k[0] + (k[1].to_i+2).to_s],
                #    [(k[0].chr.ord+1).chr + (k[1].to_i+1).to_s,
                #    (k[0].chr.ord+1).chr + (k[1].to_i+2).to_s],
                #  ]
                #
                #  pawn_positions.each do |pos|
                #    if ((pos[0][0].chr.ord) >= 'a'.chr.ord && (pos[0][0].chr.ord) <= 'h'.chr.ord && pos[0][1].to_i >= 1 && pos[0][1].to_i <= 8) && ((pos[1][0].chr.ord) >= 'a'.chr.ord && (pos[1][0].chr.ord) <= 'h'.chr.ord && pos[1][1].to_i >= 1 && pos[1][1].to_i <= 8)
                #      pawn_shield = pawn_shield && ((!@chessboard[pos[0]].nil? && @chessboard[pos[0]].class == Chess::Game::PAWN) || (!@chessboard[pos[1]].nil? && @chessboard[pos[1]].class == Chess::Game::PAWN))
                #    end
                #  end
                #
                #  valuation += PAWN_SHIELD_BONUS if pawn_shield
                #
                #end

              })

            end
          end
        end

        pieces_threads.each do |thread| thread.join end

        # Knights gain value the less pawn there are
        material += piece_number[Chess::Game::KNIGHT]*PIECE_VALUE[Chess::Game::KNIGHT]*(1 + (1 - (piece_number[Chess::Game::PAWN]/8)))*LESS_PAWN_KNIGHT_VALUE_BONUS

        # Rooks gain value the less pawn there are
        material += piece_number[Chess::Game::ROOK]*PIECE_VALUE[Chess::Game::ROOK]*(1 + (1 - (piece_number[Chess::Game::PAWN]/8)))*LESS_PAWN_ROOK_VALUE_BONUS

        # Bonus if we have a bishop pair
        valuation += BISHOP_PAIR_BONUS if piece_number[Chess::Game::BISHOP] == 2

        # Malus if we have a rook pair (?)
        valuation -= ROOK_PAIR_MALUS if piece_number[Chess::Game::ROOK] == 2

        # Malus if we have a knight pair (?)
        valuation -= KNIGHT_PAIR_MALUS if piece_number[Chess::Game::KNIGHT] == 2

        # Malus if we have no pawn
        valuation -= NO_PAWN_MALUS if piece_number[Chess::Game::PAWN] == 0

        # Malus if "if one side has no pawns left, it needs the equivalent of +4 pawns more material to win" no respected
        valuation -= INSUFFICIENT_MATERIAL_MALUS if piece_number[Chess::Game::PAWN] == 0 && material < 4*PIECE_VALUE[Chess::Game::PAWN]

        check_checkmate_thread.join
        (game[:valuation] = (valuation + material + opponent_checkmate + opponent_check + we_checkmate + we_check))

      else game[:valuation] end

    end

    ##
    #
    #  Give a JSON hash
    #
    #  @return String
    #  @scope public
    #

    def self.hash(game)
      JSON.fast_generate(game)
    end

    ##
    #
    #  Clone a game
    #
    #  @return Hash
    #  @scope public
    #

    def self.clone(game)

      chessboard = []
      for line in (0..7)
        chessboard[line] = []
        for col in (0..7)
          chessboard[line][col] = game[:chessboard][line][col]
        end
      end

      {
        :id => game[:id],
        :round => {
          :player => game[:round][:player],
          :number => game[:round][:number],
          :duration => game[:round][:duration],
          :end_time => game[:round][:end_time]
        },
        :chessboard => chessboard,
        :valuation => game[:valuation]
      }

    end

  end

end
