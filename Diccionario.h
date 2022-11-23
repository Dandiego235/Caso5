#ifndef DICCTIONARIO

#define DICCIONARIO

#include <set>
#include "StrCompare.h"
using namespace std;

// Esta clase sirve como un diccionario de palabras comunes que no se toman en cuenta.
class Diccionario{
    private:
        set<StringData*, StrCompare> *conjunto;

        static Diccionario* instance;

        Diccionario(){
            conjunto = new set<StringData*, StrCompare>();
            conjunto->insert(new StringData("alta", nullptr));
            conjunto->insert(new StringData("para", nullptr));
            conjunto->insert(new StringData("calid", nullptr));
            conjunto->insert(new StringData("much", nullptr));
            conjunto->insert(new StringData("nuest", nullptr));
            conjunto->insert(new StringData("pued", nullptr));
            conjunto->insert(new StringData("todo", nullptr));
        }

    public:
        static Diccionario* getInstance(){
            if (!instance){
                instance = new Diccionario();
            }
            return instance;
        }

        bool find(StringData* str){
            return conjunto->count(str);
        }

};


Diccionario* Diccionario::instance = nullptr;
#endif