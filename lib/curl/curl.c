/**
*
*  cURL response handlers
*  @author Adrien Thébault <me@adrien-thebault.fr>
*
*/

/** Dependencies */

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "./curl.h"
#include "../output/output.h"

/**
*
*  Init response
*
**/

void init_response(response *r) {

  r->len = 0;
  r->ptr = malloc(r->len+1);

  if (r->ptr == NULL) {
    printf(ERROR("malloc() failed\n"));
    exit(EXIT_FAILURE);
  }

  r->ptr[0] = '\0';

}

/**
*
*  Copy response data into r
*
**/

size_t response_handler(void *ptr, size_t size, size_t nmemb, response *r) {

  size_t new_len = r->len + size*nmemb;
  r->ptr = realloc(r->ptr, new_len+1);

  if (r->ptr == NULL) {
    printf(ERROR("realloc() failed\n"));
    exit(EXIT_FAILURE);
  }

  memcpy(r->ptr+r->len, ptr, size*nmemb);
  r->ptr[new_len] = '\0';
  r->len = new_len;

  return size*nmemb;

}
