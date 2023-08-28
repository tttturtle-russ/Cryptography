/*
 * @filename:spn.c
 * @author:russ
 */
#pragma GCC optimize(3,"Ofast","inline")
#include <stdio.h>
#include <stdlib.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

#define MAX_BUFSIZE (1 << 22)
char _b[MAX_BUFSIZE],*_b1 = NULL,*_b2 = NULL;
//读一个字符
#define getch() (_b1==_b2?_b2=_b+fread(_b,1,MAX_BUFSIZE,stdin),_b1=_b,*(_b1++):*(_b1++))

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

u16 buf[2000000];

int main() {
    setbuf(stdout,buf);
    // 读入第一行
    char str[1000000];
    u32 n ;
    scanf("%s",str);
    // 读掉换行符
    getchar();
    n = strtoul(str,NULL,10);
    u16 a1[n],a2[n];
    u32 key;
    u16 plain;
    spn data;
    for (int i = 0; i < n; ++i){
        // 读入16进制字符串
        //scanf("%x %hx",&key,&plain);
        data = fast_read();
        plain = data.plain;
        key = data.key;
        u16 w,u,v;
        w = plain;
        // 加密过程
        for (int j = 0; j < 4; ++j){
            u = w ^ (key & (0xffff0000 >> (4 * j))) >> (4 * (4 - j));
            s_change(u,&v);
            p_change(v,&w);
        }
        u16 y = v ^ (key & 0x0000ffff);
        a1[i] = y;
        // 最后一bit取反
        y ^= 1;
        // 解密过程
        v = y ^ (key & 0x0000ffff);
        inv_s_change(v,&u);
        for (int j = 1; j < 5; ++j){
            w = u ^ ((key & (0xffff << (4 * j))) >> (4 * j));
            p_change(w,&v);
            inv_s_change(v,&u);
        }
        a2[i] = w;
    }
    for (int i = 0; i < n; ++i)
    {
        for (int j = 3; j >= 0; j--) {
            unsigned char c = (a1[i] >> (j * 4)) & 0xf;
            putchar(c < 10 ? c + '0' : c - 10 + 'a');
        }
//        putchar((a1[i] >> 8));
//        putchar(a1[i] != 0);
        putchar(' ');
        for (int j = 3; j >= 0; j--) {
            unsigned char c = (a2[i] >> (j * 4)) & 0xf;
            putchar(c < 10 ? c + '0' : c - 10 + 'a');
        }
        putchar('\n');
//        putchar(a2[i] >> 8);
//        putchar(a2[i] != 0);
    }
        //        printf("%04x %04x\n",a1[i],a2[i]);
}