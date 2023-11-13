/*
 * parser.cpp
 *
 *  Created on: 09.04.2018
 *      Author: Sebastian Weck-Ponten
 */

#include "include.h"

using namespace rapidxml;
using namespace std;

Parser::Parser(HpCore* _core) {
		core = _core;
}



void parseString2Vector(vector<double>& vector, string xmlstring) { //void parseString2Vector(vector<double>& vector, const char* xmlstring) {
	istringstream amb_string(xmlstring);
	string val;
	while(getline(amb_string,val,' ')) {
		vector.push_back(atof(val.c_str()));
	}
}

void parseString2VectorString(vector<string>& vector, string xmlstring) {
	istringstream amb_string(xmlstring);
	string val;
	while(getline(amb_string,val,' ')) {
		vector.push_back((val));
	}
}

void parseString2List(list<string>& list, const char* xmlstring) { //void parseString2List(list<string>& list, const char* xmlstring) {
	istringstream amb_string(xmlstring);
	string val;
	while(getline(amb_string,val,' ')) {
		list.push_back(val.c_str());
	}
}


//definition of member function parse (string pf)
bool Parser::parse(const char* XML_setting) { //bool HeatPump::parse(const char* XML_setting) { // new: bool Parser::parse(string p) {

 	//parsing XML
	//cout << "Parsing..." << endl;
	xml_document<> doc;    // character type defaults to char
	xml_node<> * root_node;
	// Read the xml file into a vector
	ifstream theFile(XML_setting);//("Muster-XML_v_1.03_test.xml");
	vector<char> buffer((istreambuf_iterator<char>(theFile)), istreambuf_iterator<char>());
	buffer.push_back('\0');
	// Parse the buffer using the xml file parsing library into doc
	doc.parse<0>(&buffer[0]); //doc.parse<0>(text);    // 0 means default parse flags
	// Find our root node
	//root_node = doc.first_node()->first_node("wps:ProcessOutputs")->first_node("wps:Output")->first_node("wps:Data")->first_node()->first_node()->first_node("HeatPumpCollection");

	string undefined = "undefined"; //string for checking undefined values in XML
	string findString;


	if(doc.first_node("getisData") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''getisData''"  << "\n";
		return (false);
	}

	root_node = doc.first_node("getisData");

	if(root_node->first_node("ClimateCollection")->first_node("climate") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''climate''"  << "\n";
		return (false);
	}

	//-> At the moment not necessary
	/*if(root_node->first_node("ClimateCollection")->first_node("climate")->first_node("climate:air_temperature") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "ERROR: There is no tag ''climate:air_temperature''"  << "\n";
		return (false);
	}

	xml_node<> * HP_Tamb_node = root_node->first_node("ClimateCollection")->first_node("climate")->first_node("climate:air_temperature");
	vector<double> T_ambient;
	parseString2Vector(T_ambient,HP_Tamb_node->value());
	//cout << "T_amb vector size = " << T_ambient.size() << "\n";
	//for(vector<double>::iterator vit=T_amb.begin();vit!=T_amb.end();++vit) cout << *vit << "\n";
	core->Phy.T_amb = T_ambient;*/


	//Iterate over heat_pumps
	//cout <<"HERE COMES HEAT PUMP DATA!!!!!!!!!!!!!!!!!!!!!"<< "\n";

	if(root_node->first_node("HeatPumpCollection") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''HeatPumpCollection''"  << "\n";
		return (false);
	}


	if(root_node->first_node("HeatPumpCollection")->first_node("heat_pump") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''heat_pump''"  << "\n";
		return (false);
	}


	for (xml_node<> * HP_node = root_node->first_node("HeatPumpCollection")->first_node("heat_pump"); HP_node; HP_node = HP_node->next_sibling())
	{

		//with simulateHP
		if(HP_node->first_attribute()->next_attribute("heat_pump:id")->value() == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no value for ''heat_pump:id''"  << "\n";
			return (false);
		}
		if(HP_node->first_attribute()->next_attribute("heat_pump:id")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:id'' is ''undefined''"  << "\n";
					return (false);
		}
		string id = HP_node->first_attribute()->next_attribute()->value();
		cout <<"ID heat pump: " << id << "\n";
		core->HP.ID = id; //other option with function 'HP.set_parsedata(ID)'
		//without simulateHP
/*
		if(HP_node->first_attribute()->next_attribute("heat_pump:id")->value() == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no value for ''heat_pump:id''"  << "\n";
			return (false);
		}
		if(HP_node->first_attribute()->next_attribute("heat_pump:id")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:id'' is ''undefined''"  << "\n";
					return (false);
		}
		string id = HP_node->first_attribute()->next_attribute()->value();
		cout <<"ID heat pump: " << id << "\n";
		core->HP.ID = id; //other option with function 'HP.set_parsedata(ID)'
*/

		//simulateHP
		if(HP_node->first_attribute()->next_attribute()->next_attribute("heat_pump:simulateHP")->value() == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no value for ''heat_pump:simulateHP''"  << "\n";
			return (false);
		}
		if(HP_node->first_attribute()->next_attribute()->next_attribute("heat_pump:simulateHP")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:simulateHP'' is ''undefined''"  << "\n";
					return (false);
		}
		int value = atof(HP_node->first_attribute()->next_attribute()->next_attribute("heat_pump:simulateHP")->value());
		if(value == 1){
			core->HP.simulateHP = true;
		}else{ //if(value == 0){
			core->HP.simulateHP = false;
		}
		cout <<"simulateHP of HP " << core->HP.ID << " = " << std::boolalpha << core->HP.simulateHP << "\n";
		core->HP.simulateHP = value; //other option with function 'HP.set_parsedata(ID)'


		//linked buildings as vector
		vector<string> linked_build;
		for(unsigned i=0; linked_build.size();i++) linked_build.erase(linked_build.begin()); //erase vector entries
		if(HP_node->first_node("heat_pump:linked_buildings") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:linked_buildings''"  << "\n";
			return (false);
		}
		if(HP_node->first_node("heat_pump:linked_buildings")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:linked_buildings'' is ''undefined''"  << "\n";
					return (false);
		}
		for(xml_node<> * HP_linked_buildings_node = HP_node->first_node("heat_pump:linked_buildings"); HP_linked_buildings_node; HP_linked_buildings_node = HP_linked_buildings_node->next_sibling("heat_pump:linked_buildings"))
		parseString2VectorString(linked_build,HP_linked_buildings_node->value()); //parseString2List(linked_buildings,HP_linked_buildings_node->value());
		//cout << "Linked buildings size = " << linked_build.size() << "\n";
		//for(vector<string>::iterator vit=linked_build.begin();vit!=linked_build.end();++vit) cout << *vit << "\n";
		core->HP.linked_buildings = linked_build;


		//linked bhes as vector
		vector<string> link_bhe;
		for(unsigned i=0; link_bhe.size();i++) link_bhe.erase(link_bhe.begin());
		if(HP_node->first_node("heat_pump:linked_bhe") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:linked_bhe''"  << "\n";
			return (false);
		}
		else if(HP_node->first_node("heat_pump:linked_bhe")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:linked_bhe'' is ''undefined''"  << "\n";
					return (false);
		}
		for (xml_node<> * HP_linked_bhe_node = HP_node->first_node("heat_pump:linked_bhe"); HP_linked_bhe_node; HP_linked_bhe_node = HP_linked_bhe_node->next_sibling("heat_pump:linked_bhe"))
		parseString2VectorString(link_bhe,HP_linked_bhe_node->value());
		//cout << "Linked bhe size = " << core->HP.linked_bhe.size() << "\n";
		//for(vector<string>::iterator vit=link_bhe.begin();vit!=link_bhe.end();++vit) cout << *vit << "\n";
		core->HP.linked_bhe = link_bhe;

		//parse rho*cp for HP
		string bhe_id = HP_node->first_node("heat_pump:linked_bhe")->value();
		//Iterate over BHECollection
		if(root_node->first_node("BHECollection") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''BHECollection''"  << "\n";
			return (false);
		}
		if(root_node->first_node("BHECollection")->first_node("BHE") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''BHE''"  << "\n";
			return (false);
		}
		for (xml_node<> * BHE_node = root_node->first_node("BHECollection")->first_node("BHE"); BHE_node; BHE_node = BHE_node->next_sibling())
		{
			if(BHE_node->first_attribute()->next_attribute()->value() == 0){
				//MPI_Abbort(MPI_COMM_WORLD,66);
				cout << "HPM: Parser: ERROR: There is no value for ''bhe:id''"  << "\n";
				return (false);
			}
			if(BHE_node->first_attribute()->next_attribute()->value() == bhe_id){
				xml_node<> * HP_brine_volumetric_heat_capacity = BHE_node->first_node("bhe:brine_volumetric_heat_capacity");
				double rho_cp = atof(HP_brine_volumetric_heat_capacity->value());
				core->HP.rhoCp_soil = rho_cp;
			}
		}
		//cout << "HPM: Parser: Test: core->HP.rhoCp_soil = " << core->HP.rhoCp_soil  << "\n";
		//test if bhes on same HP have the same value? -> XML-checker

		//test
		/*for (xml_node<> * ResSCOP_WPA_node = HP_node->first_node("heat_pump:ResSCOP_WPA"); ResSCOP_WPA_node; ResSCOP_WPA_node = ResSCOP_WPA_node->next_sibling()){
			cout << "parser.cpp: writeResToXML: string StringTest = to_string(3.33333);" << "\n"; //test
			string StringTest = to_string(3.33333);
			cout << "HPC: writeResToXML: StringTest = " << StringTest << "\n"; //test
			cout << "HPC: writeResToXML: const char * text = doc.allocate_string(StringTest.c_str()); " << "\n"; //test
			const char * text = doc.allocate_string(StringTest.c_str());
			cout << "HPC: writeResToXML: const char * text = " << text << "\n"; //test
			cout << "HPC: writeResToXML: xml_node<> * StringTest = HP_node->first_node(heat_pump:ResSCOP_WPA); " << "\n"; //test
			//xml_node<> * ResSCOP_WPA_node = HP_node->first_node("heat_pump:ResSCOP_WPA");
			//cout << "HPC: writeResToXML: ResSCOP_WPA_node->value(text); " << "\n"; //test
			//ResSCOP_WPA_node->value(text);
			cout << "HPC: writeResToXML: ResSCOP_WPA_node = doc.allocate_node(node_element, text); " << "\n"; //test
			ResSCOP_WPA_node = doc.allocate_node(node_element, text); 
		}
		cout << "HPC: writeResToXML: Done! " << endl;
    	std::ofstream myfile;
    	cout << "HPC: writeResToXML: myfile.open (./xml_file.xml); " << "\n"; //test
    	myfile.open ("../run/hpm/xml_file.xml");
    	myfile << doc;*/	


		for (xml_node<> * HP_linked_bhe_node = HP_node->first_node("heat_pump:linked_bhe"); HP_linked_bhe_node; HP_linked_bhe_node = HP_linked_bhe_node->next_sibling("heat_pump:linked_bhe"))
		parseString2VectorString(core->Phy.all_linked_bhe,HP_linked_bhe_node->value());

		/*vector<string> all_bhe;
		vector<string> all_bhe_corrected;
		for (xml_node<> * HP_linked_bhe_node = HP_node->first_node("heat_pump:linked_bhe"); HP_linked_bhe_node; HP_linked_bhe_node = HP_linked_bhe_node->next_sibling("heat_pump:linked_bhe"))
		parseString2VectorString(all_bhe,HP_linked_bhe_node->value());
		for(unsigned j=0; j < all_bhe.size();j++){
			all_bhe_corrected.push_back(all_bhe[j]);
		}
		cout << "All linked bhe in parser = " << "\n";
		for(vector<string>::iterator vit=all_bhe_corrected.begin();vit!=all_bhe_corrected.end();++vit) cout << *vit << "\n";
		//std::list<int> it1;
		//it1 = all_bhe_corrected.begin();
		for(int i=0; i < all_bhe_corrected.size(); i++){
			for(int j=i+1; j < all_bhe_corrected.size(); j++){
				//++it1;
				if(all_bhe_corrected[i] == all_bhe_corrected[j]){ //&& all_bhe_corrected[i] > 0 -> i>0 ??
					for(int k=j+1; k < all_bhe_corrected.size(); k++){
						all_bhe_corrected[k-1] = all_bhe_corrected[k];
					}
					all_bhe_corrected.erase(all_bhe_corrected.begin()+j);
				}
			}
		}
		core->Phy.all_linked_bhe = all_bhe_corrected;
		cout << "All linked bhe size in parser = " << core->Phy.all_linked_bhe.size() << "\n";*/


		if(HP_node->first_node("heat_pump:name")->value() == 0){
			cout << "HPM: Parser: ERROR: There is no value for ''heat_pump:name''"  << "\n";
			return (false);
		}
		if(HP_node->first_node("heat_pump:name")->value() == undefined){
			cout << "HPM: Parser: ERROR: The tag ''heat_pump:name'' is ''undefined''"  << "\n";
			return (false);
		}
		string name = HP_node->first_node("heat_pump:name")->value();
		core->HP.HPname = name;

		if(HP_node->first_node("heat_pump:operating_mode") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:operating_mode''"  << "\n";
			return (false);
		}
		else if(HP_node->first_node("heat_pump:operating_mode")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:operating_mode'' is ''undefined''"  << "\n";
					return (false);
		}
		cout << "HPM: Parser: Value of operating mode out of XML: " << HP_node->first_node("heat_pump:operating_mode")->value() << "\n";
		xml_node<> * HP_operating_mode_node = HP_node->first_node("heat_pump:operating_mode");
		//int o_mode = atof(HP_operating_mode_node->value());
		//cout << "Operating mode: " << operating_mode << "\n";
		core->HP.operatingMode = atof(HP_operating_mode_node->value());

		if(HP_node->first_node("heat_pump:application_range") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:application_range''"  << "\n";
			return (false);
		}
		else if(HP_node->first_node("heat_pump:application_range")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:application_range'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_application_range_node = HP_node->first_node("heat_pump:application_range");
		int application_range = atof(HP_application_range_node->value());
		core->HP.app_range = application_range;

		if(HP_node->first_node("heat_pump:compressor_mode") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:compressor_mode''"  << "\n";
			return (false);
		}
		else if(HP_node->first_node("heat_pump:compressor_mode")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:compressor_mode'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_compressor_mode_node = HP_node->first_node("heat_pump:compressor_mode");
		int c_mode = atof(HP_compressor_mode_node->value());
		//cout << "Compressor mode: " << compressor_mode << "\n";
		core->HP.compressor_mode = c_mode;

		/*if(HP_node->first_node("heat_pump:Tmin_soil_in") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "ERROR: There is no tag ''heat_pump:Tmin_soil_in''"  << "\n";
			return (false);
		}
		xml_node<> * HP_Tmin_soil_in_node = HP_node->first_node("heat_pump:Tmin_soil_in");
		double TminSoilIn = atof(HP_Tmin_soil_in_node->value());
		//cout << "Tmin_soil_in: " << Tmin_soil_in << " K\n";
		core->HP.Tmin_soil_in = TminSoilIn;*/

		/*
		if(HP_node->first_node("heat_pump:Tmax_soil_in") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "ERROR: There is no tag ''heat_pump:Tmax_soil_in''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Tmax_soil_in_node = HP_node->first_node("heat_pump:Tmax_soil_in");
		double TmaxSoilIn = atof(HP_Tmax_soil_in_node->value());
		//cout << "Tmax_soil_in: " << Tmax_soil_in << " K\n";
		core->HP.Tmax_soil_in = TmaxSoilIn;*/

/*		//Tmin_soil_out -> system control
		if(HP_node->first_node("heat_pump:Tmin_soil_out") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "ERROR: There is no tag ''heat_pump:Tmin_soil_out''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Tmin_soil_out_node = HP_node->first_node("heat_pump:Tmin_soil_out");
		double TminSoilOut = atof(HP_Tmin_soil_out_node->value());
		//cout << "Tmin_soil_out: " << Tmin_soil_out << " K\n";
		core->HP.Tmin_soil_out = TminSoilOut;*/

		//Tmax_soil_out -> At the moment not necessary -> system control with heatPumps[i].T_soil_steps
		/*if(HP_node->first_node("heat_pump:Tmax_soil_out") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "ERROR: There is no tag ''heat_pump:Tmax_soil_out''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Tmax_soil_out_node = HP_node->first_node("heat_pump:Tmax_soil_out");
		double TmaxSoilOut = atof(HP_Tmax_soil_out_node->value());
		//cout << "Tmax_soil_out: " << Tmax_soil_out << " K\n";
		core->HP.Tmax_soil_out = TmaxSoilOut;*/



//+++ Phy.Tsoil_cellcrit +++
		if(HP_node->first_node("heat_pump:Tsoil_cellcrit") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Tsoil_cellcrit''"  << "\n";
			return (false);
		}
		else if(HP_node->first_node("heat_pump:Tsoil_cellcrit")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Tsoil_cellcrit'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Tsoil_cellcrit_node = HP_node->first_node("heat_pump:Tsoil_cellcrit");
		core->Phy.Tsoil_cellcrit = atof(HP_Tsoil_cellcrit_node->value());
		//cout << "Parser::Tsoil_cellcrit: " << core->Phy.Tsoil_cellcrit << " K\n";

//+++ HP.Vdot_soil, HP.Vdot_soil_bhe, HP.Vdot_soil_bhe_init, Phy.VecV_soil_bhe_nominal +++
		if(HP_node->first_node("heat_pump:V_soil") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:V_soil''"  << "\n";
			return (false);
		}
		else if(HP_node->first_node("heat_pump:V_soil")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:V_soil'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_V_soil_node = HP_node->first_node("heat_pump:V_soil");
		double V_s = atof(HP_V_soil_node->value());
		//cout << "V_soil: " << V_soil << " m³/s\n";
		core->HP.Vdot_soil_init = V_s;

		double V_s_bhe = V_s / link_bhe.size();
		core->HP.Vdot_soil_bhe =V_s_bhe;
		core->HP.Vdot_soil_bhe_init =V_s_bhe;
		for(unsigned int i=0; i < link_bhe.size(); ++i) {
			core->Phy.VecV_soil_bhe_nominal.push_back(V_s_bhe);
		}
		//for(vector<double>::iterator vit=core->Phy.VecV_soil_bhe_nominal.begin();vit!=core->Phy.VecV_soil_bhe_nominal.end();++vit) cout << *vit<< "\n"; //test

//+++ HP.P_el_soil_pump +++
		if(HP_node->first_node("heat_pump:P_el_soil_pump") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_soil_pump''"  << "\n";
			return (false);
		}
		if(HP_node->first_node("heat_pump:P_el_soil_pump")->value() == undefined){ // value or zero
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_soil_pump'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_soil_pump_node = HP_node->first_node("heat_pump:P_el_soil_pump");
		core->HP.P_el_soil_pump = atof(HP_P_el_soil_pump_node->value());
		//cout << "P_el_soil_pump: " << core->HP.P_el_soil_pump << " W\n";


//+++ HP.Vol_st  +++
		if(HP_node->first_node("heat_pump:volume_storage") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:volume_storage''"  << "\n";
			return (false);
		}
		xml_node<> * HP_volume_storage_node = HP_node->first_node("heat_pump:volume_storage");
		if(HP_volume_storage_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:volume_storage'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:volume_storage'' is set to ''0''"  << "\n";
					core->HP.Vol_st = 0;
		}
		else{
			core->HP.Vol_st = atof(HP_volume_storage_node->value());
			//cout << "Storage volume: " << Vol_st << " m³\n";
		}

//+++ HP.losses_st +++
		if(HP_node->first_node("heat_pump:static_loss_storage") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:static_loss_storage''"  << "\n";
			return (false);
		}
		xml_node<> * HP_loss_storage_node = HP_node->first_node("heat_pump:static_loss_storage");
		if(HP_loss_storage_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:static_loss_storage'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:static_loss_storage'' is set to ''0''"  << "\n";
					core->HP.losses_st = 0;
		}
		else{
			core->HP.losses_st = atof(HP_loss_storage_node->value());
		}

//+++ HP.Vol_st_dhw +++
		if(HP_node->first_node("heat_pump:volume_storage_dhw") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:volume_storage_dhw''"  << "\n";
			return (false);
		}
		xml_node<> * HP_volume_storage_dhw_node = HP_node->first_node("heat_pump:volume_storage_dhw");
		if(HP_volume_storage_dhw_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:volume_storage_dhw'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:volume_storage_dhw'' is set to ''0''"  << "\n";
					core->HP.Vol_st_dhw = 0;
		}
		else{
			core->HP.Vol_st_dhw = atof(HP_volume_storage_dhw_node->value());
			//cout << "Storage volume: " << Vol_st << " m³\n";
		}
		//cout << "Storage volume: " << Vol_st << " m³\n";


//+++ HP.losses_st_dhw +++
		if(HP_node->first_node("heat_pump:static_loss_storage_dhw") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:static_loss_storage_dhw''"  << "\n";
			return (false);
		}
		xml_node<> * HP_loss_storage_dhw_node = HP_node->first_node("heat_pump:static_loss_storage_dhw");
		if(HP_loss_storage_dhw_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:static_loss_storage_dhw'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:static_loss_storage_dhw'' is set to ''0''"  << "\n";
					core->HP.losses_st_dhw = 0;
		}
		else{
			core->HP.losses_st_dhw = atof(HP_loss_storage_dhw_node->value());
		}
		//cout << "Storage losses dhw = " << core->HP.losses_st_dhw << " W \n"; //test

//+++ heat_pump:T_dhw -> HP.T_st_max_dhw+++
		if(HP_node->first_node("heat_pump:T_dhw") == 0){ //= DHW supply temperature
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:T_dhw''"  << "\n";
			return (false);
		}
		xml_node<> * HP_T_dhw_node = HP_node->first_node("heat_pump:T_dhw");
		if(HP_T_dhw_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:T_dhw'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:T_dhw'' is set to ''273.15 K''"  << "\n";
					core->HP.T_st_max_dhw = 273.15;
		}
		else{
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
		}
		//cout << "PARSER: T_st_max_dhw = heat_pump:T_dhw = DHW supply temperature =  " << core->HP.T_st_max_dhw -273.15 << "°C" << "\n"; //test

//+++ heat_pump:Tmin_dhw -> HP.T_st_min_dhw  +++
		if(HP_node->first_node("heat_pump:Tmin_dhw") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Tmin_dhw''"  << "\n";
			return (false);
		}
		xml_node<> * HP_T_min_dhw_node = HP_node->first_node("heat_pump:Tmin_dhw");
		if(HP_T_min_dhw_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:Tmin_dhw'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:Tmin_dhw'' is set to ''273.15 K''"  << "\n";
					core->HP.T_st_min_dhw = 273.15;
		}
		else{
			core->HP.T_st_min_dhw = atof(HP_T_min_dhw_node->value());
		}
		//cout << "PARSER: T_st_min_dhw = heat_pump:Tmin_dhw = minimal DHW temperature =  " << core->HP.T_st_min_dhw -273.15 << "°C" << "\n"; //test

//+++ Tmax_dhw -> HP.T_dhw_step  +++
		if(HP_node->first_node("heat_pump:Tmax_dhw") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Tmax_dhw''"  << "\n";
			return (false);
		}
		xml_node<> * HP_T_max_dhw_node = HP_node->first_node("heat_pump:Tmax_dhw");
		if(HP_T_max_dhw_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:Tmax_dhw'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:Tmax_dhw'' is set to ''273.15 K''"  << "\n";
					core->HP.T_dhw_step = 273.15;
		}
		else{
			core->HP.T_dhw_step = atof(HP_T_max_dhw_node->value());
		}
		//cout << "PARSER: T_dhw_step = heat_pump:Tmax_dhw = temperature stage of HP =  " << core->HP.T_dhw_step -273.15 << "°C" << "\n"; //test

//+++ check temperatures  +++
		if (core->HP.T_st_max_dhw > core->HP.T_dhw_step) {
			cout << "HPM: Parser: ERROR: The tag ''heat_pump:Tmax_dhw' (= T_dhw_step) < ''heat_pump:T_dhw'' (= T_st_max_dhw) " << "\n";
			return (false);
		}

		if(core->HP.T_dhw_step <= core->HP.T_st_min_dhw){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Tmax_dhw' (= T_dhw_step) <= ''heat_pump:Tmin_dhw'' (= T_st_min_dhw)" << "\n";
					return (false);
		}

		if (core->HP.T_st_min_dhw >= core->HP.T_st_max_dhw) {
			cout << "HPM: Parser: ERROR: The tag ''heat_pump:Tmin_dhw'' (= T_st_min_dhw) >= ''heat_pump:T_dhw'' (= T_st_max_dhw) " << "\n";
			return (false);
		}

//+++ HP.T_soil_steps +++
		if(HP_node->first_node("heat_pump:heating_power_T") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:heating_power_T''"  << "\n";
			return (false);
		}
		xml_node<> * HP_heating_power_T_node = HP_node->first_node("heat_pump:heating_power_T");
		findString = HP_node->first_node("heat_pump:heating_power_T")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: One value of the tag ''heat_pump:heating_power_T'' is ''undefined''"  << "\n";
					return (false);
		}
		vector<double> T_s_steps;
		T_s_steps.clear(); //for(unsigned i=0; T_s_steps.size();i++) T_s_steps.erase(T_s_steps.begin());
		parseString2Vector(T_s_steps,HP_heating_power_T_node->value());
		//cout << "T_soil_steps vector size = " << T_s_steps.size() << "\n";
		//cout << "Temperature steps for heating power, compressor power and COP [K]: " << "\n";
		//for(vector<double>::iterator vit=T_s_steps.begin();vit!=T_s_steps.end();++vit) cout << *vit << "\n";
		core->HP.T_soil_steps = T_s_steps;

		//Table of Q_h dependent of T_h (35 °C, 45 °C, 55 °C, 60 °C)
		if(HP_node->first_node("heat_pump:Q_h_W35") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W35''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W35")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W35'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W35_node = HP_node->first_node("heat_pump:Q_h_W35");
		//cout << "Q_h_W35 [W]: " << HP_Q_h_W35_node->value() << "\n";
		vector<double> Q35;
		Q35.clear();
		parseString2Vector(Q35,HP_Q_h_W35_node->value());
		core->HP.mapQ_h[35] = Q35;
		//HP.Q35 = Q35; //first try

		if(HP_node->first_node("heat_pump:Q_h_W40") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W40''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W40")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W40'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W40_node = HP_node->first_node("heat_pump:Q_h_W40");
		//cout << "Q_h_W35 [W]: " << HP_Q_h_W35_node->value() << "\n";
		vector<double> Q40;
		Q40.clear();
		parseString2Vector(Q40,HP_Q_h_W40_node->value());
		core->HP.mapQ_h[40] = Q40;

		if(HP_node->first_node("heat_pump:Q_h_W45") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W45''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W45")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W45'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W45_node = HP_node->first_node("heat_pump:Q_h_W45");
		vector<double> Q45;
		Q45.clear();
		parseString2Vector(Q45,HP_Q_h_W45_node->value());
		core->HP.mapQ_h[45] = Q45;

		if(HP_node->first_node("heat_pump:Q_h_W50") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W50''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W50")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W50'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W50_node = HP_node->first_node("heat_pump:Q_h_W50");
		vector<double> Q50;
		Q50.clear();
		parseString2Vector(Q50,HP_Q_h_W50_node->value());
		core->HP.mapQ_h[50] = Q50;

		if(HP_node->first_node("heat_pump:Q_h_W55") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W55''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W55")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W55'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W55_node = HP_node->first_node("heat_pump:Q_h_W55");
		vector<double> Q55;
		Q55.clear();
		parseString2Vector(Q55,HP_Q_h_W55_node->value());
		core->HP.mapQ_h[55] = Q55;

		if(HP_node->first_node("heat_pump:Q_h_W60") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W60''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W60")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W60'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W60_node = HP_node->first_node("heat_pump:Q_h_W60");
		vector<double> Q60;
		Q60.clear();
		parseString2Vector(Q60,HP_Q_h_W60_node->value());
		core->HP.mapQ_h[60] = Q60;

		if(HP_node->first_node("heat_pump:Q_h_W65") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W65''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W65")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W65'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W65_node = HP_node->first_node("heat_pump:Q_h_W65");
		vector<double> Q65;
		Q65.clear();
		parseString2Vector(Q65,HP_Q_h_W65_node->value());
		core->HP.mapQ_h[65] = Q65;

		if(HP_node->first_node("heat_pump:Q_h_W75") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Q_h_W75''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:Q_h_W75")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Q_h_W75'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Q_h_W75_node = HP_node->first_node("heat_pump:Q_h_W75");
		vector<double> Q75;
		Q75.clear();
		parseString2Vector(Q75,HP_Q_h_W75_node->value());
		core->HP.mapQ_h[75] = Q75;


		//Table of P_el dependent of T_h (35 °C, 45 °C, 55 °C, 60 °C)
		if(HP_node->first_node("heat_pump:P_el_W35") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W35''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W35")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W35'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W35_node = HP_node->first_node("heat_pump:P_el_W35");
		vector<double> P35;
		P35.clear();
		parseString2Vector(P35,HP_P_el_W35_node->value());
		core->HP.mapP_el[35] = P35;

		if(HP_node->first_node("heat_pump:P_el_W40") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W40''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W40")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W40'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W40_node = HP_node->first_node("heat_pump:P_el_W40");
		vector<double> P40;
		P40.clear();
		parseString2Vector(P40,HP_P_el_W40_node->value());
		core->HP.mapP_el[40] = P40;

		if(HP_node->first_node("heat_pump:P_el_W45") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W45''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W45")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W45'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W45_node = HP_node->first_node("heat_pump:P_el_W45");
		vector<double> P45;
		P45.clear();
		parseString2Vector(P45,HP_P_el_W45_node->value());
		core->HP.mapP_el[45] = P45;

		if(HP_node->first_node("heat_pump:P_el_W50") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W50''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W50")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W50'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W50_node = HP_node->first_node("heat_pump:P_el_W50");
		vector<double> P50;
		P50.clear();
		parseString2Vector(P50,HP_P_el_W50_node->value());
		core->HP.mapP_el[50] = P50;

		if(HP_node->first_node("heat_pump:P_el_W55") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W55''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W55")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W55'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W55_node = HP_node->first_node("heat_pump:P_el_W55");
		vector<double> P55;
		P55.clear();
		parseString2Vector(P55,HP_P_el_W55_node->value());
		core->HP.mapP_el[55] = P55;

		if(HP_node->first_node("heat_pump:P_el_W60") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W60''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W60")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W60'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W60_node = HP_node->first_node("heat_pump:P_el_W60");
		vector<double> P60;
		P60.clear();
		parseString2Vector(P60,HP_P_el_W60_node->value());
		core->HP.mapP_el[60] = P60;

		if(HP_node->first_node("heat_pump:P_el_W65") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W65''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W65")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W65'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W65_node = HP_node->first_node("heat_pump:P_el_W65");
		vector<double> P65;
		P65.clear();
		parseString2Vector(P65,HP_P_el_W65_node->value());
		core->HP.mapP_el[65] = P65;

		if(HP_node->first_node("heat_pump:P_el_W75") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:P_el_W75''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:P_el_W75")->value();
		if(findString.find(undefined) != std::string::npos){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:P_el_W75'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_P_el_W75_node = HP_node->first_node("heat_pump:P_el_W75");
		vector<double> P75;
		P75.clear();
		parseString2Vector(P75,HP_P_el_W75_node->value());
		core->HP.mapP_el[75] = P75;

		//cout << "HPM:parser test test test  ..." << endl;

//+++ hysteresis +++
		if(HP_node->first_node("heat_pump:hysteresis") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:hysteresis''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:hysteresis")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: Warning: The tag ''heat_pump:hysteresis'' is ''undefined''"  << "\n";
			cout << "HPM: Parser: The tag ''heat_pump:hysteresis'' is set to ''0.0''"  << "\n";
			core->HP.hysteresis = 0.0;
		}else{
			xml_node<> * HP_hysteresis_node = HP_node->first_node("heat_pump:hysteresis");
			core->HP.hysteresis = atof(HP_hysteresis_node->value());
			//cout << "HP.hysteresis = " << core->HP.hysteresis << " K" << "\n";
		}
		//cout << "HP.hysteresis = " << core->HP.hysteresis << " K" << "\n";

//+++ blocking time +++
		//blockingTime
		if(HP_node->first_node("heat_pump:blockingTime") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:blockingTime''"  << "\n";
			return (false);
			}
		findString = HP_node->first_node("heat_pump:blockingTime")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: The tag ''heat_pump:blockingTime'' is ''undefined''"  << "\n";
			cout << "HPM: Parser: The tag ''heat_pump:blockingTime'' is set to ''0''"  << "\n";
			core->HP.blockingTime = 0;
		}
		else{
			xml_node<> * HP_blockingTime_node = HP_node->first_node("heat_pump:blockingTime");
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			core->HP.blockingTime = atof(HP_blockingTime_node->value());
		}
		cout << "HPM: Parser: core->HP.blockingTime = " << core->HP.blockingTime << "\n"; //test

		//parse blocking times create vecBlockHP with core->heatPumps..push_back()
		if(HP_node->first_node("heat_pump:blockingHoursDay") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:blockingHoursDay''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:blockingHoursDay")->value();
		vector<double> blockingHoursDay;
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: Warning: The tag ''heat_pump:blockingHoursDay'' is ''undefined''"  << "\n";
			vector<bool> test(8760, false);
			core->HP.vecBlockHP = test;
			//for(vector<bool>::iterator vit=core->HP.vecBlockHP.begin();vit!=core->HP.vecBlockHP.end();++vit) cout << *vit << "\n"; //test
			cout << "HPM: Parser: ''HP.vecBlockHP'' is set to ''false''"  << "\n";
			if(core->HP.blockingTime > 0){
				cout << "HPM: Parser: Error because blockingTime > 0"  << "\n";
				return (false);
			}

		}else{
			xml_node<> * HP_blockingHoursDay_node = HP_node->first_node("heat_pump:blockingHoursDay");
			parseString2Vector(blockingHoursDay,HP_blockingHoursDay_node->value());
			cout << "HPM: Parser: blockingHoursDay.size() = " << blockingHoursDay.size() << "; vector blockingHoursDay = " << "\n"; //test
			for(vector<double>::iterator vit=blockingHoursDay.begin();vit!=blockingHoursDay.end();++vit) cout << *vit << "\n"; //test
			vector<bool> blockDay(24, false); //one day -> with blockingHoursDay //or vector<bool> blockDay(24, 0);
			for(unsigned int i = 0; i < blockingHoursDay.size(); i++){
				blockDay[blockingHoursDay[i]] = true;
			}
			//cout << "blockDay.size() = " << blockDay.size() << " = " << "\n"; //test
			//for(vector<bool>::iterator vit=blockDay.begin();vit!=blockDay.end();++vit) cout << *vit << "\n"; //test
			vector<bool> blockYear; //(8760, 1)
			for(unsigned int i = 0; i< 365; i++){ //or while
				for(unsigned int j= 0; j < blockDay.size(); j++){
					blockYear.push_back(blockDay[j]);
				}
			}
			//cout << "blockYear.size()" << blockYear.size() << "\n"; //test
			//for(vector<bool>::iterator vit=blockYear.begin();vit!=blockYear.end();++vit) cout << *vit << "\n"; //test
			core->HP.vecBlockHP = blockYear;
			blockingHoursDay.clear();
			blockDay.clear();
			blockYear.clear();
		}



		//secHeatGenOnInBlockingTime
		if(HP_node->first_node("heat_pump:secHeatGenOnInBlockingTime") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:secHeatGenOnInBlockingTime''"  << "\n";
			return (false);
			}
		findString = HP_node->first_node("heat_pump:secHeatGenOnInBlockingTime")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: The tag ''heat_pump:secHeatGenOnInBlockingTime'' is ''undefined''"  << "\n";
			cout << "HPM: Parser: The tag ''heat_pump:secHeatGenOnInBlockingTime'' is set to ''false''"  << "\n";
			core->HP.secHeatGenOnInBlockingTime = false;
		}
		else{
			xml_node<> * HP_secHeatGenOnInBlockingTime_node = HP_node->first_node("heat_pump:secHeatGenOnInBlockingTime");
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			int value = atof(HP_secHeatGenOnInBlockingTime_node->value());
			if(value == 0){
				core->HP.secHeatGenOnInBlockingTime = false;
			}else if(value == 1){
				core->HP.secHeatGenOnInBlockingTime = true;
			}

			//core->HP.secHeatGenOnInBlockingTime = atof(HP_secHeatGenOnInBlockingTime_node->value());
		}
		cout << "HPM: Parser: core->HP.secHeatGenOnInBlockingTime = " << std::boolalpha << core->HP.secHeatGenOnInBlockingTime << "\n"; //test

//+++ nominal Power H +++
		if(HP_node->first_node("heat_pump:nominalPowerH") == 0){
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:nominalPowerH''"  << "\n";
			return (false);
			}
		findString = HP_node->first_node("heat_pump:nominalPowerH")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: The tag ''heat_pump:nominalPowerH'' is ''undefined''"  << "\n";
			cout << "HPM: Parser: The tag ''heat_pump:nominalPowerH'' is set to ''0''"  << "\n";
			core->HP.nominalPowerH = 0;
		}
		else{
			xml_node<> * HP_nominalPowerH_node = HP_node->first_node("heat_pump:nominalPowerH");
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			core->HP.nominalPowerH = atof(HP_nominalPowerH_node->value());
		}
		cout << "HPM: Parser: core->HP.nominalPowerH = " << core->HP.nominalPowerH << "\n"; //test
//+++ nominal Power dhw +++
		if(HP_node->first_node("heat_pump:nominalPowerDHW") == 0){
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:nominalPowerDHW''"  << "\n";
			return (false);
			}
		findString = HP_node->first_node("heat_pump:nominalPowerDHW")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: The tag ''heat_pump:nominalPowerDHW'' is ''undefined''"  << "\n";
			cout << "HPM: Parser: The tag ''heat_pump:nominalPowerDHW'' is set to ''0''"  << "\n";
			core->HP.nominalPowerDHW = 0;
		}
		else{
			xml_node<> * HP_nominalPowerDHW_node = HP_node->first_node("heat_pump:nominalPowerDHW");
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			core->HP.nominalPowerDHW = atof(HP_nominalPowerDHW_node->value());
		}
		cout << "HPM: Parser: core->HP.nominalPowerDHW = " << core->HP.nominalPowerDHW << "\n"; //test

//+++ nominal Power H of second heat generators for bivalent and monoenergetic systems+++
		if(HP_node->first_node("heat_pump:nominalPower2Heatgen") == 0){
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:nominalPower2Heatgen''"  << "\n";
			return (false);
			}
		findString = HP_node->first_node("heat_pump:nominalPower2Heatgen")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: The tag ''heat_pump:nominalPower2Heatgen'' is ''undefined''"  << "\n";
			cout << "HPM: Parser: The tag ''heat_pump:nominalPower2Heatgen'' is set to ''0''"  << "\n";
			core->HP.nominalPowerSecHeatGen = 0;
		}
		else{
			xml_node<> * HP_nominalPower2HeatGen_node = HP_node->first_node("heat_pump:nominalPower2Heatgen");
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			core->HP.nominalPowerSecHeatGen = atof(HP_nominalPower2HeatGen_node->value());
		}
		cout << "HPM: Parser: core->HP.nominalPower2HeatGen = " << core->HP.nominalPowerSecHeatGen << "\n"; //test


//+++ heating load where heat pumps is shut down for bivalent part parallel systems+++
		if(HP_node->first_node("heat_pump:shutDownHL") == 0){
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:shutDownHL''"  << "\n";
			return (false);
		}

		findString = HP_node->first_node("heat_pump:shutDownHL")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: The tag ''heat_pump:shutDownHL'' is ''undefined''"  << "\n";
			if(core->HP.operatingMode == 3){
				cout << "HPM: Parser: ERROR: The tag ''heat_pump:shutDownHL'' is ''undefined'' although system is bivalent part parallel."  << "\n";
				return (false);
			}else{
				cout << "HPM: Parser: HP.shutDownHL is set to ''0''"  << "\n";
				core->HP.shutDownHL = 0;
			}
		}
		else{
			xml_node<> * HP_shutDownHL_node = HP_node->first_node("heat_pump:shutDownHL");
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			core->HP.shutDownHL = atof(HP_shutDownHL_node->value());
		}
		cout << "HPM: Parser: core->HP.shutDownHL = " << core->HP.shutDownHL << "\n"; //test

//+++ cascade factor as vector+++
		if(HP_node->first_node("heat_pump:cascadeFactor") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:cascadeFactor''"  << "\n";
			return (false);
		}
		findString = HP_node->first_node("heat_pump:cascadeFactor")->value();
		if(findString.find(undefined) != std::string::npos){
			cout << "HPM: Parser: Warning: The tag ''heat_pump:cascadeFactor'' is ''undefined''"  << "\n";
			vector<double> vecTest(1, 1);
			core->HP.vecCascadeFactor = vecTest;
			for(vector<double>::iterator vit=core->HP.vecCascadeFactor.begin();vit!=core->HP.vecCascadeFactor.end();++vit) cout << *vit << "\n"; //test
			cout << "HPM: Parser: HP.vecCascadeFactor is set to ''1''"  << "\n";
		}else{
			xml_node<> * HP_vecCascadeFactor_node = HP_node->first_node("heat_pump:cascadeFactor");
			vector<double> CascadeFactor;
			parseString2Vector(CascadeFactor,HP_vecCascadeFactor_node->value());
			cout << "HPM: Parser: CascadeFactor.size() = " << CascadeFactor.size() << "; vector CascadeFactor = " << "\n"; //test
			for(vector<double>::iterator vit=CascadeFactor.begin();vit!=CascadeFactor.end();++vit) cout << *vit << "\n"; //test
			core->HP.vecCascadeFactor = CascadeFactor;
			CascadeFactor.clear();
		}

//+++ cooling +++

		//+++ coolingType +++
		if(HP_node->first_node("heat_pump:coolingType") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:coolingType''"  << "\n";
			return (false);
		}
		else if(HP_node->first_node("heat_pump:coolingType")->value() == undefined){
			cout << "HPM: Parser: ERROR: The tag ''heat_pump:coolingType'' is ''undefined''"  << "\n";
			if( (core->HP.app_range == 3) || (core->HP.app_range == 4) || (core->HP.app_range == 5) || (core->HP.app_range == 6) ){ //cooling
				cout << "HPM: Parser: ERROR: The tag ''heat_pump:coolingType'' is ''undefined'' although HP can cool."  << "\n";
				return (false);
			}else{
				cout << "HPM: Parser: HP.coolingType is set to ''0''"  << "\n";
				core->HP.coolingType = 0;
			}
		}
		cout << "HPM: Parser: Value of coolingType out of XML: " << HP_node->first_node("heat_pump:coolingType")->value() << "\n";
		xml_node<> * HP_coolingType_node = HP_node->first_node("heat_pump:coolingType");
		core->HP.coolingType = atof(HP_coolingType_node->value());
		if(core->HP.coolingType == 1){ //active
			cout << "HPM: Parser: ERROR: active ccoling is not yet implemented."  << "\n";
			return (false);
		}

		//+++ maxTempPassivCooling -> T_maxPassivCooling +++
		//usually in buildings maxTempPassivCooling ??
		if(HP_node->first_node("heat_pump:maxTempPassivCooling") == 0){ //= TWW-Nenntemperatur
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:maxTempPassivCooling''"  << "\n";
			return (false);
		}
		xml_node<> * HP_T_maxPassivCooling_node = HP_node->first_node("heat_pump:maxTempPassivCooling");
		if(HP_T_maxPassivCooling_node->value() == undefined){
					cout << "HPM: Parser: The tag ''heat_pump:maxTempPassivCooling'' is ''undefined''"  << "\n";
					cout << "HPM: Parser: The tag ''heat_pump:maxTempPassivCooling'' is set to ''273.15 K''"  << "\n";
					core->HP.T_maxPassivCooling = 273.15;
		}
		else{
			//core->HP.T_st_max_dhw = atof(HP_T_dhw_node->value());
			core->HP.T_maxPassivCooling = atof(HP_T_maxPassivCooling_node->value());
		}
		//cout << "PARSER: T_maxPassivCooling =  " << core->HP.T_maxPassivCooling -273.15 << "°C" << "\n"; //test



		//+++ HP.Vdot_soil_cooling, HP.Vdot_soil_bhe_cooling, HP.Vdot_soil_bhe_init +++
		if(HP_node->first_node("heat_pump:Vdot_soil_cooling") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''heat_pump:Vdot_soil_cooling''"  << "\n";
			return (false);
		}
		if(HP_node->first_node("heat_pump:Vdot_soil_cooling")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''heat_pump:Vdot_soil_cooling'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * HP_Vdot_soil_cooling_node = HP_node->first_node("heat_pump:Vdot_soil_cooling");
		double V_sCool = atof(HP_Vdot_soil_cooling_node->value());
		core->HP.Vdot_soilCooling = V_sCool;
		double V_sCool_bhe = V_sCool / link_bhe.size();
		core->HP.Vdot_soilBHEcoolingInit = V_sCool_bhe;



//+++ write object in vector +++
		core->heatPumps.push_back(core->HP); //write object in vector
	}

	//Iterate over BuildingCollection
	//cout <<"HERE COMES BUILDING DATA!!!!!!!!!!!!!!!!!!!!!"<< "\n";
	if(root_node->first_node("BuildingCollection") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''BuildingCollection''"  << "\n";
		return (false);
	}
	if(root_node->first_node("BuildingCollection")->first_node("building") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''building''"  << "\n";
		return (false);
	}
	for (xml_node<> * building_node = root_node->first_node("BuildingCollection")->first_node("building"); building_node; building_node = building_node->next_sibling())
	{
		if(building_node->first_attribute()->next_attribute()->value() == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no value for ''building:id''"  << "\n";
			return (false);
		}
		if(building_node->first_attribute()->next_attribute()->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''building:id'' is ''undefined''"  << "\n";
					return (false);
		}
		string bID = building_node->first_attribute()->next_attribute()->value();
		core->Build.buildingID = bID;
		//cout <<"ID building: " << core->Build.buildingID << "\n";
		core->Phy.VecAllBuildings.push_back(bID);

		vector<string> link_hp;
		for(unsigned i=0; link_hp.size();i++) link_hp.erase(link_hp.begin());
		if(building_node->first_node("building:linked_heat_pump") == 0){

			cout << "HPM: Parser: ERROR: There is no tag ''building:linked_heat_pump''"  << "\n";
			return (false);
		}
		if(building_node->first_node("building:linked_heat_pump")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''building:linked_heat_pump'' is ''undefined''"  << "\n";
					return (false);
		}
		for (xml_node<> * linked_hp_node = building_node->first_node("building:linked_heat_pump"); linked_hp_node; linked_hp_node = linked_hp_node->next_sibling("building:linked_heat_pump"))
		parseString2VectorString(link_hp,linked_hp_node->value());
		//cout << "Linked heat pumps size = " << linked_hp.size() << "\n";
		//for(vector<string>::iterator vit=link_hp.begin();vit!=link_hp.end();++vit) cout << *vit << "\n";
		core->Build.linked_hp = link_hp;

//+++ core->Build.T_heating_system +++
		if(building_node->first_node("building:T_heating_system") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:T_heating_system''"  << "\n";
			return (false);
		}
		if(building_node->first_node("building:T_heating_system")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''building:T_heating_system'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * T_heating_system_node = building_node->first_node("building:T_heating_system");
		core->Build.T_heating_system = atof(T_heating_system_node->value());
		//cout << "T_heating_system: " << core->Build.T_heating_system << " K\n";

//+++ core->Build.Tmin_heating_system +++
		if(building_node->first_node("building:Tmin_heating_system") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:Tmin_heating_system''"  << "\n";
			return (false);
		}
		if(building_node->first_node("building:Tmin_heating_system")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''building:Tmin_heating_system'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * Tmin_heating_system_node = building_node->first_node("building:Tmin_heating_system");
		core->Build.Tmin_heating_system = atof(Tmin_heating_system_node->value());
		//cout << "Tmin_heating_system: " << core->Build.Tmin_heating_system << " K\n";


		if (core->Build.Tmin_heating_system >= core->Build.T_heating_system) {
			cout << "HPM: Parser: ERROR: The tag ''building:Tmin_heating_system'' (= T_st_min) >= ''building:T_heating_system'' (= T_st_max) " << "\n";
			return (false);
		}

//+++ core->Build.Tmax_heating_system +++
		//necessary for iteration of Q_h -> step of HP -> assign in hp_core
		if(building_node->first_node("building:Tmax_heating_system") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:Tmax_heating_system''"  << "\n";
			return (false);
		}
		if(building_node->first_node("building:Tmax_heating_system")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''building:Tmax_heating_system'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * Tmax_heating_system_node = building_node->first_node("building:Tmax_heating_system");
		core->Build.Tmax_heating_system = atof(Tmax_heating_system_node->value());
		//cout << "Tmax_heating_system: " << core->Build.Tmax_heating_system << " K\n";


		if(core->Build.Tmax_heating_system <= core->Build.Tmin_heating_system){
					cout << "HPM: Parser: ERROR: The tag ''building:Tmax_heating_system'' (= T_step of HP) <= ''building:Tmin_heating_system'' (= T_st_min)" << "\n";
					return (false);
		}

		if(core->Build.Tmax_heating_system < core->Build.T_heating_system){
					cout << "HPM: Parser: ERROR: The tag ''building:Tmax_heating_system'' (= T_step of HP) < ''building:T_heating_system'' (= T_st_max)"  << "\n";
					return (false);
		}

		//not yet necessary
		/*if(building_node->first_node("building:T_heating_limit") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "ERROR: There is no tag ''building:T_heating_limit''"  << "\n";
					return (false);
		}
		xml_node<> * T_heating_limit_node = building_node->first_node("building:T_heating_limit");
		double T_h_limit = atof(T_heating_limit_node->value());
		//cout << "T_heating_limit: " << T_heating_limit << " K\n";
		core->Build.T_heating_limit = T_h_limit;*/

		vector<double> Q_HLoad(8760, 1);

		if(building_node->first_node("building:which_HL") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:which_HL''"  << "\n";
			return (false);
		}
		if(building_node->first_node("building:which_HL")->value() == undefined){
					cout << "HPM: Parser: ERROR: The tag ''building:which_HL'' is ''undefined''"  << "\n";
					return (false);
		}
		xml_node<> * which_HL_node = building_node->first_node("building:which_HL");
		double which_HL = atof(which_HL_node->value());
		//cout << "T_heating_limit: " << T_heating_limit << " K\n";
		core->Build.HL_type = which_HL;

		if(which_HL == 0){
		if(building_node->first_node("building:heating_load") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:heating_load''. Start building simulation / calculation again."  << "\n";
			return (false);
		}
		xml_node<> * HeatingLoad_node = building_node->first_node("building:heating_load");
		//cout << "Heating load vector size in parser before erase= " << Q_HLoad.size() << "\n";
		Q_HLoad.clear(); //for(unsigned i=0; Q_HLoad.size();i++) Q_HLoad.erase(Q_HLoad.begin());
		//for (xml_node<> * HeatingLoad_node = building_node->first_node("building:heating_load"); HeatingLoad_node; HeatingLoad_node = HeatingLoad_node->next_sibling("building:heating_load")) //necessary?
		parseString2Vector(Q_HLoad,HeatingLoad_node->value());
		//cout << "Heating load vector size in parser= " << Q_HLoad.size() << "\n";
		//cout << "Heating load [W] zu x = 0	1	8758	8759	Q_HLoad.size()-1	Q_HLoad.size()-2: " << "\n";
		//cout << Q_HLoad[0] << " " << Q_HLoad[1] << " " << Q_HLoad[8758] << " " << Q_HLoad[8759] << " " << Q_HLoad[Q_HLoad.size()-1] << " " << Q_HLoad[Q_HLoad.size()-2] << "\n";
		core->Build.Q_HL = Q_HLoad;
		}

		if(which_HL == 1){
		if(building_node->first_node("building:heating_load_corr_T_heating_limit") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:heating_load_corr_T_heating_limit''"  << "\n";
			return (false);
		}
		xml_node<> * HeatingLoad_corr_heating_limit_node = building_node->first_node("building:heating_load_corr_T_heating_limit");
		//cout << "Heating load vector size in parser before erase= " << Q_HLoad.size() << "\n";
		Q_HLoad.clear(); //for(unsigned i=0; Q_HLoad.size();i++) Q_HLoad.erase(Q_HLoad.begin());
		//for (xml_node<> * HeatingLoad_node = building_node->first_node("building:heating_load"); HeatingLoad_node; HeatingLoad_node = HeatingLoad_node->next_sibling("building:heating_load")) //necessary?
		parseString2Vector(Q_HLoad,HeatingLoad_corr_heating_limit_node->value());
		core->Build.Q_HL = Q_HLoad;
		}

		if(which_HL == 2){
		if(building_node->first_node("building:heating_load_no_heating_summer") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:heating_load_no_heating_summer''"  << "\n";
			return (false);
		}
		xml_node<> * HeatingLoad_no_heating_summer_node = building_node->first_node("building:heating_load_no_heating_summer");
		//cout << "Heating load vector size in parser before erase= " << Q_HLoad.size() << "\n";
		Q_HLoad.clear(); //for(unsigned i=0; Q_HLoad.size();i++) Q_HLoad.erase(Q_HLoad.begin());
		//for (xml_node<> * HeatingLoad_node = building_node->first_node("building:heating_load"); HeatingLoad_node; HeatingLoad_node = HeatingLoad_node->next_sibling("building:heating_load")) //necessary?
		parseString2Vector(Q_HLoad,HeatingLoad_no_heating_summer_node->value());
		core->Build.Q_HL = Q_HLoad;
		}
		//cout << "Heating load vector size in parser = " << Q_HLoad.size() << "\n"; //test

		//dhw-load
		if(building_node->first_node("building:dhw_load") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:dhw_load''"  << "\n";
			return (false);
		}
		xml_node<> * dhwLoad_node = building_node->first_node("building:dhw_load");
		vector<double> dhw_load(8760, 1);
		dhw_load.clear();//for(unsigned i=0; dhw_load.size();i++) dhw_load.erase(dhw_load.begin());
		parseString2Vector(dhw_load,dhwLoad_node->value());
		core->Build.Q_dhw_load = dhw_load;
		//cout << "dhw load vector size in parser = " << dhw_load.size() << "\n"; //test

		//cooling-load
		if(building_node->first_node("building:cooling_load") == 0){
			//MPI_Abbort(MPI_COMM_WORLD,66);
			cout << "HPM: Parser: ERROR: There is no tag ''building:cooling_load''"  << "\n";
			return (false);
		}else if(building_node->first_node("building:cooling_load")->value() == undefined){
			cout << "HPM: Parser: ERROR: The tag ''building:cooling_load'' is ''undefined''"  << "\n";
			cout << "HPM: Parser: HP.coolingLoad is set to ''0''"  << "\n";
			vector<double> vecCollZero(8760, 0);
			core->Build.coolingLoad = vecCollZero;
		}
		xml_node<> * coolingLoad_node = building_node->first_node("building:cooling_load");
		vector<double> vecCollZero(8760, 0);
		vecCollZero.clear();//for(unsigned i=0; dhw_load.size();i++) dhw_load.erase(dhw_load.begin());
		parseString2Vector(vecCollZero,coolingLoad_node->value());
		core->Build.coolingLoad = vecCollZero;
		cout << "coolingLoad vector size in parser = " << core->Build.coolingLoad.size() << "\n"; //test
		//for(vector<double>::iterator vit=core->Build.coolingLoad.begin();vit!=core->Build.coolingLoad.end();++vit) cout << *vit<< "\n";



		core->buildings.push_back(core->Build); //write object in vector
	}



	//Simulation setup
	//cout <<"HERE COMES simulation setup DATA!!!!!!!!!!!!!!!!!!!!!"<< "\n";
	if(root_node->first_node("SimulationSetup") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''SimulationSetup''"  << "\n";
		return (false);
	}
	if(root_node->first_node("SimulationSetup")->first_node("simhp:dt_comm") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''simhp:dt_comm''"  << "\n";
		return (false);
	}
	if(root_node->first_node("SimulationSetup")->first_node("simhp:dt_comm")->value() == undefined){
				cout << "HPM: Parser: ERROR: The tag ''simhp:dt_comm'' is ''undefined''"  << "\n";
				return (false);
	}
	xml_node<> * dt_comm_node = root_node->first_node("SimulationSetup")->first_node("simhp:dt_comm");
	core->dt_comm = atof(dt_comm_node->value());

	if(root_node->first_node("SimulationSetup")->first_node("simsoil:end_time") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''simsoil:end_time''"  << "\n";
		return (false);
	}
	if(root_node->first_node("SimulationSetup")->first_node("simsoil:end_time")->value() == undefined){
				cout << "HPM: Parser: ERROR: The tag ''simsoil:end_time'' is ''undefined''"  << "\n";
				return (false);
	}
	xml_node<> * end_time_node = root_node->first_node("SimulationSetup")->first_node("simsoil:end_time");
	double endTime;
	endTime = atof(end_time_node->value());
	core->t_sim_end = endTime;
	core->endYear = ceil(endTime / (8760*3600) ); //starts with 0 if endTime<1 -> is 1 if endTime=1

	//cout << "HPM:parser test test test before  ugModel ..." << endl;

	if(root_node->first_node("SimulationSetup")->first_node("simsoil:ugModel") == 0){
		//MPI_Abbort(MPI_COMM_WORLD,66);
		cout << "HPM: Parser: ERROR: There is no tag ''simsoil:ugModel''"  << "\n";
		return (false);
	}
	if(root_node->first_node("SimulationSetup")->first_node("simsoil:ugModel")->value() == undefined){
				cout << "HPM: Parser: ERROR: The tag ''simsoil:ugModel'' is ''undefined''"  << "\n";
				return (false);
	}
	xml_node<> * ugModel_node = root_node->first_node("SimulationSetup")->first_node("simsoil:ugModel");
	core->ugModel = atof(ugModel_node->value());

	//cout << "HPM:parser test test test ugModel = " << core->ugModel << endl;


	for(unsigned int i=0; i < core->heatPumps.size(); i++){
		cout << "HPM: HP: " << core->heatPumps[i].ID << "\n";
        for(vector<string>::iterator vit=core->heatPumps[i].linked_buildings.begin();vit!=core->heatPumps[i].linked_buildings.end();++vit) cout << "HPM: \tBuilding: " << *vit<< "\n";
		for(vector<string>::iterator vit=core->heatPumps[i].linked_bhe.begin();vit!=core->heatPumps[i].linked_bhe.end();++vit) cout << "HPM:\tBHE: " << *vit<< "\n";
	}
	cout << "HPM: All IDs of buildings in hpCore" << "\n"; //check
	for(vector<string>::iterator vit=core->Phy.VecAllBuildings.begin();vit!=core->Phy.VecAllBuildings.end();++vit) cout << "HPM: \tBuilding: "<< *vit<< "\n";
	cout << "HPM: All IDs of BHEs in hpCore before correcture" << "\n";
	for(vector<string>::iterator vit=core->Phy.all_linked_bhe.begin();vit!=core->Phy.all_linked_bhe.end();++vit) cout << "HPM: \tBHE: "<< *vit<< "\n";


	return true; //check if parsing works??

}
