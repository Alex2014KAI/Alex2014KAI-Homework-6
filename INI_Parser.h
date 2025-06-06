#pragma once
#include <iostream>
#include <fstream>
#include <vector>
#include <map>

namespace INIPARSER {

	class Section {
	public:
		std::string nameSection_;
		std::map<std::string, std::string> values_{};
	public:
		Section() {};

		int setNameSection(std::string nameSection) {
			nameSection_ = nameSection;

			return 1; // if return 0 - ERROR
		};

		std::string getNameSection() {
			return nameSection_;
		};

		int setValue(std::string nameVariable, std::string valueVariable) {
			values_[nameVariable] = valueVariable;
			return 1; // if return 0 - ERROR
		};

		std::map<std::string, std::string>* getValues() {
			return &values_;
		}

		void clear() {
			values_.clear();
			nameSection_ = "";
		};
	};

	class INI_Parser
	{
	private:
		std::string filename_;
		std::ifstream  file{};
	public:
		std::vector<Section> section{}; // [nameSection1, mapValues1; nameSection2, mapValues2; nameSection3, mapValues3;....nameSection_n, mapValues_n]
	private:
		bool is_readingSection;  // cocked when section reading occurs
		bool is_readingVariable; // cocked when data is read
		bool is_readingNameVariable; // is set when a variable name is read
		bool is_readingValueVariable; // is set when a variable value is read
		bool is_readingComment;  // cocked when the comment is being read

		std::string nameSection_;  // current section
		std::string nameVariable;  // current variable name
		std::string valueVariable; // current value of the variable
		//std::map<std::string, std::string> temporaryValues{}; // stores data for the current section

		Section debugData; // Stores temporary data for a section

		bool isBlank(const std::string& s) { // if there is a symbol then it is false, if there are only spaces then it is true
			for (char ch : s) {
				if (!std::isspace(static_cast<unsigned char>(ch))) {
					return false;
				}
			}
			return true; 
		}

		void readDataSection() {
			bool is_duplicate = false; // is raised if there is already a section with the same name
			for (int i{ 0 }; i < section.size(); i++) {
				if (section[i].getNameSection() == nameSection_) {

					for (auto It = debugData.values_.begin(); It != debugData.values_.end(); It++) {
						section[i].values_[It->first] = It->second;
					}
					debugData.clear();
					is_duplicate = true;
					break;
				}
			}

			if (!is_duplicate) {
				debugData.setNameSection(nameSection_);
				section.push_back(debugData);
				debugData.clear();
				is_duplicate = false;
			}
		}

		int (INI_Parser::*_execute)(char data); // depending on the variables: is_readingSection, is_readingVariable, is_readingComment will execute methods: readSectionName, readVariable readComment

		int execute(char data) {                // the main method for data processing
			return (this->*_execute)(data);
		};

		int readSectionName(char data) {
			if (data == ']') {				   // section name ending
				is_readingVariable = true;
				is_readingSection = false;
				is_readingComment = false;
				return 1;
			}
			else if (data == '[') {
				return 0;						// ERRor
			}
			else if (data == ';') {
				return 0;						// ERRor
			}
			else {
				nameSection_ += data;
			}
			return 1;							// if return 0 - ERROR
		};

		int readVariable(char data) {			// reading data
			if (data == ';') {
				is_readingComment = true;
				is_readingSection = false;
				is_readingVariable = false;

				is_readingNameVariable = true;
				is_readingValueVariable = false;
				return 1;
			}
			else if (data == '[') {
				is_readingSection = true;
				is_readingComment = false;
				is_readingVariable = false;

				is_readingNameVariable = true;
				is_readingValueVariable = false;

				readDataSection();
				
				nameSection_ = "";
				nameVariable = "";
				valueVariable = "";
				return 1;
			} 
			else if (data == '=') {
				is_readingNameVariable = false;
				is_readingValueVariable = true;
				return 1;
			}
			else if (data == '\n') {
				if (!isBlank(nameVariable)) { debugData.setValue(nameVariable, valueVariable); };
				is_readingNameVariable = true;
				is_readingValueVariable = false;
				nameVariable = "";
				valueVariable = "";
				return 1;
			}
			else {
				if (is_readingNameVariable) {
					nameVariable += data;
					return 1;
				}
				else {
					valueVariable += data;
					return 1;
				}
				return 1;
			}


			return 1; // if return 0 - ERROR
		};

		int readComment(char data) {
				if (data == '\n') {
				// std::cout << "perenos stroki" << std::endl;   // for debugging
				_execute = &INI_Parser::expectation;
				is_readingSection = false;
				is_readingComment = false;
				is_readingVariable = false;
				return 1;
			}

			return 1; // if return 0 - ERROR
		};

		int expectation(char data) {

			if (data == '[') { 

				is_readingSection = true;
				is_readingComment = false;
				is_readingVariable = false;

				is_readingNameVariable = true;
				is_readingValueVariable = false;

				readDataSection();

				nameSection_ = "";
				nameVariable = "";
				valueVariable = "";
				return 1;
			}
			else if (data == ';') {
				is_readingComment = true;
				is_readingSection = false;
				is_readingVariable = false;
				return 1;
			}
			else if (data == '\n') {
				return 1;
			}
			else {
				if (!isBlank(nameVariable)) { debugData.setValue(nameVariable, valueVariable); };
				is_readingNameVariable = true;
				is_readingValueVariable = false;
				nameVariable = "";
				valueVariable = "";

				nameVariable += data;             // so as not to lose 1 character in the variable name
				is_readingVariable = true;
				is_readingComment = false;
				is_readingSection = false;
				return 1;
			}

			return 1; // if return 0 - ERROR
		};

		std::string getNameSection(std::string data) {

		}

	public:
		INI_Parser(std::string filename): filename_(filename), is_readingSection(false), is_readingVariable(false), is_readingNameVariable(true),
			        is_readingValueVariable(false), is_readingComment(false), nameSection_(""), nameVariable(""), valueVariable(""), _execute(&INI_Parser::expectation) {

			file.open(filename_);
			if (file.is_open()) {
				char str;
				file.get(str);
				if (str == '[') {
					_execute = &INI_Parser::readSectionName;
				};
				if (str == ';') {
					_execute = &INI_Parser::readComment;
				};

				while (file.get(str)){
					if (is_readingSection) {
						_execute = &INI_Parser::readSectionName;
					};
					if (is_readingVariable) {
						_execute = &INI_Parser::readVariable;
					};
					if (is_readingComment) {
						_execute = &INI_Parser::readComment;
					};

					execute(str);
				};

				if (file.eof()) {
					if (nameVariable != "") { // Write the last variable if it exists
						if (!isBlank(nameVariable)) { debugData.setValue(nameVariable, valueVariable); };
						readDataSection();
					}; 
				}
			}

			file.close();
		}

		~INI_Parser() {
			file.close();
		}
		
	};

}

