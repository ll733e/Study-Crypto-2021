#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main()
{
    int my_lotto[6];
    int real_lotto[3] = {1, 5, 7};
    int i = 0, n = 0;
    srand((unsigned)time(NULL));
    
    while(1)
    {
        int temp = (rand() % 10) + 1;

        for(i = 0; i < n; i++)
        {
            if (my_lotto[i] == temp)    
            break;
        }
        if (n == i)     my_lotto[n++] = temp;
        if (n >= 3)     break;
    }

    for(int i = 0 ; i < 3 ; i++)
    printf("my_lotto[%d]=%d\n", i, my_lotto[i]);
    printf("real_lotto = [%d %d %d]\n", real_lotto[0], real_lotto[1], real_lotto[2]);
    printf("my_lotto = [%d %d %d]\n", my_lotto[0], my_lotto[1], my_lotto[2]);
    
}