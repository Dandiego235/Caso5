#ifndef REGISTERED

#define REGISTERED

#include <string>
#include <vector>
#include <set>

#include "BPlus/IData.h"
#include "Grafo/INodo.h"

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
        static vector<Registered*> comparedores;
        static vector<Registered*> vendedores;

    public:
        Registered(string pNickname, string pOffer, string pDemand, string pPostdate) {
            this->nickname = pNickname;
            this->offer = pOffer;
            this->demand = pDemand;
            this->postdate = pPostdate;
            id = contador++;
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

        static vector<Registered*> getCompradores(){
            return compradores;
        }

        static vector<Registered*> getVendedores(){
            return vendedores;
        }
};

Registered::contador = 1;
Registered::compradores = new vector<Registered>();
Registered::vendedores = new vector<Registered>();

#endif