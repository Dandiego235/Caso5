#include "BPlusTree.h"
#include "IntData.h"
#include <iostream>
#include <vector>

using namespace std;

int main(){
    BPlusTree tree(5, 4);

    tree.insert(new IntData(31));
    tree.insert(new IntData(60));
    tree.insert(new IntData(11));
    tree.insert(new IntData(38));
    tree.insert(new IntData(45));
    tree.insert(new IntData(19));
    tree.insert(new IntData(61));
    tree.insert(new IntData(51));
    tree.insert(new IntData(42));
    tree.insert(new IntData(31));
    tree.insert(new IntData(35));
    tree.insert(new IntData(1));
    tree.insert(new IntData(8));
    tree.insert(new IntData(2));
    tree.insert(new IntData(10));
    tree.insert(new IntData(15));
    tree.insert(new IntData(13));
    tree.insert(new IntData(26));
    tree.insert(new IntData(8));
    tree.insert(new IntData(26));
    tree.insert(new IntData(35));
    tree.insert(new IntData(8));

    IntData* data = new IntData(75);
    tree.insert(data);
    tree.print();

    int index = 0;
    LeafNode *leaf = tree.find(data, index);
    cout << leaf->getSecuencia()->at(index)->toString() << endl;

}
