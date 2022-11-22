#ifndef DIJKSTRA
#define DIJKSTRA 1

#include "INodo.h"
#include <vector>

using namespace std;
class Arco;
class NodoGrafo;

// Esta clase representa las entradas de la tabla de distancia del algoritmo de Dijkstra
class DijkstraNode {
    private:
        NodoGrafo* destino; // nodo de destino
        int distancia; // distancia del nodo starting al nodo destino
        Arco* camino; // Arco que cuyo origen es quien agregó a este destino
        NodoGrafo * starting; // nodo que es el punto de partida, o desde donde se va a calcular la distancia.
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

        // Esta función recibe el arco y la cantidad de nodos en el camino del anterior para actualizar la cantidad de nodos en el camino
        void addArc(Arco* pArco, int pCantidadNodos) {
            camino = pArco;
            cantidadNodos = pCantidadNodos + 1; // este nodo tiene la cantidad de nodos del anterior + 1
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