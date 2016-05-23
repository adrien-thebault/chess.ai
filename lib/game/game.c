/**
*
*  ChessGame : chess game representation & functions
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>

#include "./game.h"
#include "./bishop/bishop.h"
#include "./king/king.h"
#include "./knight/knight.h"
#include "./pawn/pawn.h"
#include "./queen/queen.h"
#include "./rook/rook.h"

/**
*
*  Returns the ennemy of player
*
*/

unsigned char ChessGame_Ennemy(unsigned char player) {
  return ~player & MASK_PLAYER;
}


/**
*
*  Applies move to game
*
*/

signed char ChessGame_Move(game *g, unsigned char from[2], unsigned char to[2], signed char replacement, bool* promotion) {

  signed char piece = g->chessboard[to[0]][to[1]], multiplier = ((g->chessboard[from[0]][from[1]] & MASK_PLAYER) == g->us) ? 1 : -1;

  if((g->chessboard[from[0]][from[1]] & MASK_PIECE) == PAWN && (((g->chessboard[from[0]][from[1]] & MASK_PLAYER) == WHITE && to[0] == 7) || ((g->chessboard[from[0]][from[1]] & MASK_PLAYER) == BLACK && to[0] == 0))) {

    g->chessboard[to[0]][to[1]] = QUEEN | (g->chessboard[from[0]][from[1]] & MASK_PLAYER);
    *promotion = true;

    /** update game valuation according to current move */
    g->valuation += multiplier * (ChessGame_PieceValue(g->chessboard[to[0]][to[1]], to[0], to[1]) - ChessGame_PieceValue(g->chessboard[from[0]][from[1]], from[0], from[1]));

  } else if(*promotion) {

    g->chessboard[to[0]][to[1]] = PAWN | (g->chessboard[from[0]][from[1]] & MASK_PLAYER);

    /** update game valuation according to current move */
    g->valuation += multiplier * (ChessGame_PieceValue(g->chessboard[to[0]][to[1]], to[0], to[1]) - ChessGame_PieceValue(g->chessboard[from[0]][from[1]], from[0], from[1]));

  } else {

    g->chessboard[to[0]][to[1]] = g->chessboard[from[0]][from[1]];

    /** update game valuation according to current move */
    g->valuation += multiplier * (ChessGame_PieceValue(g->chessboard[from[0]][from[1]], to[0], to[1]) - ChessGame_PieceValue(g->chessboard[from[0]][from[1]], from[0], from[1]));

  }

  g->chessboard[from[0]][from[1]] = replacement;

  /** update game valuation according to current move */
  if(piece != -1) g->valuation += multiplier * ChessGame_PieceValue(piece, to[0], to[1]);
  if(replacement != -1) g->valuation -= multiplier * ChessGame_PieceValue(replacement, from[0], from[1]);

  g->round_player = ChessGame_Ennemy(g->round_player);
  return piece;

}

/**
*
*  Find piece in game and puts result in pos
*
*/

bool ChessGame_FindPiece(game *g, signed char piece, unsigned char pos[2]) {

  bool found = false;

  for(unsigned char i = 0; i<64; i++) {
    if(g->chessboard[i/8][i%8] == piece) {

      pos[0] = i/8;
      pos[1] = i%8;

      found = true;
      break;

    }
  }

  return found;

}

/**
*
*  Computes possible moves starting from pos and puts result in possible_moves
*
*/

void ChessGame_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length) {

  signed char piece = g->chessboard[pos[0]][pos[1]] & MASK_PIECE;

  if(piece == BISHOP) ChessGame_Bishop_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == KING) ChessGame_King_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == KNIGHT) ChessGame_Knight_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == PAWN) ChessGame_Pawn_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == QUEEN) ChessGame_Queen_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == ROOK) ChessGame_Rook_PossibleMoves(g, pos, possible_moves, possible_moves_length);

}

/**
*
*  Reject all impossible moves (ie. the ones which would put the king in check)
*
*/

void ChessGame_RejectImpossibleMoves(game *g, unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length) {
  if(possible_moves_length > 0) {

    signed char piece; bool promotion;
    unsigned char tmp[POSSIBLE_MOVES_SIZE][2][2], tmp_size = *possible_moves_length, player = g->round_player;

    memcpy(tmp, possible_moves, POSSIBLE_MOVES_SIZE*4*sizeof(unsigned char));
    *possible_moves_length = 0;

    for(unsigned char i = 0; i<tmp_size; i++) {

      promotion = false;
      piece = ChessGame_Move(g, tmp[i][0], tmp[i][1], -1, &promotion);

      if(ChessGame_Check(g, player) == false) {

        memcpy(possible_moves[*possible_moves_length], tmp[i], 4*sizeof(unsigned char));
        (*possible_moves_length)++;

      }

      ChessGame_Move(g, tmp[i][1], tmp[i][0], piece, &promotion);

    }

  }
}

/**
*
*  Checks if position is in danger
*
*/

bool ChessGame_InDanger(game *g, unsigned char pos[2]) {

  bool in_danger = false;
  unsigned char row[2], possible_moves[POSSIBLE_MOVES_SIZE][2][2], possible_moves_length = 0, piece_color = g->chessboard[pos[0]][pos[1]] & MASK_PLAYER;

  for(unsigned char i = 0; i<64; i++) {

    row[0] = i/8; row[1] = i%8;
    if(g->chessboard[row[0]][row[1]] != -1 && (g->chessboard[row[0]][row[1]] & MASK_PLAYER) != piece_color) {

      possible_moves_length = 0;
      ChessGame_PossibleMoves(g, row, possible_moves, &possible_moves_length);

      for(unsigned char x = 0; x<possible_moves_length; x++) {
        if(memcmp(&(possible_moves[x][1]), pos, 2*sizeof(unsigned char)) == 0) {
          in_danger = true;
          break;
        }
      }

      if(in_danger) break;
      possible_moves_length = 0;

    }

  }

  return in_danger;

}

/**
*
*  Is player in check?
*
*/

bool ChessGame_Check(game *g, unsigned char player) {

  unsigned char pos[2];

  if(ChessGame_FindPiece(g, player|KING, pos)) return ChessGame_InDanger(g, pos);
  else return true;

}

/**
*
*  Is player mate ?
*
*/

bool ChessGame_Mate(game *g, unsigned char player) {

  unsigned char row[2], possible_moves[POSSIBLE_MOVES_SIZE][2][2], possible_moves_length, n = 0;
  signed char piece; bool promotion;

  for(unsigned char i = 0; i<64; i++) {

    row[0] = i/8; row[1] = i%8;
    if(g->chessboard[row[0]][row[1]] != -1 && (g->chessboard[row[0]][row[1]] & MASK_PLAYER) == player) {

      ChessGame_PossibleMoves(g, row, possible_moves, &possible_moves_length);
      for(int j = 0; j < possible_moves_length; j++) {

        promotion = false;
        piece = ChessGame_Move(g, possible_moves[j][0], possible_moves[j][1], -1, &promotion);

        if(!ChessGame_Check(g, player)) n++;
        ChessGame_Move(g, possible_moves[j][1], possible_moves[j][0], piece, &promotion);

        if(n > 0) break;

      }

      if(n > 0) break;

    }

  }

  return (n == 0);

}

/**
*
*  Is player checkmate?
*
*/

bool ChessGame_Checkmate(game *g, unsigned char player) {

  return ChessGame_Check(g, player) && ChessGame_Mate(g, player);

}


/**
*
*  Is player PAT?
*
*/

bool ChessGame_PAT(game *g, unsigned char player) {

  return !ChessGame_Check(g, player) && ChessGame_Mate(g, player);
  
}

/**
*
*  Is game in draw situation?
*
*/

bool ChessGame_Draw(game *g, unsigned char player) {
  return ChessGame_PAT(g, player);
}

/**
*
*  Give the piece value of piece at pos line,col
*
*/

int ChessGame_PieceValue(signed char piece, unsigned char line, unsigned char col) {

  static const int piece_value[6][64] = {BISHOP_VALUES, KING_VALUES, KNIGHT_VALUES, PAWN_VALUES, QUEEN_VALUES, ROOK_VALUES};

  signed char row = (line * 8) + col;
  if((piece & MASK_PLAYER) == WHITE) row = 63 - row;

  return piece_value[piece & MASK_PIECE][row];

}

/**
*
*  Give the material valuation of player on game
*
*/

float ChessGame_Material(game *g, unsigned char player) {

  signed char piece; float material = 0;

  for(unsigned char i = 0, line = 0, col = 0; i<64; i++) {

    line = i/8; col = i%8;
    piece = g->chessboard[line][col];

    if(piece != -1 && (piece & MASK_PLAYER) == player) material += ChessGame_PieceValue(piece, line, col);

  }

  return material;

}

/**
*
*  Give the material balance for player p
*
**/

float ChessGame_MaterialBalance(game *g, unsigned char player) {

  signed char piece; float balance = 0;

  for(unsigned char i = 0, line = 0, col = 0; i<64; i++) {

    line = i/8; col = i%8;
    piece = g->chessboard[line][col];

    if(piece != -1) {

      if((piece & MASK_PLAYER) == player) balance += ChessGame_PieceValue(piece, line, col);
      else balance -= ChessGame_PieceValue(piece, line, col);

    }

  }

  return balance;

}

/**
*
*  Evaluation of a game for player p
*
*/

float ChessGame_Valuation(game *g, unsigned char player) {

  //return ChessGame_MaterialBalance(g, player);

  if(g->valuation == -1) g->valuation = ChessGame_MaterialBalance(g, player);
  return g->valuation;

}
