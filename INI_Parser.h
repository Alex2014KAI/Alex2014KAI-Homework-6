#pragma once
#include <iostream>
#include <string>
#include <fstream>
#include <vector>
#include <map>
#include "MyConsts.h"

namespace INIPARSER {

	
	class ExceptionCurrentLinePosition : public std::exception {
		uint32_t currentLinePosition_;
		const char* messageErr;
		std::string message;
	public:
		ExceptionCurrentLinePosition(uint32_t line) : currentLinePosition_(line) {
			message = ("Syntax error on line " + std::to_string(currentLinePosition_));
			messageErr = message.c_str();
		};

		const char* what() const noexcept override {
			return messageErr;
		};
	};

	class ExceptionVariableValueError : public std::exception {
		std::string nameSection_;
		std::string nameValue_;
		std::map<std::string, std::string> values_;
	public:
		ExceptionVariableValueError(const std::map<std::string, std::string>& values, const std::string nameSection,
									const std::string nameValue) :
									values_(values), nameSection_(nameSection), nameValue_(nameValue){};

		const char* what() const noexcept override {
			std::cout << "In section " << nameSection_ << " variable " << nameValue_ << " has no magnitude " << std::endl;
			std::cout << "In section " << nameSection_ << " there are variables: " << std::endl;
			for (auto It = values_.begin(); It != values_.end(); It++) {
				std::cout << It->first;
				if (It->second != "") {
					std::cout << " = " << It->second;
				}
				else {
					std::cout << " not defined";
				}
				std::cout << std::endl;
			}
			return "";
		};
	};

	

	class Section {
	public:
		std::string nameSection_;
		std::map<std::string, std::string> values_{};
	public:
		Section() {}; // = delete;
		~Section() = default;

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
		std::vector<Section> section; // [nameSection1, mapValues1; nameSection2, mapValues2; nameSection3, mapValues3;....nameSection_n, mapValues_n]
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
		uint32_t currentLinePosition; // current line position

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
			switch (data)
			{
			case CONSTS::isEndSectionCharacter: {
				is_readingVariable = true;
				is_readingSection = false;
				is_readingComment = false;
				break;
			}
			case CONSTS::isSectionStartCharacter: {
				throw ExceptionCurrentLinePosition(currentLinePosition);
				return 0;						// ERRor
			}
			case CONSTS::isCommentStartCharacter: {
				throw ExceptionCurrentLinePosition(currentLinePosition);
				return 0;						// ERRor
			}
			case CONSTS::isEqualSymbol: {
				throw ExceptionCurrentLinePosition(currentLinePosition);
				return 0;						// ERRor
			}
			default:
				nameSection_ += data;
				break;
			}

			return 1;							// if return 0 - ERROR
		};

		int readVariable(char data) {			// reading data
			switch (data)
			{
			case CONSTS::isCommentStartCharacter: {
				is_readingComment = true;
				is_readingSection = false;
				is_readingVariable = false;

				is_readingNameVariable = true;
				is_readingValueVariable = false;
				return 1;
			}
			case CONSTS::isSectionStartCharacter: {
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
			case CONSTS::isEqualSymbol: {
				is_readingNameVariable = false;
				is_readingValueVariable = true;
				return 1;
			}
			case '\n': {
				if (!isBlank(nameVariable)) { debugData.setValue(nameVariable, valueVariable); };
				is_readingNameVariable = true;
				is_readingValueVariable = false;
				nameVariable = "";
				valueVariable = "";
				currentLinePosition++;
				return 1;
			}
			default: {
				if (is_readingNameVariable) {
					nameVariable += data;
					return 1;
				}
				else {
					valueVariable += data;
					return 1;
				}
				break;
			}
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
				currentLinePosition++;
				return 1;
			}

			return 1; // if return 0 - ERROR
		};

		int expectation(char data) {

			switch (data)
			{
			case CONSTS::isSectionStartCharacter: {
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
			case CONSTS::isCommentStartCharacter: {
				is_readingComment = true;
				is_readingSection = false;
				is_readingVariable = false;
				return 1;
			}
			case '\n': {
				currentLinePosition++;
				return 1;
			}
			default: {
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
				break;
			}	
			}
			return 1; // if return 0 - ERROR
		};

		std::string getNameSection(std::string data) {

		}

	public:
		INI_Parser(std::string filename): filename_(filename), is_readingSection(false), is_readingVariable(false), 
			is_readingNameVariable(true), is_readingValueVariable(false), is_readingComment(false), 
			nameSection_(""), nameVariable(""), valueVariable(""), currentLinePosition(1),
			_execute(&INI_Parser::expectation) {

			file.open(filename_);
			if (file.is_open()) {
				char str;
				file.get(str);
				if (str == CONSTS::isSectionStartCharacter) {
					_execute = &INI_Parser::readSectionName;
				};
				if (str == CONSTS::isCommentStartCharacter) {
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
			else {
				throw std::exception("File not found or corrupted!");
			}

			std::cout << currentLinePosition << std::endl;
			file.close();
		}

		~INI_Parser() {
			file.close();
		}
		
		template<typename T>
		const T get_value(std::string dataName) {
			if (isBlank(dataName)) {
				throw std::length_error("you must specify the variable name: <section_name.variable_name>");
			}

			std::string nameSection;
			std::string nameValue;
			std::string value;

			bool isSectionRecord{ true };

			for (char ch : dataName) {
				if (isSectionRecord) {
					if (ch == '.') {
						isSectionRecord = false;
						continue;
					}
					nameSection += ch;
				}
				else {
					nameValue += ch;
				}
			}

			bool isThereSection{ false };
			bool isThereValue{ false };

				for (auto vectorIt = section.begin(); vectorIt != section.end(); vectorIt++) {
					if ((*vectorIt).nameSection_ == nameSection) {
						isThereSection = true;
						for (auto It = (*vectorIt).values_.begin(); It != (*vectorIt).values_.end(); It++) {
							if (It->first == nameValue) {
								isThereValue = true;
								value = It->second;

								if (value == "") { 
									throw ExceptionVariableValueError((*vectorIt).values_, nameSection, nameValue);
								};

								if constexpr (std::is_same_v<T, std::string>) { // std::string
									return value;
								}
								if constexpr (std::is_same_v<T, int>) { // int
									int valueINT{ 0 };

									for (int i{ 0 }; i < value.length(); i++) {
										if (((static_cast<int>(value[i]) - 48) > 9) &&
											(static_cast<int>(value[i]) != 46) &&
											(static_cast<int>(value[i]) != 44)) {
											throw std::exception("Not a number");
										}
									}
									valueINT = std::stoi(value);
									return valueINT;
								}
							};
						}
					}
				}
			

			if (!isThereSection) throw std::exception("There is no section with this name");

			if (!isThereValue && isThereSection) throw std::exception("There is no such variable in this section ");

		}

	};
	
}

