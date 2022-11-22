#ifndef NODOGRAFO
#define NODOGRAFO 1

#include <vector>
#include "INodo.h"
#include <unordered_map>
#include "DijkstraNode.h"

using namespace std;

class Arco;
class Grafo;

// Esta clase representa un nodo de un grafo
class NodoGrafo {
    private:
        INodo* dato; // el contenido del nodo
        vector<Arco*>* listaArcos;
        bool visitado; // indicadores para procesos como recorridos, de si ya fueron visitados y procesados
        bool procesado;
        unordered_map<NodoGrafo*, DijkstraNode*> * caminos; // Va a contener la información de todos los caminos menores desde este nodo hasta los demás.
        unordered_map<NodoGrafo*, DijkstraNode*> * caminosMayores; // Va a contener la infromación de los caminos mayores o largos desde este nodo hasta los demás.
        vector<Arco*> * NodosEntrada; // vector que contiene los arcos de entrada al nodo.
        vector<vector<NodoGrafo*>> * ciclos; // vector que va a almacenar los ciclos a los que pertenece este nodo.

    public:

        NodoGrafo(INodo* pDato) {
            this->dato = pDato;
            this->listaArcos = new vector<Arco*>();
            this->caminos = new unordered_map<NodoGrafo*, DijkstraNode*>();
            this->caminosMayores = new unordered_map<NodoGrafo*, DijkstraNode*>();
            this->NodosEntrada = new vector<Arco*>();
            this->ciclos = new vector<vector<NodoGrafo*>>();
        }

        NodoGrafo() {
            this->listaArcos = new vector<Arco*>();     
            this->caminos = new unordered_map<NodoGrafo*, DijkstraNode*>();  
            this->NodosEntrada = new vector<Arco*>();   
        }

        void setVisitado(bool pVisitado){
            visitado = pVisitado;
        }

        bool getVisitado(){
            return visitado;
        }

        // Este método inicializa los nodos para algoritmo dijkstra con la distancia dada.
        // para calcular los mayores caninos, se debe inicializar con 0
        // para los menores, se inicializa con 999999, que es el valor predeterminado.
        void setDijkstraNodes(vector<NodoGrafo*> nodos, int distancia = 999999){
            for (NodoGrafo * nodo : nodos){
                caminos->insert_or_assign(nodo, new DijkstraNode(nodo, this, distancia));
            }
        }

        void setProcesado(bool pProcesado){
            procesado = pProcesado;
        }

        bool getProcesado(){
            return procesado;
        }

        // Establece el contenido del nodo
        void setInfo(INodo* pDato) {
           this->dato = pDato; 
        }

        // Obtiene el contenido del nodo
        INodo* getInfo() {
            return dato;
        }

        // añade el arco a la lista
        void addArc(Arco* pArco) {
            this->listaArcos->push_back(pArco);
        }

        vector<Arco*>* getArcs() const{
            return this->listaArcos;
        }

        unordered_map<NodoGrafo*, DijkstraNode*> * getCaminos() {
            return this->caminos;
        }

        unordered_map<NodoGrafo*, DijkstraNode*> * getCaminosMayores() {
            return this->caminosMayores;
        }

        // añade un arco de entrada
        void addEntrada(Arco * pEntrada){
            this->NodosEntrada->push_back(pEntrada);
        }

        vector<Arco*> * getEntradas(){
            return NodosEntrada;
        }

        // añade un ciclo al nodo
        void addCiclo(vector<NodoGrafo*> pCiclo){
            this->ciclos->push_back(pCiclo);
        }

        vector<vector<NodoGrafo*>> * getCiclos() {
            return this->ciclos;
        }

        vector<Arco*>* getNodosEntrada(){
            return NodosEntrada;
        }
};

#endif