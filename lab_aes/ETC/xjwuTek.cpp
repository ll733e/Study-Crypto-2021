#include <iostream>
#include <string>
using namespace std;

// AES, Key size - 128bit;
int main() 
{
    string pl;  // 평문

    int width, height;
    string *p = &pl;
    cout << "평문 입력 >> ";
    cin >> pl;
    
    int pl_len = pl.length();
    
    for(int re = 0 ; re < pl_len ; re++)
    cout << dec << p[re] << " ";
}