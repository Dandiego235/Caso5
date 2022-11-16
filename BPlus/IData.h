#ifndef IDATA

#define IDATA 1

#include <string>

using namespace std;

class IData{
    public:
        virtual int compareTo(IData *pToCompare) = 0; // método usado para comparar dos IData

        virtual string getKey() = 0;
        virtual string toString() = 0; // método para obtener la información del IData para imprimirla.
};

#endif