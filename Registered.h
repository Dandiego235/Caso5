#ifndef REGISTERED

#define REGISTERED

#include <string>
#include <vector>
#include "BPlus/IData.h"
#include "Grafo/INodo.h"

using namespace std;

class Registered : public INodo, public IData{
    private:
        string nickname;
        string offer;
        string demand;
        string postdate;

        vector<string> wordsOffer;
        vector<string> wordsDemand;

    public:
        Registered(string pNickname, string pOffer, string pDemand, string pPostdate) {
            this->nickname = pNickname;
            this->offer = pOffer;
            this->demand = pDemand;
            this->postdate = pPostdate;
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

        string getWordsOffer(){
            return wordsOffer;
        }

        string getWordsDemand(){
            return wordsDemand;
        }
};

#endif