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

void ChessGame_Move(game *g, unsigned char from[2], unsigned char to[2]) {

  g->chessboard[to[0]][to[1]] = g->chessboard[from[0]][from[1]];
  g->chessboard[from[0]][from[1]] = -1;

  g->round_player = ChessGame_Ennemy(g->round_player);
  g->round_end_time = time(NULL) + g->round_duration;

}

/**
*
*  Find piece in game and puts result in pos
*
*/

void ChessGame_FindPiece(game *g, signed char piece, unsigned char pos[2]) {
  for(unsigned char i = 0; i<64; i++) {
    if(g->chessboard[i/8][i%8] == piece) {

      pos[0] = i/8;
      pos[1] = i%8;

      break;

    }
  }
}

/**
*
*  Computes possible moves starting from pos and puts result in possible_moves
*
*/

void ChessGame_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length) {

  signed char piece = g->chessboard[pos[0]][pos[1]] & MASK_PIECE;

  if(piece == BISHOP) ChessGame_Bishop_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == KING) ChessGame_King_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == KNIGHT) ChessGame_Knight_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == PAWN) ChessGame_Pawn_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == QUEEN) ChessGame_Queen_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else if(piece == ROOK) ChessGame_Rook_PossibleMoves(g, pos, possible_moves, possible_moves_length);
  else *possible_moves_length = 0;

}

/**
*
*  Reject all impossible moves (ie. the ones which would put the king in check)
*
*/

void ChessGame_RejectImpossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length) {
  if(possible_moves_length != 0) {

    unsigned char tmp[POSSIBLE_MOVES_SIZE][2], tmp_size = *possible_moves_length, player = g->chessboard[pos[0]][pos[1]] & MASK_PLAYER;
    memcpy(tmp, possible_moves, *possible_moves_length*2*sizeof(unsigned char));
    *possible_moves_length = 0;

    game *clone = malloc(sizeof(game));

    for(unsigned char i = 0; i<tmp_size; i++) {

      memcpy(clone, g, sizeof(game));
      ChessGame_Move(clone, pos, tmp[i]);

      if(ChessGame_Check(clone, player) == false) {

        memcpy(possible_moves[*possible_moves_length], tmp[i], 2*sizeof(unsigned char));
        (*possible_moves_length)++;

      }

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
  unsigned char row[2], possible_moves[POSSIBLE_MOVES_SIZE][2], possible_moves_length = 0, piece_color = g->chessboard[pos[0]][pos[1]] & MASK_PLAYER;

  for(unsigned char i = 0; i<64; i++) {

    row[0] = i/8; row[1] = i%8;
    if(g->chessboard[row[0]][row[1]] != -1 && (g->chessboard[row[0]][row[1]] & MASK_PLAYER) != piece_color) {

      ChessGame_PossibleMoves(g, row, possible_moves, &possible_moves_length);
      for(unsigned char x = 0; x<possible_moves_length; x++) {
        if(memcmp(possible_moves[x], pos, sizeof(possible_moves[x])) == 0) {
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
  ChessGame_FindPiece(g, player|KING, pos);

  return ChessGame_InDanger(g, pos);

}

/**
*
*  Is player checkmate?
*
*/

bool ChessGame_Checkmate(game *g, unsigned char player) {

  if(ChessGame_Check(g, player)) {

    unsigned char row[2], possible_moves[POSSIBLE_MOVES_SIZE][2], possible_moves_length;
    bool no_possible_moves = true;

    for(unsigned char i = 0; i<64; i++) {

      row[0] = i/8; row[1] = i%8;
      if(g->chessboard[row[0]][row[1]] != -1 && (g->chessboard[row[0]][row[1]] & MASK_PLAYER) == player) {

        ChessGame_PossibleMoves(g, row, possible_moves, &possible_moves_length);
        if(possible_moves_length > 0) {

          no_possible_moves = false;
          break;

        }

      }

    }

    return no_possible_moves;

  } else return false;

}


/**
*
*  Is player PAT?
*
*/

bool ChessGame_PAT(game *g, unsigned char player) {

  if(!ChessGame_Check(g, player)) {

    unsigned char row[2], possible_moves[POSSIBLE_MOVES_SIZE][2], possible_moves_length;
    bool no_possible_moves = true;

    for(unsigned char i = 0; i<64; i++) {

      row[0] = i/8; row[1] = i%8;
      if(g->chessboard[row[0]][row[1]] != -1 && (g->chessboard[row[0]][row[1]] & MASK_PLAYER) == player) {

        ChessGame_PossibleMoves(g, row, possible_moves, &possible_moves_length);
        if(possible_moves_length > 0) {

          no_possible_moves = false;
          break;

        }

      }

    }

    return no_possible_moves;

  } else return false;

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
*  Evaluation of a game for player p
*
*/

float ChessGame_Valuation(game *g, unsigned char player) {

  static const unsigned char piece_value[6] = {BISHOP_VALUE, KING_VALUE, KNIGHT_VALUE, PAWN_VALUE, QUEEN_VALUE, ROOK_VALUE};
  unsigned char piece_number[6] = {0,0,0,0,0,0};

  float valuation = 0;
  unsigned char ennemy = ChessGame_Ennemy(player), row[2], possible_moves[POSSIBLE_MOVES_SIZE][2], possible_moves_length, material;
  signed char piece;

  /** Check/Checkmate Bonus/Malus */

  /*if(ChessGame_Check(g, player)) {

    if(ChessGame_Checkmate(g, player)) valuation -= CHECKMATE;
    valuation -= CHECK;

  }

  if(ChessGame_Check(g, ennemy)) {

    if(ChessGame_Checkmate(g, ennemy)) valuation += CHECKMATE;
    valuation += CHECK;

  }*/

  for(unsigned char i = 0, line = i/8, col = i%8; i<64; i++) {

    piece = g->chessboard[line][col];
    if(piece != -1 && (piece & MASK_PLAYER) == player) {

      row[0] = line; row[1] = col;

      piece &= MASK_PIECE;
      piece_number[piece]++;

      /** material value */
      material += piece_value[piece];

      /** Bonuses */

      if(piece != PAWN) {

        possible_moves_length = 0;
        /*ChessGame_PossibleMoves(g, row, possible_moves, &possible_moves_length);*/

        valuation += BONUS_MOBILITY*possible_moves_length*piece_value[piece];

      } else if(col == 3 || col == 4) valuation += BONUS_CENTRAL_PAWN;

      /** Maluses */

      if(ChessGame_InDanger(g, row)) valuation -= MALUS_PIECE_IN_DANGER*piece_value[piece];

      if(piece == PAWN && (col == 0 || col == 7)) valuation -= MALUS_ROOK_PAWN;
      else if(piece == BISHOP && ((player == WHITE && line == 0) || (player == BLACK && line == 7)) && (col == 2 || col == 5)) valuation -= MALUS_RETURNING_BISHOP;

    }
  }

  /** Bonuses */

  if(piece_number[BISHOP] == 2) valuation += BONUS_BISHOP_PAIR;
  valuation = BONUS_LESS_PAWN_KNIGHT*piece_number[KNIGHT] * (1 + (1 - (piece_number[PAWN]/8)));
  valuation = BONUS_LESS_PAWN_ROOK*piece_number[KNIGHT] * (1 + (1 - (piece_number[PAWN]/8)));


  /** Maluses */

  if(piece_number[KNIGHT] == 2) valuation -= MALUS_KNIGHT_PAIR;
  if(piece_number[ROOK] == 2) valuation -= MALUS_ROOK_PAIR;
  if(piece_number[PAWN] == 0) {

    valuation -= MALUS_NO_PAWN;
    if(material <= 4*piece_value[PAWN]) valuation -= MALUS_INSUFFICIENT_MATERIAL;

  }

  return valuation+material;

}
