#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <ctime>
#include <fstream>
#include <math.h>
#include <ios> //used to get stream size
#include <limits> //used to get numeric limits
//#include "socket/contenful.cpp"
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
        int contador = 0;
        while (contador < cantidad && index < compradores->size()){ // vamos a agregar la cantidad de compradores que diga la constante
            if (!grafo->getNodo(compradores->at(index)->getId())){ // si el comprador no está en el grafo, lo saltamos.
                index++;
                continue;
            }
            for (StringData* palabra : *compradores->at(index)->getWordsDemand()){
                arbol->insert(palabra); // se inserta cada palabra en el árbol
            }
            contador++;
            index++;
        }
        arbol->print();
        std::cout << endl;

        for (Registered* vendedor : *vendedores){
            // vamos a recorrer los vendedores e ir comparando sus palabras con las palabras del árbol para hacer matches.
            if (!grafo->getNodo(vendedor->getId())){ // si el vendedor no está en el grafo, lo saltamos.
                continue;
            }
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
                                if (!leaf->getBrother()){ // si ya no tiene hermano, se sale.
                                    break;
                                }
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
        if (match->getComprador() != match->getVendedor() && match->getRating() > 1){
            // si el match no es consigo mismo y el rating es mayor que 1, lo mete al grafo
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
    ofstream TopFile(".\\top10.csv", ios::out);
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
vector<NodoGrafo*>* menorCadena(Grafo* grafo, int &pConcurrencia, NodoGrafo * pStarting = nullptr){
    set<DijkstraNode*, OrdenCadenasMin> cadenas;
    if (pStarting){ // si hay un starting, calcula la cadena más larga con menor concurrencia a partir de ese nodo.
        if (!pStarting->getArcs()->size()){ // si el nodo no tiene arcos, la cadena solo contiene al nodo.
            vector<NodoGrafo*>* singleChain = new vector<NodoGrafo*>();
            singleChain->push_back(pStarting);
            return singleChain;
        }
        grafo->Dijkstra(pStarting);
        auto caminos = pStarting->getCaminos();
        for (auto iterador = caminos->begin(); iterador != caminos->end(); iterador++){
            cadenas.insert(iterador->second);
        }

    } else {
        // si no hay un starting, calcula el dijkstra para todos para sacar la cadena mas larga con menor concurrencia
        for (NodoGrafo* nodo : grafo->getNodos()){
            grafo->Dijkstra(nodo); // calcula el dijstra para ese nodo.
            auto caminos = nodo->getCaminos();
            for (auto iterador = caminos->begin(); iterador != caminos->end(); iterador++){
                cadenas.insert(iterador->second);
            }
        }
    }
    

    DijkstraNode* menor = *cadenas.rbegin(); // el menor es el que tenga la mayor concurrencia y el menor valor o rating
    for (auto rit = ++cadenas.rbegin(); rit != cadenas.rend(); rit++){
        // empieza desde el final y se devuelve hasta encontrar el menor con mayor cantidad de nodos.
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
vector<NodoGrafo*>* mayorCadena(Grafo *grafo, int &pConcurrencia, NodoGrafo * pStarting = nullptr){
    set<DijkstraNode*, OrdenCadenasMin> cadenas;
    if (pStarting){ // si hay un starting point, calcula el dijkstra alterado para calcular la cadena mayor de ese nodo
        if (!pStarting->getArcs()->size()){
            vector<NodoGrafo*>* singleChain = new vector<NodoGrafo*>();
            singleChain->push_back(pStarting);
            return singleChain;
        }
        grafo->dijkstraMayor(pStarting);
        auto caminos = pStarting->getCaminos();
        for (auto iterador = caminos->begin(); iterador != caminos->end(); iterador++){
            cadenas.insert(iterador->second);
        }
    } else {
        // si no hay starting point, encuentra las mayores longitudes a partir de todos los nodos.
        for (NodoGrafo* nodo : grafo->getNodos()){
            grafo->dijkstraMayor(nodo);
            auto caminos = nodo->getCaminos();
            for (auto iterador = caminos->begin(); iterador != caminos->end(); iterador++){
                cadenas.insert(iterador->second);
            }
        }
    }
    
    DijkstraNode* mayor = *cadenas.rbegin(); // el mayor esta en la última posición.
    pConcurrencia = mayor->getDistancia() + mayor->getDestino()->getArcs()->size() + mayor->getDestino()->getNodosEntrada()->size();

    vector<NodoGrafo*> * result = new vector<NodoGrafo*>();
    NodoGrafo * starting = mayor->getStarting();
    result->push_back(mayor->getDestino());
    while (mayor->getCamino()->getOrigen() != starting){ // reconstruye el camino de la cadena
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

// Esta función valida que una fecha sea válida.
// Tiene que venir en formato mm/dd/yyyy
bool validateDate(const string& date){
    int day;
    int month;
    int year;
    try{
        day = std::stoi(date.substr(3,2));
        month = std::stoi(date.substr(0,2));
        year = std::stoi(date.substr(6,4));
    }
    catch (...){
        cout << "Fecha invalida" << endl;
        return false;
    }
    
    if (!(1 <= month && month <= 12))
        return false;
    if (!(1 <= day && day <= 31))
        return false;
    if ((day == 31) && !(month % 2))
        return false;
    if ((day == 30) && (month == 2))
        return false;
    if ((month == 2) && (day == 29) && (year % 4 != 0))
        return false;
    if ((month == 2) && (day == 29) && (year % 400 == 0))
        return true;
    if ((month == 2) && (day == 29) && (year  % 100 == 0))
        return false;
    if ((month == 2) && (day == 29) && (year % 4 == 0))
        return true;
    return true;
}

// Esta función determina si una fecha está en un rango específico.
// Tiene que venir en formato mm/dd/yyyy
bool dateInRange(const string& date, const string& start, const string& end){
    int day = std::stoi(date.substr(3,2));
    int month = std::stoi(date.substr(0,2));
    int year = std::stoi(date.substr(6,4));
    int dateCheck = year * 10000 + month * 100 + day;

    int dayStart = std::stoi(start.substr(3,2));
    int monthStart = std::stoi(start.substr(0,2));
    int yearStart = std::stoi(start.substr(6,4));
    int dateStart = yearStart * 10000 + monthStart * 100 + dayStart;
    
    int dayEnd = std::stoi(end.substr(3,2));
    int monthEnd = std::stoi(end.substr(0,2));
    int yearEnd = std::stoi(end.substr(6,4));
    int dateEnd = yearEnd * 10000 + monthEnd * 100 + dayEnd;

    return dateStart <= dateCheck  && dateCheck <= dateEnd;
}

// void getRecords(Contenful &regs, vector<Registered*> &allrecords){
//     Registered::getCompradores()->clear();
//     Registered::getVendedores()->clear();
//     Registered::resetContadorId();
//     allrecords = regs.getRecords();
// }

int main(){

    // Contenful regs;

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
    allrecords.push_back(new Registered("poly_bridgers","Somos una empresa constructora de puentes hechos con materiales de calidad. Hemos construido puentes en ciudades como Lóndres, San Francisco, Tokyo y San José.","Necesitamos materiales de construcción de alta calidad para construir nuestros puentes.","10/22/122"));
    allrecords.push_back(new Registered("Green_Lion","Vendemos botellas hechas de un material a base de bambú, por lo que son amigables con el ambiente. Las botellas son biodegradables y se podrían usar para tomar agua, café, jugo y más.","Necesitamos un servicio de envíos rápidos para que nuestros clientes reciban sus botellas el mismo día que las compran.","10/22/122"));
    allrecords.push_back(new Registered("SuperSmashCoders122","Desarrollamos video juegos extremadamente divertidos que ofrecen cientas de horas de entretenimiento. Los jugadores pueden jugar en línea o localmente.","Buscamos un compositor de música que pueda trabajar con diferentes géneros musicales para que componga la música de nuestros juegos.","10/22/122"));
    allrecords.push_back(new Registered("PapaNoel2512","Brindamos un servicio de recepción y envíos de paquetes a una escala mundial. El tiempo estimado de envío es de una noche hasta un día completo gracias a nuestros vehículos de gran velocidad.","Necesitamos muchos arbolitos de pino para decorar nuestro almacén de productos para la época navideña. Se prefieren buenos precios.","10/22/122"));
    allrecords.push_back(new Registered("SantaClaus37","Brindamos","un","10/22/122"));
    //allrecords.push_back(new Registered("","","",""));

    while (true) {
        int opcion; // menú principal
        std::cout << "Gobiz" << endl;
        std::cout << "1. Registrarse" << endl;
        std::cout << "2. Revisar matches" << endl;
        std::cout << "3. Analizar matches" << endl;
        std::cout << "4. Salir" << endl;
        std::cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1){
            // Se pide la información del registro.
            cin.clear(); // se reestablece el cin
            cin.ignore(numeric_limits<streamsize>::max(), '\n');
            string nickname;
            while (true){
                std::cout << "Ingrese un nickname:     (x para salir)" << endl; // 
                std::getline(cin, nickname); 
                // se validan las restricciones
                if (nickname.size() < 10 || nickname.size() > 32){
                    if (nickname == "x"){
                        break;
                    }
                    std::cout << "ERROR: Ese nickname no es válido" << endl;
                    continue;
                }
                //validar existencia del nickname
                if (Registered::validateNickname(nickname)){
                    cout << "ERROR: Ese nickname ya está registrado." << endl;
                    continue;
                }
                break;
            }

            string password;
            while (true){
                std::cout << "Ingrese su contraseña" << endl;
                getline(cin, password);

                if (password.size() > 20 || password == ""){
                    std::cout << "ERROR: La contraseña no puede tener más de 20 caracteres" << endl;
                    continue;
                }
                string confirm; // confirmación de que las contraseñas sean iguales
                std::cout << "Confirme su contraseña" << endl;
                getline(cin, confirm);
                if (password != confirm) {
                    std::cout << "ERROR: Las dos contraseñas no son iguales" << endl;
                    continue;
                }
                break;
            }

            string offer;
            while (true){
                std::cout << "Ingrese su oferta o déjelo vacío si no ofrece nada" << endl;
                std::getline(cin, offer);

                if (offer.size() > 250){
                    std::cout << "ERROR: La oferta no puede tener más de 250 caracteres." << endl;
                    continue;
                }
                break;
            }

            string demand;
            while (true){
                std::cout << "Ingrese su demanda o déjelo vacío si no demanda nada" << endl;
                std::getline(cin, demand);

                if (demand.size() > 250){
                    std::cout << "ERROR: La demanda no puede tener más de 250 caracteres." << endl;
                    continue;
                }
                break;
            }

            time_t now = time(0); // se obtiene la fecha actual

            tm *ltm = localtime(&now); // Se obtiene la fecha actual
            string date = to_string(ltm->tm_mon + 1) + "/" + to_string(ltm->tm_mday);
            date += "/";
            date += to_string(ltm->tm_year + 1900);
            std::cout << "\nNickName: " << nickname << "\nContraseña: " << password << "\nOferta: " << offer << "\nDemanda: " << demand << endl;
            std::cout << "Fecha de registro: " << date << endl;
            cout << "Confirmar: Sí (s) o No (n)" << endl;
            char confirmar;
            cin >> confirmar;
            if (confirmar != 's'){
                continue;
            }
            // Subir al server
            allrecords.push_back(new Registered(nickname, offer, demand, date));

        } else if (opcion == 2){
            //imprime todo el grafo
            Grafo *grafo = crearGrafo(allrecords);
            crearMatches(grafo);
            printGraph(grafo);
            grafo->saveToFile();
            cout << "Link a la página: https://observablehq.com/d/c3f7b2fb62380649" << endl;

        } else if (opcion == 3){
            // obtener función a analizar
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
                if (opcion2 == 1){ // visualizar matches actuales
                    Grafo *grafo = crearGrafo(allrecords); // se crea el grafo
                    crearMatches(grafo); // Se crean los matches
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
                        cout << "Link a la página: https://observablehq.com/d/c3f7b2fb62380649" << endl;
                    } else {
                        NodoGrafo * nodeMatch = grafo->getNodo(idMatch);
                        std::cout << nodeMatch->getInfo()->getId() << endl;
                        std::cout << "1. Desplegar todos los matches.\n2. Desplegar los matches en un rango de fechas." << endl;
                        int desplegarFechas;
                        cin >> desplegarFechas;
                        if (desplegarFechas == 2){
                            // los matches en un rango de fechas
                            bool fecha1Val = false;
                            string fechaInicio;
                            while (!fecha1Val){
                                std::cout << "Ingrese una fecha válida (mm/dd/yyyy) para el inicio del rango:" << endl;
                                cin >> fechaInicio;
                                fecha1Val = validateDate(fechaInicio);
                            }
                            bool fecha2Val = false;
                            string fechaFinal;
                            while (!fecha2Val){
                                std::cout << "Ingrese una fecha válida (mm/dd/yyyy) para el final del rango:" << endl;
                                cin >> fechaFinal;
                                fecha2Val = validateDate(fechaFinal);
                            }
                            std::cout << "Matches" << endl;
                            contador = 1;
                            std::cout << "  Oferta:" << endl;
                            Grafo newGrafo = new Grafo(true);
                            newGrafo.addNode(nodeMatch->getInfo());
                            // si imprimen los matches de oferta
                            for (auto it = nodeMatch->getArcs()->begin(); it != nodeMatch->getArcs()->end(); it++){
                                Registered* registro = (Registered*)(void*)((*it)->getDestino()->getInfo());
                                if (!dateInRange(registro->getPostdate(), fechaInicio, fechaFinal)){
                                    continue;
                                }
                                std::cout << contador <<". " << registro->getNickname() << " con una calificación de " << (*it)->getPeso() << endl;      
                                contador++;
                                newGrafo.addNode((*it)->getDestino()->getInfo());
                                newGrafo.addArc(nodeMatch->getInfo()->getId(), (*it)->getDestino()->getInfo()->getId(), (*it)->getPeso());
                            }
                            std::cout << "  Demanda:" << endl;
                            contador = 1;
                            // si imprimen los matches de demanda
                            for (Arco * arco : *nodeMatch->getEntradas()){
                                Registered* registro = (Registered*)(void*)(arco->getOrigen()->getInfo());
                                if (!dateInRange(registro->getPostdate(), fechaInicio, fechaFinal)){
                                    continue;
                                }
                                std::cout << contador <<". " << registro->getNickname() << " con una calificación de " << arco->getPeso() << endl;      
                                contador++;
                                if (!newGrafo.getNodo(registro->getId())){
                                    newGrafo.addNode(arco->getOrigen()->getInfo());
                                }
                                newGrafo.addArc(arco->getOrigen()->getInfo()->getId(), nodeMatch->getInfo()->getId(), arco->getPeso());
                            }
                            newGrafo.saveToFile();
                            cout << "Link a la página: https://observablehq.com/d/71789c17e87feb18" << endl;
                        } else {
                            // todos los matches con el nodo.
                            std::cout << "Matches" << endl;
                            contador = 1;
                            std::cout << "  Oferta:" << endl;
                            Grafo newGrafo = new Grafo(true);
                            newGrafo.addNode(nodeMatch->getInfo());
                            // si imprimen los matches de oferta
                            for (auto it = nodeMatch->getArcs()->begin(); it != nodeMatch->getArcs()->end(); it++){
                                Registered* registro = (Registered*)(void*)((*it)->getDestino()->getInfo());
                                std::cout << contador <<". " << registro->getNickname() << " con una calificación de " << (*it)->getPeso() << endl;      
                                contador++;
                                newGrafo.addNode((*it)->getDestino()->getInfo());
                                newGrafo.addArc(nodeMatch->getInfo()->getId(), (*it)->getDestino()->getInfo()->getId(), (*it)->getPeso());
                            }
                            std::cout << "  Demanda:" << endl;
                            contador = 1;
                            // si imprimen los matches de demanda
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
                            cout << "Link a la página: https://observablehq.com/d/71789c17e87feb18" << endl;
                        }
                    }
                    
                    
                } else if (opcion2 == 2) { // ciclos por nodo
                    Grafo *grafo = crearGrafo(allrecords); // se crea el grafo
                    crearMatches(grafo); // Se crean los matches
                    for (NodoGrafo * nodo : grafo->getNodos()) { // Se imprimen los registros
                        Registered* registro = (Registered*)(void*)(nodo->getInfo());
                        std::cout << nodo->getInfo()->getId() << ". " << registro->getNickname() << endl;
                    }
                    int idCycle; // se pide la elección del usuario
                    std::cout << "Opción: ";
                    cin >> idCycle;
                    NodoGrafo * nodeCycle = grafo->getNodo(idCycle); // obtiene el nodo con base en el Id ingresado 
                    grafo->Dijkstra(nodeCycle); // se encuentran todos los menores caminos
                    grafo->findCiclo(nodeCycle); // se encuentran todos los ciclos
                    grafo->saveCycles(nodeCycle); // se salvan los ciclos al archivo
                    cout << "Link a la página: https://observablehq.com/d/3fb899b5c5c5616e" << endl;
                
                } else if (opcion2 == 3) { // cadenas de concurrencia
                    Grafo *grafo = crearGrafo(allrecords); // se crea el grafo
                    crearMatches(grafo); // Se crean los matches
                    Grafo* grados = grafo->crearGrafoGrados(); // se crea el grafo con todas las concurrencias
                    int concurrencia = 0; // se obtiene el total de concurrencia
                    while (true){
                        Grafo* grados = grafo->crearGrafoGrados(); // se crea el grafo con todas las concurrencias
                        int concurrencia = 0; // se obtiene el total de concurrencia
                        int contador = 1;
                        cout << "Cadena de valor: " << endl;
                        cout << "1. Cadena menor" << endl;
                        cout << "2. Cadena mayor" << endl;
                        cout << "3. Salir" << endl;
                        int opcion3; // se pide la elección del usuario
                        std::cout << "Opción: ";
                        cin >> opcion3;
                        if (opcion3 != 1 && opcion3 != 2){ // si no es 1 o 2 sale
                            break;
                        }

                        for (NodoGrafo * nodo : grafo->getNodos()) {
                            Registered* registro = (Registered*)(void*)(nodo->getInfo());
                            std::cout << contador << ". " << registro->getNickname() << endl;
                            contador++;
                        }
                        std::cout << contador << ". Todos" << endl;
                        int idChain; // se pide la elección del usuario
                        std::cout << "Opción: ";
                        cin >> idChain;
                        if (opcion3 == 1){
                            vector<NodoGrafo*> *cadenaMin;
                            if (idChain == contador){
                                cadenaMin = menorCadena(grados, concurrencia); // se obtiene la menor cadena
                            } else {
                                NodoGrafo * nodeChain = grados->getNodo(idChain); // obtiene el nodo con base en el Id ingresado 
                                cadenaMin = menorCadena(grados, concurrencia, nodeChain); // se obtiene la menor cadena
                            }

                            Grafo newGrafo = new Grafo(true);
                            cout << "Cadena de valor más larga con menor concurrencia: " << endl;
                            cout << "Concurrencia: " << concurrencia << endl;
                            
                            // se imprimen los nodos
                            NodoGrafo * anterior = nullptr; // almacena el nodo anterior de la cadena
                            for (NodoGrafo* nodo: *cadenaMin){ // recorre toda la cadena
                                newGrafo.addNode(nodo->getInfo()); // se añade el nodo al grafo nuevo
                                if (anterior){ // Si no es el primer nodo,
                                // se añade el arco desde el anterior al actual
                                    newGrafo.addArc(nodo->getInfo()->getId(), anterior->getInfo()->getId(), (nodo->getNodosEntrada()->size() + nodo->getArcs()->size()));
                                }
                                anterior = nodo; // se cambia el anterior al actual
                                Registered* nickname = (Registered*)(void*)(nodo->getInfo()); // se imprime el nickname
                                cout << "   " << nickname->getNickname() << ", Concurrencia: " << (nodo->getArcs()->size() + nodo->getNodosEntrada()->size()) << endl;
                            }
                            newGrafo.saveToFile();
                            cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;
                        } else if (opcion3 == 2){
                            vector<NodoGrafo*> *cadenaMax;
                            if (idChain == contador){
                                cadenaMax = mayorCadena(grados, concurrencia); // se obtiene la menor cadena
                            } else {
                                NodoGrafo * nodeChain = grados->getNodo(idChain); // obtiene el nodo con base en el Id ingresado 
                                cadenaMax = mayorCadena(grados, concurrencia, nodeChain); // se obtiene la menor cadena
                            }
                            cout << "Cadena de valor más larga con mayor concurrencia:" << endl;
                            Grafo newGrafo = new Grafo(true);
                            cout << "Concurrencia: " << concurrencia << endl;
                            // se imprimen los nodos
                            NodoGrafo * anterior = nullptr;
                            for (NodoGrafo* nodo: *cadenaMax){
                                newGrafo.addNode(nodo->getInfo()); // se añade el nodo al grafo nuevo
                                if (anterior){ // Si no es el primer nodo,
                                // se añade el arco desde el anterior al actual
                                    newGrafo.addArc(nodo->getInfo()->getId(), anterior->getInfo()->getId(), (nodo->getNodosEntrada()->size() + nodo->getArcs()->size()));
                                }
                                anterior = nodo; // se cambia el anterior al actual
                                Registered* nickname = (Registered*)(void*)(nodo->getInfo()); // se imprime el nickname
                                cout << "   " << nickname->getNickname() << ", Concurrencia: " << (nodo->getArcs()->size() + nodo->getNodosEntrada()->size()) << endl;
                            }
                            newGrafo.saveToFile();
                            cout << "Link a la página: https://observablehq.com/d/c37c21e96a92e360" << endl;
                        }
                    }
                    
                    
                    
                } else if (opcion2 == 4) { // Top 10
                    cout << "Top 10" << endl;
                    std::cout << "1. Desplegar el Top 10 de productos hasta la fecha.\n2. Desplegar el Top 10 de productos en un rango de fechas." << endl;
                    int rangoTop;
                    cin >> rangoTop;
                    Grafo *grafo;
                    if (rangoTop == 2){
                        // si es el top 10 de un rango de fechas.
                        bool fecha1ValTop = false;
                        string fechaInicioTop;
                        while (!fecha1ValTop){
                            std::cout << "Ingrese una fecha válida (mm/dd/yyyy) para el inicio del rango:" << endl;
                            cin >> fechaInicioTop;
                            fecha1ValTop = validateDate(fechaInicioTop);
                        }
                        bool fecha2ValTop = false;
                        string fechaFinalTop;
                        while (!fecha2ValTop){
                            std::cout << "Ingrese una fecha válida (mm/dd/yyyy) para el final del rango:" << endl;
                            cin >> fechaFinalTop;
                            fecha2ValTop = validateDate(fechaFinalTop);
                        }
                        vector<Registered*> rangoTop;
                        for (Registered* registro : allrecords){ // se agregan al conjunto de nodos los que están dentro de esas fecha.
                            if (!dateInRange(registro->getPostdate(), fechaInicioTop, fechaFinalTop)){
                                continue;
                            }
                            rangoTop.push_back(registro);
                        }
                        grafo = crearGrafo(rangoTop);
                    } else {
                        grafo = crearGrafo(allrecords); // se crea el grafo
                    }
                    crearMatches(grafo); // Se crean los matches
                    vector<string>* topRanking = top10(grafo); // se crea el vector con los productos más cotizados
                    for (auto it = topRanking->begin(); it != topRanking->end(); it++){
                        cout << "  " << *it << endl;
                    } // se imprimen los productos

                    saveTop10(grafo);
                    cout << "Link a la página: https://observablehq.com/d/c2adc8c189e19fe9" << endl;
                } else if (opcion2 == 5) { // Areas de mercado conexas
                    Grafo *grafo = crearGrafo(allrecords); // se crea el grafo
                    crearMatches(grafo); // Se crean los matches
                    Grafo* grados = grafo->crearGrafoGrados(); // se crea el grafo de nodos
                    vector<INodo*> anchura = grados->deepPath(grados->getNodo(1)->getInfo()); // se realiza un recorrido en anchura

                    cout << "Recorrido en anchura" << endl;
                    for (INodo* nodo : anchura){ // se recorre el vector
                        Registered * registro = (Registered*)(void*)(nodo); // se imprime el recorrido
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