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
#include <curl/curl.h>

#include "./lib/curl/curl.h"
#include "./lib/cJSON/cJSON.h"

#include "./lib/game/game.h"
#include "./lib/output/output.h"
#include "./lib/player/player.h"

/** Constants */

#define API_HOST "127.0.0.1"
#define API_PORT "8080"

/** **/
int main(int argc, char* argv[]) {

  curl_global_init(CURL_GLOBAL_DEFAULT);

  unsigned char player, move[2][2]; char from[2], to[2]; game g; response r; bool was_my_round = true;
  char url[MAX_URL_LENGTH]; CURL* curl = curl_easy_init(); CURLcode res; /** curl */
  cJSON *json, *round, *chessboard, *line; /** cJSON */

  if (argc != 3) printf(ERROR("Usage: ./chess.rb [game-id] [white|black]"));
  else {

    player = (strcmp(argv[2], "white") == 0) ? WHITE:BLACK;

    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_handler);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);

    while(true) {

      init_response(&r);

      sprintf(url, "http://%s:%s/api/get/%d", API_HOST, API_PORT, atoi(argv[1]));
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

        for(signed char i = 0; i<8; i++) {

          line = cJSON_GetArrayItem(chessboard, i);
          for(signed char j = 0; j<8; j++) g.chessboard[i][j] = (cJSON_GetArrayItem(line, j)->type == cJSON_NULL) ? -1 : cJSON_GetArrayItem(line, j)->valueint;

        }

        if(was_my_round) printf(INFO("Game loaded."));

        if(g.round_player == player) {

          printf(SUCCESS("My round!"));
          ChessPlayer_Play(&g, player, move);

          sprintf(from, "%c%hhu", 97+move[0][1], move[0][0]+1);
          sprintf(to, "%c%hhu", 97+move[1][1], move[1][0]+1);

          /*sprintf(url, "http://%s:%s/api/move/%d/%s/%s", API_HOST, API_PORT, atoi(argv[1]), from, to);
          curl_easy_setopt(curl, CURLOPT_URL, url);

          res = curl_easy_perform(curl);

          if(res != CURLE_OK) printf(ERROR("HTTP Request failed (%s)"), curl_easy_strerror(res));
          else {*/

            printf(SUCCESS("%s --> %s"), from, to);
            exit(EXIT_SUCCESS);

          /*}*/

          was_my_round = true;

        } else if(was_my_round) {

          printf(WARNING("Not my round!"));
          was_my_round = false;

        }

      }

      sleep(1); // We don't want to burn the server

    }

  }

  curl_easy_cleanup(curl);
  curl_global_cleanup();

  return EXIT_SUCCESS;

}
