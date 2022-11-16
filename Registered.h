#ifndef REGISTERED

#define REGISTERED

#include <vector>
#include <set>
#include <string>
#include "BPlus/IData.h"
#include "Grafo/INodo.h"
#include "BPlus/StringData.h"
#include <iostream>
#include<bits/stdc++.h>

using namespace std;

class Registered : public INodo{
    private:
        string nickname;
        string offer;
        string demand;
        string postdate;

        set<StringData*> *wordsOffer;
        set<StringData*> *wordsDemand;

        static int contadorId;
        static vector<Registered*> *compradores;
        static vector<Registered*> *vendedores;

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
            FillSet(wordsOffer, offer);
            FillSet(wordsDemand, demand);
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

        set<StringData*>* getWordsOffer(){
            return wordsOffer;
        }

        set<StringData*>* getWordsDemand(){
            return wordsDemand;
        }

        static vector<Registered*>* getCompradores(){
            return compradores;
        }

        static vector<Registered*>* getVendedores(){
            return vendedores;
        }

        

        void FillSet(set<StringData*> * pSet, string lista){
            transform(lista.begin(), lista.end(), lista.begin(), ::tolower);
            replace_all(lista, "á", "a");
            replace_all(lista, "é", "e");
            replace_all(lista, "í", "i");
            replace_all(lista, "ó", "o");
            replace_all(lista, "ú", "u");
            string word = "";
            for (char c : lista){
                if (c == ' ' || c == '.' || c == ','){
                    if (word != ""){
                        StringData * palabra = new StringData(word, this);
                        pSet->insert(palabra);
                        word = "";
                    }
                } else {
                    word += c;
                }
            }
            if (word != ""){
                StringData * palabra = new StringData(word, this);
                pSet->insert(palabra);
            }
            
            for (std::set<StringData*>::iterator current = (*pSet).begin() ; current != (*pSet).end(); ++current){
                word = (*current)->getPalabra();
                if (word.size() < 4){
                    current = pSet->erase(current);
                } else if (word.size() >= 4) {
                    int percent = (60 * word.size() + 100 - 1) / 100;
                    if (percent < 4){
                        percent = 4;
                    }
                    word.resize(percent);
                    (*current)->setPalabra(word);
                }
            }
        }
};

int Registered::contadorId = 1;
vector<Registered*>* Registered::compradores = new vector<Registered*>();
vector<Registered*>* Registered::vendedores = new vector<Registered*>();

#endif