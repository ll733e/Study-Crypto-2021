#include <assert.h>
#include <stdio.h>
#include <stdlib.h>
#include "lab_aes.h"

#define MUL2(a) (a<<1)^(a & 0x80 ? 0x1b : 0)
#define MUL3(a) (MUL2(a))^(a)
#define MUL4(a) (MUL2((MUL2(A))))
#define MUL8(a) MUL2((MUL2((MUL2(a)))))
#define MUL9(a)
#define MULB(a)
#define MULD(a)
#define MULE(a)

int main()
{
    u8 a, b;

    a = 0xab;
    b = MUL2(a);

    printf("MUL2(%02x)=%02x\n", a, b);

    return 0;
}