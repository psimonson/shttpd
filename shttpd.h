/*
 * shttpd.h - Header for a simple HTTP server.
 *
 * Author: Philip R. Simonson
 * Date  : 03/14/2022
 *
 */

#ifndef SHTTPD_H
#define SHTTPD_H

#define DEFAULT_PORT 8080

/* Response requests */
enum {
	RESPONSE_OKAY = 200,
	RESPONSE_MOVPERM = 301,
	RESPONSE_FOUND = 302,
	RESPONSE_BADREQ = 400,
	RESPONSE_UNAUTH = 401,
	RESPONSE_FORBIDDEN = 403,
	RESPONSE_NOTFOUND = 404
};

/* Forward declaration for response structure */
struct response;

/* Type definition for response structure */
typedef struct response response_t;

/* Response functions */
response_t *response_init(void);
void response_clear(response_t *r);
unsigned short response_get(response_t r);
const char *response_getstr(response_t r);

#endif
