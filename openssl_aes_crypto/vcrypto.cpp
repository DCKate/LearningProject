#include <stdio.h>
#include <string.h>
#include <openssl/bio.h>
#include <openssl/evp.h>
#include <openssl/buffer.h>
#include <stdint.h>
#include <ctype.h>
#include <assert.h>

#include "vcrypto.hpp"

static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789-_";
//static const char b64_table[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/";


size_t b64_encode (char* dst,const unsigned char *src, size_t len)
{
    int i = 0;
    int j = 0;
    char *enc = NULL;
    size_t size = 0;
    unsigned char buf[4];
    unsigned char tmp[3];
    
    // alloc
    enc = (char *) malloc(0);
    if (NULL == enc) { return NULL; }
    
    // parse until end of source
    while (len--) {
        // read up to 3 bytes at a time into `tmp'
        tmp[i++] = *(src++);
        
        // if 3 bytes read then encode into `buf'
        if (3 == i) {
            buf[0] = (tmp[0] & 0xfc) >> 2;
            buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
            buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
            buf[3] = tmp[2] & 0x3f;
            
            // allocate 4 new byts for `enc` and
            // then translate each encoded buffer
            // part by index from the base 64 index table
            // into `enc' unsigned char array
            enc = (char *) realloc(enc, size + 4);
            for (i = 0; i < 4; ++i) {
                enc[size++] = b64_table[buf[i]];
            }
            
            // reset index
            i = 0;
        }
    }
    
    // remainder
    if (i > 0) {
        // fill `tmp' with `\0' at most 3 times
        for (j = i; j < 3; ++j) {
            tmp[j] = '\0';
        }
        
        // perform same codec as above
        buf[0] = (tmp[0] & 0xfc) >> 2;
        buf[1] = ((tmp[0] & 0x03) << 4) + ((tmp[1] & 0xf0) >> 4);
        buf[2] = ((tmp[1] & 0x0f) << 2) + ((tmp[2] & 0xc0) >> 6);
        buf[3] = tmp[2] & 0x3f;
        
        // perform same write to `enc` with new allocation
        for (j = 0; (j < i + 1); ++j) {
            enc = (char *) realloc(enc, size + 1);
            enc[size++] = b64_table[buf[j]];
        }
        
        // while there is still a remainder
        // append `=' to `enc'
        while ((i++ < 3)) {
            enc = (char *) realloc(enc, size + 1);
            enc[size++] = '=';
        }
    }
    
    // Make sure we have enough space to add '\0' character at end.
    //enc = (char *) realloc(enc, size + 1);
    //enc[size] = '\0';
    memcpy(dst, enc, size);
    //dst[size] = '\0';
    
    free(enc);
    return size;
}

size_t b64_decode (char* dst, const char *src, size_t len)
{
    int i = 0;
    int j = 0;
    int l = 0;
    size_t size = 0;
    unsigned char *dec = NULL;
    unsigned char buf[3];
    unsigned char tmp[4];
    
    // alloc
    dec = (unsigned char *) malloc(0);
    if (NULL == dec) { return NULL; }
    
    // parse until end of source
    while (len--) {
        // break if char is `=' or not base64 char
        if ('=' == src[j]) { break; }
        if (!(isalnum(src[j]) || b64_table[62] == src[j] || b64_table[63] == src[j])) { break; }
        
        // read up to 4 bytes at a time into `tmp'
        tmp[i++] = src[j++];
        
        // if 4 bytes read then decode into `buf'
        if (4 == i) {
            // translate values in `tmp' from table
            for (i = 0; i < 4; ++i) {
                // find translation char in `b64_table'
                for (l = 0; l < 64; ++l) {
                    if (tmp[i] == b64_table[l]) {
                        tmp[i] = l;
                        break;
                    }
                }
            }
            
            // decode
            buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
            buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
            buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];
            
            // write decoded buffer to `dec'
            dec = (unsigned char *) realloc(dec, size + 3);
            for (i = 0; i < 3; ++i) {
                dec[size++] = buf[i];
            }
            
            // reset
            i = 0;
        }
    }
    
    // remainder
    if (i > 0) {
        // fill `tmp' with `\0' at most 4 times
        for (j = i; j < 4; ++j) {
            tmp[j] = '\0';
        }
        
        // translate remainder
        for (j = 0; j < 4; ++j) {
            // find translation char in `b64_table'
            for (l = 0; l < 64; ++l) {
                if (tmp[j] == b64_table[l]) {
                    tmp[j] = l;
                    break;
                }
            }
        }
        
        // decode remainder
        buf[0] = (tmp[0] << 2) + ((tmp[1] & 0x30) >> 4);
        buf[1] = ((tmp[1] & 0xf) << 4) + ((tmp[2] & 0x3c) >> 2);
        buf[2] = ((tmp[2] & 0x3) << 6) + tmp[3];
        
        // write remainer decoded buffer to `dec'
        dec = (unsigned char *) realloc(dec, size + (i - 1));
        for (j = 0; (j < i - 1); ++j) {
            dec[size++] = buf[j];
        }
    }
    
    // Make sure we have enough space to add '\0' character at end.
    //dec = (unsigned char *) realloc(dec, size + 1);
    //dec[size] = '\0';
    memcpy(dst, dec, size+1);
    //dst[size] = '\0';
    free(dec);
    return size;
}

//================================== use openssl lib ===================================//

static size_t _calcDecodeLength (const char* b64input)
{
	//Calculates the length of a decoded string
	size_t len = strlen(b64input),
		padding = 0;

	if (b64input[len-1] == '=' && b64input[len-2] == '=') //last two chars are =
		padding = 2;
	else if (b64input[len-1] == '=') //last char is =
		padding = 1;

	return (len*3)/4 - padding;
}

char* DecBase64 (vCryptoData* pCrypto, char* src)
{
	BIO *bio, *b64;
	int decodeLen;

	do {
		memset (pCrypto, 0, sizeof(vCryptoData));
		decodeLen = _calcDecodeLength(src);
		if (decodeLen>=sizeof(pCrypto->data))
		{
			pCrypto->len = -1;
			break;
		}
		(pCrypto->data)[decodeLen] = '\0';
		bio = BIO_new_mem_buf(src, -1);
		b64 = BIO_new(BIO_f_base64());
		bio = BIO_push(b64, bio);

		BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Do not use newlines to flush buffer
		(pCrypto->len) = BIO_read(bio, pCrypto->data, strlen(src));
        assert((pCrypto->len) == decodeLen); //length should equal decodeLen, else something went horribly wrong
        BIO_free_all(bio);
		
	} while(0);

	return pCrypto->data; //success
}


char* EncBase64 (vCryptoData* pCrypto, char* src, size_t srcLen)
{
	BIO *bio, *b64;
	BUF_MEM *bufferPtr;

	b64 = BIO_new(BIO_f_base64());
	bio = BIO_new(BIO_s_mem());
	bio = BIO_push(b64, bio);

	BIO_set_flags(bio, BIO_FLAGS_BASE64_NO_NL); //Ignore newlines - write everything in one line
	BIO_write(bio, src, srcLen);
	BIO_flush(bio);
	BIO_get_mem_ptr(bio, &bufferPtr);
	BIO_set_close(bio, BIO_NOCLOSE);
	BIO_free_all(bio);

	memset (pCrypto, 0, sizeof(vCryptoData));
	memcpy (pCrypto->data, bufferPtr->data, bufferPtr->length);
	pCrypto->len = bufferPtr->length;
	
	BUF_MEM_free (bufferPtr);

	return pCrypto->data; //success
}


