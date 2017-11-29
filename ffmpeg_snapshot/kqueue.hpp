//
//  kqueue.hpp
//  kQueue
//
//  Created by YuTing on 2017/3/25.
//  Copyright © 2017年 YuTing. All rights reserved.
//

#ifndef kqueue_hpp
#define kqueue_hpp

#include <stdio.h>
#include <mutex>

class knode{
public:
    knode* next;
    char* data;
    int type;
    int len;
};

class kqueue{
public:
    std::mutex lock;
    knode* pop;
    knode* push;
    int count;
};

void push_kqueue(kqueue* qu, char* data,int type,int dlen);
int pop_kqueue(kqueue* qu, char* out);
void clear_kqueue(kqueue* qu);
knode* pop_kqueue_node(kqueue* qu);
void Free_pop_node(knode* no);
kqueue* init_kqueue();

#endif /* kqueue_hpp */
