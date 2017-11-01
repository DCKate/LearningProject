//
//  main.cpp
//  openssl_aes_crypto
//
//  Created by kateh on 2017/10/31.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#include <iostream>
#include <sys/time.h>
#include <map>
#include "ossl_evp.hpp"
#include "ossl_aes.hpp"

enum run_type {
    AES,
    EVP
};

uint64_t getSystimeUTs ()
{
    
    struct timeval tp_now;
    gettimeofday(&tp_now,NULL);
    return tp_now.tv_sec*1000000 + tp_now.tv_usec ;
}
void testevp(){
    char *aes_key=(char *)"opensslasdfghjkl";
    char *aes_iv=(char *)"opensslasdfghjkl";
    char decrout[1024]={0};
    char encrout[1024]={0};
    
    size_t enclen = opensslevpencry(encrout,aes_key,aes_iv,(char *)"QWERTYUIOPLKJHGFDSA");
    size_t declen = opensslevpdecry(decrout,aes_key,aes_iv,encrout);
//    encrout[enclen]='\0';
//    printf("encryption : %s\n",encrout);
//    decrout[declen] = '\0';
//    printf("decryption : %s\n",decrout);
}

void testaes(){
    char *aes_key=(char *)"opensslasdfghjkl";
    char *aes_iv=(char *)"opensslasdfghjkl";
    char decrout[1024]={0};
    char encrout[1024]={0};
    size_t enclen = aes_encrypt((char *)"QWERTYUIOPLKJHGFDSA",aes_key,aes_iv, encrout);
    int declen = aes_decrypt(encrout,aes_key,aes_iv, decrout);
//    encrout[enclen]='\0';
//    printf("encryption : %s\n",encrout);
//    decrout[declen] = '\0';
//    printf("decryption : %s\n",decrout);
}
int main(int argc, const char * argv[]) {
    if(argc<2){
        std::cout<<"Useg: programe <ho many round:integer> <what api:EVP/AES>"<<std::endl;
        return 0;
    }
    std::map<std::string,run_type> emap;
    emap.insert(std::pair<std::string,run_type>("AES", AES));
    emap.insert(std::pair<std::string,run_type>("EVP", EVP));

    int count = std::stoi(argv[1],nullptr,10);
    std::string tyty(argv[2]);

    switch (emap[tyty]){
        case EVP:{
            uint64_t tevp=getSystimeUTs();
            for (int ii=0; ii<count; ii++) {
                testevp();
            }
            uint64_t devp = getSystimeUTs()-tevp;
            std::cout<<"EVP "<<devp<<std::endl;
            break;
        }
        case AES:{
            uint64_t taes=getSystimeUTs();
            for (int ii=0; ii<count; ii++) {
                testaes();
            }
            uint64_t daes = getSystimeUTs()-taes;
            std::cout<<"AES "<<daes<<std::endl;
            break;
        }
        default:
            std::cout<<"Oooops~~~"<<std::endl;
            break;
    }
       
    return 0;
}
