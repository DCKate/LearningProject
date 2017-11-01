//
//  ossl_aes.cpp
//  aes_cryption
//
//  Created by kateh on 2017/10/31.
//  Copyright © 2017年 FAKE. All rights reserved.
//
#include <iostream>
#include <openssl/aes.h>
#include <math.h>   /* ceil */
#include <string.h>       
#include "ossl_aes.hpp"
#include "vcrypto.hpp"
/* for ttt https://stackoverflow.com/questions/34666795/segfault-in-aes-cbc-encrypt */
#define BLOCK_SIZE  (128)

unsigned char input[BLOCK_SIZE] = {
    0x4du, 0x69u, 0x64u, 0x6eu, 0x69u, 0x67u, 0x68u, 0x74u,
    0x5fu, 0x4du, 0x61u, 0x72u, 0x6cu, 0x69u, 0x6eu, 0x05u,
    0x52u, 0x69u, 0x63u, 0x68u, 0x61u, 0x72u, 0x64u, 0x52u,
    0x69u, 0x63u, 0x68u, 0x61u, 0x72u, 0x64u, 0x06u, 0x07u};

int ttt()
{
    unsigned char ivec[BLOCK_SIZE];
    /* ivec[0..7] is the IV, ivec[8..15] is the big endian counter. */
    unsigned char outBuf[BLOCK_SIZE];
    
    memset(ivec, 0, BLOCK_SIZE);
    
//    unsigned char* ivec2 = ivec + 8;
//    unsigned long* ivec3 = (unsigned long*) ivec2;
//    *ivec3 = (unsigned long) 0xfd0;
    
    AES_KEY aesKey;
    char* myKey = (char*)"Pampers baby-dry";
    int res;
    
    if ((res = AES_set_encrypt_key((unsigned char*) myKey, BLOCK_SIZE, &aesKey)) < 0)
    {
        fprintf(stderr, "AES_set_encrypt_key: returned %d", res);
        return EXIT_FAILURE;
    }
    
    int i = 0;
    //for (int i = 0; i < 32; i += BLOCK_SIZE)
    {
        printf("ivec = ");
        
        for (int j = 0; j < BLOCK_SIZE; j++)
            printf("%.02hhx ", ivec[j]);
        
        putchar('\n');
        
        printf("input = ");
        
        for (int j = i; j < (i + BLOCK_SIZE); j++)
            printf("%.02hhx ", input[j]);
        
        putchar('\n');
        putchar('\n');
        putchar('\n');
        putchar('\n');
        
        AES_cbc_encrypt(input, outBuf, BLOCK_SIZE, &aesKey, ivec, AES_ENCRYPT);
        
        printf("outBuf = ");
        
        for (int j = 0; j < BLOCK_SIZE; j++)
            printf("%.02hhx ", outBuf[j]);
        
        putchar('\n');
        

    }
    return EXIT_SUCCESS;
}

size_t aes_encrypt(char* plan, char* aes_key,char* aes_iv, char* enout)
{
    AES_KEY aes;
    unsigned char ivec[AES_BLOCK_SIZE+1]={0};
    snprintf((char*)ivec,AES_BLOCK_SIZE+1,"%s",aes_iv);
//    for (int j = 0; j < AES_BLOCK_SIZE; j++)
//        printf("%.02hhx ", ivec[j]);
    if(AES_set_encrypt_key((unsigned char*)aes_key, BLOCK_SIZE, &aes) < 0)
    {
        return 0;
    }
    
    int len = (int)strlen(plan), en_len=0;
    int num = ceil(strlen(plan)/16.0);
    unsigned char instr[1024]={0};
    
    snprintf((char*)instr,AES_BLOCK_SIZE*num,"%s",plan);
    for (int ii = len; ii<AES_BLOCK_SIZE*num; ++ii) {
        instr[ii]=0x20;
    }

//    while(en_len<len)//AES_BLOCK_SIZE*n and n is integer
    unsigned char ciphertext[1024]={0};
    unsigned char* in = instr;
    unsigned char* out = ciphertext;
    for(int kk=0;kk<num;kk++)
    {
        AES_cbc_encrypt(in, out,AES_BLOCK_SIZE, &aes,ivec, AES_ENCRYPT);
        in+=AES_BLOCK_SIZE;
        out+=AES_BLOCK_SIZE;
        en_len+=AES_BLOCK_SIZE;
        
    }
//    for (int j = 0; j < AES_BLOCK_SIZE*3; j++)
//        printf("%.02hhx ", ciphertext[j]);
    
    size_t finallen =b64_encode(enout,(unsigned char *)ciphertext,en_len);
    return finallen;
}

int aes_decrypt(char* enb64str, char* aes_key,char* aes_iv, char* deout)
{
    AES_KEY aes;
    unsigned char ivec[AES_BLOCK_SIZE+1]={0};
    snprintf((char*)ivec,AES_BLOCK_SIZE+1,"%s",aes_iv);
    
    if(AES_set_decrypt_key((unsigned char*)aes_key, 128, &aes) < 0)
    {
        return 0;
    }
    int len = (int)strlen(enb64str), en_len=0;
    unsigned char enplan[1024]={0};
    size_t enlen = b64_decode((char*)enplan,enb64str,len);

    unsigned char* in = enplan;
    unsigned char* out = (unsigned char*)deout;
    while(en_len<enlen)
    {
        AES_cbc_encrypt(in,out,AES_BLOCK_SIZE, &aes,ivec,AES_DECRYPT);
        in+=AES_BLOCK_SIZE;
        out+=AES_BLOCK_SIZE;
        en_len+=AES_BLOCK_SIZE;
    }
    return en_len;
}