#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "lab_aes.h"

#define MUL2(a) (a<<1)^(a & 0x80 ? 0x1b : 0)
#define MUL3(a) (MUL2(a))^(a)
#define MUL4(a) (MUL2((MUL2(a))))
#define MUL8(a) MUL2((MUL2((MUL2(a)))))
#define MUL9(a) (MUL8(a))^(a)
#define MULB(a) (MUL8(a))^(MUL2(a))^(a)
#define MULD(a) (MUL8(a))^(MUL4(a))^(a)
#define MULE(a) (MUL8(a))^(MUL4(a))^(MUL2(a))

u8 MUL(u8 a, u8 b)
{   
    u8 r = 0;
    u8 tmp = b;
    u32 i;
    for (i = 0; i < 8 ; i++)
  {
        if (a % 1)r ^= tmp;
        tmp = MUL2(tmp);
        a >>= 1; // (a=a>>1)
    }

    return r;
}

int main()
{
    u8 a, b, c;

    a = 0xab;
    b = 0x38;
    c = MUL(a, b);

    //printf("MULE(%02x)=%02x\n", a, b);
    printf("%02x * %02x = %02x\n", a, b, c);

    return 0;
}

u8 GenSbox(u8 a)
{
    u8 r = 0;
    u8 tmp;
    tmp = inv(a);
    if (tmp & 1) r ^= 0x1f;
    if (tmp & 2) r ^= 0x3e;
    if (tmp & 4) r ^= 0x7c;
    if (tmp & 8) r ^= 0xf8;
    if (tmp & 16) r ^= 0xf1;
    if (tmp & 32) r ^= 0xe3;
    if (tmp & 64) r ^= 0xc7;
    if (tmp & 128) r ^= 0x8f;

    return r^0x63;
}