#include <iostream>
#include "INI_Parser.h"
#include <vector>

int main()
{
   INIPARSER::INI_Parser parser("filename.txt");
    
    //std::ifstream  file{};
    //file.open("filename.txt");

    //char ch;
    //if (file.is_open()) {

    //    while (file.get(ch)) { // считываем по одному символу
    //        std::cout << ch << std::endl;   // выводим символ
    //    }
    //}
    
    /*
    std::vector<int> a;
    a.push_back(5);
    a.push_back(5);
    a.push_back(5);
    a[0] = 2;

    std::cout << a[0];
    */
   for (auto vectorIt = parser.section.begin(); vectorIt != parser.section.end(); vectorIt++) {
       std::cout << "Section: " << (*vectorIt).getNameSection() << std::endl;
       for (auto mapIt = (*vectorIt).values_.begin(); mapIt != (*vectorIt).values_.end(); mapIt++) {
           std::cout << mapIt->first << " = " <<  mapIt->second << std::endl;
       }
       std::cout << "_______________________________" << std::endl;
  }

    return 1;
}