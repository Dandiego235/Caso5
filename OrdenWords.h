#ifndef ORDENWORDS

#define ORDENWORDS 1

#include "BPlus/StringData.h"

// Functor, clase que funciona como función, para ordenar un set de stringData* de acuerdo con su segundo valor de enteros.
struct OrdenWords{
    bool operator() (const pair<StringData*,int>& pair1, const pair<StringData*,int>& pair2) const{
        if (pair1.second != pair2.second){
            return pair1.second < pair2.second;
        } // si las cantidades son iguales, se orden por orden alfabetico y tamaño de la palabra.
        return pair1.first < pair2.first;
    }
};

#endif