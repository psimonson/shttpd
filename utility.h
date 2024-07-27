/*
 * utility.h - Header library for common functionality.
 *
 * Author: Philip R. Simonson
 * Date  : 10/14/2021
 * 
 ****************************************************************************
 */

#ifndef UTILITY_H
#define UTILITY_H

#include <stdio.h>

/* Get a string of input from a file stream.
 */
static int get_string(char *s, int sz, FILE *fp, const char *prompt)
{
    int c, i;

    /* Prompt user for input */
    if(prompt != NULL) printf("%s", prompt);

    /* Receive input from keyboard */
    for(i = 0; i < (sz - 1) && (c = getc(fp)) != EOF && c != '\n'; )
        if(c == '\b') {
            if(i > 0)
                --i;
        }
        else {
            s[i++] = c;
        }

    /* Null terminate string */
    s[i] = '\0';

    /* Check for end-of-file */
    if(c == EOF)
        return i > 0 ? i : -1;

    /* Return length of string */
    return i;
}
/* Encrypt a given string in place.
 */
static void encrypt(char *s, int sz)
{
    int i;

    for(i = 0; i < sz; i++)
        s[i] = (s[i] ^ 0x55) + 77;
}
/* Decrypt a given string in place.
 */
static void decrypt(char *s, int sz)
{
    int i;

    for(i = 0; i < sz; i++)
        s[i] = (s[i] - 77) ^ 0x55;
}

#endif