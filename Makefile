all:
	gcc ./chess.c ./lib/curl/curl.c ./lib/cJSON/cJSON.c ./lib/game/game.c ./lib/game/bishop/bishop.c ./lib/game/king/king.c ./lib/game/knight/knight.c ./lib/game/pawn/pawn.c ./lib/game/rook/rook.c ./lib/game/queen/queen.c ./lib/player/player.c -o chess -Wall -std=c99 -lcurl -lm -g

clean:
	rm ./chess
