//
//  kqueue.cpp
//  ForkProcess
//
//  Created by kateh on 2017/3/9.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#include "kqueue.hpp"
#include <stdlib.h>
#include <string.h>
#include <iostream>
void push_kqueue(kqueue* qu, char* data,int type,int dlen){
    knode* tmp = new knode();
    tmp->data = (char*)malloc(dlen);
    tmp->next = NULL;
    memcpy(tmp->data,data,dlen);
    tmp->len = dlen;
    tmp->type = type;
    {
        std::lock_guard<std::mutex> mlock(qu->lock);
        if (qu->push) {
            knode* pre = qu->push;
            pre->next = tmp;
        }
        qu->push = tmp;
        if (qu->pop==NULL) {
            qu->pop = qu->push;
        }
        qu->count++;
    }
    
}

int pop_kqueue(kqueue* qu, char* out){
    int rlen = 0;
    {
        std::lock_guard<std::mutex> mlock(qu->lock);
        if(qu->pop){
            knode* tmp = qu->pop;
            if (tmp->next==NULL) {
                qu->pop=NULL;
                qu->push=NULL;
            }else{
                qu->pop = tmp->next;
            }
            qu->count--;
            tmp->next = NULL;
            memcpy(out, tmp->data, tmp->len);
            free(tmp->data);
            rlen = tmp->len;
            delete tmp;
        }
    }
    
    return rlen;
}

void clear_kqueue(kqueue* qu){
    int count=0;
    {
        std::lock_guard<std::mutex> mlock(qu->lock);
        knode* tmp = qu->pop;
        while (tmp) {
            knode* ftmp = tmp;
            tmp = tmp->next;
            
            if (ftmp->next==NULL) {
                qu->pop=NULL;
                qu->push=NULL;
            }
            count++;
            std::cout<<count<<std::endl;
            free(ftmp->data);
            delete ftmp;
        }
        
    }
    
    delete qu;
    
}

knode* pop_kqueue_node(kqueue* qu){
    knode* tt = NULL;
    {
        std::lock_guard<std::mutex> mlock(qu->lock);
        if(qu->pop){
            knode* tmp = qu->pop;
            if (tmp->next==NULL) {
                qu->pop=NULL;
                qu->push=NULL;
            }else{
                qu->pop = tmp->next;
            }
            qu->count--;
            tmp->next = NULL;
            tt = tmp;
        }
    }
    return tt;
}

void Free_pop_node(knode* no){
    if(no){
        free(no->data);
        free(no);
        no = NULL;
    }
}

kqueue* init_kqueue(){
    kqueue* qu = new kqueue();
    qu->pop = NULL;
    qu->push = NULL;
    qu->count = 0;
    return qu;
}

void printkqueue(kqueue* qu){
    {
        std::lock_guard<std::mutex> mlock(qu->lock);
        knode* tmp = qu->pop;
        while (tmp) {
            std::cout<<tmp->data<<std::endl;
            tmp = tmp->next;
        }
    }
}
