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

        vector<vector<Arco*>> componentesConexas; // este vector contiene todos las componentes conexas, que a su vez son vectores
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
            NodoGrafo* result = nullptr; // almacena el nodo que no ha sido visitado
            for (std::vector<NodoGrafo*>::iterator current = listaNodos.begin() ; current != listaNodos.end(); ++current) {
                NodoGrafo* actual = (*current);
                if (!actual->getVisitado()) {
                    return actual; // retorna el primer nodo que no ha sido visitado
                }
            }
            return nullptr; // si todos han sido visitados, retorna nulo
        }

        // esta función encuentra el nodo que tenga la menor distancia desde el punto de partida 
        NodoGrafo * minimo(unordered_map<NodoGrafo*,NodoGrafo*>  * VmenosF, NodoGrafo * starting){
            int mx = 999999; // almacena la distancia
            NodoGrafo * v = nullptr; // almacena el nodo con la menor distancia
            for (auto nodo : (*VmenosF)){ // por cada nodo que no ha sido visitado
                if (!v){ // si el nodo es el primero, establece el nodo como el primero
                    v = nodo.first;
                }
                if (starting->getCaminos()->at(nodo.first)->getDistancia()<= mx){
                    mx = starting->getCaminos()->at(nodo.first)->getDistancia();
                    v = nodo.first;
                } 
            }
            return v;
        }

        NodoGrafo * maximo(unordered_map<NodoGrafo*,NodoGrafo*>  * VmenosF, NodoGrafo * s){
            int max = 0; // es igual a la función de mínimo, nada más que al revés
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
            // se abren los archivos
            //ofstream Gobiz("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\gobiz.csv", ios::out);
            ofstream Gobiz(".\\gobiz.csv", ios::out);
            Gobiz << "Id,Name,Tipo,Fecha,Descripcion\n";

            //ofstream Links("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\links.csv", ios::out);
            ofstream Links(".\\links.csv", ios::out);
            Links << "Source,Target,Type\n";
            vector<int> ids;
            
            for (NodoGrafo * nodo : listaNodos){ // por cada nodo en la lista
                Registered* registro = (Registered*)(void*)(nodo->getInfo()); // se obtiene el registro correspondente
                string offer = registro->getOffer(); // Se obtiene la oferta del registro 
                registro->replace_all(offer, ",", ";"); // se cambian las comas por semicolons para evitar problemas en parsing del CSV
                string demand = registro->getDemand();
                registro->replace_all(demand, ",", ";");
                int tipo; // almacena el tipo, si es de oferta, demanda o ambos
                if (offer == "" && demand != ""){
                    tipo = 1;
                } else if (offer != "" && demand == ""){
                    tipo = 0;
                } else {
                    tipo = 2;
                }
                // se escribe la información del nodo en el archivo
                for (int i : ids){
                    if (nodo->getInfo()->getId() == i){
                        goto cnt;
                    }
                }
                Gobiz << nodo->getInfo()->getId() << "," << registro->getNickname() << ","  << tipo << "," << registro->getPostdate() << "," << offer << " " << demand << "\n";
                ids.push_back(nodo->getInfo()->getId());
                cnt:;
                // Se escribe la información de los arcos de ese nodo en el archivo 
                for (Arco * arco : (*nodo->getArcs())){
                    Links << arco->getOrigen()->getInfo()->getId() << "," << arco->getDestino()->getInfo()->getId() << "," << arco->getPeso() << "\n";
                }
            }
            // se cierran los archivos
            Gobiz.close();
            Links.close();
        }

        void saveCycles(NodoGrafo * nodo){
            // se abren los archivos
            ofstream Gobiz(".\\gobiz.csv", ios::out);
           // Gobiz.open("gobiz.csv");
            Gobiz << "Id,Name,Tipo,Fecha,Descripcion\n";

            ofstream Links(".\\links.csv", ios::out);
            Links << "Source,Target,Type\n";
            int index = 0; // se utiliza para diferenciar los nodos y los distintos ciclos que hay

            for (vector<NodoGrafo*> ciclo : *(nodo->getCiclos())){ // se recorre la lista de ciclos del nodo
                Registered* registro = (Registered*)(void*)(nodo ->getInfo()); // se obtiene el registro correspondiente
                string offer = registro->getOffer(); // se reemplazan las comas por semicolons
                registro->replace_all(offer, ",", ";");
                string demand;
                registro->replace_all(demand, ",", ";");
                int tipo = 2;
                NodoGrafo * anterior; // almacena el nodo anterior del ciclo
                int quantity = 0; // almacena la cantidad de nodos en el ciclo
                int starting = nodo->getInfo()->getId() + index; // almacena el principio del nodo
                // se escribe la información del primer nodo en el archivo
                Gobiz << (starting) << "," << registro->getNickname() << ","  << tipo << "," << registro->getPostdate() << "," << offer << " " << demand << "\n";
                for (NodoGrafo * nodoE : ciclo){ // por cada nodo en el ciclo
                    registro = (Registered*)(void*)(nodoE->getInfo()); // se obtiene su información
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
                    if ((nodoE->getInfo()->getId() + index) != starting){ // si el nodo no es el primer
                    // se escribe en el archivo
                        Gobiz << (nodoE->getInfo()->getId() + index) << "," << registro->getNickname() << ","  << tipo << "," << registro->getPostdate() << "," << offer << " " << demand << "\n";
                    }
                    if (quantity != 0){
                        // si el nodo no es el primero, se escribe la información del 
                        Links << nodoE->getInfo()->getId() + index << "," << anterior->getInfo()->getId() + index << "," << 0 << "\n";
                    }
                    anterior = nodoE; // se establece el nodo actual como el anterior
                    quantity++; // se incrementa la cantidad de nodos
                }
                index += listaNodos.size(); // se incrementa el index la cantidad de nodos en el grafo
                if (anterior != nodo){ // se retorna el anterior
                    Links << anterior->getInfo()->getId() + index << "," << nodo->getInfo()->getId() + index << ","  << 0 << "\n";
                }
                
            }
            // se cierran los archivos
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
            
            int contador = 0; // contador de componentes conexas
            do {
                vector<Arco*> componente; // vector para la componente conexa
                componente.push_back(new Arco(NULL, puntoPartida, 0)); // se agrega el punto de partida a la componente
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
                            componente.push_back(arco); // se agrega a la componente
                            std::cout << arco->getPeso() << endl;
                            nodosProcesados.push(adyacente); // lo mete en la pila
                            adyacente->setProcesado(true); // lo marca
                        }
                    }
                }
                if (!cantidadComponentes){ // Si todavía no se han calculado las componentes conexas, se agregan al vector de componentes.
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
            
            int contador = 0; // contador de componentes conexas

            do {
                vector<Arco*> componente; // vector para la componente conexa
                componente.push_back(new Arco(NULL, puntoPartida, 0)); // se agrega el punto de partida a la componente
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

                        if (!adyacente->getProcesado()) { // si el nodo no ha sido procesado, se agrega a la componente
                            componente.push_back(arco);
                            nodosProcesados.push(adyacente);
                            adyacente->setProcesado(true);
                        }
                    }
                }
                if (!cantidadComponentes){ // Si todavía no se han calculado las componentes conexas, se agregan al vector de componentes.
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

            cantidadComponentes = contador; // se establece la cantidad de componentes como el contador.
            return result;
        }

        vector<vector<Arco*>>* getComponentesConexas(){
            return &componentesConexas;
        }

        // esta función salva todas las componentes conexas del grafo al archivo
        void saveComponentes(){
            int contador = 1; // contador de componentes
            ofstream Componentes(".\\componentes.json", ios::out);
            
            Componentes << "{ \n \"name\": \"Grafo\", \"children\" : [\n";

            // se recorren todas las componentes
            for (vector<Arco*> componente : componentesConexas){
                // se escribe el formato
                int porcentaje = 100 * componente.size() / listaNodos.size();
                Componentes << "{\n \"name\" : \"Componente " << contador << "\", \"size\" : " << porcentaje << ", \n \"children\": [\n";
                int contador2 = 1; // se utiliza para numerar los elementos de la componente
                for (Arco * arco : componente){ // se recorre cada arco de la componente
                    Registered* registro = (Registered*)(void*)(arco->getDestino()->getInfo());
                    int peso = arco->getPeso();
                    if (!peso){
                        peso = 1; // si el peso es 0, se establece en 1 para que se vea en el grafico
                    }

                    // se escribe en el archivo la información
                    Componentes << "{\"name\": \"" << registro->getNickname() << "\", \"size\" : " << peso << "}";
                    if (contador2 != componente.size()){ // si es el último, no se pone una coma. Si no, sí
                        Componentes << ",";
                    } 
                    Componentes << "\n";
                    contador2++; // se incrementa el contador
                } // Se escribe el final del formato de cada componente conexa.
                Componentes << "]\n}";
                if (contador != componentesConexas.size()){ // Si no es la última componente conexa, escribe una coma
                    Componentes << ",";
                }
                contador++; // se incrementa el contador
            }
            Componentes << "]\n}";
            Componentes.close(); // se cierra el archivo
        }


        // Esta función encuentra todos los caminos más cortos desde un punto de partida
        void Dijkstra(NodoGrafo * starting){
            
            unordered_map<NodoGrafo*, NodoGrafo*> VmenosF; // se almacenan los nodos que de los cuales no se han encontrado su distancia más corta
            for (NodoGrafo * nodo : listaNodos){ // se meten todos los nodos a la lista
                VmenosF.insert_or_assign(nodo, nodo);
            }
            starting->setDijkstraNodes(listaNodos); // se establecen los dijkstra nodes, los cuales contienen la distancia desde el punto de partida hasta ese nodo
            unordered_map<NodoGrafo*, DijkstraNode*> * distancias = starting->getCaminos();
            if (!starting->getArcs()->size()){
                return;
            }
            
            // En esta versión no se borra el starting del conjunto VmenosF, ya que si así se obtiene un ciclo, si este existe
            // Se recorre el vector de arcos del punto de partida para establecer sus distancias.
            for (std::vector<Arco*>::iterator current = starting->getArcs()->begin() ; current != starting->getArcs()->end(); ++current){
                distancias->at((*current)->getDestino())->setDistancia((*current)->getPeso());
                distancias->at((*current)->getDestino())->addArc((*current), distancias->at((*current)->getOrigen())->getCantidadNodos());
            }
            
            // mientras el conjunto de VmenosF no esté vacío
            while (!VmenosF.empty()){
                NodoGrafo * menor = minimo(&VmenosF, starting); // se obtiene el nodo que tiene la menor distancia
                for (std::vector<Arco*>::iterator current = menor->getArcs()->begin() ; current != menor->getArcs()->end(); ++current){ // por cada adyacente a ese nodo    
                    // Si la distancia de ese adyacente es mayor que la distancia desde el punto de partida al menor más la distancia entre el menor y el adyacente, se actualiza        
                    
                    if (distancias->at((*current)->getDestino())->getDistancia() > distancias->at((*current)->getOrigen())->getDistancia() + (*current)->getPeso() && distancias->at((*current)->getOrigen())->getDistancia() + (*current)->getPeso() < 999999){
                        distancias->at((*current)->getDestino())->setDistancia(distancias->at((*current)->getOrigen())->getDistancia() + (*current)->getPeso()); // se cambia la distancia
                        distancias->at((*current)->getDestino())->addArc((*current), distancias->at((*current)->getOrigen())->getCantidadNodos()); // se actualiza el camino
                    }
                }
                VmenosF.erase(menor); // se elimina del vector el menor ya que ya se procesó
            }
        }

        // Esta función obtiene los caminos más largos desde un punto de partida a todos los nodos
        void dijkstraMayor(NodoGrafo * starting){
            // el procedimiento es exactamente igual al dijkstran normal, nada más que en esta versión sí se borra el punto de partida
            // del conjunto VmenosF, para evitar que se haga esos recorridos
            resetNodes();
            unordered_map<NodoGrafo*, NodoGrafo*> VmenosF;
            for (NodoGrafo * nodo : listaNodos){
                VmenosF.insert_or_assign(nodo, nodo);
            }
            VmenosF.erase(starting);
            starting->setDijkstraNodes(listaNodos, 0);
            starting->setVisitado(true);
            unordered_map<NodoGrafo*, DijkstraNode*> * distancias = starting->getCaminos();
            distancias->at(starting)->setDistancia(0);
            for (Arco* arco : *starting->getArcs()){
                distancias->at(arco->getDestino())->setDistancia(arco->getPeso());
                distancias->at(arco->getDestino())->addArc(arco, distancias->at(arco->getOrigen())->getCantidadNodos());
            }
            
            while (!VmenosF.empty()){
                NodoGrafo * mayor = maximo(&VmenosF, starting); // se obtiene el nodo con la mayor distancia.
                for (Arco* arco : *mayor->getArcs()){    // Se recorren todos los arcos.
                    // si la distancia al adyacente es menor que la distancia desde el punto inicial al mayor + la distancia desde el mayor al adyacente
                    // y el origen no ha sido visitado y la distancia desde el mayor no es 0, se actualiza la distancia 
                    if (!arco->getDestino()->getVisitado() && distancias->at(arco->getDestino())->getDistancia() < distancias->at(arco->getOrigen())->getDistancia() + arco->getPeso() && distancias->at(arco->getOrigen())->getDistancia() + arco->getPeso() != arco->getPeso()){
                        distancias->at(arco->getDestino())->setDistancia(distancias->at(arco->getOrigen())->getDistancia() + arco->getPeso());
                        distancias->at(arco->getDestino())->addArc(arco, distancias->at(arco->getOrigen())->getCantidadNodos());
                        arco->getOrigen()->setVisitado(true);
                    }
                }
                VmenosF.erase(mayor); // se elimina el mayor del conjunto
            }
        }

        // Esta función auxiliar se utiliza para realizar el procedimiento recursivo de encontrar los ciclos.
        // Se recorren todos los nodos del grafo y si se llega al punto inicial, añade todo el camino al vector de ciclos del nodo
        // Los caminos se van recorriendo al revés, revisando las entradas cada nodo.
        void findCicloAux(vector<NodoGrafo*> pCiclo, NodoGrafo * currentNode, NodoGrafo * starting){
            // Recibe el camino que se lleva, el nodo actual, y el punto de partida
            for (Arco * entrada : *(currentNode->getEntradas())){ // por cada entrada del nodo
                if (entrada->getOrigen() == starting){ // si el anterior a ese nodo es el punto inicial,
                    if (pCiclo.size() >= 3){ // si el ciclo que se lleva tiene un tamaño mayor o igual a 3
                        pCiclo.push_back(starting); // se añade el punto de partida al ciclo
                        starting->addCiclo(pCiclo); // se añade el ciclo al vector
                        pCiclo.pop_back(); // se elimina el punto de partida del ciclo para que no aparezca en el ciclo de la siguiente entrada
                    }
                    // Si no, revisa si la distancia al nodo actual no es infinito y si la entrada no ha sido visitada
                } else if (starting->getCaminos()->at(currentNode)->getDistancia() != 999999 && !entrada->getOrigen()->getVisitado()){
                    // si es el caso, 
                    pCiclo.push_back(entrada->getOrigen()); // añade la entrada al ciclo
                    entrada->getOrigen()->setVisitado(true); // la establece como visitada
                    findCicloAux(pCiclo, entrada->getOrigen(), starting); // vuelve a llamar la función con los valores actualizados
                    pCiclo.pop_back(); // elimina la entrada del ciclo para que no aparezca en la siguiente iteración
                } 
            }
        }

        // esta es la función principal de encontrar ciclos.
        void findCiclo(NodoGrafo * starting){
            // si hay un camino desde el punto de partida al punto de partida
            if (starting->getCaminos()->at(starting)->getDistancia() != 999999){
                // Se recorre cada entrada del punto de partida
                for (Arco * entrada : *(starting->getEntradas())){
                    vector<NodoGrafo*> ciclo; // se crea el vector del ciclo
                    ciclo.push_back(starting); // se añade el punto de partida al ciclo
                    ciclo.push_back(entrada->getOrigen()); // se añade la entrada al ciclo
                    findCicloAux(ciclo, entrada->getOrigen(), starting); // se llama a la función auxiliar
                }
                
                // se imprimen los ciclos encontrados
                for (vector<NodoGrafo*> ciclo : *(starting->getCiclos())){
                    std::cout << "Ciclo " << endl;
                    for (NodoGrafo * nodo : ciclo){
                        std::cout <<"Nodo: " << nodo->getInfo()->getId() << endl;
                    }
                }
                
            }
        }
        

        // Esta función crea el grafo de grados de cada nodo del grafo actual
        Grafo* crearGrafoGrados(){
            Grafo* result = new Grafo(true); // el nuevo grafo
            for (NodoGrafo* nodo : listaNodos){ // Se añaden todos los nodos
                result->addNode(nodo->getInfo());
            }
            for (NodoGrafo* nodo : listaNodos){ // se establecen los mismos arcos con la concurrencia como peso.
                int concurrencia = nodo->getArcs()->size() + nodo->getNodosEntrada()->size();
                for (Arco* arco : *nodo->getArcs()){ 
                    result->addArc(result->getNodo(nodo->getInfo()->getId()),result->getNodo(arco->getDestino()->getInfo()->getId()), concurrencia);
                }
            }
            return result; // se retorna el grafo
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