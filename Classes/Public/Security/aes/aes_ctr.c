/* crypto/aes/aes_ctr.c -*- mode:C; c-file-style: "eay" -*- */
/* ====================================================================
 * Copyright (c) 1998-2002 The OpenSSL Project.  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 *
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer. 
 *
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in
 *    the documentation and/or other materials provided with the
 *    distribution.
 *
 * 3. All advertising materials mentioning features or use of this
 *    software must display the following acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit. (http://www.openssl.org/)"
 *
 * 4. The names "OpenSSL Toolkit" and "OpenSSL Project" must not be used to
 *    endorse or promote products derived from this software without
 *    prior written permission. For written permission, please contact
 *    openssl-core@openssl.org.
 *
 * 5. Products derived from this software may not be called "OpenSSL"
 *    nor may "OpenSSL" appear in their names without prior written
 *    permission of the OpenSSL Project.
 *
 * 6. Redistributions of any form whatsoever must retain the following
 *    acknowledgment:
 *    "This product includes software developed by the OpenSSL Project
 *    for use in the OpenSSL Toolkit (http://www.openssl.org/)"
 *
 * THIS SOFTWARE IS PROVIDED BY THE OpenSSL PROJECT ``AS IS'' AND ANY
 * EXPRESSED OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED.  IN NO EVENT SHALL THE OpenSSL PROJECT OR
 * ITS CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 * SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT,
 * STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED
 * OF THE POSSIBILITY OF SUCH DAMAGE.
 * ====================================================================
 *
 */

#include "aes_locl.h"
#include "aes.h"
#include <assert.h>

static void AES_ctr128_inc(unsigned char *counter) {
    unsigned long c;
    
    /* Grab bottom dword of counter and increment */
    c = GETU32(counter + 12);
    c++;    c &= 0xFFFFFFFF;
    PUTU32(counter + 12, c);
    
    /* if no overflow, we're done */
    if (c)
        return;
    
    /* Grab 1st dword of counter and increment */
    c = GETU32(counter +  8);
    c++;    c &= 0xFFFFFFFF;
    PUTU32(counter +  8, c);
    
    /* if no overflow, we're done */
    if (c)
        return;
    
    /* Grab 2nd dword of counter and increment */
    c = GETU32(counter +  4);
    c++;    c &= 0xFFFFFFFF;
    PUTU32(counter +  4, c);
    
    /* if no overflow, we're done */
    if (c)
        return;
    
    /* Grab top dword of counter and increment */
    c = GETU32(counter +  0);
    c++;    c &= 0xFFFFFFFF;
    PUTU32(counter +  0, c);
}

void AES_ctr128_encrypt(const unsigned char *in, unsigned char *out,
			size_t length, const AES_KEY *key,
			unsigned char ivec[AES_BLOCK_SIZE],
			unsigned char ecount_buf[AES_BLOCK_SIZE],
			unsigned int *num) {
    unsigned int n;
    unsigned long l=length;
    
    assert(in && out && key && ecount_buf && num);
    assert(*num < AES_BLOCK_SIZE);
    
    n = *num;
    
    while (l--) {
        if (n == 0) {
            AES_encrypt_fixed(ivec, ecount_buf, key);
            AES_ctr128_inc(ivec);
        }
        *(out++) = *(in++) ^ ecount_buf[n];
        n = (n+1) % AES_BLOCK_SIZE;
    }
    
    *num=n;
}


void AES_ctr128_decrypt(const unsigned char *in, unsigned char *out,
                        size_t length, const AES_KEY *key,
                        unsigned char ivec[AES_BLOCK_SIZE],
                        unsigned char ecount_buf[AES_BLOCK_SIZE],
                        unsigned int *num) {
    unsigned int n;
    unsigned long l=length;
    
    assert(in && out && key && ecount_buf && num);
    assert(*num < AES_BLOCK_SIZE);
    
    n = *num;
    
    while (l--) {
        if (n == 0) {
            AES_decrypt_fixed(ivec, ecount_buf, key);
            AES_ctr128_inc(ivec);
        }
        *(out++) = *(in++) ^ ecount_buf[n];
        n = (n+1) % AES_BLOCK_SIZE;
    }
    
    *num=n;
}
