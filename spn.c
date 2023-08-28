/*
 * @filename:spn.c
 * @author:russ
 */

#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
// S盒
u16 s_box[16] = {
        0x0e, 0x04, 0x0d, 0x01,
        0x02, 0x0f, 0x0b, 0x08,
        0x03, 0x0a, 0x06, 0x0c,
        0x05, 0x09, 0x00, 0x07
};

// P盒
//u8 p_box[16] = {
//        1,5,9,13,
//        2,6,10,14,
//        3,7,11,15,
//        4,8,12,16
//};

u8 p_box[16] = {
        1,5,9,13,
        2,6,10,14,
        3,7,11,15,
        4,8,12,16
};

void s_change(u16 u,u16* v){
    u16 tmp_v = 0;
    for (int i = 0; i < 4; ++i){
        // 获得u的第i个4位
        u8 tmp = (u & (0xf000 >> (4 * i))) >> (4 * (3 - i));
        // 将s转换后的数放到tmp_v中
        tmp_v ^= s_box[tmp] << (4 * (3 - i));
    }
    *v = tmp_v;
}

void p_change(u16 v,u16* w){
    u16 tmp_w = 0;
    for (int i = 0; i < 16; ++i){
        int j = p_box[i] - 1;
        // 获得v的第j位
        u8 tmp = (v & (0x8000 >> j)) >> (15 - j);
        tmp_w ^= tmp << (15 - i);
    }
    *w = tmp_w;
}


int main() {
    int n;
    n = scanf("%d\n",&n);
    u32 key;
    u16 plain;
    for (int i = 0; i < n; ++i){
        // 读入16进制字符串
        scanf("%x %hx",&key,&plain);
        u16 w,u,v;
        w = plain;
        // 加密过程
        for (int j = 0; j < 4; ++j){
            u = w ^ (key & (0xffff0000 >> (4 * j))) >> (4 * (4 - j));
            s_change(u,&v);
            p_change(v,&w);
        }
        u16 y = v ^ (key & 0x0000ffff);
        printf("%x ",y);
        // 最后一bit取反
        y ^= 1;
        // 解密过程
        for (int j = 0; j < 4; ++j){

        }
    }
}
