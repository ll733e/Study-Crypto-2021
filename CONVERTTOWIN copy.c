#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <math.h>
#include <time.h>

#define _FOLD_ "\\vmware-host\\Shared Folders\\데스크탑\\CPA" // 파일 경로
#define TraceFN "AES.traces" // 대상 파일 이름
#define AlignedTraceFN "AlignedAES.traces" // 새로 만들어질 파일 이름

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
        for(j =- threshold ; j < threshold ; j++)
        {
            maxcovpos = 0;
            maxcov = 0;
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
}

void Alignment()
{   
    int windowsize = 500; // 부분부붑 정렬을 맞추고 싶은 파장의 길이
    int stepsize = 450; // 한 부분의 정렬을 맞춘 후에 몇 포인트씩 이동 후 다시 정렬을 맞출 것인지 결정
    int threshold = 100; // 좌우로 얼마나 흔들면서 cov 값을 계산해서 최대값과 이동할 포인트 수를 계산
    int err, TraceLength, TraceNum, i;
    char buf[256];
    FILE *rfp, *wfp;
    float *data, *data1;

    sprintf_s(buf, 256 * sizeof(char), "%s%s", _FOLD_, TraceFN);
    if((err = fopen_s(&rfp, buf, "rb")))
    {
        printf("File Open Error!\n");
    }
    sprintf_s(buf, 256 * sizeof(char), "%s%s", _FOLD_, AlignedTraceFN);
    if((err = fopen_s(&wfp, buf, "wb")))
    {
        printf("File Open Error!\n");
    }
    fread(&TraceLength, sizeof(int), 1, rfp);
    fwrite(&TraceLength, sizeof(int), 1, wfp);
    fread(&TraceNum, sizeof(int), 1, rfp);
    fwrite(&TraceNum, sizeof(int), 1, wfp);

    data = (float*)calloc(TraceLength, sizeof(float));
    data1 = (float*)calloc(TraceLength, sizeof(float));

    fread(&data, sizeof(float), TraceLength, rfp);
    fwrite(&data, sizeof(float), TraceLength, wfp);
    for(i = 1 ; i < TraceNum ; i++)
    {
        fread(data1, sizeof(float), TraceLength, rfp);
        subalign(data, data1, windowsize, stepsize, threshold, TraceLength);
        fwrite(data1, sizeof(float), TraceLength, wfp);
    }

    fclose(rfp);
    fclose(wfp);

    free(data);
    free(data1);
}

int main()
{
    float X[10] = {1, 2, 3, 5, 6, 4, 6, 7, 6, 5};
    float Y[10] = {2, 3, 6, 8, 10, 7, 11, 12, 11, 9};
    printf("%lf, %lf", cov(X,Y,10), corr(X,Y,10));
    
    Alignment();
}