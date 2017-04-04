#define _exchangeclass
#include <iostream>
#include <functional>
using namespace std;

class exchange {
public:
    exchange(const std::function<void (void *,void *, void *, void *)> & c) {
        this->callback = c;
    }
    void send(int data,int i, int j, int k){
       this->callback(&data,&i,&j,&k);
    }
private:
    std::function<void (void *,void *, void *, void *)> callback;
};

#include "c_exchange.h"
