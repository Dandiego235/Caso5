#ifndef INTSTRING

#define INTSTRING 1

#include <sstream>
#include <string>
#include "IData.h"

class StringData : public IData{
    private:
        string palabra;

    public:
        StringData(string pPalabra){
            palabra = pPalabra;
        }

        string getPalabra(){
            return palabra;
        }

        void setPalabra(string pPalabra){
            palabra = pPalabra;
        }

        // método usado para comparar dos IData
        int compareTo(IData *pToCompare){
            StringData * compare = dynamic_cast<StringData*>(pToCompare);
            if (palabra == compare->getPalabra()){
                return 1;
            }
            return 0;
        }

        string getKey(){
            return palabra;
        }

        string toString(){ // método para obtener la información del IData para imprimirla.
            return getKey();
        }
};

#endif