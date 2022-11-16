#ifndef MATCH

#define MATCH 1

#include "Registered.h"

class Match{
    private:
        int peso;
        int rating;
        Registered* comprador;
        Registered* vendedor;
    public:
        Match(Registered* pComprador, Registered* pVendedor){
            comprador = pComprador;
            vendedor = pVendedor;
            peso = 0;
            rating = 0;
        }

        Registered* getComprador(){
            return comprador;
        }

        Registered* getVendedor(){
            return vendedor;
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