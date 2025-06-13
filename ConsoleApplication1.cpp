#include <iostream>
#include "INI_Parser.h"
#include <vector>


int main()
{
    
   INIPARSER::INI_Parser parser("filename.txt");
    

   
   for (auto vectorIt = parser.section.begin(); vectorIt != parser.section.end(); vectorIt++) {
       std::cout << "Section: " << (*vectorIt).getNameSection() << std::endl;
       for (auto mapIt = (*vectorIt).values_.begin(); mapIt != (*vectorIt).values_.end(); mapIt++) {
           std::cout << mapIt->first << " = " <<  mapIt->second << "/" << std::endl;
          //  std::cout << std::format("{}  = {}\n", mapIt->firs, mapIt->second);
       }
       std::cout << "_______________________________" << std::endl;
  }

   try {
       std::cout << "Section1.var1 = " << parser.get_value<std::string>("Section1.var1") << std::endl;

       std::cout << "Section2.var1 = " << parser.get_value<int>("Section4.Ь") << std::endl;
   }
   catch (const std::exception& err) {
       std::cout << "ERROR: ";
       std::cout << err.what();
   }
  
   

    return 0;
}


// вопрос: (7 комментарий) const std::string& getNameSection() const; как мне вывести значение на экран ведь по сути я возвращаю ссылку на первый элемент в массиве char

