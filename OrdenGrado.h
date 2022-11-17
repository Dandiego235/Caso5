#ifndef ORDENGRADO

#define ORDENGRADO 1

#include "Grafo/NodoGrafo.h"
struct OrdenGrado{
    bool operator() (const NodoGrafo* nodo1, const NodoGrafo* nodo2) const{
        return nodo1->getArcs()->size() < nodo2->getArcs()->size();
    }
};

#endif