#include <iostream>
#include <string>
using namespace std;
typedef unsigned char u8;

inline void prt(u8 a[16])
{
    for(int index = 0 ; index < 16 ; index++)
    {
        cout << hex << a[index] << " ";
        if((index + 1) % 4 == 0)
        cout << endl;
    }
    cout << endl;
}

// AES, Key size - 128bit;
int main() 
{
    u8 state[2][16] = {
    {
    0x6E, 0x74, 0x77, 0x68,
    0x6F, 0x74, 0x68, 0x61,
    0x6D, 0x65, 0x61, 0x70,
    0x61, 0x72, 0x74, 0x70
    }, 
    {
    0x65, 0x13, 0x13, 0x13,
    0x6E, 0x13, 0x13, 0x13,
    0x73, 0x13, 0x13, 0x13,
    0x13, 0x13, 0x13, 0x13
    }
    };
    
    u8 key[16] = {
    0x64, 0x6C, 0x73, 0x6C,
    0x6F, 0x6F, 0x6D, 0x03,
    0x6E, 0x6F, 0x61, 0x03,
    0x74, 0x6B, 0x6C, 0x03
    };

    for(int state_num = 0 ; state_num < 2 ; state_num++)
    {
        cout << state_num+1 << "번째 State" << endl;
        prt(state[state_num]);
        cout << endl;
    }
    prt(key);

    
}
