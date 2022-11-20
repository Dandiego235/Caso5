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
#include <climits>
#include <fstream>
#include <stack>
#include "../Registered.h"

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

        void resetProcessed(vector<Arco*> pComponente) { // se itera por la lista
            for (Arco* arco : pComponente) {
                NodoGrafo* actual = arco->getDestino();
                actual->setProcesado(false); // reestablece los valores
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

        NodoGrafo * minimo(unordered_map<NodoGrafo*,NodoGrafo*>  * VmenosF, NodoGrafo * s){
            int mx = INT_MAX;
            NodoGrafo * v = nullptr;
            for (auto nodo : (*VmenosF)){
                if (!v){
                    v = nodo.first;
                }
                if (s->getCaminos()->at(nodo.first)->getDistancia()<= mx){
                    mx = s->getCaminos()->at(nodo.first)->getDistancia();
                    v = nodo.first;
                } 
            }
            return v;
        }

        NodoGrafo * maximo(unordered_map<NodoGrafo*,NodoGrafo*>  * VmenosF, NodoGrafo * s){
            int max = 0;
            NodoGrafo * v = nullptr;
            for (auto nodo : (*VmenosF)){
                if (!v){
                    v = nodo.first;
                }
                if (s->getCaminos()->at(nodo.first)->getDistancia() >= max){
                    max = s->getCaminos()->at(nodo.first)->getDistancia();
                    v = nodo.first;
                } 
            }
            return v;
        }

    public:

        void saveToFile(){
            ofstream Gobiz("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\gobiz.csv", ios::out);
            //ofstream Gobiz("C:\\Users\\Dandiego\\OneDrive - Estudiantes ITCR\\Estructuras de datos\\Caso5\\gobiz.csv", ios::out);
            Gobiz << "Id,Name,Tipo,Fecha,Descripcion\n";

            ofstream Links("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\links.csv", ios::out);
            //ofstream Links("C:\\Users\\Dandiego\\OneDrive - Estudiantes ITCR\\Estructuras de datos\\Caso5\\links.csv
            Links << "Source,Target,Type\n";
            
            for (NodoGrafo * nodo : listaNodos){
                Registered* registro = (Registered*)(void*)(nodo->getInfo());
                string offer = registro->getOffer();
                registro->replace_all(offer, ",", ";");
                // std::cout << offer << endl;
                string demand = registro->getDemand();
                registro->replace_all(demand, ",", ";");
                // std::cout << demand << endl;
                int tipo;
                if (offer == "" && demand != ""){
                    tipo = 1;
                } else if (offer != "" && demand == ""){
                    tipo = 0;
                } else {
                    tipo = 2;
                }
                Gobiz << nodo->getInfo()->getId() << "," << registro->getNickname() << ","  << tipo << "," << registro->getPostdate() << "," << offer << " " << demand << "\n";
            
                for (Arco * arco : (*nodo->getArcs())){
                    Links << arco->getOrigen()->getInfo()->getId() << "," << arco->getDestino()->getInfo()->getId() << "," << arco->getPeso() << "\n";
                    std::cout << arco->getOrigen()->getInfo()->getId() << "," << arco->getDestino()->getInfo()->getId() << "," << arco->getPeso() << "\n";
                }
            }
            Gobiz.close();
            Links.close();
        }

        void saveCycles(NodoGrafo * nodo){
            ofstream Gobiz("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\gobiz.csv", ios::out);
           // Gobiz.open("gobiz.csv");
            Gobiz << "Id,Name,Tipo,Fecha,Descripcion\n";

            ofstream Links("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\links.csv", ios::out);
            Links << "Source,Target,Type\n";
            int index = 0;

            for (vector<NodoGrafo*> ciclo : *(nodo->getCiclos())){
                Registered* registro = (Registered*)(void*)(nodo ->getInfo());
                string offer = registro->getOffer();
                registro->replace_all(offer, ",", ";");
                string demand;
                registro->replace_all(demand, ",", ";");
                int tipo = 2;
                NodoGrafo * anterior;
                int quantity = 0;
                int starting = nodo->getInfo()->getId() + index;
                Gobiz << (starting) << "," << registro->getNickname() << ","  << tipo << "," << registro->getPostdate() << "," << offer << " " << demand << "\n";
                for (NodoGrafo * nodoE : ciclo){
                    registro = (Registered*)(void*)(nodoE->getInfo());
                    offer = registro->getOffer();
                    registro->replace_all(offer, ",", ";");
                    demand = registro->getDemand();
                    registro->replace_all(demand, ",", ";");
                    
                    if (offer == "" && demand != ""){
                        tipo = 1;
                    } else if (offer != "" && demand == ""){
                        tipo = 0;
                    } else {
                        tipo = 2;
                    }
                    if ((nodoE->getInfo()->getId() + index) != starting){
                        Gobiz << (nodoE->getInfo()->getId() + index) << "," << registro->getNickname() << ","  << tipo << "," << registro->getPostdate() << "," << offer << " " << demand << "\n";
                    }
                    if (quantity != 0){
                        Links << nodoE->getInfo()->getId() + index << "," << anterior->getInfo()->getId() + index << "," << 0 << "\n";
                    }
                    anterior = nodoE;
                    quantity++;
                }
                index += listaNodos.size();
                if (anterior != nodo){
                    Links << anterior->getInfo()->getId() + index << "," << nodo->getInfo()->getId() + index << ","  << 0 << "\n";
                }
                
            }
            
            Gobiz.close();
            Links.close();
        }

        Grafo(bool pDirigido) { // constructor del grafo
            this->esDirigido =  pDirigido;
            cantidadComponentes = 0;
        }

        int getSize() { // obtiene la cantidad de nodos
            return this->listaNodos.size();
        }

        vector<NodoGrafo*> getNodos(){
            return listaNodos;
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
            pDestino->addEntrada(newArc);
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
            NodoGrafo *result;
            try {
                result = hashNodos.at(pId);
            } catch (...){
                result = nullptr;
            }
            return result;
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
                        NodoGrafo* adyacente = arco->getDestino(); // obtiene el nodo adyacente en sí

                        if (!adyacente->getProcesado()) { // si no está procesado
                            componente.push_back(arco);
                            std::cout << arco->getPeso() << endl;
                            nodosProcesados.push(adyacente); // lo mete en la pila
                            adyacente->setProcesado(true); // lo marca
                        }
                    }
                }
                if (!cantidadComponentes){
                    componentesConexas.push_back(componente);
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
                vector<Arco*> componente;
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
                        NodoGrafo* adyacente = arco->getDestino();

                        if (!adyacente->getProcesado()) {
                            componente.push_back(arco);
                            nodosProcesados.push(adyacente);
                            adyacente->setProcesado(true);
                        }
                    }
                }
                if (!cantidadComponentes){
                    componentesConexas.push_back(componente);
                    contador++;
                }

                if (visitados < this->getSize()) {
                    // si todavía no hemos visitado todos, hacemos otro recorrido a partir de un nuevo punto de partida
                    puntoPartida = this->findNotVisited();
                    nodosProcesados.push(puntoPartida);
                    puntoPartida->setProcesado(true);
                }

            } while (visitados < this->getSize()); 

            cantidadComponentes = contador;
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

        vector<vector<Arco*>>* getComponentesConexas(){
            return &componentesConexas;
        }

        void saveComponentes(){
            int contador = 1;
            ofstream Componentes("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\componentes.json", ios::out);
            
            Componentes << "{ \n \"name\": \"Grafo\", \"children\" : [\n";

            for (vector<Arco*> componente : componentesConexas){
                Componentes << "{\n \"name\" : \"Componente " << contador << "\",\n \"children\": [\n";
                int contador2 = 1;
                for (Arco * arco : componente){
                    Registered* registro = (Registered*)(void*)(arco->getDestino()->getInfo());
                    int peso = arco->getPeso();
                    if (!peso){
                        peso = 1;
                    }

                    Componentes << "{\"name\": \"" << registro->getNickname() << "\", \"size\" : " << peso << "}";
                    if (contador2 != componente.size()){
                        Componentes << ",";
                    }
                    Componentes << "\n";
                    contador2++;
                }
                Componentes << "]\n}";
                if (contador != componentesConexas.size()){
                    Componentes << ",";
                }
                contador++;
            }
            Componentes << "]\n}";
            Componentes.close();
        }

        void Dijkstra(NodoGrafo * starting){
            unordered_map<NodoGrafo*, NodoGrafo*> F;
            
            F.insert_or_assign(starting,starting);
            unordered_map<NodoGrafo*, NodoGrafo*> VmenosF;
            for (NodoGrafo * nodo : listaNodos){
                VmenosF.insert_or_assign(nodo, nodo);
            }
            //VmenosF.erase(starting);
            starting->setDijkstraNodes(listaNodos);
            unordered_map<NodoGrafo*, DijkstraNode*> * distancias = starting->getCaminos();
            
            //distancias->at(starting)->setDistancia(0);
            for (std::vector<Arco*>::iterator current = starting->getArcs()->begin() ; current != starting->getArcs()->end(); ++current){
                distancias->at((*current)->getDestino())->setDistancia((*current)->getPeso());
                distancias->at((*current)->getDestino())->addArc((*current), distancias->at((*current)->getOrigen())->getCantidadNodos());
            }
            
            while (!VmenosF.empty()){
                NodoGrafo * menor = minimo(&VmenosF, starting);
                for (std::vector<Arco*>::iterator current = menor->getArcs()->begin() ; current != menor->getArcs()->end(); ++current){                    
                    if (distancias->at((*current)->getDestino())->getDistancia() > distancias->at((*current)->getOrigen())->getDistancia() + (*current)->getPeso()){
                        distancias->at((*current)->getDestino())->setDistancia(distancias->at((*current)->getOrigen())->getDistancia() + (*current)->getPeso());
                        distancias->at((*current)->getDestino())->addArc((*current), distancias->at((*current)->getOrigen())->getCantidadNodos());
                    }
                }
                F.insert_or_assign(menor,menor);
                VmenosF.erase(menor);
            }
            for (auto nodo : *(distancias)){
                std::cout << "Distancia más corta desde " << starting->getInfo()->getId() << " a " << nodo.first->getInfo()->getId() << " es " << distancias->at(nodo.first)->getDistancia()<< " con nodos " <<  distancias->at(nodo.first)->getCantidadNodos() << endl;               
            }
        }

        void dijkstraMayor(NodoGrafo * starting){
            unordered_map<NodoGrafo*, NodoGrafo*> F;
            
            F.insert_or_assign(starting,starting);
            unordered_map<NodoGrafo*, NodoGrafo*> VmenosF;
            for (NodoGrafo * nodo : listaNodos){
                VmenosF.insert_or_assign(nodo, nodo);
            }
            VmenosF.erase(starting);
            starting->setDijkstraNodes(listaNodos, 0);
            unordered_map<NodoGrafo*, DijkstraNode*> * distancias = starting->getCaminos();
            distancias->at(starting)->setDistancia(0);
            for (Arco* arco : *starting->getArcs()){
                distancias->at(arco->getDestino())->setDistancia(arco->getPeso());
                distancias->at(arco->getDestino())->addArc(arco, distancias->at(arco->getOrigen())->getCantidadNodos());
            }
            
            while (!VmenosF.empty()){
                NodoGrafo * mayor = maximo(&VmenosF, starting);
                for (Arco* arco : *mayor->getArcs()){   
                   // cout << distancias->at(arco->getDestino())->getDistancia() << " " << distancias->at(arco->getOrigen())->getDistancia() << " " << arco->getPeso() << endl;
                    if (distancias->at(arco->getDestino())->getDistancia() < distancias->at(arco->getOrigen())->getDistancia() + arco->getPeso() && distancias->at(arco->getOrigen())->getDistancia() + arco->getPeso() != arco->getPeso()){
                        distancias->at(arco->getDestino())->setDistancia(distancias->at(arco->getOrigen())->getDistancia() + arco->getPeso());
                        distancias->at(arco->getDestino())->addArc(arco, distancias->at(arco->getOrigen())->getCantidadNodos());
                        cout << "Desde " << arco->getOrigen()->getInfo()->getId() << " a " << arco->getDestino()->getInfo()->getId() << endl;
                    }
                }
                F.insert_or_assign(mayor,mayor);
                VmenosF.erase(mayor);
            }
            for (auto nodo : *(distancias)){
                std::cout << "Distancia más larga desde " << starting->getInfo()->getId() << " a " << nodo.first->getInfo()->getId() << " es " << distancias->at(nodo.first)->getDistancia()<< " con nodos " <<  distancias->at(nodo.first)->getCantidadNodos() << endl;               
            }
        }

        void findCicloAux(vector<NodoGrafo*> pCiclo, NodoGrafo * currentNode, NodoGrafo * starting){
            for (Arco * entrada : *(currentNode->getEntradas())){
                if (entrada->getOrigen() == starting){
                    if (pCiclo.size() >= 3){
                        pCiclo.push_back(starting);
                        starting->addCiclo(pCiclo);
                    }
                } else if (starting->getCaminos()->at(currentNode)->getDistancia() != INT_MAX && !entrada->getOrigen()->getVisitado()){
                    pCiclo.push_back(entrada->getOrigen());
                    entrada->getOrigen()->setVisitado(true);
                    findCicloAux(pCiclo, entrada->getOrigen(), starting);
                    pCiclo.pop_back();
                } 
            }
        }

        void findCiclo(NodoGrafo * starting){

            if (starting->getCaminos()->at(starting)->getDistancia() != INT_MAX){
                
                for (Arco * entrada : *(starting->getEntradas())){
                    if (starting->getCaminos()->at(starting)->getDistancia() != INT_MAX){
                        vector<NodoGrafo*> ciclo;
                        ciclo.push_back(starting);
                        ciclo.push_back(entrada->getOrigen());
                        findCicloAux(ciclo, entrada->getOrigen(), starting);
                    }
                }
                
                for (vector<NodoGrafo*> ciclo : *(starting->getCiclos())){
                    std::cout << "Ciclo " << endl;
                    for (NodoGrafo * nodo : ciclo){
                        std::cout <<"Nodo: " << nodo->getInfo()->getId() << endl;
                    }
                }
                
            }
        }
        


        Grafo* crearGrafoGrados(){
            Grafo* result = new Grafo(true);
            for (NodoGrafo* nodo : listaNodos){
                result->addNode(nodo->getInfo());
            }
            for (NodoGrafo* nodo : listaNodos){
                int concurrencia = nodo->getArcs()->size() + nodo->getNodosEntrada()->size();
                std::cout << nodo->getArcs()->size() << " + " << nodo->getNodosEntrada()->size() << " = " << concurrencia << endl;
                for (Arco* arco : *nodo->getArcs()){
                    // std::cout << nodo->getInfo()->getId() << endl;
                    // std::cout << arco->getDestino()->getInfo()->getId() << endl;
                    result->addArc(result->getNodo(nodo->getInfo()->getId()),result->getNodo(arco->getDestino()->getInfo()->getId()), concurrencia);
                }
            }
            return result;
        }

        // Este método imprime los nodos y la cantidad de arcos que tienen 
        void printCounters() {
            for (std::vector<NodoGrafo*>::iterator current = listaNodos.begin() ; current != listaNodos.end(); ++current) {
                NodoGrafo* actual = (*current);
                std::cout << actual->getInfo()->getId() << " tiene " << actual->getArcs()->size() << endl;
                
            }
        }
};

#endif