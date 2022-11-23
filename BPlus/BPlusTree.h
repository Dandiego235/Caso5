#ifndef BPLUSTREE

#define BPLUSTREE 1

#include "BNode.h"
#include "LeafNode.h"
#include "List.h"
#include "queue.h"
#include <iostream>
#include "StringData.h"
using namespace std;

class BPlusTree{
    private:
        int order; // orden del arbol
        int size; // tamaño de los bloques del conjunto secuencia
        BNode *root;
        LeafNode* leaf; // puntero a una hoja
        BNode* searchPointer; // puntero para buscar 
        int index;

    public:

        BPlusTree(int pOrder, int pSize){
            order = pOrder;
            size = pSize;
            root = nullptr;
        }

        // función para insertar un nodo
        void insert(IData* data){
            vector<IData*>::iterator leafPosition; // posición de donde voy a insertar el nodo
            if (!root){ // si no hay raiz, creamos un nuevo nodo para el conjunto secuencia.
                root = new LeafNode(size, nullptr, 0, order);
            }
            leafPosition = search(data); // buscamos la posicion de insercion
            BNode* result = leaf->insert(data, leafPosition); // La inserción retorna la nueva raíz, si fuera del caso.
            if (result){
                root = result;
            }
        }

        // funcion para buscar un elemento en el árbol para encontrar la posición de inserción.
        // Retorna un iterador a la posición donde debería ir el nodo a insertar.
        vector<IData*>::iterator search(IData* data){
            searchPointer = root; // search pointer empieza en la raiz
            index = 0; // indice del padre del nodo

            while (!searchPointer->getType()){ // mientras no sea hoja

                //obtenemos las llaves y los hijos
                vector<IData*> *searchKeys = searchPointer->getKeys();
                vector<BNode*> *searchChildren = searchPointer->getChildren();

                // iteramos sobre las llaves para encontrar la posición donde debemos bajar
                for (index = 0; index < searchKeys->size(); index++){
                    if (data->compareTo(searchKeys->at(index)) <= 0){ // si es menor o igual, bajamos por el hijo izquierdo de la llave
                        break;
                    }
                }
                // si no encontró una llave menor o mayor, baja por el último hijo, la rama del extremo derecho.

                searchPointer = searchChildren->at(index);
                // el searchpointer apunta al hijo en la posicion.
            }

            leaf = (LeafNode*)(void*)(searchPointer); // cuando llega a la hoja, convierte el BNode a LeafNode.

            vector<IData*> *secuencia = leaf->getSecuencia();

            vector<IData*>::iterator element = secuencia->begin(); // iterador para recorrer la secuencia del nodo hoja

            if (element != secuencia->end()){ // si el bloque no está vacío

                // recorre la lista hasta encontrar un elemento mayor o un campo donde puede insertar el elemento.
                while (data->compareTo(*element) >= 0){
                    if (element + 1 == secuencia->end()){
                        // si llegó al final de este bloque
                        if (secuencia->size() < size || !leaf->getBrother()){
                            // si hay espacio para insertarlo o llegó al final del conjunto secuencia, lo mete en la ultima posicion
                            element++;
                            break;
                        }
                        // si no hay espacio, se pasa al hermano del conjunto.
                        leaf = leaf->getBrother();
                        secuencia = leaf->getSecuencia();
                        element = secuencia->begin();
                    } else {
                        // si no, pasa el siguiente elemento.
                        element++;
                    }
                }
            }
            return element;
        }

        // funcion para buscar un elemento en el árbol y retornar el nodo y el índice donde se ubica
        // Retorna el nodo hoja de donde se ubica la primera aparición en el árbol.
        LeafNode* find(IData* data, int &pIndex){
            searchPointer = root; // search pointer empieza en la raiz
            index = 0; // indice del padre del nodo

            while (!searchPointer->getType()){ // mientras no sea hoja

                //obtenemos las llaves y los hijos
                vector<IData*> *searchKeys = searchPointer->getKeys();
                vector<BNode*> *searchChildren = searchPointer->getChildren();

                // iteramos sobre las llaves para encontrar la posición donde debemos bajar
                for (index = 0; index < searchKeys->size(); index++){
                    if (data->compareTo(searchKeys->at(index)) <= 0){ // si es menor o igual, bajamos por el hijo izquierdo de la llave
                        break;
                    }
                }
                // si no encontró una llave menor o mayor, baja por el último hijo, la rama del extremo derecho.

                searchPointer = searchChildren->at(index);
                // el searchpointer apunta al hijo en la posicion.
            }

            leaf = (LeafNode*)(void*)(searchPointer); // cuando llega a la hoja, convierte el BNode a LeafNode.

            vector<IData*> *secuencia = leaf->getSecuencia();

            vector<IData*>::iterator element = secuencia->begin(); // iterador para recorrer la secuencia de 

            pIndex = 0;

            if (element != secuencia->end()){ // si el bloque no está vacío

                // recorre la lista hasta encontrar la primera aparición del elemento.
                while (data->compareTo(*element) > 0){
                    if (element + 1 == secuencia->end()){
                        // si llegó al final de este bloque
                        if (leaf->getBrother()){ // si hay hermano, se pasa al hermano.
                            leaf = leaf->getBrother();
                            secuencia = leaf->getSecuencia();
                            element = secuencia->begin();
                            pIndex = 0;
                            continue;
                        }
                        // si no hay hermano, no encontró el elemento y retorna nulo
                        return nullptr;
                    } else {
                        // si no, pasa el siguiente elemento.
                        element++;
                        pIndex++;
                    }
                }
                if (data->compareTo(*element) == -1){
                    return nullptr; // el elemento buscado no está en el árbol.
                }
                return leaf;
            }
            return nullptr;
        }

        // función para imprimir el árbol. Solamente imprime los niveles y bloques, no representa las ramas.
        // Las ramas se interpretan de acuerdo con la cantidad de elementos que hay en cada nivel y en el nivel siguiente.
        void print(){
            BNode* nodePtr = root;
            if (!nodePtr){ // si el árbol está vacío, no imprime nada.
                return;
            }
            Queue<BNode> *cola = new List<BNode>(); // cola para imprimir los nodos

            int contadorNodosEnNivel = 1;
            int contador = 0;
            int contadorNodosNivelSiguiente = 0;

            while (!nodePtr->getType()){ // mientras no sea hoja.
                for (IData* data : *(nodePtr->getKeys())){ // imprime las llaves del BNode
                    cout << data->getKey() << " ";
                }
                cout << "// ";

                for (BNode* child : *(nodePtr->getChildren())){ // encolamos cada hijo para imprimirlo primero.
                    cola->enqueue(child);
                }

                contadorNodosNivelSiguiente += nodePtr->getChildren()->size(); // le sumamos la cantidad de hijos del siguiente nivel
                contador++;
                if (contador == contadorNodosEnNivel){ // si llegamos a la cantidad de nodos en el nivel
                    cout << endl;
                    contador = 0; // reestablecemos los contadores
                    contadorNodosEnNivel = contadorNodosNivelSiguiente; // el nivel actual pasa a ser el siguiente.
                    contadorNodosNivelSiguiente = 0;
                }
                nodePtr = cola->dequeue(); // sacamos el siguiente nodo de la cola.
            }

            // se imprime el conjunto secuencia recorriéndolo.
            cout << "Conjunto secuencia" << endl;
            LeafNode* leafPtr = (LeafNode*)(void*)(nodePtr);
            while (true) {
                for (IData* data : *(leafPtr->getSecuencia())){
                    cout << data->toString() << " ";
                }
                cout << "// ";
                if (leafPtr->getBrother()){ // si tiene hermano, se pasa al hermano, si no, se sale.
                    leafPtr = leafPtr->getBrother();
                } else {
                    break;
                }
            }
            cout << endl;
        }

        int getSize(){
            return size;
        }

        int getOrden(){
            return order;
        }
};

#endif