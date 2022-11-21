#ifndef ORDENCADENASMIN

#define ORDENCADENASMIN 1

#include "Grafo/DijkstraNode.h"

// functor para ordenar las cadenas en un set de acuerdo con su cantidad de nodos y si estas son iguales, de acuerdo a la concurrencia.
struct OrdenCadenasMin{
    bool operator() (const DijkstraNode* nodo1, const DijkstraNode* nodo2) const{
        if (nodo1->getCantidadNodos() != nodo2->getCantidadNodos()){
            return nodo1->getCantidadNodos() < nodo2->getCantidadNodos();
        }
        int concurrencia1 = nodo1->getDestino()->getArcs()->size() + nodo1->getDestino()->getNodosEntrada()->size();
        int concurrencia2 = nodo2->getDestino()->getArcs()->size() + nodo2->getDestino()->getNodosEntrada()->size();
        return nodo1->getDistancia() + concurrencia1 < nodo2->getDistancia() + concurrencia2;
    }
};

#endif