#ifndef BNODE

#define BNODE 1

#include "IData.h"
#include <vector>
#include <iostream>

using namespace std;

// clase para representar los nodos internos del árbol B+
class BNode{
    private:
        vector<IData*> *keys; // vector de con M llaves
        vector<BNode*> *children; // vector de los hijos. Va a tener M + 1 elementos.

    protected:
        int order; // orden M
        bool type; // no es hoja: false. es hoja: true
        BNode* parent; // puede que un nodo tenga 1 o 2 padres.
        int parentIndex; // posicion donde se inserta si se hace una partición

    public:
        BNode(){
            keys = nullptr;
            children = nullptr;
            order = 0;
            type = false;
            parent = nullptr;
            parentIndex = 0;
        }

        BNode(int pOrder, BNode* pParent, int pParentIndex){
            type = false;
            order = pOrder;
            keys = new vector<IData*>();
            children = new vector<BNode*>();
            parent = pParent;
            parentIndex = pParentIndex;
        }

        vector<IData*>* getKeys(){
            return keys;
        }

        vector<BNode*>* getChildren(){
            return children;
        }

        BNode* getParent(){
            return parent;
        }

        void setParent(BNode* pParent){
            parent = pParent;
        }

        BNode* rebalance(){ // calcular el rebalanceo del B+
            BNode* result = nullptr; // puntero para el resultado, va a retornar la raiz nueva que quede.

            if (keys->size() > order){ // si las llaves sobrepasaron el orden

                if (!parent){ // si toca rebalancear la raíz, se crea la nueva raíz.
                    parent = new BNode(order, nullptr, 0);
                    parent->getChildren()->insert(parent->getChildren()->begin(), this); // insertamos este nodo como primer hijo
                    result = parent;
                }

                vector<IData*>::iterator element; // iterador para ir pasando los elementos de vector
                int half = (keys->size() - 1) >> 1; // punto donde se va a hacer el recorte de la mitad.
                vector<IData*>::iterator halfPos = keys->begin() + half;

                BNode* brother = new BNode(order, parent, parentIndex + 1); // se crea el nuevo nodo de la partición.
                vector<IData*> *newKeys = brother->getKeys();
                vector<BNode*> *newChildren = brother->getChildren();

                // se le van a ir pasando las llaves y los hijos del nodo actual hasta llegar a la mitad.
                for (element = --(keys->end()); element != halfPos; element--){
                    newKeys->insert(newKeys->begin(), *element);
                    newChildren->insert(newChildren->begin(), *children->rbegin());
                    children->pop_back();
                    keys->pop_back();
                    (*newChildren->begin())->setParent(brother);
                }
                newChildren->insert(newChildren->begin(), *children->rbegin()); // agregamos el primer hijo del hermano
                (*newChildren->begin())->setParent(brother);

                parent->getKeys()->insert(parent->getKeys()->begin() + parentIndex, *halfPos); // insertamos el elemento del medio en el padre.
                parent->getChildren()->insert(parent->getChildren()->begin() + parentIndex + 1, brother);

                children->pop_back(); // borramos el elemento que subimos del nodo
                keys->pop_back();

                updateIndex(parentIndex + 1);
                brother->updateIndex(0);
                // actualizmos las posiciones de inserción de los hijos del brother, ya que ahora insertan en el brother.

                result = parent->rebalance(); // rebalanceamos los padres hasta que quede balanceado o hasta crear la raíz nueva.
                return result;
            }
            if (!parent){ // si el nodo actual es la raíz, retornamos este nodo, sino, se va a retornar nullptr.
                result = this;
            }
            updateIndex(parentIndex + 1);
            // actualizamos las posiciones de inserción de los nodos después de donde insertamos.
            return result;
        }

        virtual bool getType(){
            return type;
        }

        void setParentIndex(int pParentIndex){
            parentIndex = pParentIndex;
        }

        // funcion que actualiza los indices de inserción a partir de un punto de partida.
        void updateIndex(int startingPoint){
            for(auto iterador = children->begin() + startingPoint; iterador != children->end(); iterador++){
                (*iterador)->setParentIndex(startingPoint++);
            }
        }

};

#endif