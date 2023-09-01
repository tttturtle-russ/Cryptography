//
// Created by russ on 8/29/23.
//

#include <stdio.h>
#include <stdbool.h>
#include <math.h>
#include <stdlib.h>
#include <string.h>
typedef unsigned char u8;
typedef unsigned short u16;
typedef unsigned int u32;
typedef unsigned long long u64;

typedef struct big_num{
    char* left;
    u64 len;
}bn;

static inline long long gcd(long long  a,long long  b){
    while (b){
        int tmp = a % b;
        a = b;
        b = tmp;
    }
    return a;
}

// 扩展欧几里得算法求逆元
static inline u64 exgcd(u64 a, u64 b, int * x, int * y) {
    if(b == 0) {
        *x = 1; // 设置b=0时的特殊解
        *y = 0;
        return a;
    }
    int ans = exgcd(b, a % b, x, y);
    int t = *x; // 将x2, y2换算成x1, y1
    *x = *y;
    *y = t - a / b * (*y);
    return ans;
}

static inline bool isPrime(u64 n){
    for (int i = 2; i < (int)sqrt(n); ++i) {
        if (n % i == 0)
            return false;
    }
    return true;
}

static inline bn read(){
    char ch;
    bn result = {.left = (char*) malloc(sizeof (char) * 2048),.len=0};
    while ((ch = getchar()) != ' ' && ch != '\n'){
        result.left[result.len] = ch;
        result.len ++;
    }
    return result;
}

static inline u64 bn2num(bn b){
    u64 ans = 0;
    for (int i = 0; i < b.len; ++i) {
        ans *= 10;
        ans += b.left[i] - '0';
    }
    return ans;
}

bn multiply(bn a,bn b){
    bn ans = {.left = (char *) malloc(sizeof(char) * 4096),.len=0};
    memset(ans.left,0,ans.len);
    for (int i = 0; i < a.len; ++i) {
        for (int j = 0; j < b.len; ++j) {
            ans.left[i + j] += (a.left[i] - '0') * (b.left[j] - '0');
            if (ans.left[i + j] >= 10){
                ans.left[i + j + 1] += ans.left[i + j] / 10;
                ans.left[i + j] %= 10;
            }
        }
    }
    ans.len = a.len + b.len;
    while (ans.left[ans.len] == 0)
        ans.len --;
    return ans;
}


void read_ll(long long * e,long long *q,long long *p) {
    char ch;
    *e = 0;
    *q = 0;
    *p = 0;
    while ((ch = getchar()) != ' ' && ch != '\n') {
        *e *= 10;
        *e += ch - '0';
    }
    while ((ch = getchar()) != ' ' && ch != '\n') {
        *q *= 10;
        *q += ch - '0';
    }
    while ((ch = getchar()) != ' ' && ch != '\n') {
        *p *= 10;
        *p += ch - '0';
    }
}

int main(){
    int num = 0;
    scanf("%d",&num);
    getchar();
    for (int i = 0; i < num; ++i) {
//        bn e,q,p;
//        e = read();
//        q = read();
//        p = read();
        long long e,q,p;
        read_ll(&e,&q,&p);
        if (!isPrime(p) || !isPrime(q)){
            puts("ERROR\n");
            continue;
        }
        long long n = p * q;
//        bn n = multiply(p,q);
        long long  phi = (p - 1) * (q - 1);
        if (gcd(e,phi) != 1){
            puts("ERROR\n");
            continue;
        }
        int x,y;
        exgcd(e,phi,&x,&y);
        if (x < 0)
            x += phi;
        printf("%lld\n",x);
    }
    return 0;
}