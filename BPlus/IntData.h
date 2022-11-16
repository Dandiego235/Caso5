#ifndef INTDATA

#define INTDATA 1

#include <sstream>
#include <string>

class IntData : public IData{
    private:
        int number;

    public:
        IntData(int pNumber){
            number = pNumber;
        }

        int getNumber(){
            return number;
        }

        // método usado para comparar dos IData
        int compareTo(IData *pToCompare){
            IntData* compare = dynamic_cast<IntData*>(pToCompare);
            if (number < compare->getNumber()){
                return -1;
            } else if (number > compare->getNumber()){
                return 1;
            }
            return 0;
        }

        string getKey(){
            stringstream ss;
            ss << number;
            string result;
            ss >> result;
            return result;
        }

        string toString(){ // método para obtener la información del IData para imprimirla.
            return getKey();
        }
};

#endif