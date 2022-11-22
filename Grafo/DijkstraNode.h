#ifndef DIJKSTRA
#define DIJKSTRA 1

#include "INodo.h"
#include <vector>

using namespace std;
class Arco;
class NodoGrafo;
class DijkstraNode {
    private:
        NodoGrafo* destino;
        int distancia;
        Arco* camino;
        NodoGrafo * starting;
        int cantidadNodos;

    public:
        // constructor
        DijkstraNode(NodoGrafo * pDestino, NodoGrafo * pStarting, int pDistancia) {
            this->destino = pDestino;
            this->distancia = pDistancia;
            this->starting = pStarting;
            cantidadNodos = 1;
        }

        // getters del nodo dijkstra
        NodoGrafo* getDestino() const{
            return this->destino;
        }

        int getDistancia() const{
            return this->distancia;
        }

        void addArc(Arco* pArco, int pCantidadNodos) {
            camino = pArco;
            cantidadNodos = pCantidadNodos + 1;
        }

        void setDistancia(int pDistancia){
            distancia = pDistancia;
        }

        Arco* getCamino(){
            return camino;
        }

        int getCantidadNodos() const{
            return cantidadNodos;
        }

        NodoGrafo * getStarting(){
            return starting;
        }

};

#endif