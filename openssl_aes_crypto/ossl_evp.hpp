//
//  ossl_evp.hpp
//  aes_cryption
//
//  Created by kateh on 2017/10/31.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#ifndef ossl_evp_hpp
#define ossl_evp_hpp

#include <stdio.h>
size_t opensslevpdecry (char* finalout,char* aes_key,char* aes_iv,char* enplan);//base64 encode out
size_t opensslevpencry (char* finalstr,char* aes_key,char* aes_iv,char* plan);//base64 encode in

#endif /* ossl_evp_hpp */
