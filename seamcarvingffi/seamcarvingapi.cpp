#include <iostream>
#include <stdio.h>
#include <functional>
#include "../seamcarvinglib/exchange.h"
#include "../seamcarvinglib/myseamcarving.h"
#include "seamcarvingapi.h"

using namespace std;

int rescale(int oI[1000*1000*3],int n,int m,int r,int c,int nI[1000*1000*3]) {
exchange x(
  [&](void * data, void * i, void * j, void * k) {
    int *dptr = (int *) data;
    int *iptr = (int *) i;
    int *jptr = (int *) j;
    int *kptr = (int *) k;
    nI[3000*(*iptr)+3*(*jptr)+(*kptr)] = *dptr;
  }
);
rescaleImage(oI,n,m,r,c,(void*)&x);

return 1;
}
// // int imageRescaleTwoD(int orignalImage[][1000][3], int newImage[][1000][3], int r, int c) {
// // }
//
// int getPrimes(int under, int primes[]) {
// 	int count = 0;
//     exchange x(
//         [&](void * data) {
//             int * iptr = (int *) data;
//             primes[count++] = *iptr;
//         }
//     );
//
//     generate_primes(under, (void*)&x);
//
//     return count;
// }
