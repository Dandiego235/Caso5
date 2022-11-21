#ifndef STRCOMPARE

#define STRCOMPARE 1

#include "BPlus/StringData.h"

// Functor, clase que se comporta como función, que compara dos punteros a StringData de acuerdo a su contenido
struct StrCompare{
    bool operator() (const StringData* str1, const StringData* str2) const{
        return *str1 < *str2;
    }
};

#endif