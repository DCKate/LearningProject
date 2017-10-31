#ifndef __VCRYPTO_H__
#define __VCRYPTO_H__

typedef struct {
	int  len;
	char data[2048];
} vCryptoData;


size_t b64_encode (char* dst,const unsigned char *src, size_t len);
size_t b64_decode (char* dst, const char *src, size_t len);

char* EncBase64 (vCryptoData* pCrypto, char* src, size_t srcLen);
char* DecBase64 (vCryptoData* pCrypto, char* src);

#endif //__VCRYPTO_H__

