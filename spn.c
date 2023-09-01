/*
 * @filename:spn.c
 * @author:russ
 */
#pragma GCC optimize(3,"Ofast","inline")
#include <stdio.h>
#include "Cryptography.h"

u16 sp[65536];
u16 inv_sp[65535];
typedef struct spn{
    u32 key;
    u16 plain;
} spn;

spn fast_read(){
    char ch = getchar();
    spn result = {.plain= 0,.key = 0};
    int flag = 0;
    while ((ch >= '0' && ch <= '9') || (ch >= 'a' && ch <= 'f')) {
        if (flag) {
            result.plain *= 16;
            if (ch <= '9')
                result.plain += ch - '0';
            else
                result.plain += ch - 'a' + 10;
        }
        else {
            result.key *= 16;
            if (ch <= '9')
                result.key += ch - '0';
            else
                result.key += ch - 'a' + 10;
        }
        if ((ch = getchar()) == ' '){
            flag = 1;
            ch = getchar();
        };
    }
    return result;
}

inline void s_change(u16 u,u16* v){
    u16 tmp_v = 0;
    for (int i = 0; i < 4; ++i){
        // 获得u的第i个4位
        u8 tmp = (u & (0xf000 >> (4 * i))) >> (4 * (3 - i));
        // 将s转换后的数放到tmp_v中
        tmp_v ^= s_box[tmp] << (4 * (3 - i));
    }
    *v = tmp_v;
}

inline void inv_s_change(u16 v,u16* u){
    u16 tmp_u = 0;
    for (int i = 0; i < 4; ++i){
        // 获得v的第i个4位
        u8 tmp = (v & (0xf000 >> (4 * i))) >> (4 * (3 - i));
        // 将s转换后的数放到tmp_v中
        tmp_u ^= s_box_inv[tmp] << (4 * (3 - i));
    }
    *u = tmp_u;
}

inline void p_change(u16 v,u16* w){
    u16 tmp_w = 0;
    for (int i = 0; i < 16; ++i){
        int j = p_box[i] - 1;
        // 获得v的第j位
        u8 tmp = (v & (0x8000 >> j)) >> (15 - j);
        tmp_w ^= tmp << (15 - i);
    }
    *w = tmp_w;
}


void init(){
    for (int u = 0; u < 65536; ++u){
        u16 tmp_v = 0,tmp_w = 0;
        s_change(u,&tmp_v);
        p_change(tmp_v,&tmp_w);
        sp[u] = tmp_w;
        inv_sp[tmp_w] = u;
    }
}

int main() {
    init();
    // 读入第一行
    int n = read();
    u16 a1[n],a2[n];
    u32 key;
    u16 plain;
    spn data;
    for (int i = 0; i < n; ++i){
        // 读入16进制字符串
        data = fast_read();
        plain = data.plain;
        key = data.key;
        u16 w,u,v;
        w = plain;
        // 加密过程
        for (int j = 0; j < 4; ++j){
            u = w ^ (key & (0xffff0000 >> (4 * j))) >> (4 * (4 - j));
//            s_change(u,&v);
//            p_change(v,&w);
            w = sp[u];
        }
        s_change(u,&v);
        u16 y = v ^ (key & 0x0000ffff);
        for (int j = 3; j >= 0; j--) {
            u8 c = (y >> (j * 4)) & 0xf;
            putchar(c < 10 ? c + '0' : c - 10 + 'a');
        }
        putchar(' ');
        a1[i] = y;
        // 最后一bit取反
        y ^= 1;
        // 解密过程
        v = y ^ (key & 0x0000ffff);
        inv_s_change(v,&u);
        for (int j = 1; j < 5; ++j){
            w = u ^ ((key & (0xffff << (4 * j))) >> (4 * j));
//            p_change(w,&v);
//            inv_s_change(v,&u);
            u = inv_sp[w];
        }
        a2[i] = w;
        for (int j = 3; j >= 0; j--) {
            u8 c = (w >> (j * 4)) & 0xf;
            putchar(c < 10 ? c + '0' : c - 10 + 'a');
        }
        putchar('\n');
    }
}