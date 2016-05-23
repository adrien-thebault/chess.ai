/**
*
*  Bishop-specific functions
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdbool.h>
#include <string.h>
#include "../game.h"

/**
*
*  Computes every possible move for a bishop at pos and puts result in possible_moves
*
*/

void ChessGame_Bishop_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length) {

  unsigned char color = g->chessboard[pos[0]][pos[1]] & MASK_PLAYER;
  unsigned char ennemy = ChessGame_Ennemy(color);

  signed char line = pos[0], col_left = 0, col_right = 0, orientation = -1;
  bool finished_left = false, finished_right = false;

  while(!(finished_left && finished_right && orientation == 1)) {

    if(line > 7 || line < 0) {
      finished_left = true;
      finished_right = true;
    } else {

      if(line > pos[0]) {
        col_right = pos[1] + (line - pos[0]);
        col_left = pos[1] - (line - pos[0]);
      } else {
        col_right = pos[1] + (pos[0] - line);
        col_left = pos[1] - (pos[0] - line);
      }

      if(col_right > 7) finished_right = true;
      if(col_left < 0) finished_left = true;

    }

    if(!finished_left) {

      if(g->chessboard[line][col_left] == -1 || (g->chessboard[line][col_left] & MASK_PLAYER) == ennemy) {

        memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
        possible_moves[*possible_moves_length][1][0] = line;
        possible_moves[*possible_moves_length][1][1] = col_left;

        (*possible_moves_length)++;

      }

      if ((g->chessboard[line][col_left] != -1 && line != pos[0]) || col_left == 0) finished_left = true;

    }

    if(!finished_right) {

      if(g->chessboard[line][col_right] == -1 || (g->chessboard[line][col_right] & MASK_PLAYER) == ennemy) {

        memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
        possible_moves[*possible_moves_length][1][0] = line;
        possible_moves[*possible_moves_length][1][1] = col_right;

        (*possible_moves_length)++;

      }

      if ((g->chessboard[line][col_right] != -1 && line != pos[0]) || col_right == 7) finished_right = true;

    }

    if((finished_left && finished_right && line <= pos[0]) || (line == 0 && orientation == -1)) {

      line = pos[0];
      finished_left = false;
      finished_right = false;
      orientation = 1;

    }

    line += orientation;

  }

}
