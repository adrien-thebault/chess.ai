/**
*
*  Queen-specific functions
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdbool.h>
#include <string.h>

#include "../game.h"
#include "../bishop/bishop.h"
#include "../rook/rook.h"

/**
*
*  Computes every possible move for a Queen at pos and puts result in possible_moves
*
*/

void ChessGame_Queen_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length) {

  ChessGame_Bishop_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  ChessGame_Rook_PossibleMoves(g, pos, possible_moves, possible_moves_length);

}
