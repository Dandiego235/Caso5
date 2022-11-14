#ifndef NODOGRAFO
#define NODOGRAFO 1

#include <vector>
#include "INodo.h"

using namespace std;

class Arco;

class NodoGrafo {
    private:
        INodo* dato;
        vector<Arco*>* listaArcos;
        bool visitado;
        bool procesado;

    public:

        NodoGrafo(INodo* pDato) {
            this->dato = pDato;
            this->listaArcos = new vector<Arco*>();
        }

        NodoGrafo() {
            this->listaArcos = new vector<Arco*>();            
        }

        void setVisitado(bool pVisitado){
            visitado = pVisitado;
        }

        bool getVisitado(){
            return visitado;
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

        vector<Arco*>* getArcs() {
            return this->listaArcos;
        }
};

#endif