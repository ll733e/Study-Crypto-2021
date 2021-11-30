#include <stdio.h>
#include <memory.h>
#include <string.h>

#define DIR "/Users/louxsoen/Desktop/hello/myfile.txt"
int main()
{
    FILE *a;
    char buf[256];
    a = fopen(DIR, "r");
    
    if(a == NULL)
    {
        printf("파일 열기 실패\n");
        return -1;
    }
    else
    {
        printf("파일 열기 성공\n");
        while(fgets(buf, sizeof(buf), a) != NULL)
        {
            printf("%s", buf);
            memset(buf, 0, sizeof(buf));
        }
    }
    fclose(a); 
}

    /*
    memset(buf, 0, sizeof(buf));
    scanf("%s", buf);
    buf[strlen(buf)] = '\n';
    fputs(buf, a);
    */