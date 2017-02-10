#include "exchange.h"

void pass(void * exchanger, int data, int i, int j, int k) {
    exchange * x = (exchange *) exchanger;
    x->send(data,i,j,k);
}
