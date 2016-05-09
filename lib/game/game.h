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

#define POSSIBLE_MOVES_SIZE 128

  /** Pieces values */

  // OLD valuations
  // #define CENTIPAWN     1
  // #define BISHOP_VALUE  350
  // #define KING_VALUE    10000
  // #define KNIGHT_VALUE  350
  // #define PAWN_VALUE    100
  // #define QUEEN_VALUE   1000
  // #define ROOK_VALUE    500

  #define BISHOP_VALUES { 797, 824, 817, 808, 808, 817, 824, 797, \
                          814, 841, 834, 825, 825, 834, 841, 814, \
                          818, 845, 838, 829, 829, 838, 845, 818, \
                          824, 851, 844, 835, 835, 844, 851, 824, \
                          827, 854, 847, 838, 838, 847, 854, 827, \
                          826, 853, 846, 837, 837, 846, 853, 826, \
                          817, 844, 837, 828, 828, 837, 844, 817, \
                          792, 819, 812, 803, 803, 812, 819, 792 }

  #define KING_VALUES   { 60098, 60132, 60073, 60025, 60025, 60073, 60132, 60098, \
                          60119, 60153, 60094, 60046, 60046, 60094, 60153, 60119, \
                          60146, 60180, 60121, 60073, 60073, 60121, 60180, 60146, \
                          60173, 60207, 60148, 60100, 60100, 60148, 60207, 60173, \
                          60196, 60230, 60171, 60123, 60123, 60171, 60230, 60196, \
                          60224, 60258, 60199, 60151, 60151, 60199, 60258, 60224, \
                          60287, 60321, 60262, 60214, 60214, 60262, 60321, 60287, \
                          60298, 60332, 60273, 60225, 60225, 60273, 60332, 60298 }

  #define KNIGHT_VALUES { 627, 762, 786, 798, 798, 786, 762, 627, \
                          763, 798, 822, 834, 834, 822, 798, 763, \
                          817, 852, 876, 888, 888, 876, 852, 817, \
                          797, 832, 856, 868, 868, 856, 832, 797, \
                          799, 834, 858, 870, 870, 858, 834, 799, \
                          758, 793, 817, 829, 829, 817, 793, 758, \
                          739, 774, 798, 810, 810, 798, 774, 739, \
                          683, 718, 742, 754, 754, 742, 718, 683 }

  #define PAWN_VALUES   { 198, 198, 198, 198, 198, 198, 198, 198, \
                          178, 198, 198, 198, 198, 198, 198, 178, \
                          178, 198, 198, 198, 198, 198, 198, 178, \
                          178, 198, 208, 218, 218, 208, 198, 178, \
                          178, 198, 218, 238, 238, 218, 198, 178, \
                          178, 198, 208, 218, 218, 208, 198, 178, \
                          178, 198, 198, 198, 198, 198, 198, 178, \
                          198, 198, 198, 198, 198, 198, 198, 198 }

  #define QUEEN_VALUES  { 2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529, \
                          2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529, \
                          2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529, \
                          2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529, \
                          2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529, \
                          2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529, \
                          2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529, \
                          2529, 2529, 2529, 2529, 2529, 2529, 2529, 2529 }

  #define ROOK_VALUES   { 1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258, \
                          1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258, \
                          1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258, \
                          1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258, \
                          1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258, \
                          1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258, \
                          1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258, \
                          1258, 1263, 1268, 1272, 1272, 1268, 1263, 1258 }

  /** Bonuses */

  #define BONUS_BISHOP_PAIR       50
  #define BONUS_PAWN_SHIELD       200
  #define BONUS_CENTRAL_PAWN      50
  #define BONUS_LESS_PAWN_KNIGHT  175
  #define BONUS_LESS_PAWN_ROOK    250
  #define BONUS_MOBILITY          0.05

  /** Maluses */

  #define MALUS_ROOK_PAIR               50
  #define MALUS_KNIGHT_PAIR             50
  #define MALUS_RETURNING_BISHOP        50
  #define MALUS_NO_PAWN                 50
  #define MALUS_ROOK_PAWN               50
  #define MALUS_PIECE_IN_DANGER         3
  #define MALUS_INSUFFICIENT_MATERIAL   300

  /** Positions */

  #define CHECK     300
  #define CHECKMATE 99999

/** Types */

typedef struct {

  unsigned char id;

  unsigned char round_player;
  unsigned char round_duration;
  unsigned int round_end_time;

  signed char chessboard[8][8];

  signed char us;
  float valuation;

} game;

/** Functions headers */

unsigned char ChessGame_Ennemy(unsigned char player);
signed char ChessGame_Move(game *g, unsigned char from[2], unsigned char to[2], signed char replacement, bool* promotion);
bool ChessGame_FindPiece(game *g, signed char piece, unsigned char pos[2]);
void ChessGame_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length);
void ChessGame_RejectImpossibleMoves(game *g, unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2][2], unsigned char *possible_moves_length);
bool ChessGame_InDanger(game *g, unsigned char pos[2]);
bool ChessGame_Check(game *g, unsigned char player);
bool ChessGame_Checkmate(game *g, unsigned char player);
bool ChessGame_PAT(game *g, unsigned char player);
bool ChessGame_Draw(game *g, unsigned char player);
int ChessGame_PieceValue(signed char piece, unsigned char line, unsigned char row);
float ChessGame_Material(game *g, unsigned char player);
float ChessGame_Valuation(game *g, unsigned char player);

#endif
