#ifndef ORDENWORDS

#define ORDENWORDS 1

#include "BPlus/StringData.h"
struct OrdenWords{
    bool operator() (const pair<StringData*,int>& pair1, const pair<StringData*,int>& pair2) const{
        if (pair1.second != pair2.second){
            return pair1.second < pair2.second;
        }
        return pair1.first < pair2.first;
    }
};

#endif