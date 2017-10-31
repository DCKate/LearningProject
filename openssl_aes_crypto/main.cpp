//
//  main.cpp
//  openssl_aes_crypto
//
//  Created by kateh on 2017/10/31.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#include <iostream>
#include <sys/time.h>
#include "ossl_evp.hpp"
#include "ossl_aes.hpp"

uint64_t getSystimeUTs ()
{
    
    struct timeval tp_now;
    gettimeofday(&tp_now,NULL);
    return tp_now.tv_sec*1000000 + tp_now.tv_usec ;
}
void testevp(){
    char *utc_key=(char *)"opensslasdfghjkl";
    char *utc_iv=(char *)"opensslasdfghjkl";
    char decrout[1024]={0};
    char encrout[1024]={0};
    
    size_t enclen = opensslevpencry(encrout,utc_key,utc_iv,(char *)"QWERTYUIOPLKJHGFDSA");
    size_t declen = opensslevpdecry(decrout,utc_key,utc_iv,encrout);
//    encrout[enclen]='\0';
//    printf("encryption : %s\n",encrout);
//    decrout[declen] = '\0';
//    printf("decryption : %s\n",decrout);
}

void testaes(){
    char *utc_key=(char *)"opensslasdfghjkl";
    char *utc_iv=(char *)"opensslasdfghjkl";
    char decrout[1024]={0};
    char encrout[1024]={0};
    size_t enclen = aes_encrypt((char *)"QWERTYUIOPLKJHGFDSA",utc_key,utc_iv, encrout);
    int declen = aes_decrypt(encrout,utc_key,utc_iv, decrout);
//    encrout[enclen]='\0';
//    printf("encryption : %s\n",encrout);
//    decrout[declen] = '\0';
//    printf("decryption : %s\n",decrout);
}
int main(int argc, const char * argv[]) {
    uint64_t tevp = 0;
    uint64_t taes = 0;
    tevp=getSystimeUTs();
    for (int ii=0; ii<1000; ii++) {
        testevp();
    }
    uint64_t devp = getSystimeUTs()-tevp;
    taes=getSystimeUTs();
    for (int ii=0; ii<1000; ii++) {
        testaes();
    }
    uint64_t daes = getSystimeUTs()-taes;
    
    std::cout<<"EVP "<<devp<<std::endl;
    std::cout<<"AES "<<daes<<std::endl;
    return 0;
}
