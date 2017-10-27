//
//  main.c
//  ReplaceKeyValue
//
//  Created by kateh on 2017/10/27.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#include <stdio.h>
#include <string.h>

static uint64_t getSystimeTs(){
    struct timeval tp_now;
    gettimeofday(&tp_now,NULL);
    return tp_now.tv_sec*1000+tp_now.tv_usec/1000;
}

int replacevalue(char* out, char* ins,char* key, char* val){
    int size = 0;//(int)strlen(ins);
    char kk[128];
    snprintf(kk,128,"\"%s\"",key);
    char tt[128];
    int len = snprintf(tt,128,"\"%s\":\"%s\"",key,val);
    char* pch = strtok (ins,"{,}");
    
    while (pch != NULL)
    {
        char *find=strstr(pch, kk);
        char *cp;
        if (find) {
            cp=tt;
        }else{
            cp=pch;
        }
        
        if(size==0){
            size+=snprintf(out,128,"{%s",cp);
        }else{
            size+=snprintf(out+size,128,",%s",cp);
        }
        printf ("%s\n",pch);
        pch = strtok (NULL, "{},");
    }
    size+=snprintf(out+size,8,"}");
    return size;
}

int main(int argc, const char * argv[]) {
    char inp[]="{\"name\":\"KID\",\"timestamp\" : \"26611073\",\"message\":\"keep~going\"}";
    char kk[128];
    snprintf(kk,128,"%llu",getSystimeTs());
    char aaa[1024]={0};
    int ll = replacevalue(aaa, inp,"name", "呆萌米雞狗");
    printf("[%d] %s\n",ll,aaa);
    return 0;
}

