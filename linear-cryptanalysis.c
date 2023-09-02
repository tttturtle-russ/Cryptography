//
// Created by russ on 8/29/23.
//
#pragma GCC optimize(3,"Ofast","inline")
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>
#include <string.h>

typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;
// 正向S盒
u8 s_box[16] = {
        0x0e, 0x04, 0x0d, 0x01,
        0x02, 0x0f, 0x0b, 0x08,
        0x03, 0x0a, 0x06, 0x0c,
        0x05, 0x09, 0x00, 0x07
};

// 逆向S盒
u8 s_box_inv[16] = {
        0x0e,0x03,0x04,0x08,
        0x01,0x0c,0x0a,0x0f,
        0x07,0x0d,0x09,0x06,
        0x0b,0x02,0x00,0x05
};

// 正向(逆向)P盒
u8 p_box[16] = {
        1,5,9,13,
        2,6,10,14,
        3,7,11,15,
        4,8,12,16
};

u16 sp[65536];
u16 inv_sp[65536];

static inline u16 readn(){
    char ch;
    u16 n = 0;
    while ((ch = getchar()) != '\n'){
        n *= 10;
        n += ch - '0';
    }
    return n;
}

static inline u16 read(){
    char ch;
    u16 buf = 0;
    for(int i = 0; i < 4; ){
        ch = getchar();
        if(ch >= '0' && ch <= '9'){
            buf = (buf << 4) | (ch ^ 48);
            i++;
        }
        else if(ch >= 'a' && ch <= 'z'){
            buf = (buf << 4) | (ch - 'a' + 10);
            i++;
        }
    }
    return buf;
}


static inline void input(u16 *plain,u16* cipher) {
    for (int i = 0; i < 8000; ++i){
        plain[i] = read();
        cipher[i] = read();
    }
}

int key24_cnt[16][16] = {0};
int key13_cnt[2][16][16] = {0};
int key13_total_cnt[16][16] = {0};
u8 key1,key2,key3,key4;
u32 key;

static inline void s_change(u16 u,u16* v){
    u16 tmp_v = 0;
    for (int i = 0; i < 4; ++i){
        // 获得u的第i个4位
        u8 tmp = (u & (0xf000 >> (4 * i))) >> (4 * (3 - i));
        // 将s转换后的数放到tmp_v中
        tmp_v ^= s_box[tmp] << (4 * (3 - i));
    }
    *v = tmp_v;
}

static inline void p_change(u16 v,u16* w){
    u16 tmp_w = 0;
    for (int i = 0; i < 16; ++i){
        int j = p_box[i] - 1;
        // 获得v的第j位
        u8 tmp = (v & (0x8000 >> j)) >> (15 - j);
        tmp_w ^= tmp << (15 - i);
    }
    *w = tmp_w;
}

static void init(){
    for (int u = 0; u < 65536; ++u){
        u16 tmp_v = 0,tmp_w = 0;
        s_change(u,&tmp_v);
        p_change(tmp_v,&tmp_w);
        sp[u] = tmp_w;
        inv_sp[tmp_w] = u;
    }
}

static inline bool spn(u16 x,u16 y ) {
    u16 u,v,w = x;
    for (int i = 0; i < 4; ++i){
        u = w ^ (key & (0xffff0000 >> (4 * i))) >> (4 * (4 - i));
        w = sp[u];
    }
    s_change(u,&v);
    return y == (v ^ (key & 0x0000ffff));
}


int main() {
    init();
    int n = readn();
    u16 plain[8000], cipher[8000];
    for (int j = 0; j < n; ++j) {
        bool has_found = false;
        input(plain, cipher);
        memset(key24_cnt, 0, sizeof(int) * 256);
        for (int j = 0; j < 8000; ++j) {
            u16 x = plain[j], y = cipher[j];
            u8 y2 = (y & 0x0f00) >> 8;
            u8 y4 = y & 0x000f;
            u8 x8 = (x >> 8) & 0x1;  //取 x8
            u8 x7 = (x >> 9) & 0x1;  //取 x7
            u8 x5 = (x >> 11) & 0x1; //取 x5
            for (u8 l1 = 0; l1 < 16; ++l1) {
                for (u8 l2 = 0; l2 < 16; ++l2) {
                    u8 v2 = l1 ^ y2;
                    u8 v4 = l2 ^ y4;
                    u8 u2 = s_box_inv[v2];
                    u8 u4 = s_box_inv[v4];
                    u8 result = (x8) ^ (x7) ^ (x5) ^
                                (u2 & 0x0001) ^ //取 u8
                                ((u2 >> 2) & 0x1) ^ //取 u6
                                (u4 & 0x0001) ^ //取 u16
                                ((u4 >> 2) & 0x1); // 取 u14
                    if (!result)
                        key24_cnt[l1][l2]++;
                }
            }
        }

//        for(int L1 = 0; L1 < 16; ++L1){
//            for(int L2 = 0; L2 < 16; ++L2){
//                key24_cnt[L1][L2] = abs(key24_cnt[L1][L2] - 4000);
//            }
//        }

        for (int i = 0; i < 64; ++i){
            int max = -1;
            key2 = -1;
            key4 = -1;
            for (u8 l1 = 0; l1 < 16; ++l1) {
                for (u8 l2 = 0; l2 < 16; ++l2) {
                    if (key24_cnt[l1][l2] == 0)
                        continue;
                    int num = abs(key24_cnt[l1][l2] - 4000);
                    if (num > max) {
                        max = num;
                        key2 = l1;
                        key4 = l2;
                    }
                }
            }
            key24_cnt[key2][key4] = 0;
            // 求key1和key3
            memset(key13_cnt, 0, sizeof(int) * 512);
            for (int j = 0; j < 8000; ++j) {
                u16 x = plain[j], y = cipher[j];
                u8 y1 = (y & 0xf000) >> 12;
                u8 y2 = (y & 0x0f00) >> 8;
                u8 y3 = (y & 0x00f0) >> 4;
                u8 y4 = (y & 0x000f);
                u8 x1 = (x >> 15) & 0x1; //取x1
                u8 x2 = (x >> 14) & 0x1; //取x2
                u8 x4 = (x >> 12) & 0x1; //取x4
                u8 x9 = (x >> 7) & 0x1; //取x9
                u8 x10 = (x >> 6) & 0x1; //取x10
                u8 x12 = (x >> 4) & 0x1; //取x12
                for (u8 l1 = 0; l1 < 16; ++l1) {
                    for (u8 l2 = 0; l2 < 16; ++l2) {
                        u8 v1 = l1 ^ y1;
                        u8 v2 = y2 ^ key2;
                        u8 v3 = l2 ^ y3;
                        u8 v4 = y4 ^ key4;
                        u8 u1 = s_box_inv[v1];
                        u8 u2 = s_box_inv[v2];
                        u8 u3 = s_box_inv[v3];
                        u8 u4 = s_box_inv[v4];
                        u8 result = (x1) ^ (x2) ^ (x4) ^
                                    ((u1 >> 3) & 0x1) ^ //取u1
                                    ((u2 >> 3) & 0x1) ^ //取u5
                                    ((u3 >> 3) & 0x1) ^ //取u9
                                    ((u4 >> 3) & 0x1); //取u13
                        if (!result)
                            key13_cnt[0][l1][l2]++;
                        result = (x9) ^ (x10) ^ (x12) ^
                                 ((u1 >> 1) & 0x1) ^ //取u3
                                 ((u2 >> 1) & 0x1) ^ //取u7
                                 ((u3 >> 1) & 0x1) ^ //取u11
                                 ((u4 >> 1) & 0x1); //取u15
                        if (!result)
                            key13_cnt[1][l1][l2]++;
                    }
                }
            }
            for (u8 l1 = 0; l1 < 16; ++l1) {
                for (u8 l2 = 0; l2 < 16; ++l2) {
                    key13_cnt[0][l1][l2] = abs(key13_cnt[0][l1][l2] - 4000);
                    key13_cnt[1][l1][l2] = abs(key13_cnt[1][l1][l2] - 4000);
                    key13_total_cnt[l1][l2] = key13_cnt[0][l1][l2] + key13_cnt[1][l1][l2];
                }
            }
            for (int i = 0; i < 2; ++i){
                max = -1;
                for (u8 l1 = 0; l1 < 16; ++l1) {
                    for (u8 l2 = 0; l2 < 16; ++l2) {
                        if (key24_cnt[l1][l2] > max) {
                            max = key24_cnt[l1][l2];
                            key1 = l1;
                            key3 = l2;
                        }
                    }
                }
                key13_total_cnt[key1][key3] = 0;
                u16 tail = (key1 << 12) | (key2 << 8) | (key3 << 4) | key4;
                // 穷举
                for (int j = 65536; j >= 0; --j){
                    key = (j << 16) | tail;
                    int count = 0;
                    for (; count < 4;count++){
                        if (!spn(plain[count],cipher[count]))
                            break;
                    }
                    if (count == 4){
                        has_found = true;
                        break;
                    }
                }
                if (has_found)
                    break;
            }
            if (has_found)
                break;
        }
        for (int i = 0; i < 8; ++i){
            char c = (key >> (28 - 4 * i)) & 0xf;
            if (c <= 9)
                putchar(c + '0');
            else
                putchar(c - 10 + 'a');
        }
        putchar('\n');
    }
    return 0;
}