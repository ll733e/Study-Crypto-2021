#include <stdio.h>
#include <memory.h>
#include <string.h>


#define MAC_FILE "TEST.traces" // 대상 파일 이름
#define MAC_DIR "/Users/louxsoen/Documents/Univ/부채널연구/현대암호/전력분석 수업 실습자료/" // 파일 경로
#define MAC_NEW "REVISE.traces" // 새로 만들어진 파일 이름
#define PlaintextFN "plaintext.txt"
#define CiphertextFN "ciphertext.txt"
#define startpoint 22550
#define endpoint 31050

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