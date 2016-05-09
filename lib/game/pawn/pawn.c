/**
*
*  Pawn-specific functions
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdbool.h>
#include <string.h>
#include "../game.h"

/**
*
*  Computes every possible move for a Pawn at pos and puts result in possible_moves
*
*/

void ChessGame_Pawn_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length) {

  unsigned char player = g->chessboard[pos[0]][pos[1]] & MASK_PLAYER;
  signed char forward = (player == WHITE) ? 1 : -1;

  signed char line = pos[0]+forward, col = pos[1];
  if(line > -1 && line < 8 && g->chessboard[line][col] == -1) {

    memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
    possible_moves[*possible_moves_length][1][0] = line;
    possible_moves[*possible_moves_length][1][1] = col;

    (*possible_moves_length)++;

  }

  col = pos[1]+1;
  if(line > -1 && line < 8 && col > -1 && col < 8 && g->chessboard[line][col] != -1 && (g->chessboard[line][col] & MASK_PLAYER) != player) {

    memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
    possible_moves[*possible_moves_length][1][0] = line;
    possible_moves[*possible_moves_length][1][1] = col;

    (*possible_moves_length)++;

  }

  col = pos[1]-1;
  if(line > -1 && line < 8 && col > -1 && col < 8 && g->chessboard[line][col] != -1 && (g->chessboard[line][col] & MASK_PLAYER) != player) {

    memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
    possible_moves[*possible_moves_length][1][0] = line;
    possible_moves[*possible_moves_length][1][1] = col;

    (*possible_moves_length)++;

  }

  line = pos[0]+2*forward; col = pos[1];
  if(((forward == 1 && pos[0] == 1) || (forward == -1 && pos[0] == 6)) && g->chessboard[line-forward][col] == -1 && g->chessboard[line][col] == -1) {

    memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
    possible_moves[*possible_moves_length][1][0] = line;
    possible_moves[*possible_moves_length][1][1] = col;

    (*possible_moves_length)++;

  }


}
