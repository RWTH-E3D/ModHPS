/*
 * parser.h
 *
 *  Created on: 09.04.2018
 *      Author: Sebastian Weck-Ponten
 */

#ifndef PARSER_H_
#define PARSER_H_

//#include "include.h" //not necessary

using namespace std;


//!Parses all relevant information from the XML file


class Parser {
public:
	Parser(HpCore* _core);
	HpCore* core;


	//declaration of member functions = methods
	bool parse(const char* pf); //parsing function: bool parse(const char* XML_setting);
	bool xmlCheck(string xml);


private:
	//friend class HPLink;
	friend class HeatPump;
	friend class HpCore;
	friend class Building;



};

#endif /* PARSER_H_ */
