#ifndef _GRAFO_
#define _GRAFO_ 1
#include <vector>
#include "NodoGrafo.h"
#include "INodo.h"
#include <map>
#include <queue>
#include <stack>
#include "Arco.h"
#include <iostream>

using namespace std;

class Grafo {
    private:
        vector<NodoGrafo*> listaNodos; // Contiene todos los nodos del grafo
        bool esDirigido = true; // indica si es un grafo dirigido. Se usa para establecer los arcos. Si es dirigido, solo tiene que hacer uno
                                // pero si no es dirigido, tiene que hacer ida y vuelta
        std::map<int,NodoGrafo*> hashNodos; // se usa para buscar el nodo más fácilmente.

        vector<vector<Arco*>> componentesConexas;
        int cantidadComponentes;

        // esta función se usa para restaurar los valores de los nodos de visitado y procesado para el siguiente recorrido
        void resetNodes() { // se itera por la lista
            for (std::vector<NodoGrafo*>::iterator current = listaNodos.begin() ; current != listaNodos.end(); ++current) {
                NodoGrafo* actual = (*current);
                actual->setProcesado(false); // reestablece los valores
                actual->setVisitado(false);
            }
        }

        // esta función busca el primer nodo que no ha sido visitado por el recorrido
        NodoGrafo* findNotVisited() {
            NodoGrafo* result = nullptr;
            for (std::vector<NodoGrafo*>::iterator current = listaNodos.begin() ; current != listaNodos.end(); ++current) {
                NodoGrafo* actual = (*current);
                if (!actual->getVisitado()) {
                    result = actual;
                    break;
                }
            }
            return result; // si todos han sido visitados, retorna nulo
        }

    public:
        Grafo(bool pDirigido) { // constructor del grafo
            this->esDirigido =  pDirigido;
            int cantidadComponentes = 0;
        }

        int getSize() { // obtiene la cantidad de nodos
            return this->listaNodos.size();
        }

        void addNode(INodo* pNodo) { // agrega un nodo al grafo
            NodoGrafo* nuevoNodo = new NodoGrafo(pNodo); // crea un puntero al nodo y le establece que sus datos son el parámetro de INodo
            this->listaNodos.push_back(nuevoNodo); // añade el nodo a la lista
            hashNodos.insert(pair<int,NodoGrafo*>(pNodo->getId(),nuevoNodo)); // añade el nodo al hashmap
        }

        // Esta función añade un arco entre nodos
        void addArc(NodoGrafo* pOrigen, NodoGrafo* pDestino) { // esta función función no toma en cuenta el peso
            this->addArc(pOrigen, pDestino, 0); 
        }

        // Esta función añade un arco entre nodos y le establece su peso
        void addArc(NodoGrafo* pOrigen, NodoGrafo* pDestino, int pPeso) {
            Arco* newArc = new Arco(pOrigen, pDestino, pPeso); // crea el nuevo arco

            pOrigen->addArc(newArc); // crea establece el arco desde el origen al destino
            if (!this->esDirigido) { // si no es dirigido, crea un arco en dirección contraria.
                Arco* reverseArc =  new Arco(pDestino, pOrigen , pPeso);
                pDestino->addArc(reverseArc);
            }
        }

        // todas estas funciones son para tener más flexibilidad en añadir arcos
        void addArc(INodo* pOrigen, INodo* pDestino) {
            this->addArc(pOrigen->getId(), pDestino->getId(), 0);
        }

        void addArc(INodo* pOrigen, INodo* pDestino, int pPeso) {
            this->addArc(pOrigen->getId(), pDestino->getId(), pPeso);
        }

        void addArc(int pOrigen, int pDestino) {
            this->addArc(pOrigen, pDestino, 0);
        }

        void addArc(int pOrigen, int pDestino, int pPeso) {
            this->addArc(this->getNodo(pOrigen), this->getNodo(pDestino), pPeso);
        }


        // esta función busca un nodo en el hash map y lo retorna
        NodoGrafo* getNodo(int pId) { 
            return hashNodos.at(pId);
        }


        // Esta función es el recorrido en profundidad
        vector<INodo*> deepPath(INodo* pOrigen) {
            // El INodo que recibe es el punto de partida

            vector<INodo*> result; // aquí se van a almacenar los nodos visitados
            stack<NodoGrafo*> nodosProcesados; // pila de nodos procesados
            int visitados = 0; // Tiene un contador de la cantidad de visitados
            
            resetNodes(); // restaura todos los nodos para que no aparezcan como visitados

            // Establece el punto de partida, que es el nodo que se envió de parámetro
            NodoGrafo* puntoPartida = this->getNodo(pOrigen->getId());
            nodosProcesados.push(puntoPartida); // lo mete a la pila
            puntoPartida->setProcesado(true); // establece que ese nodo es procesado
            
            int contador = 0;
            do {
                vector<Arco*> componente;
                componente.push_back(new Arco(NULL, puntoPartida, 0));
                while (!nodosProcesados.empty()) { // mientras la lista de procesados no esté vacía
                    NodoGrafo* actual = nodosProcesados.top(); // obtiene el primer nodo
                    nodosProcesados.pop(); // lo saca de la pila

                    actual->setVisitado(true); // establece el nodo como visitado
                    visitados++; // incrementa el contador
                    result.push_back(actual->getInfo()); // agregamos el nodo visitado al recorrido

                    vector<Arco*> *adyacentes = actual->getArcs(); // obtiene el vector de adyacentes

                    for (Arco* arco : *adyacentes) {
                        // recorremos los arcos para encontrar los nodos adyacentes
                        NodoGrafo* adyacente = (NodoGrafo*)arco->getDestino(); // obtiene el nodo adyacente en sí

                        if (!adyacente->getProcesado()) { // si no está procesado
                            componente.push_back(arco);
                            nodosProcesados.push(adyacente); // lo mete en la pila
                            adyacente->setProcesado(true); // lo marca
                        }
                    }
                }
                if (!cantidadComponentes){
                    componentesConexas.pushback(componente);
                    contador++;
                }
                
                if (visitados < this->getSize()) {
                    // si todavía no hemos visitado todos, hacemos otro recorrido a partir de un nuevo punto de partida
                    puntoPartida = this->findNotVisited(); // obtiene el siguiente no visitado
                    nodosProcesados.push(puntoPartida); // mete el punto de partida en la pila
                    puntoPartida->setProcesado(true); // establece el nodo como procesado
                }
                
            } while (visitados < this->getSize());

            cantidadComponentes = contador;

            return result; // retorna el vector de visitados por el recorrido
        } 

        vector<INodo*> broadPath(INodo* pOrigen) { // recorrido en anchura
            // El INodo que recibe es el punto de partida
            // el procedimiento es igual que el recorrido en profundidad, nada más que utilizando una cola en lugar de una pila
            vector<INodo*> result;
            queue<NodoGrafo*> nodosProcesados; // se crea la cola de nodos procesados
            int visitados = 0;
            
            resetNodes();

            NodoGrafo* puntoPartida = this->getNodo(pOrigen->getId());
            nodosProcesados.push(puntoPartida);
            puntoPartida->setProcesado(true);
            
            int contador = 0;

            do {
                componente.push_back(new Arco(NULL, puntoPartida, 0));
                while (!nodosProcesados.empty()) {
                    NodoGrafo* actual = nodosProcesados.front(); // obtiene el primero de la cola
                    nodosProcesados.pop(); // lo saca de la cola

                    actual->setVisitado(true);
                    visitados++;
                    result.push_back(actual->getInfo()); // agregamos el nodo visitado al recorrido

                    vector<Arco*> *adyacentes = actual->getArcs();

                    for (Arco* arco : *adyacentes) {
                        // recorremos los arcos para encontrar los nodos adyacentes
                        NodoGrafo* adyacente = (NodoGrafo*)arco->getDestino();

                        if (!adyacente->getProcesado()) {
                            componente.push_back(arco);
                            nodosProcesados.push(adyacente);
                            adyacente->setProcesado(true);
                        }
                    }
                }
                if (!cantidadComponentes){
                    componentesConexas.pushback(componente);
                    contador++;
                }

                if (visitados < this->getSize()) {
                    // si todavía no hemos visitado todos, hacemos otro recorrido a partir de un nuevo punto de partida
                    puntoPartida = this->findNotVisited();
                    nodosProcesados.push(puntoPartida);
                    puntoPartida->setProcesado(true);
                }

            } while (visitados < this->getSize()); 

            contadorComponentes = contador;
            return result;
        }

        /*
        Este método retorna el camino, pero todavía no está implementado
        vector<INodo> path(INodo* pOrigen, INodo* pDestino) { // debe retornar un camino, el primero que encuentre estre el nodo origen y destino
            // en caso de que no haya camino, result se retorna vacío
            vector<INodo> result;

            return result;
        }
        */

        // Este método imprime los nodos y la cantidad de arcos que tienen 
        void printCounters() {
            for (std::vector<NodoGrafo*>::iterator current = listaNodos.begin() ; current != listaNodos.end(); ++current) {
                NodoGrafo* actual = (*current);
                cout << actual->getInfo()->getId() << " tiene " << actual->getArcs()->size() << endl;
            }
            for (std::vector<NodoGrafo*>::iterator current = listaNodos.begin() ; current != listaNodos.end(); ++current) {
                NodoGrafo* actual = (*current);
                cout << actual->getInfo()->getId() << " tiene " << actual->getArcs()->size() << endl;
            }
        }
};

#endif