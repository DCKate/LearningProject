//
//  main.cpp
//  ffmpegsnapshot
//
//  Created by kateh on 2017/11/28.
//  Copyright © 2017年 FAKE. All rights reserved.
//

#include <iostream>
#include "ffsnapshot.hpp"

int main(int argc, const char * argv[]) {
    // insert code here...
    std::cout << "Hello, World!\n";
    ffProcessor ff;
    ff.init("/Users/kateh/Documents/VBShare/f720_1M.mp4");
//    ff.decode();
    ff.getJPG("/Users/kateh/Documents/VBShare/yoyo.jpg");
    return 0;
}
