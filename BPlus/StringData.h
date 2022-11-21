#ifndef STRINGDATA

#define STRINGDATA 1

#include <sstream>
#include <string>
#include "IData.h"

#define PORCENTAJE_SIMILITUD 0.8
class Registered;

// Clase para contener la información de los fragmentos de palabras en el árbol
class StringData : public IData{
    private:
        string palabra;
        Registered * usuario; // usuario al que pertenece esa palabra.

    public:
        StringData(string pPalabra, Registered * pUsuario){
            palabra = pPalabra;
            usuario = pUsuario;
        }

        string getPalabra() const{
            return palabra;
        }

        void setPalabra(string pPalabra){
            palabra = pPalabra;
        }

        // método usado para comparar dos StringData
        int compareTo(const StringData &compare) const{
            int contador = 0; // contador de caracteres similares para el porcentaje de coincidencia
            string comparePalabra = compare.getPalabra();
            if (!palabra.compare(comparePalabra)){ // si son iguales, retorna 0
                return 0;
            } else if (palabra.compare(comparePalabra) > 0){
                // si es mayor, compara la menor con palabra, contando las coincidencias de letras
                for (int character = 0; character < comparePalabra.size(); character++){
                    if (palabra[character] == comparePalabra[character]){
                        contador++;
                    } else {
                        break;
                    }
                }
                // Si el final, las dos palabras son similares en al menos el porcentaje establecido, las toma como iguales
                // Esto se usa para tomar en cuenta las pequeñas variaciones de palabras, como programa, programado, program, programo, etc.
                if (contador >= (comparePalabra.size() * PORCENTAJE_SIMILITUD)){
                    return 0;
                }
                return 1;
            }
            // Si es menor, se compara la mayor con palabra para encontrar las coincidencias de letras.
            for (int character = 0; character < palabra.size(); character++){
                if (palabra[character] == comparePalabra[character]){
                    contador++;
                }
            }
            // Si el final, las dos palabras son similares en al menos el porcentaje establecido, las toma como iguales
            // Esto se usa para tomar en cuenta las pequeñas variaciones de palabras, como programa, programado, program, programo, etc.    
            if (contador >= palabra.size() * PORCENTAJE_SIMILITUD){
                return 0;
            }
            return -1;
        }

        // método usado para comparar dos IData
        int compareTo(IData *pToCompare){
            StringData * compare = dynamic_cast<StringData*>(pToCompare);
            return this->compareTo(*compare);
        }

        string getKey(){
            return palabra;
        }

        string toString(){ // método para obtener la información del IData para imprimirla.
            return getKey();
        }

        Registered * getUsuario(){
            return usuario;
        }

        bool operator == (const StringData& compareStr) const { 
            return palabra == compareStr.getPalabra(); 
        }

        bool operator < (const StringData& compareStr) const {
            // if (palabra.compare(compareStr.getPalabra()) < 0){
            //     return true;
            // }
            // return false;
            return (this->compareTo(compareStr) < 0);
        }

        bool operator > (const StringData& compareStr) const { 
            return palabra.compare(compareStr.getPalabra()); 
        }
};

#endif