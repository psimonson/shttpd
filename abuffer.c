/*
 * abuffer.c - Source for an append buffer.
 *
 * Author: Philip R. Simonson
 * Date  : 07/04/2022
 *
 ****************************************************************************
 */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

/* Define the structure for the append buffer. */
struct AppendBuffer {
    char *string;
    unsigned int length;
};

/* Initialize the given append buffer structure.
 */
struct AppendBuffer *ab_init(void)
{
    struct AppendBuffer *ab;

    ab = (struct AppendBuffer *)calloc(1, sizeof(struct AppendBuffer));
    if(ab != NULL) {
        ab->string = NULL;
        ab->length = 0;
    }
    return ab;
}

/* Append given string to buffer.
 */
int ab_append(struct AppendBuffer *ab, const char *string, unsigned int length)
{
    char *tmp;

    if(ab == NULL || string == NULL) return -1;

    tmp = (char*)realloc(ab->string, sizeof(char) * (ab->length + length + 1));
    if(tmp != NULL) {
        memcpy(&tmp[ab->length], string, length);
        tmp[ab->length + length] = '\0';
        ab->string = tmp;
        ab->length = ab->length + length;
        return ab->length;
    }
    return -1;
}

/* Free given append buffer.
 */
void ab_free(struct AppendBuffer *ab)
{
    if(ab != NULL) {
        free(ab->string);
        free(ab);
    }
}

/* Get the data from the buffer.
 */
char *ab_getdata(struct AppendBuffer *ab)
{
    if(ab != NULL) {
        return ab->string;
    }
    return NULL;
}

/* Get the size of data from the buffer.
 */
unsigned int ab_getsize(struct AppendBuffer *ab)
{
    if(ab != NULL) {
        return ab->length;
    }
    return 0;
}
