/**
*
*  cURL response handlers headers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

#ifndef CURL_H
#define CURL_H

/** Constants */

#define MAX_URL_LENGTH 256
#define MAX_RESPONSE_SIZE 1024

/** Types **/

typedef struct {
  char *ptr;
  size_t len;
} response;

/** Function headers */

void init_response(response *r);
size_t response_handler(void *ptr, size_t size, size_t nmemb, response *r);

#endif
