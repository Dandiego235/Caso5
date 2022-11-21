#include "BPlusTree.h"
#include "IntData.h"
#include <iostream>
#include <vector>

using namespace std;

int main(){
    BPlusTree tree(4, 4);

    tree.insert(new IntData(31));
    tree.print();
    tree.insert(new IntData(60));
    tree.print();
    tree.insert(new IntData(11));
    tree.print();
    tree.insert(new IntData(38));
    tree.print();
    tree.insert(new IntData(45));
    tree.print();
    tree.insert(new IntData(19));
    tree.print();
    tree.insert(new IntData(61));
    tree.print();
    tree.insert(new IntData(51));
    tree.print();
    tree.insert(new IntData(42));
    tree.print();
    tree.insert(new IntData(31));
    tree.print();
    tree.insert(new IntData(35));
    tree.print();
    tree.insert(new IntData(1));
    tree.print();
    tree.insert(new IntData(8));
    tree.print();
    tree.insert(new IntData(2));
    tree.print();
    tree.insert(new IntData(10));
    tree.print();
    tree.insert(new IntData(15));
    tree.print();
    tree.insert(new IntData(13));
    tree.print();
    tree.insert(new IntData(26));
    tree.print();
    tree.insert(new IntData(8));
    tree.print();
    tree.insert(new IntData(26));
    tree.print();
    tree.insert(new IntData(35));
    tree.print();
    tree.insert(new IntData(8));
    tree.print();

    IntData* data = new IntData(19);
    tree.insert(data);
    tree.print();

    int index = 0;
    LeafNode *leaf = tree.find(data, index);
    cout << leaf->getSecuencia()->at(index)->toString() << endl;

}
