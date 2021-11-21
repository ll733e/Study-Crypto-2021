#include <iostream>
#include <string>
using namespace std;


int main() 
{
    string pl;  // 평문

    int width, height;
    cout << "평문 입력 >> ";
    cin >> pl;
    
    int pl_len = pl.length();
    
    for(int re = 0 ; re < pl_len ; re++)
    cout << dec << pl[re] << " ";
}