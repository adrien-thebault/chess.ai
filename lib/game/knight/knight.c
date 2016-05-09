/**
*
*  Knight-specific functions
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdbool.h>
#include <string.h>
#include "../game.h"

/**
*
*  Computes every possible move for a Knight at pos and puts result in possible_moves
*
*/

void ChessGame_Knight_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length) {

  signed char offset_line[8] = {1,1,-1,-1,2,-2,2,-2};
  signed char offset_col[8] = {2,-2,2,-2,1,1,-1,-1};

  signed char line, col, player = g->chessboard[pos[0]][pos[1]] & MASK_PLAYER;

  for(int i = 0; i<8; i++) {

    line = pos[0]+offset_line[i];
    col = pos[1]+offset_col[i];

    if(line > -1 && line < 8 && col > -1 && col < 8 && (g->chessboard[line][col] == -1 || (g->chessboard[line][col] & MASK_PLAYER) != player)) {

      memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
      possible_moves[*possible_moves_length][1][0] = line;
      possible_moves[*possible_moves_length][1][1] = col;

      (*possible_moves_length)++;

    }

  }

}
