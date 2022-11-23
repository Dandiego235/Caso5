#ifndef REGISTERED

#define REGISTERED 1

#include <vector>
#include <set>
#include <string>
#include <iostream>
#include <bits/stdc++.h>
#include <unordered_map>
#include "BPlus/IData.h"
#include "Grafo/INodo.h"
#include "BPlus/StringData.h"
#include "StrCompare.h"
#include "Match.h"
#include "Diccionario.h"

using namespace std;

// Clase para representar los registros de la plataforma. Hereda de INodo para poder ser metido en el grafo.
class Registered : public INodo{
    private:
        string nickname;
        string offer;
        string demand;
        string postdate;

        set<StringData*, StrCompare> *wordsOffer; // conjunto de las palabras de oferta
        set<StringData*, StrCompare> *wordsDemand; // conjunto de las palabras de demanda

        vector<Match> *matchesEntrada;
        vector<Match> *matchesSalida;

        bool comprador;
        bool vendedor;

        static int contadorId;
        static vector<Registered*> *compradores;
        static vector<Registered*> *vendedores;

        static unordered_map<string, int> *hashIDs;

        static unordered_map<string, Registered*> *nicknames; // hashmap que contiene todos los nicknames registrados.

        unordered_map<string, string> *fullWordsOffer; // hashmaps que contienen parejas del fragmento y su palabra completa
        unordered_map<string, string> *fullWordsDemand;

    public:
        Registered(string pNickname, string pOffer, string pDemand, string pPostdate) {
            this->nickname = pNickname;
            this->offer = pOffer;
            this->demand = pDemand;
            this->postdate = pPostdate;
            id = contadorId++;
            wordsOffer = new set<StringData*, StrCompare>();
            wordsDemand = new set<StringData*, StrCompare>();
            fullWordsOffer = new unordered_map<string, string>();
            fullWordsDemand = new unordered_map<string, string>();
            fillSet(wordsOffer, offer, fullWordsOffer); // se llenan los conjuntos de palabras
            fillSet(wordsDemand, demand, fullWordsDemand);
            matchesEntrada = new vector<Match>();
            matchesSalida = new vector<Match>();

            if (!pOffer.size() == 0){ // se establecen las banderas de si son compradores o vendedores
                vendedor = true;
                vendedores->push_back(this);
            } else {
                vendedor = false;
            }

            if (!pDemand.size() == 0){
                comprador = true;
                compradores->push_back(this);
            } else {
                comprador = false;
            }

            nicknames->insert(pair<string, Registered*>(nickname, this));
            
            hashIDs->insert(pair<string,int>(nickname, id));
        }

        string getNickname() {
            return this->nickname;
        }

        string getOffer() {
            return this->offer;
        }

        string getDemand() {
            return this->demand;
        }

        string getPostdate() {
            return this->postdate;
        }

        set<StringData*, StrCompare>* getWordsOffer(){
            return wordsOffer;
        }

        set<StringData*, StrCompare>* getWordsDemand(){
            return wordsDemand;
        }

        static vector<Registered*>* getCompradores(){
            return compradores;
        }

        static vector<Registered*>* getVendedores(){
            return vendedores;
        }

        unordered_map<string, string>* getFullWordsOffer(){
            return fullWordsOffer;
        }

        unordered_map<string, string>* getFullWordsDemand(){
            return fullWordsDemand;
        }

        int getId(){
            return id;
        }

        // Función que retorna el id a partir del nickname.
        static int findId(string pNickname){
            int id;
            try{
                id = hashIDs->at(pNickname);
            } catch (...){
                id = -1; // su no lo encuentra retorna -1
            }
            return id;
        }
        
        // Función que reemplaza todas las apariciones de un string en un string por otro string
        void replace_all(string& s, string const& toReplace, string const& replaceWith) {
            string buf;
            size_t pos = 0;
            size_t prevPos;

            // Reserves rough estimate of final size of string.
            buf.reserve(s.size());

            while (true) {
                prevPos = pos;
                pos = s.find(toReplace, pos);
                if (pos == std::string::npos)
                    break;
                buf.append(s, prevPos, pos - prevPos);
                buf += replaceWith;
                pos += toReplace.size();
            }

            buf.append(s, prevPos, s.size() - prevPos);
            s.swap(buf);
        }

        // método que llena un conjunto con los fragmentos de palabras de la parte de oferta
        void fillSet(set<StringData*, StrCompare> * pSet, string lista, unordered_map<string, string>* fullWords){
            transform(lista.begin(), lista.end(), lista.begin(), ::tolower);
            replace_all(lista, "á", "a");
            replace_all(lista, "é", "e");
            replace_all(lista, "í", "i");
            replace_all(lista, "ó", "o");
            replace_all(lista, "ú", "u");
            string word = "";
            for (char c : lista){
                if (c == ' ' || c == '.' || c == ','){ // si llega a una división de palabra
                    if (word != "" && word.size() >= 4){ // si la palabra tiene una longitud mayor que 3. 
                        int percent = (60 * word.size() + 100 - 1) / 100; 
                        if (percent < 4){ // si recorta a menos de 4 letras, le pone 4 letras como mínimo.
                            percent = 4;
                        }
                        string full = word;
                        word.resize(percent); // saca el 60 porciento de la palabra
                        StringData * palabra = new StringData(word, this);
                        if (!Diccionario::getInstance()->find(palabra)){
                            pSet->insert(palabra);
                            fullWords->insert(pair<string, string>(word, full));
                        }
                    }
                    word = "";
                } else {
                    word += c;
                }
            }
            if (word != ""){ // si al final quedó con una palabra, la analiza.
                int percent = (60 * word.size() + 100 - 1) / 100;
                if (percent < 4){
                    percent = 4;
                }
                string full = word;
                word.resize(percent);
                StringData * palabra = new StringData(word, this);
                if (!Diccionario::getInstance()->find(palabra)){
                    pSet->insert(palabra);
                    fullWords->insert(pair<string, string>(word, full));
                }
                pSet->insert(palabra);
                fullWords->insert(pair<string, string>(word, full));
            }
        }

        void addMatchEntrada(Match &pMatch){
            matchesEntrada->push_back(pMatch);
        }

        vector<Match>* getMatchesEntrada(){
            return matchesEntrada;
        }

        void addMatchSalida(Match &pMatch){
            matchesSalida->push_back(pMatch);
        }

        vector<Match>* getMatchesSalida(){
            return matchesSalida;
        }

        // Método para validar si un nickname ya está registrado.
        static bool validateNickname(string pNickname){
            return nicknames->count(pNickname);
        }

        static void resetContadorId(){
            contadorId = 1;
        }
};

// inicialización de variables estáticas
int Registered::contadorId = 1;
vector<Registered*>* Registered::compradores = new vector<Registered*>();
vector<Registered*>* Registered::vendedores = new vector<Registered*>();
unordered_map<string, int>* Registered::hashIDs = new unordered_map<string, int>();
unordered_map<string, Registered*> *Registered::nicknames = new unordered_map<string, Registered*>();

#endif