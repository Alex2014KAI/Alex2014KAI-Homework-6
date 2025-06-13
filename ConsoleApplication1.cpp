#include <iostream>
#include "INI_Parser.h"
#include <vector>

int main()
{
   try {
       INIPARSER::INI_Parser parser("filename.txt");



       for (auto vectorIt = parser.section.begin(); vectorIt != parser.section.end(); vectorIt++) {
           std::cout << "Section: " << (*vectorIt).getNameSection() << std::endl;
           for (auto mapIt = (*vectorIt).values_.begin(); mapIt != (*vectorIt).values_.end(); mapIt++) {
               std::cout << mapIt->first << " = " << mapIt->second << "/" << std::endl;
               //  std::cout << std::format("{}  = {}\n", mapIt->firs, mapIt->second);
           }
           std::cout << "_______________________________" << std::endl;
       }

       std::cout << "Section1.var1 = " << parser.get_value<std::string>("Section1.var1") << std::endl;

       std::cout << "Section2.var1 = " << parser.get_value<int>("Section4.Mode") << std::endl;
   }
   catch (const std::exception& err) {
       std::cout << "ERROR: ";
       std::cout << err.what();
   }
   catch (const INIPARSER::ExceptionCurrentLinePosition& err) {
       std::cout << "ERROR: ";
       std::cout << err.what();
   }
   catch (const INIPARSER::ExceptionVariableValueError& err) {
       std::cout << "ERROR: ";
       std::cout << err.what();
   }
   catch (const std::length_error& err) {
       std::cout << "ERROR: ";
       std::cout << err.what();
   }
  

    return 0;
}


// вопрос: (7 комментарий) const std::string& getNameSection() const; как мне вывести значение на экран ведь по сути я возвращаю ссылку на первый элемент в массиве char

