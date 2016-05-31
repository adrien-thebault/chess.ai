/**
*
*  ChessGame functions headers && types definitions
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

#include "./../../chess.h"

#include "./../curl/curl.h"
#include "./../cJSON/cJSON.h"

#include "./../game/game.h"
#include "./../output/output.h"
#include "./../player/player.h"

/**
*
*  Force the program to send the mvmt before the end of the round
*
*/

void* watchdog(void* current) {

  bool abort = false;
  unsigned char m[2][2];

  response r; game g = *(game*)current;
  char url[MAX_URL_LENGTH]; CURL* curl = curl_easy_init(); CURLcode res;

  curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, response_handler);
  curl_easy_setopt(curl, CURLOPT_WRITEDATA, &r);

  while(true) {

    init_response(&r);

    sprintf(url, "http://%s:%s/api/get/%d", API_HOST, API_PORT, g.id);
    curl_easy_setopt(curl, CURLOPT_URL, url);

    res = curl_easy_perform(curl);

    if(res != CURLE_OK) continue;
    else {

      g.round_player = cJSON_GetObjectItem(cJSON_GetObjectItem(cJSON_Parse(r.ptr), "round"), "player")->valueint;

      if(g.round_player != g.us) break;
      else if(g.round_end_time - time(NULL) < 6) {

        abort = true;
        break;

      } else sleep(1);

    }

  }

  if(abort) {

    ChessPlayer_Stop(m);
    sleep(1);

    printf(WARNING("|"));
    printf(WARNING("|\tNo time left, sending last best move."));
    printf(WARNING("|\tMove : %c%hhu -> %c%hhu"), m[0][1]+97, m[0][0]+1, m[1][1]+97, m[1][0]+1);
    printf(WARNING("|"));

    memcpy(((game*)current)->move, m, 4*sizeof(unsigned char));

  }

  pthread_exit(NULL);

}
