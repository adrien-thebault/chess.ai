/**
*
*  Rook-specific functions
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <string.h>
#include <stdbool.h>
#include "../game.h"

/**
*
*  Computes every possible move for a Rook at pos and puts result in possible_moves
*
*/

void ChessGame_Rook_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length) {

  unsigned char color = g->chessboard[pos[0]][pos[1]] & MASK_PLAYER;
  unsigned char ennemy = ChessGame_Ennemy(color), offset = 0, line = 0, col = 0;

  bool finished_top = false, finished_bottom = false, finished_left = false, finished_right = false;
  while(!(finished_top && finished_bottom && finished_left && finished_right)) {

    if(!finished_top) {

      line = pos[0]+offset; col = pos[1];
      if((line > -1 && line < 8 && col > -1 && col < 8) && (g->chessboard[line][col] == -1 || (g->chessboard[line][col] & MASK_PLAYER) == ennemy)) {

        memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
        possible_moves[*possible_moves_length][1][0] = line;
        possible_moves[*possible_moves_length][1][1] = col;

        (*possible_moves_length)++;

      }

      if((g->chessboard[line][col] != -1 && offset > 0) || line == 7) finished_top = true;

    }

    if(!finished_bottom) {

      line = pos[0]-offset; col = pos[1];
      if((line > -1 && line < 8 && col > -1 && col < 8) && (g->chessboard[line][col] == -1 || (g->chessboard[line][col] & MASK_PLAYER) == ennemy)) {

        memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
        possible_moves[*possible_moves_length][1][0] = line;
        possible_moves[*possible_moves_length][1][1] = col;

        (*possible_moves_length)++;

      }

      if((g->chessboard[line][col] != -1 && offset > 0) || line == 0) finished_bottom = true;

    }

    if(!finished_left) {

      line = pos[0]; col = pos[1]-offset;
      if((line > -1 && line < 8 && col > -1 && col < 8) && (g->chessboard[line][col] == -1 || (g->chessboard[line][col] & MASK_PLAYER) == ennemy)) {

        memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
        possible_moves[*possible_moves_length][1][0] = line;
        possible_moves[*possible_moves_length][1][1] = col;

        (*possible_moves_length)++;

      }

      if((g->chessboard[line][col] != -1 && offset > 0) || col == 0) finished_left = true;

    }

    if(!finished_right) {

      line = pos[0]; col = pos[1]+offset;
      if((line > -1 && line < 8 && col > -1 && col < 8) && (g->chessboard[line][col] == -1 || (g->chessboard[line][col] & MASK_PLAYER) == ennemy)) {

        memcpy(possible_moves[*possible_moves_length][0], pos, 2*sizeof(unsigned char));
        possible_moves[*possible_moves_length][1][0] = line;
        possible_moves[*possible_moves_length][1][1] = col;

        (*possible_moves_length)++;

      }

      if((g->chessboard[line][col] != -1 && offset > 0) || col == 7) finished_right = true;

    }

    offset++;

  }

}
