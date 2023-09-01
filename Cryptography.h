//
// Created by russ on 8/29/23.
//

#ifndef CRYPTOGRAPHY_CRYPTOGRAPHY_H
#define CRYPTOGRAPHY_CRYPTOGRAPHY_H

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





#endif //CRYPTOGRAPHY_CRYPTOGRAPHY_H
