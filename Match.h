#ifndef MATCH

#define MATCH 1

//#include "Registered.h"

class Registered;

// clase que representa un Match entre vendedor y comprador
class Match{
    private:
        int peso;
        int rating;
        Registered* comprador;
        Registered* vendedor;
        vector<StringData*> *words; // palabras que son compartidas por el match.

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
        
        void incrementPeso(){ // el peso se incrementa y se suma al rating del match.
            rating += ++peso;
        }

        int getRating() const{
            return rating;
        }

        // Función para comparar dos matches de acuerdo a su rating.
        bool operator < (const Match& match) const { 
            return rating < match.getRating(); 
        }


};

#endif