#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define MAC_FILE "TEST.traces" // 대상 파일 이름
#define MAC_DIR "/Users/louxsoen/Documents/Univ/부채널연구/현대암호/전력분석 수업 실습자료/" // 파일 경로
#define MAC_NEW "REVISE.traces" // 새로 만들어진 파일 이름
#define PlaintextFN "plaintext.txt"
#define CiphertextFN "ciphertext.txt"
#define startpoint 22551
#define endpoint 31050

static unsigned char SBOX[256] = 
{
    0x63, 0x7c, 0x77, 0x7b, 0xf2, 0x6b, 0x6f, 0xc5, 0x30, 0x01, 0x67, 0x2b, 0xfe, 0xd7, 0xab, 0x76,
    0xca, 0x82, 0xc9, 0x7d, 0xfa, 0x59, 0x47, 0xf0, 0xad, 0xd4, 0xa2, 0xaf, 0x9c, 0xa4, 0x72, 0xc0,
    0xb7, 0xfd, 0x93, 0x26, 0x36, 0x3f, 0xf7, 0xcc, 0x34, 0xa5, 0xe5, 0xf1, 0x71, 0xd8, 0x31, 0x15,
    0x04, 0xc7, 0x23, 0xc3, 0x18, 0x96, 0x05, 0x9a, 0x07, 0x12, 0x80, 0xe2, 0xeb, 0x27, 0xb2, 0x75,
    0x09, 0x83, 0x2c, 0x1a, 0x1b, 0x6e, 0x5a, 0xa0, 0x52, 0x3b, 0xd6, 0xb3, 0x29, 0xe3, 0x2f, 0x84,
    0x53, 0xd1, 0x00, 0xed, 0x20, 0xfc, 0xb1, 0x5b, 0x6a, 0xcb, 0xbe, 0x39, 0x4a, 0x4c, 0x58, 0xcf,
    0xd0, 0xef, 0xaa, 0xfb, 0x43, 0x4d, 0x33, 0x85, 0x45, 0xf9, 0x02, 0x7f, 0x50, 0x3c, 0x9f, 0xa8,
    0x51, 0xa3, 0x40, 0x8f, 0x92, 0x9d, 0x38, 0xf5, 0xbc, 0xb6, 0xda, 0x21, 0x10, 0xff, 0xf3, 0xd2,
    0xcd, 0x0c, 0x13, 0xec, 0x5f, 0x97, 0x44, 0x17, 0xc4, 0xa7, 0x7e, 0x3d, 0x64, 0x5d, 0x19, 0x73,
    0x60, 0x81, 0x4f, 0xdc, 0x22, 0x2a, 0x90, 0x88, 0x46, 0xee, 0xb8, 0x14, 0xde, 0x5e, 0x0b, 0xdb,
    0xe0, 0x32, 0x3a, 0x0a, 0x49, 0x06, 0x24, 0x5c, 0xc2, 0xd3, 0xac, 0x62, 0x91, 0x95, 0xe4, 0x79,
    0xe7, 0xc8, 0x37, 0x6d, 0x8d, 0xd5, 0x4e, 0xa9, 0x6c, 0x56, 0xf4, 0xea, 0x65, 0x7a, 0xae, 0x08,
    0xba, 0x78, 0x25, 0x2e, 0x1c, 0xa6, 0xb4, 0xc6, 0xe8, 0xdd, 0x74, 0x1f, 0x4b, 0xbd, 0x8b, 0x8a,
    0x70, 0x3e, 0xb5, 0x66, 0x48, 0x03, 0xf6, 0x0e, 0x61, 0x35, 0x57, 0xb9, 0x86, 0xc1, 0x1d, 0x9e,
    0xe1, 0xf8, 0x98, 0x11, 0x69, 0xd9, 0x8e, 0x94, 0x9b, 0x1e, 0x87, 0xe9, 0xce, 0x55, 0x28, 0xdf,
    0x8c, 0xa1, 0x89, 0x0d, 0xbf, 0xe6, 0x42, 0x68, 0x41, 0x99, 0x2d, 0x0f, 0xb0, 0x54, 0xbb, 0x16 
    };


double cov(float *x, float *y, int size)
{
    double Sxy = 0, Sx = 0, Sy = 0;
    int i;
    for(i = 0 ; i < size ; i++)
    {
        Sxy += x[i] * y[i]; // E(XY)
        Sx += x[i];
        Sy += y[i];
    }
    return (Sxy - Sx * Sy / (double)size) / (double)size;
}
double corr(float *x, float *y, int size)
{
    double Sxy = 0, Sx = 0, Sy = 0, Sxx = 0, Syy = 0; // var(X) = E(X^2) - E(X)^2
    int i;
    for(i = 0 ; i < size ; i++)
    {
        Sxy += x[i] * y[i];// E(XY)
        Sx += x[i];
        Sy += y[i];
        Sxx += x[i] * x[i];
        Syy += y[i] * y[i];
    }
    return ((double)size*Sxy - Sx * Sy) /sqrt(((double)size * Sxx - Sx * Sx)*((double)size * Syy - Sy* Sy));
}
void subalign(float *data1, float *data2, int windowsize, int stepsize, int threshold, int TraceLength)
{
    // data 배열에 저장되어 잇는 전력 파형을 기준으로 data1 배열에 저장되어 있는 전력파혀을 정렬
    int m, j, size, maxcovpos, k;
    float *x, *y;
    double covval, maxcov;
    for(m = 0 ; m < (TraceLength - windowsize) ; m+= stepsize)
    {
        maxcovpos = 0;
        maxcov = 0;
        for(j = -threshold ; j < threshold ; j++)
        {
             if(j < 0)
            {
                x = data1 + m;
                y = data2 + m - j;
                size = windowsize + j;
            }
            else
            {
                x = data1 + m + j;
                y = data2 + m;
                size = windowsize - j;
            }
            covval = cov(x, y, size);
            if(covval > maxcov) 
            {
                maxcovpos = j;
                maxcov = covval;
            }
        }
        if(maxcovpos < 0)
        {
            for(k = m ; k < TraceLength + maxcovpos ; k++)
            {
                data2[k] = data2[k - maxcovpos];
            }
        }
        else
        {
            for(k = (TraceLength - maxcovpos - 1) ; k >= m ; k--)
            {
                data2[k + maxcovpos] = data2[k];
            }
        }
    }
}
void Alignment()
{   
    int windowsize = 500; // 부분부붑 정렬을 맞추고 싶은 파장의 길이
    int stepsize = 450; // 한 부분의 정렬을 맞춘 후에 몇 포인트씩 이동 후 다시 정렬을 맞출 것인지 결정
    int threshold = 100; // 좌우로 얼마나 흔들면서 cov 값을 계산해서 최대값과 이동할 포인트 수를 계산
    int err, TraceLength, TraceNum, i;
    char buf[256];
    FILE *DIRR, *DIRW;
    float *data, *data1;

    sprintf(buf, "%s%s", MAC_DIR, MAC_FILE);
    DIRR = fopen(MAC_FILE, "rb");
    if(DIRR == NULL)
    {
        printf("File Open Error! (RB PROBLEM)\n");
    }
    sprintf(buf, "%s%s", MAC_DIR, MAC_NEW);
    DIRW = fopen(MAC_NEW, "wb");
    if(DIRW == NULL)
    {
        printf("File Open Error! (WB PROBLEM)\n");
    }
    fread(&TraceLength, sizeof(int), 1, DIRR);
    fwrite(&TraceLength, sizeof(int), 1, DIRW);
    fread(&TraceNum, sizeof(int), 1, DIRR);
    fwrite(&TraceNum, sizeof(int), 1, DIRW);

    data = (float*)calloc(TraceLength, sizeof(float));
    data1 = (float*)calloc(TraceLength, sizeof(float));

    fread(data, sizeof(float), TraceLength, DIRR);
    fwrite(data, sizeof(float), TraceLength, DIRW);

    for(i = 1 ; i < TraceNum ; i++)
    {
        fread(data1, sizeof(float), TraceLength, DIRR);
        subalign(data, data1, windowsize, stepsize, threshold, TraceLength);
        fwrite(data1, sizeof(float), TraceLength, DIRW);
    }


    fclose(DIRR);
    fclose(DIRW);

    free(data);
    free(data1);
}

void CPA()
{
    unsigned char **plaintext = NULL; // 2000 * 16bytes의 평문을 2000 * 16크기의 배열에 저장
    unsigned char x, y, iv, hw_iv;
    char temp[34];
    float **data; // 정렬된 파형을 한번에 메모리에 올려서 작업
    double *Sx, *Sxx, *Sxy, *corrT, a, b, c;
    double Sy, Syy, max;
    char buf[256];
    int TraceLength, TraceNum, i, j, k, key, maxkey;
    FILE *DIRR, *DIRW;
    
    sprintf(buf, "%s%s", MAC_DIR, MAC_NEW);
    DIRR = fopen(buf, "rb");
    if(DIRR == NULL)
    {
        printf("File Open Error! (RB111 PROBLEM)\n");
    }
    fread(&TraceLength, sizeof(int), 1, DIRR);
    fread(&TraceNum, sizeof(int), 1, DIRR);
    
    data = (float **)calloc(TraceNum, sizeof(float*));
    for(i = 0 ; i < TraceNum ; i++)
    {
        data[i] = (float*)calloc(TraceLength, sizeof(float));
    }
    for(i = 0 ; i < TraceNum ; i++)
    {
        fread(data[i], sizeof(float), TraceLength, DIRR);
    }
    fclose(DIRR);

    sprintf(buf, "%s%s", MAC_DIR, PlaintextFN);
    DIRR = fopen(buf, "rb");
    if(DIRR == NULL)
    {
        printf("File Open Error! (Plaintext)\n");
    }

    plaintext = (unsigned char**)calloc(TraceNum, sizeof(unsigned char*));

    for(i = 0 ; i < TraceNum ; i++)
    {
        plaintext[i] = (unsigned char*)calloc(16, sizeof(unsigned char));
    }

    for(i = 0 ; i < TraceNum ; i++)
    {
        fread(temp, sizeof(char), 34, DIRR); // --> 16bytes로 바꿔서 Plaintext[i]에 저장 필요
        for(j = 0 ; j < 16 ; j++)
        {
            x = temp[2 * j];
            y = temp[2 * j + 1];
            if(x >= 'A' && x <= 'Z') x = x - 'A' + 10; // '0' ~ '9', 'A' ~ 'F', 'a' ~ 'f'
            else if(x >= 'a' && x <= 'z') x = x - 'a' + 10;
            else if(x >= '0' && x <= '9') x -= '0';
            if(y >= 'A' && y <= 'Z') y = y - 'A' + 10; // '0' ~ '9', 'A' ~ 'F', 'a' ~ 'f'
            else if(y >= 'a' && y <= 'z') y = y - 'a' + 10;
            else if(y >= '0' && y <= '9') y -= '0';
            plaintext[i][j] = x * 16 + y;
        }
    }

    Sx = (double*)calloc(TraceLength, sizeof(double));
    Sxx = (double*)calloc(TraceLength, sizeof(double));
    Sxy = (double*)calloc(TraceLength, sizeof(double));
    corrT = (double*)calloc(TraceLength, sizeof(double));

    for(i = 0 ; i < TraceNum ; i++)
    {
        for(j = startpoint ; j < endpoint; j++) // 범위지정
        {
            Sx[j] += data[i][j];
            Sxx[j] += data[i][j] * data[i][j];
        }
    }
    
    for(i = 0 ; i < 16 ; i++)
    {
        max = 0;
        maxkey = 0; 
        for(key = 0 ; key < 256 ; key++)
        {
            Sy = 0;
            Syy = 0;
            memset(Sxy, 0, sizeof(double)*TraceLength);
            for(j = 0 ; j < TraceNum ; j++)
            {
                iv = SBOX[plaintext[j][i] ^ key];
                hw_iv = 0;
                for(k = 0 ; k < 8 ; k++) hw_iv += ((iv >> k) & 1);
                Sy += hw_iv;
                Syy += hw_iv * hw_iv;
                for(k = startpoint ; k < endpoint ; k++) // 범위 지정
                {
                    Sxy[k] += hw_iv * data[j][k];
                }
            }
            for(k = startpoint ; k < endpoint ; k++)
            {
                a = ((double)TraceNum * Sxy[k] - Sx[k] * Sy);
                b = sqrt(((double)TraceNum * Sxx[k] - Sx[k] * Sx[k]));
                c = sqrt(((double)TraceNum * Syy - Sy * Sy));

                corrT[k] = a/ (b * c);
                if(fabs(corrT[k]) > max)
                {
                    maxkey = key;
                    max = fabs(corrT[k]);
                }
            }
            sprintf(buf, "%scorrtrace/%02dth_block_%02d.corrtrace", MAC_DIR, i, key);
            DIRW = fopen(buf, "wb");
            if(DIRW == NULL)
            {
                printf("File Open Error! (LAST)\n");
            }
            fwrite(corrT, sizeof(double), TraceLength, DIRW);
            fclose(DIRW);
        }
        printf("%02dth_block : maxkey(%02x), maxcorr(%lf)\n", i, maxkey, max);
    }
    free(Sx);
    free(Sxx);
    free(Sxy);
    free(corrT);   
    free(data);
    free(plaintext); 
}

int main()
{
    CPA();
}
    /*
    float X[10] = {1, 2, 3, 5, 6, 4, 6, 7, 6, 5};
    float Y[10] = {2, 3, 6, 8, 10, 7, 11, 12, 11, 9};
    printf("%lf, %lf", cov(X,Y,10), corr(X,Y,10));
    */