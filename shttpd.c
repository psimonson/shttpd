/*
 * shttpd.c - Source for a simple http server.
 *
 * Author: Philip R. Simonson
 * Date  : 03/14/2022
 *
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdarg.h>
#include <string.h>

#include "abuffer.h"
#include "network.h"
#include "threadpool.h"
#include "shttpd.h"

/* ----------------------------- Response Stuff --------------------------- */

struct response {
	unsigned short response;
	char *buffer;
	AppendBuffer *ab;
};

/* Make a response string from given value.
 */
char *response_make(unsigned short value)
{
	switch(value) {
		case RESPONSE_OKAY:
			return "OK";
		case RESPONSE_BADREQ:
			return "Bad Request";
		case RESPONSE_UNAUTH:
			return "Unauthorised";
		case RESPONSE_FORBIDDEN:
			return "Forbidden";
		case RESPONSE_NOTFOUND:
			return "Not Found";
		default:
			return "Unhandled";
	}
	return "<null>";
}

/* Initialize response structure.
 */
response_t *response_init(void)
{
	static response_t r;
	r.response = RESPONSE_OKAY;
	r.ab = ab_init();
	return &r;
}

/* Clear response structure.
 */
void response_clear(response_t *r)
{
	if(r != NULL) {
		r->response = 0;
		ab_free(r->ab);
		r->ab = ab_init();
	}
}

/* Set response code to response structure.
 */
void response_set(response_t *r, unsigned short value)
{
	if(r != NULL) {
		r->response = value;
		r->buffer = response_make(value);
	}
}

/* Get response code from response structure.
 */
unsigned short response_get(response_t r)
{
	return r.response;
}

/* Get response message from response structure.
 */
const char *response_getstr(response_t r)
{
	return r.buffer;
}

/* ------------------------------ Main Program -------------------------- */

/* Strip new line from buffer.
 */
void strip(char *s)
{
	while(*s != '\0') {
		if(*s == '\r' || *s == '\n')
			*s = 0;
		s++;
	}
}

/* Send response from server to client.
 */
void send_response(SOCKET fd, response_t r)
{
	long nbytes;

	nbytes = send(fd, ab_getdata(r.ab), ab_getsize(r.ab), 0);
	if(nbytes <= 0) {
		if(nbytes < 0) {
			fprintf(stderr, "Error: Failed to send data.\n");
		}
		else if(nbytes == 0) {
			fprintf(stderr, "Connection closed.\n");
		}
		else if(nbytes != ab_getsize(r.ab)) {
			fprintf(stderr, "Warning: Could not send all data.\n");
		}
	}
}

/* Process request from client.
 */
static void process_request(void *p)
{
	SOCKET fd = *(SOCKET*)p;
	response_t r;
	char buffer[4096];
	int nbytes;

	r = *response_init();

	nbytes = recv(fd, buffer, sizeof(buffer)-1, 0);
	if(nbytes <= 0) {
		if(nbytes < 0)
			printf("Error: Could not receive data.\n");
		close(fd);
		return;
	}
	else {
		char path[1024];

		/* Null terminate buffer and strip newlines */
		buffer[nbytes] = '\0';
		strip(buffer);

		/* Process GET request */
		if(sscanf(buffer, "GET %s HTTP/1.0", path) != 1) {
			fprintf(stderr, "Error: Invalid request.\n");
		}
		else {
			char *endstr = strchr(path, '\n');
			if(endstr != NULL) {
				*endstr = '\0';
			}

			/* Check path to see if it's valid */
			if(strncmp(path, "/", 1) == 0) {
				char response[2048];
				char filename[1024];
				char dir[512];
				FILE *fp;

				/* Send okay response */
				response_set(&r, RESPONSE_OKAY);
				snprintf(response, sizeof(response)-1, "HTTP/1.0 %hu %s\r\n\r\n",
					response_get(r), response_getstr(r));
				ab_append(r.ab, response, strlen(response));
				memset(filename, 0, sizeof(filename)-1);
				getcwd(dir, sizeof(dir)-1);
				strncpy(filename, dir, sizeof(filename)-1);
				strcat(filename, "/");

				if(path[0] == '/' && path[1] == '\0') {
					strcat(filename, "index.html");
				}
				else if(path[0] == '/' && path[1] != '\0') {
					strcat(filename, path);
				}

				fp = fopen(filename, "rt");
				if(fp == NULL) {
					fprintf(stderr, "Error: Can't find file '%s'.\n", filename);
					close(fd);
					return;
				}

				while((nbytes = fread(buffer, 1, sizeof(buffer), fp)) > 0) {
					ab_append(r.ab, buffer, nbytes);
				}
				fclose(fp);
				send_response(fd, r);
			}
			else {
				response_set(&r, RESPONSE_NOTFOUND);
				fprintf(stderr, "GET %s : %hu - %s\n", path,
					response_get(r), response_getstr(r));
				send_response(fd, r);
			}
		}
	}
	close(fd);
}

int main(int argc, char *argv[])
{
	unsigned short port = 8080;
	SOCKET server, client;
	threadpool_t *tpool;

	if(argc > 2) {
		fprintf(stderr, "Usage: %s [port]\n", argv[0]);
		return 1;
	}

	server = server_socket_open(&port);
	if(server == INVALID_SOCKET)
		return 1;

	tpool = threadpool_create(5);
	while(1) {
		client = server_socket_accept(server);
		if(client == INVALID_SOCKET)
			break;

		threadpool_add_task(tpool, process_request, &client);
	}

	threadpool_wait(tpool);
	threadpool_destroy(tpool);
	close(server);
	return 0;
}
