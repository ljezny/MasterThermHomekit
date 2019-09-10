#ifndef xSHA1_H
#define xSHA1_H

/*
   SHA-1 in C
   By Steve Reid <steve@edmweb.com>
   100% Public Domain
 */

#include "stdint.h"

typedef struct
{
    uint32_t state[5];
    uint32_t count[2];
    unsigned char buffer[64];
} xSHA1_CTX;

void xSHA1Transform(
    uint32_t state[5],
    const unsigned char buffer[64]
    );

void xSHA1Init(
    xSHA1_CTX * context
    );

void xSHA1Update(
    xSHA1_CTX * context,
    const unsigned char *data,
    uint32_t len
    );

void xSHA1Final(
    unsigned char digest[20],
    xSHA1_CTX * context
    );

void xSHA1(
    char *hash_out,
    const char *str,
    int len);

#endif /* xSHA1_H */
