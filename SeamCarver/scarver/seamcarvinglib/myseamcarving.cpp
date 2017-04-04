#include <iostream>
#include "c_exchange.h"
using namespace std;

#define INF 1e8
#define store(x) pass(out,x,i,j,k)  //utility macro for storing pixel value

int dr[4]={-1,0,1,0},dc[4]={0,1,0,-1},N,M;
int image[1000][1000][3];
int newImage[1000][1000][3];

void copyArray(int a[1000][1000][3], int b[1000][1000][3], int r=1000) {
  for(int i = 0; i < r; i++) {
    for(int j = 0; j < 1000; j++) {
      for(int k = 0; k < 3; k++)
        a[i][j][k] = b[i][j][k];
    }
  }
}

bool check(int r,int c) {
    return r>=0 && r<N && c>=0 && c<M;
}

float Energy(int r,int c) {
    int ret=0,cnt=0;
    for(int d=0;d<4;d++) {
        if(!check(r+dr[d],c+dc[d])) continue;
        cnt++;
        for(int color=0;color<3;color++)
            ret += abs(image[r+dr[d]][c+dc[d]][color] - image[r][c][color]);
    }
    return (ret*1.0)/cnt;
}

int pos[1000];
float seamDp[1000][1000];
int dpChoice[1000][1000];

void rescaleImage(int oI[1000][1000][3],int n,int m,int r,int c,void* out) {
    copyArray(image, oI);
    M=m;

    for(int iter=0;iter<c;iter++) {
        cout<<M<<' '<<N<<'\n';

        int i,j;

        for(i=0;i<M;i++)
            seamDp[0][i] = Energy(0,i);

        for(i=1;i<N;i++) {
            for(j=0;j<M;j++) {
                float e = Energy(i,j);
                seamDp[i][j] = e + seamDp[i-1][j];
                dpChoice[i][j] = j;
                if(j && (seamDp[i][j] > e + seamDp[i-1][j-1])) {
                    seamDp[i][j] = e + seamDp[i-1][j-1];
                    dpChoice[i][j] = j-1;
                }
                if(j+1 < M && seamDp[i][j] > e + seamDp[i-1][j+1]) {
                    seamDp[i][j] = e + seamDp[i-1][j+1];
                    dpChoice[i][j] = j+1;
                }
            }
        }

        float minn = INF;
        int minpos=0;
        for(i=0;i<M;i++)
        {
            if(seamDp[N-1][i] < minn)
            {
                minn = seamDp[N-1][i];
                minpos = i;
            }
        }

        cout<<minn/n<<'\n';

        int cur = minpos;
        pos[N-1] = minpos;
        for(i=N-2;i>=0;--i)
        {
            cur = dpChoice[i+1][cur];
            pos[i] = cur;
        }

        for(i=0;i<N;i++)
        {
            int p=0;
            for(j=0;j<M;j++)
                if(pos[i]!=j) {
                  for(int color=0;color<3;color++)
                  newImage[i][p++][color] = image[i][j][color];
                }
        }
        M--;  //Decrease the number of columns
    }

    //Output the newImage into the nI array
    for(int i = 0; i < N; i++) {
      for(int j = 0; j < M; j++) {
        for(int k = 0; k < 3; k++)
        store(newImage[i][j][k]);
      }
    }
}
