#ifndef DIJKSTRA
#define DIJKSTRA 1

#include "INodo.h"
#include <vector>
#include <climits>

using namespace std;
class Arco;
class NodoGrafo;
class DijkstraNode {
    private:
        NodoGrafo* destino;
        int distancia;
        vector<Arco*> camino;

    public:
        // constructor
        DijkstraNode(NodoGrafo * pDestino) {
            this->destino = pDestino;
            this->distancia = INT_MAX;
        }

        // getters del nodo dijkstra
        NodoGrafo* getDestino() {
            return this->destino;
        }

        int getDistancia() {
            return this->distancia;
        }

        void addArc(Arco* pArco) {
            this->camino.clear();
            this->camino.push_back(pArco);
            
        }

        void setDistancia(int pDistancia){
            distancia = pDistancia;
        }

        vector<Arco*> getCamino(){
            return camino;
        }
};

#endif