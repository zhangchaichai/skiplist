#include <iostream>
#include "skiplist.h"
#define STORE_FILE "D:\\projects\\skiplist\\File\\skiplistFile.txt"
using namespace std;
int main() {
    int n = 6;
    SkipList<int, std::string> skipList(6);
    skipList.insert_element(1, "1");
    skipList.insert_element(3, "3");
    skipList.insert_element(7, "7");
    skipList.insert_element(8, "8");
    skipList.insert_element(9, "9");
    skipList.insert_element(19, "19");
    skipList.insert_element(21, "21");
    skipList.insert_element(23, "23");
    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.dump_file();

    // skipList.load_file();

    skipList.search_element(9);
    skipList.search_element(18);


    skipList.display_list();

    skipList.delete_element(3);
    skipList.delete_element(7);

    std::cout << "skipList size:" << skipList.size() << std::endl;

    skipList.display_list();
}
