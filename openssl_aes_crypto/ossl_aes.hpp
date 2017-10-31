//
//  ossl_aes.hpp
//  aes_cryption
//
//  Created by kateh on 2017/10/31.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#ifndef ossl_aes_hpp
#define ossl_aes_hpp

#include <stdio.h>
size_t aes_encrypt(char* plan, char* aes_key,char* aes_iv, char* enout);
int aes_decrypt(char* enb64str, char* aes_key,char* aes_iv, char* deout);
#endif /* ossl_aes_hpp */
