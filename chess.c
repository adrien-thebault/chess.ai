/**
*
*  Artificial intelligence for https://src.adrien-thebault.fr/adrien-thebault/chess
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <unistd.h>
#include <time.h>
#include <pthread.h>
#include <signal.h>
#include <curl/curl.h>

#include "./chess.h"

#include "./lib/curl/curl.h"
#include "./lib/cJSON/cJSON.h"

#include "./lib/watchdog/watchdog.h"
#include "./lib/game/game.h"
#include "./lib/output/output.h"
#include "./lib/player/player.h"

/** **/

int main(int argc, char* argv[]) {

  curl_global_init(CURL_GLOBAL_DEFAULT);

  unsigned char game_id, player, algorithm; char from[2], to[2]; game g; response r; bool was_my_round = true;
  char url[MAX_URL_LENGTH]; CURL* curl = curl_easy_init(); CURLcode res; /** curl */
  cJSON *json, *round, *chessboard, *line; /** cJSON */
  pthread_t watchdog_thread;

  if (argc != 7) printf(ERROR("Usage: ./chess.rb --game [game-id] --player [white|black] --algorithm [alphabeta|mtdf|pvs]"));
  else {

    game_id = atoi(argv[2]);
    player = (strcmp(argv[4], "white") == 0) ? WHITE:BLACK;

    if(strcmp(argv[6], "alphabeta") == 0) algorithm = ALPHABETA;
    else if(strcmp(argv[6], "mtdf") == 0) algorithm = MTDF;
    else if(strcmp(argv[6], "pvs") == 0) algorithm = PVS;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_handler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);

    while(true) {

      init_response(&r);

      sprintf(url, "http://%s:%s/api/get/%d", API_HOST, API_PORT, game_id);
      curl_easy_setopt(curl, CURLOPT_URL, url);

      res = curl_easy_perform(curl);

      if(res != CURLE_OK) printf(ERROR("HTTP Request failed (%s)"), curl_easy_strerror(res));
      else {

        json = cJSON_Parse(r.ptr);
        round = cJSON_GetObjectItem(json, "round");
        chessboard = cJSON_GetObjectItem(json, "chessboard");

        g.id = cJSON_GetObjectItem(json, "id")->valueint;
        g.round_player = cJSON_GetObjectItem(round, "player")->valueint;
        g.round_duration = cJSON_GetObjectItem(round, "duration")->valueint;
        g.round_end_time = (cJSON_GetObjectItem(round, "end_time")->valueint == -1) ? (time(NULL)+g.round_duration) : cJSON_GetObjectItem(round, "end_time")->valueint;
        g.valuation = -1;
        g.us = player;

        for(signed char i = 0; i<8; i++) {

          line = cJSON_GetArrayItem(chessboard, i);
          for(signed char j = 0; j<8; j++) g.chessboard[i][j] = (cJSON_GetArrayItem(line, j)->type == cJSON_NULL) ? -1 : cJSON_GetArrayItem(line, j)->valueint;

        }

        if(was_my_round) printf(INFO("Game loaded."));

        if(ChessGame_Checkmate(&g, g.round_player) || ChessGame_Draw(&g, g.round_player)) break;
        else if(g.round_player == player) {

          printf(SUCCESS("My round!"));

          pthread_create(&watchdog_thread, NULL, watchdog, (void*)&g);
          ChessPlayer_Play(&g, player, g.move, algorithm);
          pthread_join(watchdog_thread, NULL);

          sprintf(from, "%c%hhu", 97+g.move[0][1], g.move[0][0]+1);
          sprintf(to, "%c%hhu", 97+g.move[1][1], g.move[1][0]+1);

          if((g.chessboard[g.move[0][0]][g.move[0][1]] & MASK_PIECE) == PAWN && ((player == WHITE && g.move[1][0] == 7) || (player == BLACK && g.move[1][0] == 0))) sprintf(url, "http://%s:%s/api/move/%d/%s/%s=queen", API_HOST, API_PORT, game_id, from, to);
          else sprintf(url, "http://%s:%s/api/move/%d/%s/%s", API_HOST, API_PORT, game_id, from, to);

          curl_easy_setopt(curl, CURLOPT_URL, url);
          res = curl_easy_perform(curl);

          if(res != CURLE_OK) printf(ERROR("HTTP Request failed (%s)"), curl_easy_strerror(res));
          else printf(SUCCESS("%s --> %s"), from, to);

          was_my_round = true;

        } else if(was_my_round) {

          printf(WARNING("Not my round!"));
          was_my_round = false;

        }

      }

      free(json);
      free(round);
      free(chessboard);
      free(line);

      sleep(1); // We don't want to burn the server

    }

  }

  if(ChessGame_Checkmate(&g, g.round_player)) {

    printf(INFO("Checkmate!"));

    if(g.round_player == player) printf(ERROR("I lost, I'm so sorry..."));
    else printf(SUCCESS("I win! I'm the best ahahah"));

  } else if(ChessGame_Draw(&g, g.round_player)) printf(WARNING("Nobody wins, nobody loses, what a shitty game!"));

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  return EXIT_SUCCESS;

}
