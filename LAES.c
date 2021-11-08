#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "LAES.h"
#define MUL2(a) (a<<1)^(a&0x80?0x1b:0)
#define MUL3(a) MUL2(a)^a
#define MUL4(a) MUL2((MUL2(a)))
#define MUL8(a) MUL2((MUL2((MUL2(a)))))
#define MUL9(a) (MUL8(a))^(a)
#define MULB(a) (MUL8(a))^(MUL2(a))^(a)
#define MULD(a) (MUL8(a))^(MUL4(a))^(a)
#define MULE(a) (MUL8(a))^(MUL4(a))^(MUL2(a))
typedef unsigned char u8;
typedef unsigned int u32;

u32 u4byte_in(u8 *x)
{
    return (x[0] << 24) | (x[1] << 16) | (x[2] << 8) | x[3];
}

void u4byte_out(u8 *x, u32 y)
{
    x[0] = (y >> 24) & 0xFF;
    x[1] = (y >> 16) & 0xFF;
    x[2] = (y >> 8) & 0xFF;
    x[3] = y & 0xFF;
}

void AES_KeyWordToByte(u32 W[], u8 RK[])
{
    int i;
    for(i = 0 ; i < 44 ; i++)
    u4byte_out(RK + 4 * i, W[i]); 
}

u32 Rcons[10] = { 0x01000000, 0x02000000, 0x04000000, 0x08000000, 0x10000000, 0x20000000, 0x40000000, 0x80000000, 0x1b000000, 0x36000000 };

#define RotWord(x) ((x<<8) | (x>>24))
#define SubWord(x)  \
    ((u32)Sbox[(u8)(x >> 24)] << 24) \
    | ((u32)Sbox[(u8)((x >> 16) & 0xFF)] << 16) \
    | ((u32)Sbox[(u8)((x >> 8) & 0xFF)] << 8) \
    | ((u32)Sbox[(u8)(x & 0xff)]) \

void RoundkeyGeneration128(u8 MK[], u8 RK[])
{
    u32 W[44];
    int i;
    u32 T;

    W[0] = u4byte_in(MK); // W[0] = MK[0] || MK[1] || MK[2] || MK[3]
    W[1] = u4byte_in(MK + 4);
    W[2] = u4byte_in(MK + 8);
    W[3] = u4byte_in(MK + 12);

    for(i = 0 ; i < 10 ; i++)
    {
        //T = G_func(W[4 * i + 3])
        T = W[4 * i + 3];
        T = RotWord(T);
        T = SubWord(T);
        T ^= Rcons[i];

        W[4 * i + 4] = W[4 * i] ^ T;
        W[4 * i + 5] = W[4 * i + 1] ^ W[4 * i + 4];
        W[4 * i + 6] = W[4 * i + 2] ^ W[4 * i + 5];
        W[4 * i + 7] = W[4 * i + 3] ^ W[4 * i + 6];
    }
    AES_KeyWordToByte(W, RK);
}

void AES_KeySchedule(u8 MK[], u8 RK[], int keysize)
{
    if(keysize == 128)    RoundkeyGeneration128(MK, RK);
    //if(keysize == 192)    RoundkeyGeneration192(MK, RK);
    //if(keysize == 256)    RoundkeyGeneration256(MK, RK);
}

void AddRoundKey(u8 S[16], u8 RK[16])
{
    S[0] ^= RK[0]; S[1] ^= RK[1]; S[2] ^= RK[2]; S[3] ^= RK[3];
    S[4] ^= RK[4]; S[5] ^= RK[5]; S[6] ^= RK[6]; S[7] ^= RK[7];
    S[8] ^= RK[8]; S[9] ^= RK[9]; S[10] ^= RK[10]; S[11] ^= RK[11];
    S[12] ^= RK[12]; S[13] ^= RK[13]; S[14] ^= RK[14]; S[15] ^= RK[15];
}

void SubBytes(u8 S[16])
{
    S[0] = Sbox[S[0]]; S[1] = Sbox[S[1]]; S[2] = Sbox[S[2]]; S[3] = Sbox[S[3]];
    S[4] = Sbox[S[4]]; S[5] = Sbox[S[5]]; S[6] = Sbox[S[6]]; S[7] = Sbox[S[7]];
    S[8] = Sbox[S[8]]; S[9] = Sbox[S[9]]; S[10] = Sbox[S[10]]; S[11] = Sbox[S[11]];
    S[12] = Sbox[S[12]]; S[13] = Sbox[S[13]]; S[14] = Sbox[S[14]]; S[15] = Sbox[S[15]];
}

void Shiftrows(u8 S[16])
{
    u8 temp;
    temp = S[1]; S[1] = S[5]; S[5] = S[9]; S[9] = S[13]; S[13] = temp;
    temp = S[2]; S[2] = S[10]; S[10] = temp; temp = S[6]; S[6] = S[14]; S[14] = temp;
    temp = S[15]; S[15] = S[11]; S[11] = S[7]; S[7] = S[3]; S[3] = temp;
}

void Mixcolumns(u8 S[16])
{
    u8 temp[16];

    for(int i = 0 ; i < 16; i += 4) // 비효율적이라고 생각함
    {  
        temp[i] = MUL2(S[i]) ^ MUL3(S[i+1]) ^ S[i+2] ^ S[i+3];
        temp[i+1] = S[i] ^ MUL2(S[i+1]) ^ MUL3(S[i+2]) ^ S[i+3];
        temp[i+2] = S[i] ^ S[i+1] ^ MUL2(S[i+2]) ^ MUL3(S[i+3]);
        temp[i+3] = MUL3(S[i]) ^ S[i+1] ^ S[i+2] ^ MUL2(S[i+3]);
    }
    S[0] = temp[0]; S[1] = temp[1]; S[2] = temp[2]; S[3] = temp[3];
    S[4] = temp[4]; S[5] = temp[5]; S[6] = temp[6]; S[7] = temp[7];
    S[8] = temp[8]; S[9] = temp[9]; S[10] = temp[10]; S[11] = temp[11];
    S[12] = temp[12]; S[13] = temp[13]; S[14] = temp[14]; S[15] = temp[15];
    
    /*
    temp[0] = MUL2(S[0]) ^ MUL3(S[1]) ^ S[2] ^ S[3];
    temp[1] = S[0] ^ MUL2(S[1]) ^ MUL3(S[2]) ^ S[3];
    temp[2] = S[0] ^ S[1] ^ MUL2(S[2]) ^ MUL3(S[3]);
    temp[3] = MUL3(S[0]) ^ S[1] ^ S[2] ^ MUL2(S[3]);
    */
}

void prt(u8 CT[16])
{   
    printf("\n");
    for(int i = 0 ; i < 4 ; i++)
    {
    for(int k = 0 ; k < 4 ; k++)
    printf("%02X ", CT[k * 4 + i]);
    printf("\n");
    }
    printf("\n");
}

void AES_ENC(u8 PT[16], u8 RK[16],u8 CT[16], int keysize)
{
    int Nr = keysize / 32 + 6;
    int i;

    for(i = 0 ; i < 16 ; i++)
    CT[i] = PT[i];

    // prt(RK);
    AddRoundKey(CT, RK);
    // prt(CT);

    for(int i = 0 ; i < Nr - 1 ; i++)
    {   
        // printf("%d라운드 SubBytes || ", i+1);
        SubBytes(CT);
        // prt(CT);
        // printf("%d라운드 ShiftRows || ", i+1);
        Shiftrows(CT);
        // prt(CT);
        // printf("%d라운드 Mixcolumns || ", i+1);
        Mixcolumns(CT);
        // prt(CT);
        // printf("%d라운드 Addroundkey || ", i+1);
        AddRoundKey(CT, RK + 16 * (i + 1));
        // prt(CT);
    }
    SubBytes(CT);
    Shiftrows(CT);
    AddRoundKey(CT, RK + 16 * 10);
}

int main()
{
    int i;
    u8 PT[16] = { 0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77, 0x88, 0x99, 0xaa, 0xbb, 0xcc, 0xdd, 0xee, 0xff };
    u8 MK[16] = { 0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef, 0x12, 0x34, 0x56, 0x78, 0x9a, 0xbc, 0xde, 0xf0 };
    u8 CT[16] = { 0x00, };
    u8 RK[240] = { 0x00, }; 
    int keysize = 128;

    AES_KeySchedule(MK, RK, keysize);
    AES_ENC(PT, RK, CT, keysize);

    for(int i = 0 ; i < 16 ; i++)
    printf("%02x ", CT[i]);
    printf("\n");
}