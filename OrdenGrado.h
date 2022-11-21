#ifndef ORDENGRADO

#define ORDENGRADO 1

#include "Grafo/NodoGrafo.h"

// Functor, una clase que se usa como función, para ordenar un set de acuerdo con el grado.
struct OrdenGrado{
    bool operator() (const NodoGrafo* nodo1, const NodoGrafo* nodo2) const{
        if (nodo1->getArcs()->size() <= nodo2->getArcs()->size()) { // si el grado es menor o igual es verdadero.
            return true;
        }
        return false;
    }
};

#endif