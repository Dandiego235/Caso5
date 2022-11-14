#ifndef _INODO_
#define _INODO_ 1

class INodo {
    public:
        int getId() {
            return id;
        }

        void setId(int pId) {
            this->id = pId;
        }

        // virtual int compareTo(INodo *pToCompare) = 0; // método usado para comparar dos IData

        // virtual string toString() = 0; // método para obtener la información del IData para imprimirla.
        
    protected:
        int id;
};

#endif