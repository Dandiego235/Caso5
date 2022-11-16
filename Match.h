#ifndef MATCH

#define MATCH 1

#include "Registered.h"

class Match{
    private:
        int peso;
        int rating;
        Registered* comprador;
        Registered* vendedor;
        vector<StringData*> *words;

    public:
        Match(Registered* pComprador, Registered* pVendedor){
            comprador = pComprador;
            vendedor = pVendedor;
            peso = 0;
            rating = 0;
            words = new vector<StringData*>();
        }

        Registered* getComprador(){
            return comprador;
        }

        Registered* getVendedor(){
            return vendedor;
        }

        void addWord(StringData *data){
            words->push_back(data);
        }

        vector<StringData*>* getWords(){
            return words;
        }
        
        void incrementPeso(){
            rating += ++peso;
        }

        int getRating() const{
            return rating;
        }

        bool operator < (const Match& match) const { 
            return rating < match.getRating(); 
        }


};

#endif