#include <iostream>
#include "INI_Parser.h"
#include <vector>

int main()
{
   INIPARSER::INI_Parser parser("filename.txt");
    

   
   for (auto vectorIt = parser.section.begin(); vectorIt != parser.section.end(); vectorIt++) {
       std::cout << "Section: " << (*vectorIt).getNameSection() << std::endl;
       for (auto mapIt = (*vectorIt).values_.begin(); mapIt != (*vectorIt).values_.end(); mapIt++) {
           std::cout << mapIt->first << " = " <<  mapIt->second << std::endl;
       }
       std::cout << "_______________________________" << std::endl;
  }

   std::cout << "Section1.var1 = " << parser.get_value<std::string>("Section1.var1") << std::endl;

   std::cout << "Section1.var1 = " << parser.get_value<int>("Section1.var2") << std::endl;
   
    return 1;
}