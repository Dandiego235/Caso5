#ifndef REGISTERED

#define REGISTERED

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

class Registered : public INodo{
    private:
        string nickname;
        string offer;
        string demand;
        string postdate;

        set<StringData*, StrCompare> *wordsOffer;
        set<StringData*, StrCompare> *wordsDemand;

        vector<Match> *matchesEntrada;
        vector<Match> *matchesSalida;

        bool comprador;
        bool vendedor;

        static int contadorId;
        static vector<Registered*> *compradores;
        static vector<Registered*> *vendedores;

        static unordered_map<string, int> *hashIDs;

        unordered_map<string, string> *fullWordsOffer;
        unordered_map<string, string> *fullWordsDemand;

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
            fillSet(wordsOffer, offer, fullWordsOffer);
            fillSet(wordsDemand, demand, fullWordsDemand);
            matchesEntrada = new vector<Match>();
            matchesSalida = new vector<Match>();

            if (!pOffer.size() == 0){
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
            
            cout << nickname << endl;
            for (auto it = wordsOffer->begin(); it != wordsOffer->end(); it++){
                cout << (*it)->toString() << endl;
            }
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

        static int findId(string pNickname){
            return hashIDs->at(pNickname);
        }
        

        void fillSet(set<StringData*, StrCompare> * pSet, string lista, unordered_map<string, string>* fullWords){
            transform(lista.begin(), lista.end(), lista.begin(), ::tolower);
            replace_all(lista, "á", "a");
            replace_all(lista, "é", "e");
            replace_all(lista, "í", "i");
            replace_all(lista, "ó", "o");
            replace_all(lista, "ú", "u");
            string word = "";
            for (char c : lista){
                if (c == ' ' || c == '.' || c == ','){
                    if (word != "" && word.size() >= 4){
                        int percent = (60 * word.size() + 100 - 1) / 100;
                        //cout << word << " Tamaño " << word.size() << " Percent " << percent << endl;
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
                    }
                    word = "";
                } else {
                    word += c;
                }
            }
            if (word != ""){
                int percent = (60 * word.size() + 100 - 1) / 100;
                //cout << word << " Tamaño " << word.size() << " Percent " << percent << endl;
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
};

int Registered::contadorId = 1;
vector<Registered*>* Registered::compradores = new vector<Registered*>();
vector<Registered*>* Registered::vendedores = new vector<Registered*>();
unordered_map<string, int>* Registered::hashIDs = new unordered_map<string, int>();

#endif