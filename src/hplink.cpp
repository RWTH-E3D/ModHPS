/*
 * hplink.cpp
 *
 *  Created on: 12.12.2017
 *      Author: Sebastian Weck-Ponten
 */

#include "include.h"

using namespace std;

HPLink::HPLink() {
}


bool HPLink::initialize(){ //void HPLink::initialize(){ //double HPLink::initialize(){
	//!function for initialization; assignement of heat pumps, buildings and borehole heat exchangers (BHE); triggers HpCore::assignment() and HpCore::setInitValues();

	//cout << "HPLink::initialize() ..." << endl;

	bool check = Core.coreParse();
	if(check  == false){
		return false;
	}
	Core.assignment();
	Core.setInitValues();


	//cout << "HPLink::initialize() TEST ..." << endl;

	return true;

}

void HPLink::get_dt_comm(double deltat){ //only if HP is the slave

	//!function to get the time step size if ModHPS is the slave model

	Core.dt_comm = deltat;

}

double HPLink::parse_dt_comm(){

	//!function to parse the time step size from XML

	return Core.dt_comm;



}

double HPLink::parse_t_end(){
	//!function to trigger HpCore::coreParse() to parse the simulation end time from XML

	return Core.t_sim_end;

}

unsigned int HPLink::parse_ugModel(){
	//!function to parse the type of subsurface model which is coupled to ModHPS from XML

	/*!
	0 = const. soil temperature; 1 = decreasing soil temperature; 2 = soil temperature data set; 3 = MPFluidGeTIS (numerical); 4 = analytical GeTIS; 5 = other iterative model; 6 = other non-iterative model; 7 = district heating network model
	*/

	return Core.ugModel;

}


vector<string> HPLink::sendBHEseq(){ //vector<string> HPLink::sequence(string s){ //with parsing
	//!function to parse the vector of all BHEs linked to every heat pump instance from XML -> strict sequence

	//Core.sequence();
	//cout << "HPLink::sendBHEseq(): " << endl;
	//for(vector<string>::iterator vit=Core.Phy.all_linked_bhe.begin();vit!=Core.Phy.all_linked_bhe.end();++vit) cout << *vit<< "\n";
	//return(Core.all_bhe);

	return Core.Phy.all_linked_bhe;
}

vector<double> HPLink::sendVbheNominal(){
	//!function to parse the vector of the nominal volume flow rate of all BHEs (strict sequence) linked to every heat pump instance from XML

	//cout << "HPLink::sendVbheNominal(): " << endl;

	return(Core.Phy.VecV_soil_bhe_nominal);
}


//+++++ test validation +++++
/*void HPLink::getVsoil_test(double v){

	//cout << "Get Tsoil_out..." << endl;
	Core.V_soil_test_val = v;
	//cout << "getTsoil_out end" << endl;
}*/
//+++++ end test validation +++++


void HPLink::getTsoil_out(vector<double> v){
	//!function to get the vector of Tsoil_out of the BHEs (strict sequence) linked to every heat pump instance (from the coupled subsurface model or hps.cpp)

	//cout << "Get Tsoil_out..." << endl;

	Core.vecT_soil_out =v;

/*	if(Core.t == Core.t_sim_end - Core.dt_comm){ //test
		cout << "T_soil_out of last time step = " <<  Core.vecT_soil_out[0] << " °C" << endl; //test
	}*/

	//cout << "getTsoil_out end" << endl;

	//test
	//cout << "Core.vecT_soil_out =  " << "\n"; //test
	//for(vector<double>::iterator vit=Core.vecT_soil_out.begin();vit!=Core.vecT_soil_out.end();++vit) cout << *vit << "\n"; //test
}

void HPLink::getTsoil_min(vector<double> v){
	//!function to get the vector of T_soil_cell_min of the BHEs (from the coupled subsurface model or hps.cpp) -> for coupling to numeric subsurface models

	//cout << "getTsoil_min ..." << endl;


	//cout << "HP::Link T_soil_cell_min as vector" << endl;
	//for(vector<double>::iterator vit=v.begin();vit!=v.end();++vit) cout << *vit << "\n";
	Core.VecT_soil_cell_min =v;

	//cout << "getTsoil_min end" << endl;
}


void HPLink::hpCalc(){
	//!function to trigger HpCore::calculate() to call calculation in HpCore

	Core.calculate();

}

void HPLink::getIterCommand(bool b){ //for coupling with MPFluidGETIS or with an iterative subsurface model
	//!function to get an iteration command (from the coupled subsurface model or hps.cpp) -> for coupling to implicit subsurface models

	Core.iterCommand = b;
	//cout << "HPL: iterCommand from MPFluidGeTIS =  " << std::boolalpha << Core.iterCommand << "\n"; //test
	if(Core.iterCommand == false){
		//current = valid;
		Core.resetValues(); //function reset values
	}
	else{ //if(Core.iterCommand == true)
		//valid = current && push in vector
		//cout << "HPL: Core.updateValues() with iterCommand = " << std::boolalpha << Core.iterCommand << "\n"; //test
		Core.updateValues();
	}

}

/*bool HPLink::CheckHeatFluxSoil(){ //test -> not necessary -> only for testing

	if(Core.countErr * Core.dt_comm >= 43200){
		return false;
	}
	else{
		return true;
	}

}*/

vector<double> HPLink::sendT(){
	//!function to send the vector of Tsoil_in of the BHEs (strict sequence) linked to every heat pump instance (to the coupled subsurface model)

	//cout << "Send T..." << endl;
	vecT_soil_in.clear();

	for(unsigned int i=0; i < Core.heatPumps.size(); i++){
		//cout << "i =  " << i << "\n"; //test
		for(unsigned int j=0; j < Core.heatPumps[i].linked_bhe.size(); ++j){
					vecT_soil_in.push_back(Core.heatPumps[i].T_soil_in);
		}
	}

	//test
	//cout << "HPL: vecT_soil_in =  " << "\n"; //test
	//for(vector<double>::iterator vit=vecT_soil_in.begin();vit!=vecT_soil_in.end();++vit) cout << *vit - 273.15 << " °C" << "\n"; //test

	return(vecT_soil_in);
}

vector<double> HPLink::sendV(){
	//!function to send the vector of the time step dependent volume flow rate of all BHEs (strict sequence) linked to every heat pump instance (to the coupled subsurface model; 0 or nominal volume flow rate)

	//cout << "Send V ..." << endl;
	VecV_soil_bhe.clear();

	for(unsigned int i=0; i < Core.heatPumps.size(); i++){
		//cout << "VecV_soil_bhe.push_back(heatPumps[i].Vdot_soil_bhe)" << "\n";//test

		for(unsigned int j=0; j < Core.heatPumps[i].linked_bhe.size(); ++j){
			VecV_soil_bhe.push_back(Core.heatPumps[i].Vdot_soil_bhe);
		}
	}

	//test
	//cout << "HPL: VecV_soil_bhe =  " << "\n"; //test
	//for(vector<double>::iterator vit=VecV_soil_bhe.begin();vit!=VecV_soil_bhe.end();++vit) cout << *vit << "\n"; //test

	return(VecV_soil_bhe); //send V
}

/*double HPLink::sendCurrentTime(){ //necessary? only for coupling with MPFluidGeTIS? only for testing?

	return Core.t; //or in Core.calculate()??

}*/

void HPLink::postprocessing(){
	//!function to trigger the post processing: make .txt result files (trigger HpCore::makeTXT()); automatic plotting of results in diagrams in PDF (trigger HpCore::resPlot())

	if(Core.t_sim_end >= 8760 * 3600){
		Core.makeTXTend();
		Core.resPlot();
		//ToDo: Core.writeResToXML();
	}

}





