#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include <algorithm>
#include <set>
#include "Grafo/Grafo.h"
#include "Grafo/Animal.h"
#include "Grafo/INodo.h"
#include "Registered.h"
#include "Match.h"
#include "BPlus/StringData.h"
#include "BPlus/BPlusTree.h"
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
                int index = 0;
                LeafNode* leaf = arbol->find(palabra, index);
                if (leaf){
                    while (!palabra->compareTo(leaf->getSecuencia()->at(index))){
                        StringData* strData = (StringData*)(void*)(leaf->getSecuencia()->at(index));
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
                        index++;
                        match->addWord(palabra);

                        if (index == leaf->getSecuencia()->size()){ // si llegó al final 
                            if (leaf->getSecuencia()->size() == arbol->getSize()){
                                leaf = leaf->getBrother();
                                index = 0;
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
    Match match = *riterator;
    while(match.getRating() > 1){
        if (match.getComprador() != match.getVendedor()){
            cout << match.getVendedor()->getNickname() << " apunta a " << match.getComprador()->getNickname() << " con rating " << match.getRating() << " con " << endl;
            for (StringData* str : *match.getWords()){
                cout << str->toString() << " ";
            }
            cout << endl;
            grafo->addArc(match.getVendedor(), match.getComprador(), match.getRating());
            match.getVendedor()->addMatchSalida(&match);
            match.getComprador()->addMatchEntrada(&match);
        }
        riterator++;
        match = *riterator;
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
        for (Match* match : *registro->getMatchesSalida()){
            for (StringData* word : *match->getWords()){
                try{
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
            entry += registro->getFullWordsOffer()->at(fragment);
            entry += " ";
        }
        riterator++;
        contadorRanking++;
    }
    return topRanking;
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
    //allrecords.push_back(new Registered("","","",""));

    Grafo *grafo = crearGrafo(allrecords);

    crearMatches(grafo);
    

    // NodoGrafo * dijkstra = grafo->getNodo(Registered::findId("Wakanda_Med"));
    // grafo->Dijkstra(dijkstra);

    // grafo->findCiclo(dijkstra);

    // auto ciclos = dijkstra->getCiclos();

    // for (vector<NodoGrafo*> ciclo: *ciclos){
    //     cout << "Ciclo" << endl;
    //     for (auto rit = ciclo.rbegin(); rit != ciclo.rend(); rit++){
    //         Registered* record = (Registered*)(void*)((*rit)->getInfo());
    //         cout << "     " << record->getNickname() << endl;
    //     }
    // }

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



*/