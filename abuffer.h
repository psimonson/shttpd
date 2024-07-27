/*
 * abuffer.h - Header for an append buffer.
 *
 * Author: Philip R. Simonson
 * Date  : 07/04/2022
 *
 ****************************************************************************
 */

#ifndef _ABUFFER_H_
#define _ABUFFER_H_

/* Forward delcaration of struct and define typedef. */
struct AppendBuffer;
typedef struct AppendBuffer AppendBuffer;

/* Initialize the append buffer. */
extern struct AppendBuffer *ab_init(void);

/* Append to the append buffer. */
extern int ab_append(struct AppendBuffer *ab, const char *string,
    unsigned int length);

/* Free the append buffer. */
extern void ab_free(struct AppendBuffer *ab);

/* Get the data from the buffer. */
extern char *ab_getdata(struct AppendBuffer *ab);

/* Get the size of data from the buffer. */
extern unsigned int ab_getsize(struct AppendBuffer *ab);

#endif
