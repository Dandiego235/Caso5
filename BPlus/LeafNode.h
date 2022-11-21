#ifndef LEAFNODE

#define LEAFNODE 1

#include "BNode.h"
#include <vector>
#include "IData.h"

using namespace std;

class LeafNode : public BNode {
    private:
        int size;
        vector<IData*> *secuencia;
        LeafNode* brother; // puntero al siguiente nodo del conjunto secuencia.

    public:
        LeafNode(int pSize, BNode* pParent, int pParentIndex, int pOrder){
            type = true;
            size = pSize;
            parent = pParent;
            parentIndex = pParentIndex;
            secuencia = new vector<IData*>();
            brother = nullptr;
            order = pOrder;
        }

        // función que inserta un nodo. Eventualmente va a retornar la nueva raíz
        // recibe el dato y la posición donde va a insertar el nuevo dato.
        BNode* insert(IData* data, vector<IData*>::iterator element) {
            BNode* result = nullptr;

            if (!secuencia->empty()){
                secuencia->insert(element, data); // insertamos el nuevo dato en el elemento

                if (secuencia->size() > size){ // si nos pasamos del tamaño permitido, se realiza la partición
                    if (!parent){ // si no tiene padre, crea un padre. (si es la raíz)
                        parent = new BNode(order, nullptr, 0);
                        parent->getChildren()->insert(parent->getChildren()->begin(), this);
                        result = parent;
                    }
                    int half = (secuencia->size() - 1) >> 1; // punto donde se va a hacer el recorte de la mitad.
                    vector<IData*>::iterator halfPos = secuencia->begin() + half;

                    LeafNode* brother = new LeafNode(size, parent, parentIndex + 1, order);
                    // se crea el nuevo nodo de la partición.
                    brother->setBrother(this->getBrother());
                    this->setBrother(brother);
                    vector<IData*> *newSecuencia = brother->getSecuencia();

                    // se le van a ir pasando las llaves y los hijos del nodo actual hasta llegar a la mitad.
                    for (element = --(secuencia->end()); element != halfPos; element--){
                        newSecuencia->insert(newSecuencia->begin(), *element);
                        secuencia->pop_back();
                    }

                    // insertamos el elemento medio en el padre.                    
                    parent->getKeys()->insert(parent->getKeys()->begin() + parentIndex, *halfPos);
                    parent->getChildren()->insert(parent->getChildren()->begin() + parentIndex + 1, brother);

                    result = parent->rebalance(); // se rebalancea desde el padre
                }
            } else {
                secuencia->push_back(data);
            }
            return result; // se retorna algo no nulo si se cambió la raíz del árbol.
        }

        vector<IData*>* getSecuencia(){
            return secuencia;
        }

        LeafNode* getBrother(){
            return brother;
        }

        void setBrother(LeafNode* pBrother){
            brother = pBrother;
        }
        
};

#endif