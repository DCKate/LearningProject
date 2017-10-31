//
//  ossl_evp.cpp
//  aes_cryption
//
//  Created by kateh on 2017/10/31.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#include <iostream>
#include <openssl/evp.h>
#include <openssl/err.h>
#include <openssl/aes.h>
#include <openssl/conf.h>
#include <string.h> 
#include <math.h>       /* ceil */
#include "ossl_evp.hpp"
#include "vcrypto.hpp"
#define NOPADDING 1
static void hex_print(const void* pv, size_t len)
{
    const unsigned char * p = (const unsigned char*)pv;
    if (NULL == pv)
        printf("NULL");
    else
    {
        size_t i = 0;
        for (; i<len;++i)
            printf("%02x ", *p++);
    }
    printf("\n");
}

void handleErrors(void)
{
    ERR_print_errors_fp(stderr);
    abort();
}

int encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext)
{
    EVP_CIPHER_CTX *ctx;
    
    int len;
    
    int ciphertext_len;
    
    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
    
    /* Initialise the encryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_EncryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
        handleErrors();
    
#if NOPADDING
    if(1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) handleErrors();
#endif    /* Provide the message to be encrypted, and obtain the encrypted output.
* EVP_EncryptUpdate can be called multiple times if necessary
*/
    if(1 != EVP_EncryptUpdate(ctx, ciphertext, &len, plaintext, plaintext_len))
        handleErrors();
    ciphertext_len = len;
    
    /* Finalise the encryption. Further ciphertext bytes may be written at
     * this stage.
     */
#if NOPADDING
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext, &len)) handleErrors();
#else
    if(1 != EVP_EncryptFinal_ex(ctx, ciphertext + len, &len)) handleErrors();
#endif
    ciphertext_len += len;
    
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    
    return ciphertext_len;
}

int decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
            unsigned char *iv, unsigned char *plaintext)
{
    EVP_CIPHER_CTX *ctx;
    
    int len;
    
    int plaintext_len;
    
    /* Create and initialise the context */
    if(!(ctx = EVP_CIPHER_CTX_new())) handleErrors();
    
    /* Initialise the decryption operation. IMPORTANT - ensure you use a key
     * and IV size appropriate for your cipher
     * In this example we are using 256 bit AES (i.e. a 256 bit key). The
     * IV size for *most* modes is the same as the block size. For AES this
     * is 128 bits */
    if(1 != EVP_DecryptInit_ex(ctx, EVP_aes_128_cbc(), NULL, key, iv))
        handleErrors();
#if NOPADDING
    if(1 != EVP_CIPHER_CTX_set_padding(ctx, 0)) handleErrors();
#endif
    /* Provide the message to be decrypted, and obtain the plaintext output.
     * EVP_DecryptUpdate can be called multiple times if necessary
     */
    if(1 != EVP_DecryptUpdate(ctx, plaintext, &len, ciphertext, ciphertext_len))
        handleErrors();
    plaintext_len = len;
    
    /* Finalise the decryption. Further plaintext bytes may be written at
     * this stage.
     */
#if NOPADDING
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext, &len)) handleErrors();
#else
    if(1 != EVP_DecryptFinal_ex(ctx, plaintext + len, &len)) handleErrors();
#endif
    plaintext_len += len;
    
    /* Clean up */
    EVP_CIPHER_CTX_free(ctx);
    
    return plaintext_len;
}
size_t opensslevpencry (char* finalstr,char* aes_key,char* aes_iv,char* plan)
{
    /* Set up the key and iv. Do I need to say to not hard code these in a
     * real application? :-)
     */
    
    /* A 256 bit key */
    unsigned char key[AES_BLOCK_SIZE];
    memset(key, 0, AES_BLOCK_SIZE);
    size_t copylen = ((AES_BLOCK_SIZE)>=strlen((char*)aes_key))?strlen((char*)aes_key):AES_BLOCK_SIZE;
    memcpy(key, aes_key, copylen);
    //printf("KEY:\n");
    //hex_print(key, AES_BLOCK_SIZE);
    
    /* A 128 bit IV */
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0, AES_BLOCK_SIZE);
    size_t icopylen = ((AES_BLOCK_SIZE)>=strlen((char*)aes_iv))?strlen((char*)aes_iv):AES_BLOCK_SIZE;
    memcpy(iv, aes_iv, icopylen);
//    printf("IV:\n");
//    hex_print(iv, AES_BLOCK_SIZE);
    
    /* Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, dependant on the
     * algorithm and mode
     */
    unsigned char ciphertext[1024];
    
    /* Buffer for the decrypted text */
    int ciphertext_len;
    
    int num = ceil(strlen(plan)/16.0);
    
    char instr[1024]={0};
    size_t inlen = snprintf((char*)instr,AES_BLOCK_SIZE*num,plan);
    /* Initialise the library */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
    
    for (int ii = (int)strlen(plan); ii<AES_BLOCK_SIZE*num; ++ii) {
        instr[ii]=0x20;
    }
    
    /* Encrypt the plaintext */
    ciphertext_len = encrypt ((unsigned char *)instr, AES_BLOCK_SIZE*num, key, iv,
                              ciphertext);
//    hex_print(ciphertext, ciphertext_len);
    size_t finallen = b64_encode(finalstr,(unsigned char *)ciphertext,ciphertext_len);
    /* Clean up */
    EVP_cleanup();
    ERR_free_strings();
    return finallen;
    
}

size_t opensslevpdecry (char* finalout,char* aes_key,char* aes_iv,char* base64str)
{
    /* Set up the key and iv. Do I need to say to not hard code these in a
     * real application? :-)
     */
    
    /* A 256 bit key */
    unsigned char key[AES_BLOCK_SIZE];
    memset(key, 0, AES_BLOCK_SIZE);
    size_t copylen = ((AES_BLOCK_SIZE)>=strlen((char*)aes_key))?strlen((char*)aes_key):AES_BLOCK_SIZE;
    memcpy(key, aes_key, copylen);
    //printf("KEY:\n");
    //hex_print(key, AES_BLOCK_SIZE);
    
    /* A 128 bit IV */
    unsigned char iv[AES_BLOCK_SIZE];
    memset(iv, 0, AES_BLOCK_SIZE);
    size_t icopylen = ((AES_BLOCK_SIZE)>=strlen((char*)aes_iv))?strlen((char*)aes_iv):AES_BLOCK_SIZE;
    memcpy(iv, aes_iv, icopylen);
    //printf("IV:\n");
    //hex_print(iv, AES_BLOCK_SIZE);
    
    /* Buffer for ciphertext. Ensure the buffer is long enough for the
     * ciphertext which may be longer than the plaintext, dependant on the
     * algorithm and mode
     */
    //unsigned char ciphertext[128];
    
    /* Buffer for the decrypted text */
    unsigned char decryptedtext[1024];
    
    int decryptedtext_len;//, ciphertext_len;
    
    /* Initialise the library */
    ERR_load_crypto_strings();
    OpenSSL_add_all_algorithms();
    OPENSSL_config(NULL);
    
    char enplan[10240]={0};
    size_t enlen = b64_decode(enplan,base64str,strlen(base64str));

    /* Decrypt the ciphertext */
    decryptedtext_len = decrypt((unsigned char *)enplan, (int)enlen, key, iv,
                                decryptedtext);
    
    /* Add a NULL terminator. We are expecting printable text */
//    decryptedtext[decryptedtext_len] = '\0';
    
    /* Show the decrypted text */
//    printf("Decrypted text is:\n");
//    printf("%s\n", decryptedtext);
//    hex_print(decryptedtext, decryptedtext_len);

    size_t finallen = snprintf(finalout,(size_t)decryptedtext_len,(char *)decryptedtext);
//    finalout[finallen]='\0';
    
//    printf("Plain text[%s] is: %s\n",base64str, finalout);
    
    /* Clean up */
    EVP_cleanup();
    ERR_free_strings();
    return finallen;
    
}
