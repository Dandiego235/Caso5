#include <vector>
#include <iostream>
#include <string>
#include <unordered_map>
#include "Grafo/Grafo.h"
#include "Grafo/Animal.h"
#include "Grafo/INodo.h"
#include "Registered.h"
#include "Match.h"
#include "BPlus/StringData.h"
#include "BPlus/BPlusTree.h"

#define NUM_ANIMALES 14

#define COMPRADORES_POR_ARBOL 6
#define ORDEN 5
#define SIZE 6
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
    while (index < compradores->size()){
        int cantidad = compradores->size() - index;
        if (cantidad > COMPRADORES_POR_ARBOL){
            cantidad = COMPRADORES_POR_ARBOL;
        }

        BPlusTree *arbol = new BPlusTree(ORDEN, SIZE);
        for (int contador = 0; contador < cantidad; contador++, index++){
            for (StringData* palabra : *compradores->at(index)->getWordsOffer()){
                arbol->insert(palabra);
            }
        }
        arbol->print();

        vector<Match> ranking;
        for (Registered* vendedor : *vendedores){
            unordered_map<string, Match*> *matches = new unordered_map<string, Match*>();
            for (StringData* palabra : *vendedor->getWordsDemand()){
                int index = 0;
                LeafNode* leaf = arbol->find(palabra, index);
                while (!palabra->compareTo(leaf->getSecuencia()->at(index))){
                    StringData* strData = (StringData*)(void*)(leaf->getSecuencia()->at(index));
                    Registered* comprador = strData->getRegistered();
                    if (matches->find(comprador->getNickname()) == matches->end()){
                        matches->insert(pair<string, Match*> (comprador->getNickname(), new Match(comprador, vendedor)));
                    }
                    Match *match = matches->at(comprador->getNickname());
                    match->incrementPeso();
                    index++;
                    if (index == leaf->getSecuencia()->size()){ // si llegó al final 
                        if (leaf->getSecuencia()->size() == arbol->getSize()){
                            leaf = leaf->getBrother();
                            index = 0;
                        }
                    }
                    
                    for(auto iterador = matches->begin(); iterador != matches->end(); iterador++){
                        ranking.push_back(*(iterador->second));
                    }
                }
            }

        }
        std::sort(ranking);
        auto riterator = ranking.rbegin();
        for (int contador = 0; contador < ranking.size() / 2; contador++){
            Match match = *riterator;
            grafo->addArc(match.getVendedor(), match.getComprador(), match.getRating());
            riterator++;
        }

    }
}

int main(){

    vector<Registered*> allrecords;
    allrecords.push_back(new Registered("Wakanda_Med","Vendemos máquinas con fines médicos de alta tecnología y calidad, hechas de metales exóticos, para tratamientos de enfermedades o heridas fatales.",
    "Necesitamos muchos programadores con mucha experiencia para desarrollar el software de nuestros dispositivos médicos","11/16/2022"));
    allrecords.push_back(new Registered("vendePollos38","Se vende pollo asado, frito o al ajillo","carne de pollo de la más alta calidad y aceite","11/03/2022"));
    allrecords.push_back(new Registered("elvendeTacos29","venta de tacos al estilo mexicano tradicional","Verduras frescas de la más alta calidad y carne de res o cerdo","11/02/2022"));
    allrecords.push_back(new Registered("badbunny_in_concert","conciertos a estadio lleno de gente escuchando pum pum con el mismo acorde por 2 horas","transporte y seguridad en todos los paises que visita y mucha fiesta tambien","11/02/2022"));
    allrecords.push_back(new Registered("rock_odin_gold","soy programador python con experiencia","","01/23/2019"));
    allrecords.push_back(new Registered("metal_year_2000","","requiero de 5 ingenieros en computación con 8 años de experiencia, conocimientos en python.","10/28/2022"));
    allrecords.push_back(new Registered("josearnoldowood","esta es la oferta","esta es la demanda","10/10/2022"));




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