#ifndef _ARCO_
#define _ARCO_ 1

#include "INodo.h"
#include "NodoGrafo.h"

using namespace std;

// Clase para los arcos del grafo
class Arco {
    private:
        NodoGrafo* origen;
        NodoGrafo* destino;
        int peso;

    public:
        // constructor
        Arco(NodoGrafo* pOrigen, NodoGrafo* pDestino, int pPeso) {
            this->origen = pOrigen;
            this->destino = pDestino;
            this->peso = pPeso;
        }

        // getters del arco
        NodoGrafo* getOrigen() {
            return this->origen;
        }

        NodoGrafo* getDestino() {
            return this->destino;
        }

        int getPeso() {
            return this->peso;
        }
};

#endif