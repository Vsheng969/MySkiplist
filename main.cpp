#include <iostream>
#include "MySkiplish.h"
#define FILE_PATH "./store/dumpFile"

int main() {

    // 键值中的key用int型，如果用其他类型，需要自定义比较函数
    // 而且如果修改key的类型，同时需要修改skipList.load_file函数
    SkipList<int, std::string> skipList(18);
    for(int i=0;i<100;i++){
        skipList.insert_element(i, std::to_string(i));
    }

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