/**
*
*  ChessGame functions headers && types definitions
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef GAME_H
#define GAME_H

/** Constants */

#define WHITE    0x0
#define BLACK    0x8

#define BISHOP   0x0
#define KING     0x1
#define KNIGHT   0x2
#define PAWN     0x3
#define QUEEN    0x4
#define ROOK     0x5

#define MASK_PLAYER 0x8
#define MASK_PIECE  0x7

#define POSSIBLE_MOVES_SIZE 64

  /** Pieces values */

  #define CENTIPAWN     0.01
  #define BISHOP_VALUE  CENTIPAWN*350
  #define KING_VALUE    CENTIPAWN*0
  #define KNIGHT_VALUE  CENTIPAWN*350
  #define PAWN_VALUE    CENTIPAWN*100
  #define QUEEN_VALUE   CENTIPAWN*1000
  #define ROOK_VALUE    CENTIPAWN*100

  /** Bonuses */

  #define BONUS_BISHOP_PAIR       PAWN_VALUE*0.5
  #define BONUS_PAWN_SHIELD       PAWN_VALUE*2
  #define BONUS_CENTRAL_PAWN      PAWN_VALUE*0.5
  #define BONUS_LESS_PAWN_KNIGHT  KNIGHT_VALUE*0.5
  #define BONUS_LESS_PAWN_ROOK    ROOK_VALUE*0.5
  #define BONUS_MOBILITY          0.05

  /** Maluses */

  #define MALUS_ROOK_PAIR               PAWN_VALUE*0.5
  #define MALUS_KNIGHT_PAIR             PAWN_VALUE*0.5
  #define MALUS_RETURNING_BISHOP        PAWN_VALUE*0.5
  #define MALUS_NO_PAWN                 PAWN_VALUE*0.5
  #define MALUS_ROOK_PAWN               PAWN_VALUE*0.5
  #define MALUS_PIECE_IN_DANGER         3
  #define MALUS_INSUFFICIENT_MATERIAL   PAWN_VALUE*1000

  /** Positions */

  #define CHECK     CENTIPAWN*300
  #define CHECKMATE CENTIPAWN*99999

/** Types */

typedef struct {

  unsigned char id;

  unsigned char round_player;
  unsigned char round_duration;
  unsigned int round_end_time;

  signed char chessboard[8][8];

} game;

/** Functions headers */

unsigned char ChessGame_Ennemy(unsigned char player);
void ChessGame_Move(game *g, unsigned char from[2], unsigned char to[2]);
void ChessGame_FindPiece(game *g, signed char piece, unsigned char pos[2]);
void ChessGame_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length);
void ChessGame_RejectImpossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length);
bool ChessGame_InDanger(game *g, unsigned char pos[2]);
bool ChessGame_Check(game *g, unsigned char player);
bool ChessGame_Checkmate(game *g, unsigned char player);
bool ChessGame_PAT(game *g, unsigned char player);
bool ChessGame_Draw(game *g, unsigned char player);
float ChessGame_Valuation(game *g, unsigned char player);

#endif
