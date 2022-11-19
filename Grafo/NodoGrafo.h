#ifndef NODOGRAFO
#define NODOGRAFO 1

#include <vector>
#include "INodo.h"
#include <unordered_map>
#include "DijkstraNode.h"

using namespace std;

class Arco;
class Grafo;

class NodoGrafo {
    private:
        INodo* dato;
        vector<Arco*>* listaArcos;
        bool visitado;
        bool procesado;
        unordered_map<NodoGrafo*, DijkstraNode*> * caminos;
        unordered_map<NodoGrafo*, DijkstraNode*> * caminosMayores;
        vector<NodoGrafo*> * NodosEntrada; 
        vector<vector<NodoGrafo*>> * ciclos; 

    public:

        NodoGrafo(INodo* pDato) {
            this->dato = pDato;
            this->listaArcos = new vector<Arco*>();
            this->caminos = new unordered_map<NodoGrafo*, DijkstraNode*>();
            this->caminosMayores = new unordered_map<NodoGrafo*, DijkstraNode*>();
            this->NodosEntrada = new vector<NodoGrafo*>();
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

        void setDijkstraNodes(vector<NodoGrafo*> nodos){
            for (NodoGrafo * nodo : nodos){
                caminos->insert_or_assign(nodo, new DijkstraNode(nodo, this));
            }
        }

        void setProcesado(bool pProcesado){
            procesado = pProcesado;
        }

        bool getProcesado(){
            return procesado;
        }

        void setInfo(INodo* pDato) {
           this->dato = pDato; 
        }

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

        void addEntrada(NodoGrafo * pEntrada){
            this->NodosEntrada->push_back(pEntrada);
        }

        vector<Arco*> * getEntradas(){
            return NodosEntrada;
        }

        void addCiclo(vector<NodoGrafo*> pCiclo){
            this->ciclos->push_back(pCiclo);
        }

        vector<vector<NodoGrafo*>> * getCiclos() {
            return this->ciclos;
        }

        vector<NodoGrafo*>* getNodosEntrada(){
            return NodosEntrada;
        }
};

#endif