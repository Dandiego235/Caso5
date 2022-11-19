#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>
#include <ctime>
#include "Grafo/Grafo.h"
#include "Grafo/Animal.h"
#include "Grafo/INodo.h"
#include "Registered.h"
#include "Match.h"
#include "BPlus/StringData.h"
#include "BPlus/BPlusTree.h"
#include "OrdenCadenasMin.h"
#include "OrdenGrado.h"
#include "OrdenWords.h"

#define NUM_ANIMALES 14

#define COMPRADORES_POR_ARBOL 6
#define ORDEN 5
#define SIZE 6
#define WORDS_TOP 3

using namespace std;

Grafo* crearGrafo(vector<Registered*> records){
    Grafo *grafo = new Grafo(true);
    for (Registered* record : records){
        grafo->addNode(record);
    }
    return grafo;
}

void crearMatches(Grafo* grafo){
    vector<Registered*> *compradores = Registered::getCompradores();
    vector<Registered*> *vendedores = Registered::getVendedores();

    int index = 0;
    vector<Match> ranking;
    while (index < compradores->size()){
        int cantidad = compradores->size() - index;
        if (cantidad > COMPRADORES_POR_ARBOL){
            cantidad = COMPRADORES_POR_ARBOL;
        }

        BPlusTree *arbol = new BPlusTree(ORDEN, SIZE);
        for (int contador = 0; contador < cantidad; contador++, index++){
            for (StringData* palabra : *compradores->at(index)->getWordsDemand()){
                arbol->insert(palabra);
            }
        }
        arbol->print();
        cout << endl;

        for (Registered* vendedor : *vendedores){
            unordered_map<string, Match*> *matches = new unordered_map<string, Match*>();
            for (StringData* palabra : *vendedor->getWordsOffer()){
                int leafIndex = 0;
                LeafNode* leaf = arbol->find(palabra, leafIndex);
                if (leaf){
                    while (!palabra->compareTo(leaf->getSecuencia()->at(leafIndex))){
                        StringData* strData = (StringData*)(void*)(leaf->getSecuencia()->at(leafIndex));
                        Registered* comprador = strData->getUsuario();
                        if (matches->find(comprador->getNickname()) == matches->end()){
                            matches->insert(pair<string, Match*> (comprador->getNickname(), new Match(comprador, vendedor)));
                        }
                        Match *match;
                        try{
                            match = matches->at(comprador->getNickname());
                        } 
                        catch (...) {
                            cout << comprador->getNickname() << " not found" << endl;
                            for (auto it = matches->begin(); it != matches->end(); it++){
                                cout << it->first << ' ' << it->second << endl;
                            }
                            return;
                        }

                        match->incrementPeso();
                        leafIndex++;
                        match->addWord(palabra);

                        if (leafIndex == leaf->getSecuencia()->size()){ // si llegó al final 
                            if (leaf->getSecuencia()->size() == arbol->getSize()){
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
                ranking.push_back(*(iterador->second));
            }
        }
    }
    std::sort(ranking.begin(), ranking.end());
    auto riterator = ranking.rbegin();
    Match *match = &(*riterator);
    while(match->getRating() > 1){
        if (match->getComprador() != match->getVendedor()){
            cout << match->getVendedor()->getNickname() << " apunta a " << match->getComprador()->getNickname() << " con rating " << match->getRating() << " con " << endl;
            for (StringData* str : *match->getWords()){
                cout << str->toString() << " ";
            }
            cout << endl;
            grafo->addArc(match->getVendedor(), match->getComprador(), match->getRating());
            match->getVendedor()->addMatchSalida(*match);
            match->getComprador()->addMatchEntrada(*match);
        }
        riterator++;
        match = &(*riterator);
    }
}

vector<string>* top10(Grafo* grafo){
    auto nodos = grafo->getNodos();
    set<NodoGrafo*, OrdenGrado> ordenGrado;
    for (NodoGrafo* nodo : nodos){
        ordenGrado.insert(nodo);        
    }

    vector<string> *topRanking = new vector<string>();

    int contadorRanking = 0;
    auto riterator = ordenGrado.rbegin();
    while (contadorRanking < 10 && riterator != ordenGrado.rend()){
        Registered* registro = (Registered*)(void*)((*riterator)->getInfo());
        unordered_map<StringData*, int> palabras;
        for (Match match : *registro->getMatchesSalida()){
            //cout << match.getWords()->size() << endl;
            for (StringData* word : *match.getWords()){
                //cout << word->getPalabra() << endl;
                try{
                    //cout << palabras.at(word) << endl;
                    palabras.at(word) = ++palabras.at(word);
                } catch (...){
                    palabras.insert(pair<StringData*, int>(word, 1));
                }
            }
        }
        set<pair<StringData*, int>, OrdenWords> rankingPalabras(palabras.begin(), palabras.end());
        int contadorPalabras = 0;
        auto ritSet = rankingPalabras.rbegin();
        string entry = "";
        while (contadorPalabras < WORDS_TOP && ritSet != rankingPalabras.rend()){
            string fragment = (*ritSet).first->getPalabra();
            //cout << fragment << " " << (*ritSet).first->getUsuario()->getNickname() << endl;
            entry += registro->getFullWordsOffer()->at(fragment);
            entry += " ";
            contadorPalabras++;
            ritSet++;
        }
        if (entry != ""){
            topRanking->push_back(entry);
        }
        riterator++;
        contadorRanking++;
    }
    return topRanking;
}

vector<NodoGrafo*>* menorCadena(Grafo* grafo){
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
        cout << (*rit)->getStarting()->getInfo()->getId() << " a " << (*rit)->getDestino()->getInfo()->getId() << " " << (*rit)->getDistancia() + concurrencia << " " << (*rit)->getCantidadNodos() << endl;
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
    cout << menor->getDestino()->getInfo()->getId() << ' ' << menor->getStarting()->getInfo()->getId() << endl;
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
    //allrecords.push_back(new Registered("","","",""));

    Grafo *grafo = crearGrafo(allrecords);

    crearMatches(grafo);
    
    while (true) {
        int opcion;
        cout << "Gobiz" << endl;
        cout << "1. Registrarse" << endl;
        cout << "2. Revisar matches" << endl;
        cout << "3. Analizar matches" << endl;
        cout << "4. Salir" << endl;
        cout << "Opción: ";
        cin >> opcion;

        if (opcion == 1){
            while (true) {
                string nickname;
                cout << "Ingrese un nickname:" << endl;
                cin >> nickname;
                if (nickname.size() < 10 || nickname.size() > 32){
                    cout << "ERROR: El nickname debe tener una longitud entre 10 y 32 caracteres." << endl;
                    continue;
                }

                // validar existencia del nickname
                if (Registered::validateNickname(nickname)){
                    cout << "ERROR: Ese nickname ya está registrado." << endl;
                    continue;
                }

                string contraseña;
                cout << "Ingrese su contraseña" << endl;
                cin >> contraseña;

                if (contraseña.size() > 20 || contraseña == ""){
                    cout << "ERROR: La contraseña no puede tener más de 20 caracteres" << endl;
                    continue;
                }

                string confirm;
                cout << "Confirme su contraseña" << endl;
                cin >> confirm;
                if (contraseña != confirm) {
                    cout << "ERROR: Las dos contraseñas no son iguales" << endl;
                    continue;
                }

                string offer;
                cout << "Ingrese su oferta o déjelo vacío si no ofrece nada" << endl;
                cin >> offer;

                if (offer.size() > 250){
                    cout << "ERROR: La oferta no puede tener más de 250 caracteres." << endl;
                    continue;
                }
                string demand; 
                cout << "Ingrese su demanda o déjelo vacío si no demanda nada" << endl;
                cin >> demand;

                if (demand.size() > 250){
                    cout << "ERROR: La demanda no puede tener más de 250 caracteres." << endl;
                    continue;
                }

                // current date/time based on current system
                time_t now = time(0);

                tm *ltm = localtime(&now);
                string date = ltm->tm_mon + "/" + ltm->tm_mday;
                date += "/";
                date += ltm->tm_year;
                cout << date << endl;
                // Subir al server
                //regs.registerUser(nickname, offer, demand, password, ltm->tm_mday, ltm->mon, ltm->tm_year);
                allrecords.push_back(new Registered(nickname, offer,demand,date));
            }
        } else if (opcion == 2){
            // print all matches

        } else if (opcion == 3){
            // obtain function to analize
            
            while (true) {
                int opcion2;
                cout << "Analizar" << endl;
                cout << "1. Visualizar matches actuales" << endl;
                cout << "2. Comercio circular" << endl;
                cout << "3. Tamaño de cadena de valor" << endl;
                cout << "4. Gráfico con el Top 10 de productos más codiciados" << endl;
                cout << "5. Áreas de mercado conexas" << endl;
                cout << "6. Salir" << endl;
                cout << "Opción: ";
                cin >> opcion2;
                if (opcion == 1){
                    for (NodoGrafo * nodo : grafo->getNodos()) {
                        Registered* registro = (Registered*)(void*)(nodo->getInfo());
                        cout << nodo->getInfo()->getId() << ". " << registro->getNickname() << endl;
                    }
                    string opcion3;
                    cout << "Opción: ";
                    cin >> opcion3;
                    int idMatch = std::stoi(opcion3);
                    NodoGrafo * nodeMatch = grafo->getNodo(idMatch);
                } else if (opcion == 2) {
                    for (NodoGrafo * nodo : grafo->getNodos()) {
                        Registered* registro = (Registered*)(void*)(nodo->getInfo());
                        cout << nodo->getInfo()->getId() << ". " << registro->getNickname() << endl;
                    }
                    string opcion3;
                    cout << "Opción: ";
                    cin >> opcion3;
                    int idCycle = std::stoi(opcion3);
                    NodoGrafo * nodeCycle = grafo->getNodo(idCycle);
                    grafo->Dijkstra(nodeCycle);
                    grafo->findCiclo(nodeCycle);
                    grafo->saveCycles(dijkstra);
                } else if (opcion == 3) {
                    for (NodoGrafo * nodo : grafo->getNodos()) {
                        Registered* registro = (Registered*)(void*)(nodo->getInfo());
                        cout << nodo->getInfo()->getId() << ". " << registro->getNickname() << endl;
                    }
                    string opcion3;
                    cout << "Opción: ";
                    cin >> opcion3;
                    int idCycle = std::stoi(opcion3);
                    NodoGrafo * nodeCycle = grafo->getNodo(idCycle);
                } else if (opcion == 4) {

                } else if (opcion == 5) {

                } else {
                    break;
                }
            }
        } else if (opcion ==4) {
            break;
        } else {
            cout << "ERROR: OPCIÓN NO ES VÁLIDA" << endl;
        }
    }

    // Cadena de valor menor
    // Grafo* grados = grafo->crearGrafoGrados();

    // for (auto it = topRanking->begin(); it != topRanking->end(); it++){
    //     cout << *it << endl;
    // }
    
    // cout << "\nCadena" << endl;
    // NodoGrafo * dijkstra = grafo->getNodo(Registered::findId("Wakanda_Med"));
    // grafo->Dijkstra(dijkstra);

    // grafo->findCiclo(dijkstra);

    // vector<NodoGrafo*> *cadenaMin = menorCadena(grados);

    // for (auto rit = cadenaMin->rbegin(); rit != cadenaMin->rend(); rit++){
    //     Registered* nickname = (Registered*)(void*)((*rit)->getInfo());

    //     cout << nickname->getNickname() << endl;
    // }
    
    //grafo->saveCycles(dijkstra);

    // auto ciclos = dijkstra->getCiclos();

    // for (vector<NodoGrafo*> ciclo: *ciclos){
    //     cout << "Ciclo" << endl;
    //     for (auto rit = ciclo.rbegin(); rit != ciclo.rend(); rit++){
    //         Registered* record = (Registered*)(void*)((*rit)->getInfo());
    //         cout << "     " << record->getNickname() << endl;
    //     }
    // }

    //grados->saveToFile();

    // allrecords.push_back(new Registered("","","",""));
    // allrecords.push_back(new Registered("","","",""));
    // allrecords.push_back(new Registered("","","",""));

    // Grafo zoologico(true); // grafo dirigido

    // string animales [NUM_ANIMALES] = {"Perro", "Gato", "Vaca", "Oso", "León", "Loro", "Mono", "Búho", "Pato", "Rata", "Naga", "Zorro", "Jaguar", "Toro"};

    // for (int index = 1; index <= NUM_ANIMALES; index++){
    //     zoologico.addNode(new Animal(index, animales[index - 1]));
    // }

    // zoologico.addArc(1, 2, 8);
    // zoologico.addArc(1, 3, 3);
    // zoologico.addArc(1, 6, 13);
    // zoologico.addArc(2, 3, 2);
    // zoologico.addArc(2, 4, 1);
    // zoologico.addArc(3, 2, 3);
    // zoologico.addArc(3, 4, 9);
    // zoologico.addArc(3, 5, 2);
    // zoologico.addArc(4, 5, 4);
    // zoologico.addArc(4, 8, 2);
    // zoologico.addArc(4, 7, 6);
    // zoologico.addArc(5, 1, 5);
    // zoologico.addArc(5, 6, 5);
    // zoologico.addArc(5, 4, 6);
    // zoologico.addArc(5, 9, 4);
    // zoologico.addArc(6, 9, 7);
    // zoologico.addArc(6, 7, 1);
    // zoologico.addArc(7, 5, 3);
    // zoologico.addArc(7, 8, 4);
    // zoologico.addArc(8, 9, 3);
    // zoologico.addArc(9, 7, 5);
    // zoologico.addArc(10, 6, 0);
    // zoologico.addArc(11, 12, 0);
    // zoologico.addArc(11, 13, 0);
    // zoologico.addArc(12, 13, 0);
    // zoologico.addArc(13, 12, 0);
    // zoologico.addArc(13, 14, 0);
    // zoologico.addArc(14, 11, 0);

    // NodoGrafo * dijkstra = zoologico.getNodo(4);
    // zoologico.Dijkstra(dijkstra);

    // zoologico.findCiclo(dijkstra);

    // vector<INodo*> profundidad = zoologico.deepPath(zoologico.getNodo(1)->getInfo());
    
    // cout << "Recorrido en profundidad" << endl;
    // for (INodo* nodo : profundidad){
    //     Animal* animal = (Animal*)(void*)nodo;
    //     cout << "     " << animal->getNombre() << endl;
    // }

    // vector<INodo*> anchura = zoologico.broadPath(zoologico.getNodo(1)->getInfo());

    // cout << "Recorrido en anchura" << endl;
    // for (INodo* nodo : anchura){
    //     Animal* animal = (Animal*)(void*)(nodo);
    //     cout << "     " << animal->getNombre() << endl;
    // }

    // cout << "Componentes conexas" << endl;
    // vector<vector<Arco*>> componentes = *zoologico.getComponentesConexas();

    // for(vector<Arco*> componente : componentes){
    //     cout << "  Componente" << endl;
    //     for (Arco* arco : componente){
    //         NodoGrafo* nodo = (NodoGrafo*)arco->getDestino();
    //         Animal* animCon = (Animal*)(void*)(nodo->getInfo());
    //         cout << "    " << animCon->getNombre() << endl;
    //     }
    // }
    
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