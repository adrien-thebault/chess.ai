/**
*
*  Pawn function headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef PAWN_H
#define PAWN_H

void ChessGame_Pawn_PossibleMoves(game *g, unsigned char pos[2], unsigned char possible_moves[POSSIBLE_MOVES_SIZE][2], unsigned char *possible_moves_length);

#endif
