#ifndef STRCOMPARE

#define STRCOMPARE 1

#include "BPlus/StringData.h"
struct StrCompare{
    bool operator() (const StringData* str1, const StringData* str2) const{
        return *str1 < *str2;
    }
};

#endif