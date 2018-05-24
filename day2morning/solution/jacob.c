#include <stdio.h>

#define M 6
#define N 8

int main(void)
{
    float t[M][N];
    int i,j;
    for (i=0;i<M;i++)
        for (j=0;j<N;j++)
            t[i][j]=0.0;
    for(i=0;i<M;i++)
        t[i][0]=1.0;
    for(j=0;j<N;j++)
        t[0][j]=1.0;
    for(i=1;i<M-1;i++)
        for (j=1;j<N-1;j++)
            t[i][j]=0.25*(t[i+1][j]+t[i-1][j]+t[i][j-1]+t[i][j+1]);
    for (i=0;i<M;i++) {
        for (j=0;j<N;j++)
            printf("%5.3f\t",t[i][j]);
        printf("\n");
    }
    return 0;
}
