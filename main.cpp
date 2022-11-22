#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <ctime>
#include <fstream>
#include <math.h>
#include "Grafo/Grafo.h"
#include "Grafo/INodo.h"
#include "Grafo/Arco.h"
#include "Registered.h"
#include "Match.h"
#include "BPlus/StringData.h"
#include "BPlus/BPlusTree.h"
#include "OrdenCadenasMin.h"
#include "OrdenGrado.h"
#include "OrdenWords.h"

#define NUM_ANIMALES 14

#define COMPRADORES_POR_ARBOL 6
#define ORDEN 10
#define SIZE 20
#define WORDS_TOP 3
#define ELEMENTOS_TOP 10
#define RANKING_PERCENT 0.8

using namespace std;

// Función que crea un grafo a partir del conjunto de registros y retorna el nuevo grafo creado
Grafo* crearGrafo(vector<Registered*> records){
    Grafo *grafo = new Grafo(true);
    for (Registered* record : records){
        grafo->addNode(record); // agrega un nuevo nodo por cada registro.
    }
    return grafo;
}

// Función que crea analiza los matches y crea los arcos en el grafo que recibe.
void crearMatches(Grafo* grafo){
    vector<Registered*> *compradores = Registered::getCompradores();
    vector<Registered*> *vendedores = Registered::getVendedores();

    int index = 0;
    vector<Match> ranking;
    while (index < compradores->size()){ // mientras el índice no haya llegado al final.
        int cantidad = compradores->size() - index;
        if (cantidad > COMPRADORES_POR_ARBOL){ // Si la cantidad se pasa del máximo permitido, lo limitamos al máximo.
            cantidad = COMPRADORES_POR_ARBOL;
        }

        BPlusTree *arbol = new BPlusTree(ORDEN, SIZE); // se crea un nuevo árbol
        for (int contador = 0; contador < cantidad; contador++, index++){ // vamos a agregar la cantidad de compradores que diga la constante
            for (StringData* palabra : *compradores->at(index)->getWordsDemand()){
                arbol->insert(palabra); // se inserta cada palabra en el árbol
            }
        }
        arbol->print();
        std::cout << endl;

        for (Registered* vendedor : *vendedores){
            // vamos a recorrer los vendedores e ir comparando sus palabras con las palabras del árbol para hacer matches.

            unordered_map<string, Match*> *matches = new unordered_map<string, Match*>();
            // hashmap que va a almacenar los matches con cada comprador
            
            for (StringData* palabra : *vendedor->getWordsOffer()){ // se itera sobre las palabras del vendedor
                int leafIndex = 0;
                LeafNode* leaf = arbol->find(palabra, leafIndex); // la posición de la palabra queda en leaf y en el índice leafIndex
                if (leaf){ // si no es nullptr, se encontró la palabra en el árbol
                    while (!palabra->compareTo(leaf->getSecuencia()->at(leafIndex))){
                        // recorre el conjunto secuencia mientras las palabras sean iguales, haciendo los matches.

                        StringData* strData = (StringData*)(void*)(leaf->getSecuencia()->at(leafIndex)); // extrae el data del nodo hoja
                        Registered* comprador = strData->getUsuario();
                        if (matches->find(comprador->getNickname()) == matches->end()){ // Si el comprador no está en el hash, lo agrega
                            matches->insert(pair<string, Match*> (comprador->getNickname(), new Match(comprador, vendedor)));
                        }
                        Match *match = matches->at(comprador->getNickname()); // extraemos el match con el comprador;

                        match->incrementPeso(); // incrementa el peso del match
                        leafIndex++; // se corre a la siguiente posición
                        match->addWord(palabra); // guarda la palabra que hizo match

                        if (leafIndex == leaf->getSecuencia()->size()){ // si llegó al final 
                            if (leaf->getSecuencia()->size() == arbol->getSize()){ // revisa si ya llegamos al final del nodo
                                // se pasa al hermano.
                                leaf = leaf->getBrother();
                                leafIndex = 0;
                            } else {
                                break;
                            }
                        }
                    }
                }
            }
            for(auto iterador = matches->begin(); iterador != matches->end(); iterador++){
                // mete los matches en el ranking para quedarse con los mejores.
                ranking.push_back(*(iterador->second));
            }
        }
    }
    std::sort(ranking.begin(), ranking.end()); // ordena el ranking de menor a mayor en cuanto al rating del match
    auto riterator = ranking.rbegin();
    Match *match = &(*riterator);
    int cantidadMatches = 0;
    int maxCantidad = ceil(ranking.size() * RANKING_PERCENT);
    while(cantidadMatches < maxCantidad && riterator != ranking.rend()){
        if (match->getComprador() != match->getVendedor()){
            // std::cout << match->getVendedor()->getNickname() << " apunta a " << match->getComprador()->getNickname() << " con rating " << match->getRating() << " con " << endl;
            // for (StringData* str : *match->getWords()){
            //     std::cout << str->toString() << " ";
            // }
            // std::cout << endl;
            grafo->addArc(match->getVendedor(), match->getComprador(), match->getRating());
            match->getVendedor()->addMatchSalida(*match);
            match->getComprador()->addMatchEntrada(*match);
            cantidadMatches++;
        }
        riterator++;
        match = &(*riterator);
    }
}

// Esta función averigua el Top 10
vector<string>* top10(Grafo* grafo){
    auto nodos = grafo->getNodos();
    set<NodoGrafo*, OrdenGrado> ordenGrado;
    // se ordenan los nodos del grafo en cuanto al grado de salida, ya que los arcos apuntan del vendedor al comprador.
    // Los nodos con más arcos de salida representan el producto más deseado.
    for (NodoGrafo* nodo : nodos){
        ordenGrado.insert(nodo);        
    }

    vector<string> *topRanking = new vector<string>();

    int contadorRanking = 0;
    auto riterator = ordenGrado.rbegin();
    while (contadorRanking < ELEMENTOS_TOP && riterator != ordenGrado.rend()){
        // se recorre el orden al revés para empezar con el mayor.

        Registered* registro = (Registered*)(void*)((*riterator)->getInfo());
        unordered_map<StringData*, int> palabras; // hash map para contar las apariciones de las palabras en los matches.
        for (Match match : *registro->getMatchesSalida()){
            for (StringData* word : *match.getWords()){
                try{
                    palabras.at(word) = ++palabras.at(word); // si la palabra existe en el map, se incrementa el contador.
                } catch (...){
                    // si no está en el map, se inserta al map.
                    palabras.insert(pair<StringData*, int>(word, 1));
                }
            }
        }

        set<pair<StringData*, int>, OrdenWords> rankingPalabras(palabras.begin(), palabras.end());
        // Se ordenan las parejas de palabras en orden de apariciones de menor a mayor.
        int contadorPalabras = 0;
        auto ritSet = rankingPalabras.rbegin();
        string entry = ""; // string donde se almacenan las primeras palabras más comunes para formar la categoría.
        while (contadorPalabras < WORDS_TOP && ritSet != rankingPalabras.rend()){
            string fragment = (*ritSet).first->getPalabra();
            string str = registro->getFullWordsOffer()->at(fragment); // se obtiene la palabra completa a partir del fragmento.
            str[0] = toupper(str[0]); // se pasa la primera letra a mayúscula.
            entry += str; // se concatena la palabra al string.
            contadorPalabras++;
            ritSet++;
        }
        if (entry != ""){ // agregamos el último entry al ranking.
            topRanking->push_back(entry);
        }
        riterator++;
        contadorRanking++;
    }
    return topRanking;
}

// Esta función salva el Top 10 en un archivo para desplegarlo
void saveTop10(Grafo * pGrafo){ 
    ofstream TopFile("C:\\Users\\dandi\\OneDrive - Estudiantes ITCR\\Documentos\\TEC\\II Semestre\\Estructura de Datos\\Caso5\\top10.csv", ios::out);
    TopFile << "id,value" << endl;
    vector<string>* topRanking = top10(pGrafo);
    int contador = 1;
    for (auto it = topRanking->rbegin(); it != topRanking->rend(); it++){
        TopFile << "flare.Posición " << contador << "." << *it << "," << pow(contador,2)+10 << endl;
        contador++;
    }
    TopFile.close();
}

// Esta función calcula la mayor cadena con menor concurrencia de un grafo.
// El grafo que recibe debe ser el grafo donde los arcos son las concurrencias del nodo.
// Deja la mayor concurrencia en la variable dada por referencia.
vector<NodoGrafo*>* menorCadena(Grafo* grafo, int &pConcurrencia){
    set<DijkstraNode*, OrdenCadenasMin> cadenas;
    for (NodoGrafo* nodo : grafo->getNodos()){
        grafo->Dijkstra(nodo);
        auto caminos = nodo->getCaminos();
        for (auto iterador = caminos->begin(); iterador != caminos->end(); iterador++){
            cadenas.insert(iterador->second);
        }
    }

    for (auto rit = cadenas.rbegin(); rit != cadenas.rend(); rit++){
        int concurrencia = (*rit)->getDestino()->getArcs()->size() + (*rit)->getDestino()->getNodosEntrada()->size();
        std::cout << (*rit)->getStarting()->getInfo()->getId() << " a " << (*rit)->getDestino()->getInfo()->getId() << " " << (*rit)->getDistancia() + concurrencia << " " << (*rit)->getCantidadNodos() << endl;
    }

    DijkstraNode* menor = *cadenas.rbegin();
    for (auto rit = ++cadenas.rbegin(); rit != cadenas.rend(); rit++){
        if ((*rit)->getCantidadNodos() == menor->getCantidadNodos()){
            int concurrenciaMenor = menor->getDestino()->getArcs()->size() + menor->getDestino()->getNodosEntrada()->size();
            int concurrenciaRit = (*rit)->getDestino()->getArcs()->size() + (*rit)->getDestino()->getNodosEntrada()->size();
            if ((*rit)->getDistancia() + concurrenciaRit < menor->getDistancia() + concurrenciaMenor){
                menor = *rit;
            }
        } else {
            break;
        }
    }
    pConcurrencia = menor->getDistancia() + menor->getDestino()->getArcs()->size() + menor->getDestino()->getNodosEntrada()->size();
    std::cout << menor->getDestino()->getInfo()->getId() << ' ' << menor->getStarting()->getInfo()->getId() << endl;
    vector<NodoGrafo*> * result = new vector<NodoGrafo*>();
    NodoGrafo * starting = menor->getStarting();
    result->push_back(menor->getDestino());
    while (menor->getCamino()->getOrigen() != starting){
        result->push_back(menor->getCamino()->getOrigen());
        menor = starting->getCaminos()->at(menor->getCamino()->getOrigen());
    }
    result->push_back(starting);
    return result;
}

// Esta función calcula la mayor cadena con mayor concurrencia de un grafo.
// El grafo que recibe debe ser el grafo donde los arcos son las concurrencias del nodo.
// Deja la mayor concurrencia en la variable dada por referencia.
vector<NodoGrafo*>* mayorCadena(Grafo *grafo, int &pConcurrencia){
    set<DijkstraNode*, OrdenCadenasMin> cadenas;
    for (NodoGrafo* nodo : grafo->getNodos()){
        grafo->dijkstraMayor(nodo);
        auto caminos = nodo->getCaminos();
        for (auto iterador = caminos->begin(); iterador != caminos->end(); iterador++){
            cadenas.insert(iterador->second);
        }
    }

    for (auto rit = cadenas.rbegin(); rit != cadenas.rend(); rit++){
        int concurrencia = (*rit)->getDestino()->getArcs()->size() + (*rit)->getDestino()->getNodosEntrada()->size();
        std::cout << (*rit)->getStarting()->getInfo()->getId() << " a " << (*rit)->getDestino()->getInfo()->getId() << " " << (*rit)->getDistancia() + concurrencia << " " << (*rit)->getCantidadNodos() << endl;
    }
    
    DijkstraNode* mayor = *cadenas.rbegin();
    pConcurrencia = mayor->getDistancia() + mayor->getDestino()->getArcs()->size() + mayor->getDestino()->getNodosEntrada()->size();
    std::cout << mayor->getDestino()->getInfo()->getId() << ' ' << mayor->getStarting()->getInfo()->getId() << endl;
    vector<NodoGrafo*> * result = new vector<NodoGrafo*>();
    NodoGrafo * starting = mayor->getStarting();
    result->push_back(mayor->getDestino());
    while (mayor->getCamino()->getOrigen() != starting){
        result->push_back(mayor->getCamino()->getOrigen());
        mayor = starting->getCaminos()->at(mayor->getCamino()->getOrigen());
    }
    result->push_back(starting);
    return result;
}

// Esta función imprime todos los matches que tiene un grafo.
void printGraph(Grafo* grafo){
    int contador = 1;
    for (NodoGrafo * nodo : grafo->getNodos()) {
        Registered* registro = (Registered*)(void*)(nodo->getInfo());
        std::cout << registro->getId() << ". " << registro->getNickname() << endl;
        contador++;
        std::cout << "Matches" << endl;
        contador = 1;
        std::cout << "  Oferta:" << endl;
        for (Arco * arco : *nodo->getArcs()){
            Registered* registro = (Registered*)(void*)(arco->getDestino()->getInfo());
            std::cout << "    " << contador <<". " << registro->getNickname() << " con una calificación de " << arco->getPeso() << endl;      
            contador++;
        }
        std::cout << "  Demanda:" << endl;
        contador = 1;
        for (Arco * arco : *nodo->getEntradas()){
            Registered* registro = (Registered*)(void*)(arco->getOrigen()->getInfo());
            std::cout << "    " << contador <<". " << registro->getNickname() << " con una calificación de " << arco->getPeso() << endl;      
            contador++;
        }
    }
}

int main(){

    vector<Registered*> allrecords;
    allrecords.push_back(new Registered("Wakanda_Med","Vendemos máquinas con fines médicos de alta tecnología y calidad, hechas de metales exóticos, para tratamientos de enfermedades o heridas fatales.","Necesitamos muchos programadores con mucha experiencia para desarrollar el software de nuestros dispositivos médicos","11/16/2022"));
    allrecords.push_back(new Registered("vendePollos38","Se vende pollo asado, frito o al ajillo","carne de pollo de la más alta calidad y aceite","11/03/2022"));
    allrecords.push_back(new Registered("elvendeTacos29","venta de tacos al estilo mexicano tradicional","Verduras frescas de la más alta calidad y carne de res o cerdo","11/02/2022"));
    allrecords.push_back(new Registered("badbunny_in_concert","conciertos a estadio lleno de gente escuchando pum pum con el mismo acorde por 2 horas","transporte y seguridad en todos los paises que visita y mucha fiesta tambien","11/02/2022"));
    allrecords.push_back(new Registered("rock_odin_gold","soy programador python con experiencia","","01/23/2019"));
    allrecords.push_back(new Registered("metal_year_2000","","requiero de 5 ingenieros en computación con 8 años de experiencia, conocimientos en python.","10/28/2022"));
    allrecords.push_back(new Registered("josearnoldowood","esta es la oferta","esta es la demanda","10/10/2022"));
    allrecords.push_back(new Registered("Hospital_Tec","Somos una institución que brinda un servicio de alta calidad en el área de la salud. Nos especializamos en tratar lesiones provocadas por accidentes en el área de trabajo.","Necesitamos dispositivos médicos de alta calidad y tecnología, hechos de metales resistentes, para sacar radiografías de fracturas, resonancias magnéticas, y quirófanos.","11/16/2022"));
    allrecords.push_back(new Registered("EstructurasMina","Somos una empresa constructora que construye edificios modernos y espaciosos de oficinas. Nuestros edificios pueden acomodar la última tecnología fácilmente y están diseñados para tener altas velocidades de internet.","Un convenio con una institución de salud para atender a nuestros empleados que resulten lesionados por accidentes en el área laboral para que se recuperen rápidamente.","11/16/2022"));
    allrecords.push_back(new Registered("RealSolutions","Somos una empresa desarrolladora de software. Contamos con un equipo de programadores con mucha experiencia y conocimientos en muchos lenguajes como Python. Nuestros ingenieros pueden trabajar en todo tipo de dispositivos.","Necesitamos un edificio de oficinas para acomodar a nuestro equipo de cincuenta personas. Debe poder tener altas velocidades de Internet, ser moderno y tecnológico.","11/16/2022"));
    allrecords.push_back(new Registered("construSoluciones2001","Ofrecemos mano de obra y materiales para la construccion de su casa, edificio o proyect","Materiales de construccion de las mas alta claidad e ingenieros con experiencia","11/71/2022"));
    allrecords.push_back(new Registered("AstraMedical222","Somos una empresa farmacéutica que desarrolla medicamentos para combatir enfermedades respiratorias para niños de escuela con factores de riesgo","Dispositivos médicos de alta tecnología y calidad para ayudarnos en nuestro trabajo de investigación de medicamentos para tratar enfermedades respiratorias infantiles.","11/17/2022"));
    allrecords.push_back(new Registered("Mesa_School","Somos una institución educativa para niños y adolescentes talentosos que busca explotar su potencial intelectual desarrollando sus habilidades STEM para llegar a ser los futuros ingenieros en computación.","Necesitamos medicamentos para los estudiantes de la escuela, especialmente para los niños de primaria ya que las enfermedades respiratorias tienen una alta incidencia en la población estudiantil.","11/17/2022"));
    allrecords.push_back(new Registered("CompuYankeeDev129","Soy un ingeniero en computación con cuatro años de experiencia. He trabajado como programador en Python y Java en empresas desarrolladoras de software como RealSolutions.","Busco entretenimiento. Me gustaría asistir a un concierto de 2 horas con el estadio lleno de gente y no me preocupa la falta de complejidad armónica en los acordes ya que me gusta el reguetón.","11/17/2022"));
    allrecords.push_back(new Registered("OlcoManGold","Me desempeño como programador y desarrollador de software. Tengo mucha experiencia tras trabajar once años en la industria. Soy experto en lenguajes como Python y me especializo en el desarrollo del firmware para dispositivos","Soy aficionado a la gastronomía mexicana, por lo que necesito poder almorzar tacos de pollo, ya sea asado, frito o al ajillo, todos los días y desde un punto de venta accesible.","11/17/2022"));
    allrecords.push_back(new Registered("BobTheBuilder250","Vendemos materiales para la construcción de edificios, casas y proyectos de la más alta calidad. Adicionalmente, tenemos un gran equipo de ingenieros civiles con mucha experiencia en el desarrollo de infraestructura pública.","Ocupo una institución educativa para mis hijos, quienes poseen gran talento para las áreas STEM, y quienes desean ser futuros ingenieros.","11/17/2022"));
    allrecords.push_back(new Registered("the_agustd7","composicion de letras y beats de diferentes generos musicales","artistas musicales talentosos con facilidad de adaptarse facilmente a un proyecto musical","11/18/2022"));
    allrecords.push_back(new Registered("Kenneth483883","vendo","","11/02/2022"));
    allrecords.push_back(new Registered("arbolito4848","se vende arbolitos de pino a buen precio","","11/02/2022"));
    //allrecords.push_back(new Registered("","","",""));

    while (true) {
        int opcion;
        std::cout << "Gobiz" << endl;
        std::cout << "1. Registarse" << endl;
        std::cout << "2. Revisar matches" << endl;
        std::cout << "3. Analizar matches" << endl;
        std::cout << "4. Salir" << endl;
        std::cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1){
            while (true) { // Se pide la información del registro.
                string nickname;
                std::cout << "Ingrese un nickname:" << endl; // 
                cin >> nickname; 
                // se validan las restricciones
                if (nickname.size() < 10 || nickname.size() > 32){
                    std::cout << "ERROR: Ese nickname no es válido" << endl;
                    continue;
                }

                //validar existencia del nickname
                if (Registered::validateNickname(nickname)){
                    cout << "ERROR: Ese nickname ya está registrado." << endl;
                    continue;
                }

                string contraseña;
                std::cout << "Ingrese su contraseña" << endl;
                cin >> contraseña;

                if (contraseña.size() > 20 || contraseña == ""){
                    std::cout << "ERROR: La contraseña no puede tener más de 20 caracteres" << endl;
                    continue;
                }

                string confirm;
                std::cout << "Confirme su contraseña" << endl;
                cin >> confirm;
                if (contraseña != confirm) {
                    std::cout << "ERROR: Las dos contraseñas no son iguales" << endl;
                    continue;
                }

                string offer;
                std::cout << "Ingrese su oferta o déjelo vacío si no ofrece nada" << endl;
                cin >> offer;

                if (offer.size() > 250){
                    std::cout << "ERROR: La oferta no puede tener más de 250 caracteres." << endl;
                    continue;
                }
                string demand; 
                std::cout << "Ingrese su demanda o déjelo vacío si no demanda nada" << endl;
                cin >> demand;

                if (demand.size() > 250){
                    std::cout << "ERROR: La demanda no puede tener más de 250 caracteres." << endl;
                    continue;
                }

                time_t now = time(0); // se obtiene la fecha actual

                tm *ltm = localtime(&now); // Se obtiene la fecha actual
                string date = ltm->tm_mon + "/" + ltm->tm_mday;
                date += "/";
                date += ltm->tm_year;
                std::cout << date << endl;
                // Subir al server
                //regs.registerUser(nickname, offer, demand, password, ltm->tm_mday, ltm->mon, ltm->tm_year);
                //regs.registerUser("EstructurasMina", "Somos una empresa constructora que construye edificios de oficinas modernos y espaciosos. Nuestros edificios pueden acomodar la última tecnología fácilmente y están diseñados para tener altas velocidades de internet.", "Un convenio con una institución de salud para atender a nuestros empleados que resulten lesionados por accidentes en el área laboral para que se recuperen rápidamente.", "SteveCEO067", 16, 11, 2022);
                allrecords.push_back(new Registered(nickname, offer,demand,date));
            }

        } else if (opcion == 2){
            //print all matches
            //imprime todo el grafo
            Grafo *grafo = crearGrafo(allrecords);
            crearMatches(grafo);
            printGraph(grafo);
            grafo->saveToFile();
            cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;

        } else if (opcion == 3){
            //obtain function to analize
            Grafo *grafo = crearGrafo(allrecords); // se crea el grafo
            crearMatches(grafo); // Se crean los matches
            while (true) {
                int opcion2; // se pide la elección del usuario
                std::cout << "Analizar" << endl;
                std::cout << "1. Visualizar matches actuales" << endl;
                std::cout << "2. Comercio circular" << endl;
                std::cout << "3. Tamaño de cadena de valor" << endl;
                std::cout << "4. Gráfico con el Top 10 de productos más codiciados" << endl;
                std::cout << "5. Áreas de mercado conexas" << endl;
                std::cout << "6. Salir" << endl;
                std::cout << "Opción: ";
                cin >> opcion2;
                if (opcion2 == 1){
                    int contador = 1;
                    for (NodoGrafo * nodo : grafo->getNodos()) {
                        Registered* registro = (Registered*)(void*)(nodo->getInfo());
                        std::cout << contador << ". " << registro->getNickname() << endl;
                        contador++;
                    }
                    std::cout << contador << ". Todos" << endl;
                    int idMatch;
                    std::cout << "Opción: ";
                    cin >> idMatch;
                    if (idMatch == contador){
                        //imprime todo el grafo
                        printGraph(grafo);
                        grafo->saveToFile();
                        cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;
                    } else {
                        NodoGrafo * nodeMatch = grafo->getNodo(idMatch);
                        std::cout << "Matches" << endl;
                        contador = 1;
                        std::cout << "Oferta:" << endl;
                        Grafo newGrafo = new Grafo(true);
                        newGrafo.addNode(nodeMatch->getInfo());
                        for (auto it = nodeMatch->getArcs()->begin(); it != nodeMatch->getArcs()->end(); it++){
                            Registered* registro = (Registered*)(void*)((*it)->getDestino()->getInfo());
                            std::cout << contador <<". " << registro->getNickname() << " con una calificación de " << (*it)->getPeso() << endl;      
                            contador++;
                            newGrafo.addNode((*it)->getDestino()->getInfo());
                            newGrafo.addArc(nodeMatch->getInfo()->getId(), (*it)->getDestino()->getInfo()->getId(), (*it)->getPeso());
                        }
                        std::cout << "Demanda:" << endl;
                        contador = 1;
                        for (Arco * arco : *nodeMatch->getEntradas()){
                            Registered* registro = (Registered*)(void*)(arco->getOrigen()->getInfo());
                            std::cout << contador <<". " << registro->getNickname() << " con una calificación de " << arco->getPeso() << endl;      
                            contador++;
                            if (!newGrafo.getNodo(registro->getId())){
                                newGrafo.addNode(arco->getOrigen()->getInfo());
                            }
                            newGrafo.addArc(arco->getOrigen()->getInfo()->getId(), nodeMatch->getInfo()->getId(), arco->getPeso());
                        }
                        newGrafo.saveToFile();
                        cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;
                    }
                    
                    
                } else if (opcion2 == 2) {
                    for (NodoGrafo * nodo : grafo->getNodos()) { // Se imprimen los registros
                        Registered* registro = (Registered*)(void*)(nodo->getInfo());
                        std::cout << nodo->getInfo()->getId() << ". " << registro->getNickname() << endl;
                    }
                    string opcion3; // se pide la elección del usuario
                    std::cout << "Opción: ";
                    cin >> opcion3;
                    int idCycle = std::stoi(opcion3);
                    NodoGrafo * nodeCycle = grafo->getNodo(idCycle); // obtiene el nodo con base en el Id ingresado 
                    grafo->Dijkstra(nodeCycle); // se encuentran todos los menores caminos
                    grafo->findCiclo(nodeCycle); // se encuentran todos los ciclos
                    grafo->saveCycles(nodeCycle); // se salvan los ciclos al archivo
                    cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;
                
                } else if (opcion2 == 3) { 
                    Grafo* grados = grafo->crearGrafoGrados(); // se crea el grafo con todas las concurrencias
                    int concurrencia = 0; // se obtiene el total de concurrencia
                    while (true){
                        cout << "Cadena de valor: " << endl;
                        cout << "1. Cadena menor" << endl;
                        cout << "2. Cadena mayor" << endl;
                        cout << "3. Salir" << endl;
                        int opcion3; // se pide la elección del usuario
                        std::cout << "Opción: ";
                        cin >> opcion3;
                        if (opcion3 == 1){
                            vector<NodoGrafo*> *cadenaMin = menorCadena(grados, concurrencia); // se obtiene la menor cadena
                            Grafo newGrafo = new Grafo(true);
                            cout << "Cadena de valor más larga con menor concurrencia: " << endl;
                            cout << "Concurrencia: " << concurrencia << endl;
                            
                            // se imprimen los nodos
                            NodoGrafo * anterior = nullptr; // almacena el nodo anterior de la cadena
                            for (NodoGrafo* nodo: *cadenaMin){ // recorre toda la cadena
                                newGrafo.addNode(nodo->getInfo()); // se añade el nodo al grafo nuevo
                                if (anterior){ // Si no es el primer nodo,
                                // se añade el arco desde el anterior al actual
                                    newGrafo.addArc(anterior->getInfo()->getId(),nodo->getInfo()->getId(), (anterior->getNodosEntrada()->size() + anterior->getArcs()->size()));
                                }
                                anterior = nodo; // se cambia el anterior al actual
                                Registered* nickname = (Registered*)(void*)(nodo->getInfo()); // se imprime el nickname
                                cout << "   " << nickname->getNickname() << ", Concurrencia: " << (nodo->getArcs()->size() + nodo->getNodosEntrada()->size()) << endl;
                            }
                            newGrafo.saveToFile();
                            cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;
                        } else if (opcion3 == 2){
                            cout << "Cadena de valor más larga con mayor concurrencia:" << endl;
                            Grafo newGrafo = new Grafo(true);
                            vector<NodoGrafo*> *cadenaMax = mayorCadena(grados, concurrencia); // se obtiene la cadena más larga
                            cout << "Concurrencia: " << concurrencia << endl;
                            // se imprimen los nodos
                            NodoGrafo * anterior = nullptr;
                            for (NodoGrafo* nodo: *cadenaMax){
                                newGrafo.addNode(nodo->getInfo()); // se añade el nodo al grafo nuevo
                                if (anterior){ // Si no es el primer nodo,
                                // se añade el arco desde el anterior al actual
                                    newGrafo.addArc(anterior->getInfo()->getId(),nodo->getInfo()->getId(), (anterior->getNodosEntrada()->size() + anterior->getArcs()->size()));
                                }
                                anterior = nodo; // se cambia el anterior al actual
                                Registered* nickname = (Registered*)(void*)(nodo->getInfo()); // se imprime el nickname
                                cout << "   " << nickname->getNickname() << ", Concurrencia: " << (nodo->getArcs()->size() + nodo->getNodosEntrada()->size()) << endl;
                            }
                            newGrafo.saveToFile();
                            cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;
                        } else {
                            break;    
                        }
                    }
                } else if (opcion2 == 4) {
                    cout << "Top 10" << endl;
                    vector<string>* topRanking = top10(grafo); // se crea el vector con los productos más cotizados
                    for (auto it = topRanking->begin(); it != topRanking->end(); it++){
                        cout << "  " << *it << endl;
                    } // se imprimen los productos
                    cout << "Link a la página: https://observablehq.com/d/c2adc8c189e19fe9" << endl;
                } else if (opcion2 == 5) {
                    Grafo* grados = grafo->crearGrafoGrados(); // se crea el grafo de nodos
                    vector<INodo*> anchura = grados->deepPath(grados->getNodo(1)->getInfo()); // se realiza un recorrido en anchura

                    cout << "Recorrido en anchura" << endl;
                    for (INodo* nodo : anchura){ // se recorre el vector
                        Registered * registro = (Registered*)(void*)(nodo); // se imprime el 
                        cout << "     " << registro->getNickname() << endl;
                    }

                    cout << "Componentes conexas" << endl;
                    vector<vector<Arco*>> * componentes = grados->getComponentesConexas(); // se obtienen las componentes conexas

                    for(vector<Arco*> componente : *componentes){ // se  recorre el vector de las componentes conexas
                        int porcentaje = 100 * componente.size() / grados->getNodos().size(); // se obtiene el porcentaje del
                        cout << "  Componente, " << "Porcentaje: " << porcentaje << endl; // obtiene el porcentaje del componente
                        for (Arco* arco : componente){ // se recorre cada arco de la componente
                            NodoGrafo* nodo = (NodoGrafo*)arco->getDestino(); // se imprimen los nicknames de cada componente
                            Registered* registro = (Registered*)(void*)(nodo->getInfo());
                            cout << "    " << registro->getNickname() <<  endl;
                        }
                    } 
                    grados->saveComponentes(); // se salvan las componentes
                    cout << "Link: https://observablehq.com/d/0e47ebf585a89363" << endl;
                } else {
                    break;
                }
            }
        } else if (opcion == 4) {
            break;
        } else {
            std::cout << "ERROR: OPCIÓN NO ES VÁLIDA" << endl;
            break;
        }
    }
}

/*
Hospital_Tec
Somos una institución que brinda un servicio de alta calidad en el área de la salud. Nos especializamos en tratar lesiones provocadas por accidentes en el área de trabajo.
Necesitamos dispositivos médicos de alta calidad y tecnología, hechos de metales resistentes, para sacar radiografías de fracturas, resonancias magnéticas, y quirófanos.
salud1234
11 16 2022

EstructurasMina
Somos una empresa constructora que construye edificios modernos y espaciosos de oficinas. Nuestros edificios pueden acomodar la última tecnología fácilmente y están diseñados para tener altas velocidades de internet.
Un convenio con una institución de salud para atender a nuestros empleados que resulten lesionados por accidentes en el área laboral para que se recuperen rápidamente.
SteveCEO067
11 16 2022

RealSolutions
Somos una empresa desarrolladora de software. Contamos con un equipo de programadores con mucha experiencia y conocimientos en muchos lenguajes como Python. Nuestros ingenieros pueden trabajar en todo tipo de dispositivos.
Necesitamos un edificio de oficinas para acomodar a nuestro equipo de cincuenta personas. Debe poder tener altas velocidades de Internet, ser moderno y tecnológico.
RealHackers007
11 16 2022

AstraMedical222
Somos una empresa farmacéutica que desarrolla medicamentos para combatir enfermedades respiratorias para niños de escuela con factores de riesgo.
Dispositivos médicos de alta tecnología y calidad para ayudarnos en nuestro trabajo de investigación de medicamentos para tratar enfermedades respiratorias infantiles.
Asthma_No8261
11 17 2022

Mesa_School
Somos una institución educativa para niños y adolescentes talentosos que busca explotar su potencial intelectual desarrollando sus habilidades STEM para llegar a ser los futuros ingenieros en computación.
Necesitamos medicamentos para los estudiantes de la escuela, especialmente para los niños de primaria ya que las enfermedades respiratorias tienen una alta incidencia en la población estudiantil.
GiftInspire100
11 17 2022

CompuYankeeDev129
Soy un ingeniero en computación con cuatro años de experiencia. He trabajado como programador en Python y Java en empresas desarrolladoras de software como RealSolutions.
Busco entretenimiento. Me gustaría asistir a un concierto de 2 horas con el estadio lleno de gente y no me preocupa la falta de complejidad armónica en los acordes ya que me gusta el reguetón.
DaddyYankeeFan8807
11 17 2022

OlcoManGold
Me desempeño como programador y desarrollador de software. Tengo mucha experiencia tras trabajar once años en la industria. Soy experto en lenguajes como Python y me especializo en el desarrollo del firmware para dispositivos.
Soy aficionado a la gastronomía mexicana, por lo que necesito poder almorzar tacos de pollo, ya sea asado, frito o al ajillo, todos los días y desde un punto de venta accesible.
SuperGold$$
11 17 2022

BobTheBuilder250
Vendemos materiales para la construcción de edificios, casas y proyectos de la más alta calidad. Adicionalmente, tenemos un gran equipo de ingenieros civiles con mucha experiencia en el desarrollo de infraestructura pública.
Ocupo una institución educativa para mis hijos, quienes poseen gran talento para las áreas STEM, y quienes desean ser futuros ingenieros.
DizzyScoop999
11 17 2022

*/