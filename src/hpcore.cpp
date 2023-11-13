/*
 * hpcore.cpp
 *
 *  Created on: 10.04.2018
 *      Author: Sebastian Weck-Ponten
 */

#include "include.h"

using namespace rapidxml;
using namespace std;


HpCore::HpCore() {
	input = "../import/xml_file.xml";
	t = 0.0;
	t_valid = 0.0;
	year = 0;
	actYear = 0;
	endYear = 0;
	dt_comm = 0.0;
	t_sim_end = 0.0;
	step = 0;
	//Vdot_soil_test_val = 0; //for validation test!!!
	ugModel = 0; //0 = const. soil temperature; 1 = decreasing soil temperature; 2 = soil temperature data set; 3 = MPFluidGeTIS (numerical); 4 = analytical GeTIS; 5 = other iterative model; 6 = other non-iterative model; 7 = district heating network model)" << "\n";
	iterCommand = true;
	counterIter = 0;
	//countErr = 0;
	sumVecH = 0.0;
	sumVecDHW = 0.0;
	yearVecSize = 1;
	rho_w = 1000; //[kg/m³]
	cp_w = 4180; // [J/kg*K]
	//cascade
	sumFactorCascade = 1;
}

bool HpCore::coreParse() {
	//!function to parse the simulation end time from XML

	//cout << "HpCore::coreParse() ..." << endl;

	Parser ParseObj(this);


	input = "../import/xml_file.xml"; 



	ifstream ifile(input);
	if (!ifile) {

		ifstream afile(input);
		if (!afile) {
			cout << "ERROR: Input file " << input << " does not exist!" << endl;
			return false;
			//MPI_Abort(MPI_COMM_WORLD, 1981);
		}
	}

	return ParseObj.parse(input);

}


void HpCore::setInitValues() {
	//!function to set initial values

	//cout << "HpCore::setInitValues() ..." << endl;
	//t = dt_comm;
	//cout << "HpCore::setInitValues() t = " << t << endl;
	actYear = 1;

	t_valid = t;

	yearVecSize = floor(8760 * 3600 / dt_comm);

	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		heatPumps[i].T_soil_in = 273.15 + 8;
		//heatPumps[i].T_soil_out = 273.15 + 2; //test
		heatPumps[i].ValidT_soil_in = heatPumps[i].T_soil_in;
		heatPumps[i].ValidT_soil_out = heatPumps[i].T_soil_out;
		heatPumps[i].ValidU_st_useful = heatPumps[i].U_st_useful;
		heatPumps[i].ValidT_st = heatPumps[i].T_st;
		heatPumps[i].ValidU_st_useful_dhw = heatPumps[i].U_st_useful_dhw;
		heatPumps[i].ValidT_st_dhw = heatPumps[i].T_st_dhw;
		heatPumps[i].ValidQ_h = heatPumps[i].Q_h;
		heatPumps[i].ValidQ_dhw = heatPumps[i].Q_dhw;
		heatPumps[i].ValidP_el = heatPumps[i].P_el;
		heatPumps[i].ValidCOP = heatPumps[i].COP;
		heatPumps[i].ValidVdot_soil_bhe = heatPumps[i].Vdot_soil_bhe_init;
		heatPumps[i].ValidHP_mode = heatPumps[i].HP_mode;
		heatPumps[i].ValidHP_on = heatPumps[i].HP_on;
		heatPumps[i].ValidTimeCritSoil = heatPumps[i].timeCritSoil;
		//weighting because of time steps which are not a part of 3600
		heatPumps[i].ValidQ_HL_hp_weight = heatPumps[i].Q_HL_hp_weight;
		heatPumps[i].ValidQ_HL_hp_time = heatPumps[i].Q_HL_hp_time;
		heatPumps[i].ValidQ_dhw_hp_weight = heatPumps[i].Q_dhw_hp_weight;
		heatPumps[i].ValidQ_dhw_hp_time = heatPumps[i].Q_dhw_hp_time;
		//+++ blocking time+++
		heatPumps[i].ValidBlockCounter = heatPumps[i].blockCounter;
		heatPumps[i].ValidBlockHP = heatPumps[i].blockHP;
		heatPumps[i].ValidBlockMarker = heatPumps[i].blockMarker;
		heatPumps[i].ValidFullLoad = heatPumps[i].fullLoad;
		//+++ bivalent +++
		heatPumps[i].ValidSecHeatGen_on = heatPumps[i].secHeatGen_on;
		heatPumps[i].ValidQ_hSecHeatGen = heatPumps[i].Q_hSecHeatGen;
		heatPumps[i].ValidQ_dhwSecHeatGen = heatPumps[i].Q_dhwSecHeatGen;
		heatPumps[i].energySecHeatGenH = 0.0;
		heatPumps[i].energySecHeatGenDHW = 0.0;
		heatPumps[i].ValidEnergySecHeatGenH = heatPumps[i].energySecHeatGenH;
		heatPumps[i].ValidEnergySecHeatGenDHW = heatPumps[i].energySecHeatGenDHW;
		//+++cooling+++
		heatPumps[i].ValidEnergyCooling = heatPumps[i].energyCooling;
		heatPumps[i].vaildVdot_soilBHEcooling = heatPumps[i].Vdot_soilBHEcooling;

	}

	//cout << "VecV_soil_bhe_nominal =  " << "\n"; //test
	//for(vector<double>::iterator vit=Phy.VecV_soil_bhe_nominal.begin();vit!=Phy.VecV_soil_bhe_nominal.end();++vit) cout << *vit << "\n"; //test
	//test VecV_soil_bhe_nominal
}

void HpCore::assignment() {
	//!function to assign buildings (hourly heating, dhw and cooling loads; heating system temperatures; ...) and storage tanks to the heat pump instances

	//cout << "HpCore::assignment() ..." << endl;
	cout << "HpCore::assignment() t_sim_end = " << t_sim_end << " s" << endl;
	cout << "HpCore::assignment() endYear = " << endYear << " a" << endl;

	//delta_t from MPF
	cout << "HpCore::assignment() delta t communication in assignment() = " << dt_comm << "\n";

	//cout << "HPCore::Assignment Tsoil_cellcrit = " << heatPumps[i].Tsoil_cellcrit << "\n";


	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		//cout << "actual HP:  " << "'"<< heatPumps[i].ID<< "'" << "\n";

//+++++++++++++++Assignment of buildings to HP -> Q_HL+++++++++++++++
		vector<double> Q_HL_sum(8760, 0); //[W]
		vector<double> Q_dhw_sum(8760, 0); //[W]
		vector<double> Q_cool_sum(8760, 0); //[W]
		//cout << "linked building size of " << heatPumps[i].ID << " = " << heatPumps[i].linked_buildings.size() << "\n"; //test
		//for(vector<string>::iterator vit=heatPumps[i].linked_buildings.begin();vit!=heatPumps[i].linked_buildings.end();++vit) cout << *vit << "\n"; //test
		for (unsigned int j = 0; j < heatPumps[i].linked_buildings.size(); j++) {
			for (unsigned int k = 0; k < buildings.size(); k++) {
				if (heatPumps[i].linked_buildings[j] == buildings[k].buildingID) {
					double res_HL;
					double res_dhw;
					double res_cool;
					for (int l = 0; l < 8760; l++) { //8760 h
						res_HL = buildings[k].Q_HL[l] / buildings[k].linked_hp.size();
						res_dhw = buildings[k].Q_dhw_load[l] / buildings[k].linked_hp.size();
						res_cool = buildings[k].coolingLoad[l] / buildings[k].linked_hp.size();
						Q_HL_sum[l] = Q_HL_sum[l] + res_HL;
						Q_dhw_sum[l] = Q_dhw_sum[l] + res_dhw;
						Q_cool_sum[l] = Q_cool_sum[l] + res_cool;
					}
					heatPumps[i].vecPreQ_HL_hp = Q_HL_sum; //vector of hourly heating powers (over the Year) of all linked buildings -> [W at every hour]
					heatPumps[i].vecPreQ_dhw_hp = Q_dhw_sum; //vector of hourly dhw powers (over the Year) of all linked buildings -> [W at every hour]
					heatPumps[i].vecPreQ_COOL_hp = Q_cool_sum; //vector of hourly cooling powers (over the Year) of all linked buildings -> [W at every hour]
					//cout << "vecPreQ_HL_hp size  = " << heatPumps[i].vecPreQ_HL_hp.size() << "\n"; //test
					//for(vector<double>::iterator vit=heatPumps[i].vecPreQ_HL_hp.begin();vit!=heatPumps[i].vecPreQ_HL_hp.end();++vit) cout << *vit << "\n"; //test
					//build_linked_hp = buildings[k].linked_hp.size();
					//cout << "LINKED hp size OF ACTUAL BUILDING = " << build_linked_hp << "\n";
					//cout << "T_h_step of "<< heatPumps[i].ID << " before if = "  << heatPumps[i].T_h_step << "\n"; //test
					if (buildings[k].Tmax_heating_system > heatPumps[i].T_h_step) { //necessary for more than one building
						heatPumps[i].T_h_step =	buildings[k].Tmax_heating_system;
						//cout << "T_h_step of "<< heatPumps[i].ID << " in if = "  << heatPumps[i].T_h_step << "\n"; //test
					}
					//cout << "variable k = " << k << "\n";
					//cout << "buildings[k].Tmax_heating_system of "<< buildings[k].buildingID << " = " << buildings[k].Tmax_heating_system << "\n"; //test
					//cout << "T_dhw_step of " << heatPumps[i].ID << " = "  << heatPumps[i].T_dhw_step << "\n"; //test
				}
			}
		}
		//cout << "Q_HL_sum[0] of HP " << heatPumps[i].ID << " after loop = " << Q_HL_sum[0] << "\n";
		//cout << "Q_dhw_sum[0] of HP " << heatPumps[i].ID << " after loop = " << Q_dhw_sum[0] << "\n";
		//cout << "T_h_step of "<< heatPumps[i].ID << " after loop = "  << heatPumps[i].T_h_step << "\n"; //test

		double sum_vec = 0;
		for (std::vector<double>::iterator it = heatPumps[i].vecPreQ_HL_hp.begin();it != heatPumps[i].vecPreQ_HL_hp.end(); ++it)sum_vec += *it; //sum of all yearly heating loads of HP [W]
		heatPumps[i].buildEnergyHeating = sum_vec * 3600; //sum of vector [w at every hour] * 3600 = [J]
		//cout << "heatPumps[i].buildEnergyHeating at assignment= " << heatPumps[i].buildEnergyHeating / 1000 / 3600 << " kWh" << "\n";
		sum_vec = 0;
		for (std::vector<double>::iterator it = heatPumps[i].vecPreQ_dhw_hp.begin();it != heatPumps[i].vecPreQ_dhw_hp.end(); ++it)sum_vec += *it; //sum of all yearly dhw loads of HP [W]
		heatPumps[i].buildEnergyDHW = sum_vec * 3600; //sum of vector [w at every hour] * 3600 = [J]
		//cout << "heatPumps[i].buildEnergyDHW at assignment = " << heatPumps[i].buildEnergyDHW  / 1000 / 3600 << " kWh" << "\n";
		sum_vec = 0;
		for (std::vector<double>::iterator it = heatPumps[i].vecPreQ_COOL_hp.begin();it != heatPumps[i].vecPreQ_COOL_hp.end(); ++it)sum_vec += *it; //sum of all yearly dhw loads of HP [W]
		heatPumps[i].buildEnergyCooling = sum_vec * 3600; //sum of vector [w at every hour] * 3600 = [J]
		cout << "HPC: heatPumps[i].buildEnergyCooling at assignment = " << heatPumps[i].buildEnergyCooling  / 1000 / 3600 << " kWh" << "\n";


		//+++assignment buildings and storages to HP for T_st_min & T_st_max & T_st_min/max & T_st_dhw_min/max+++
		if ((heatPumps[i].app_range == 0) || (heatPumps[i].app_range == 2) || (heatPumps[i].app_range == 3) || (heatPumps[i].app_range == 4)) { //with heating
			//+++assignment buildings to HP for T_st_min & T_st_max+++
			for (unsigned int j = 0; j < heatPumps[i].linked_buildings.size(); j++) {
				for (unsigned int k = 0; k < buildings.size(); k++) {
					if (heatPumps[i].linked_buildings[j] == buildings[k].buildingID) {
						////cout << "App range of "<< heatPumps[i].ID << " = "  << heatPumps[i].app_range << "\n";
						////cout << "Linked buildings size of "<< heatPumps[i].ID << " = "  << heatPumps[i].linked_buildings.size() << "\n";
						////cout << "Actual linked building of " << heatPumps[i].ID << " = " << heatPumps[i].linked_buildings[j] << "\n";
						////cout << "variable k = " << k << "\n";
						////cout << "Actual linked buildings[k].buildingID "<< heatPumps[i].ID << " = "  << buildings[k].buildingID << "\n"; //buildings[k].buildingID
						if (buildings[k].Tmin_heating_system > heatPumps[i].T_st_min) { //necessary for more than one building
							heatPumps[i].T_st_min =	buildings[k].Tmin_heating_system; //no hysteresis ! - (heatPumps[i].heating_hyst / 2); //only hysteresis at lower limit, so that HP runs longer
						}
						if (buildings[k].T_heating_system > heatPumps[i].T_st_max) { //necessary for more than one building
							heatPumps[i].T_st_max = buildings[k].T_heating_system; //a hysteresis at upper limit makes no sense for floor heating, as HP should supply only 35 °C!!!
						}
					}
				}
			}
			//cout << "T_st_min of "<< heatPumps[i].ID << "after loop = "  << heatPumps[i].T_st_min << "\n";
			//cout << "T_st_max of "<< heatPumps[i].ID << "after loop = "  << heatPumps[i].T_st_max << "\n";

//+++assignment storages to HP for T_st_min/max & T_st_dhw_min/max & Volume ...+++
			if ((heatPumps[i].app_range == 0) || (heatPumps[i].app_range == 3)) { //no dhw
				heatPumps[i].U_st_useful_dhw = 0.0;
				heatPumps[i].T_st_dhw = 273.15;
				heatPumps[i].T_st_min_dhw = 273.15;
				heatPumps[i].T_st_max_dhw = 273.15;
				heatPumps[i].losses_st_dhw = 0.0;
				if (heatPumps[i].Vol_st > 0) {
					heatPumps[i].HP_storage = 1; //1 = only heating storage;
					heatPumps[i].T_st = (heatPumps[i].T_st_min + heatPumps[i].T_st_max) / 2;
					heatPumps[i].U_st_useful = heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_st_min);
				} else { //+++if(heatPumps[i].Vol_st == 0)+++
					heatPumps[i].HP_storage = 3; //3 = no storages;
					heatPumps[i].T_st = 273.15;
					heatPumps[i].T_st_min = 273.15;
					heatPumps[i].T_st_max = 273.15;
					heatPumps[i].U_st_useful = 0.0;
					heatPumps[i].losses_st = 0.0;
				}
			} else if((heatPumps[i].app_range == 2) || (heatPumps[i].app_range == 4)){ //heating + dhw
				if (heatPumps[i].Vol_st > 0) {
					heatPumps[i].T_st = (heatPumps[i].T_st_min
							+ heatPumps[i].T_st_max) / 2;
					heatPumps[i].U_st_useful = heatPumps[i].Vol_st * rho_w
							* cp_w
							* (heatPumps[i].T_st - heatPumps[i].T_st_min);
					if (heatPumps[i].Vol_st_dhw > 0) {
						heatPumps[i].HP_storage = 0; //0 = heating and dhw storages;
						heatPumps[i].T_st_dhw = (heatPumps[i].T_st_min_dhw + heatPumps[i].T_st_max_dhw) / 2;
						heatPumps[i].U_st_useful_dhw = heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_st_min_dhw);
					} else { //if(heatPumps[i].Vol_st_dhw == 0)
						heatPumps[i].HP_storage = 1; //1 = only heating storage;
						heatPumps[i].T_st_dhw = 273.15;
						heatPumps[i].T_st_min_dhw = 273.15;
						heatPumps[i].T_st_max_dhw = 273.15;
						heatPumps[i].U_st_useful_dhw = 0.0;
						heatPumps[i].losses_st_dhw = 0.0;
					}
				} else { //if(heatPumps[i].Vol_st == 0)
					heatPumps[i].T_st = 273.15;
					heatPumps[i].T_st_min = 273.15;
					heatPumps[i].T_st_max = 273.15;
					heatPumps[i].U_st_useful = 0.0;
					heatPumps[i].losses_st = 0.0;
					if (heatPumps[i].Vol_st_dhw > 0) {
						heatPumps[i].HP_storage = 2; //2 = only dhw storage;
						heatPumps[i].T_st_dhw = (heatPumps[i].T_st_min_dhw + heatPumps[i].T_st_max_dhw) / 2;
						heatPumps[i].U_st_useful_dhw = heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_st_min_dhw);
					} else { //if(heatPumps[i].Vol_st_dhw == 0)
						heatPumps[i].HP_storage = 3; //3 = no storages;
						heatPumps[i].T_st_dhw = 273.15;
						heatPumps[i].T_st_min_dhw = 273.15;
						heatPumps[i].T_st_max_dhw = 273.15;
						heatPumps[i].U_st_useful_dhw = 0.0;
						heatPumps[i].losses_st_dhw = 0.0;
					}
				}
			}
		} else if( (heatPumps[i].app_range == 1) || (heatPumps[i].app_range == 6) ){ // no heating
			heatPumps[i].T_st = 273.15;
			heatPumps[i].T_st_min = 273.15;
			heatPumps[i].T_st_max = 273.15;
			heatPumps[i].U_st_useful = 0.0;
			heatPumps[i].losses_st = 0.0;
			if (heatPumps[i].Vol_st_dhw > 0) {
				heatPumps[i].HP_storage = 2; //2 = only dhw storage;
				heatPumps[i].T_st_dhw = (heatPumps[i].T_st_min_dhw + heatPumps[i].T_st_max_dhw) / 2;
				heatPumps[i].U_st_useful_dhw = heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_st_min_dhw);
			} else {
				heatPumps[i].HP_storage = 3; //3 = no storages;
				heatPumps[i].T_st_dhw = 273.15;
				heatPumps[i].T_st_min_dhw = 273.15;
				heatPumps[i].T_st_max_dhw = 273.15;
				heatPumps[i].U_st_useful_dhw = 0.0;
				heatPumps[i].losses_st_dhw = 0.0;
			}
		}else if(heatPumps[i].app_range == 5){
			heatPumps[i].HP_storage = 3; //3 = no storages;
			heatPumps[i].T_st = 273.15;
			heatPumps[i].T_st_min = 273.15;
			heatPumps[i].T_st_max = 273.15;
			heatPumps[i].U_st_useful = 0.0;
			heatPumps[i].losses_st = 0.0;
			heatPumps[i].T_st_dhw = 273.15;
			heatPumps[i].T_st_min_dhw = 273.15;
			heatPumps[i].T_st_max_dhw = 273.15;
			heatPumps[i].U_st_useful_dhw = 0.0;
			heatPumps[i].losses_st_dhw = 0.0;
		}

		//cout << "T_st_min of "<< heatPumps[i].ID << " after loop = "  << heatPumps[i].T_st_min << " = "<< heatPumps[i].T_st_min -273.15 << "\n";
		//cout << "T_st of "<< heatPumps[i].ID << " after loop = "  << heatPumps[i].T_st << " = "<< heatPumps[i].T_st -273.15 << "\n";
		//cout << "T_st_max of "<< heatPumps[i].ID << " after loop = "  << heatPumps[i].T_st_max << " = "<< heatPumps[i].T_st_max -273.15 << "\n";
		//cout << "T_h_step of "<< heatPumps[i].ID << " = "  << heatPumps[i].T_h_step << " = "<< heatPumps[i].T_h_step -273.15 << "\n";
		//cout << "T_st_min_dhw of "<< heatPumps[i].ID << " = "  << heatPumps[i].T_st_min_dhw << " = "<< heatPumps[i].T_st_min_dhw -273.15 << "\n";
		//cout << "T_st_dhw of "<< heatPumps[i].ID << " after loop = "  << heatPumps[i].T_st_dhw << " = "<< heatPumps[i].T_st_dhw -273.15 << "\n";
		//cout << "T_st_max_dhw of "<< heatPumps[i].ID << " = "  << heatPumps[i].T_st_max_dhw << " = "<< heatPumps[i].T_st_max_dhw -273.15 << "\n";
		//cout << "T_dhw_step of "<< heatPumps[i].ID << " = "  << heatPumps[i].T_dhw_step << " = "<< heatPumps[i].T_dhw_step -273.15 << "\n";
		//cout << "HP_storage of HP "<< heatPumps[i].ID << " = "  << heatPumps[i].HP_storage << "\n";

/*		heatPumps[i].timerLoadStFull = ceil(heatPumps[i].Vol_st * rho_w * cp_w *(heatPumps[i].T_st_max - heatPumps[i].T_st_min) / heatPumps[i].nominalPowerH /60) * 60; //max 1 h (because of predictive control [step +1]) //-> /60) * 60; for ceil //20 min //depends on volume storage = ceil(heatPumps[i].Vol_st * rho_w * cp_w *(heatPumps[i].T_st_max - heatPumps[i].T_st_min) / heatPumps[i].nominalPowerH)
		//if there is no load! -> is too little, if load occurs!
		if(heatPumps[i].timerLoadStFull > 3600){
			heatPumps[i].timerLoadStFull = 3600; //max 1 h (because of predictive control [step +1])
			cout << "ASSIGNEMENT: if(heatPumps[i].timerLoadStFull > 3600) -> heatPumps[i].timerLoadStFull = 3600; "<< "\n";
			//cout << "ASSIGNEMENT: heatPumps[i].timerLoadStFull = " << heatPumps[i].timerLoadStFull / 60 << "min > 60 min."<< "\n";
		}
		//min timer = 10 min?
		cout << "ASSIGNEMENT: heatPumps[i].timerLoadStFull "<< heatPumps[i].timerLoadStFull / 60 << " = min"  << "\n";*/
	}		//end for(int i=0; i < heatPumps.size(); i++){

}

void HpCore::calculate() {
	//!function to calculate T_soil,in, T_st, T_st,dhW, ...

	//cout << "HPC: vector size of T_s_out of linked bhes = " << vecT_soil_out.size() << "\n"; //test
	//for(vector<double>::iterator vit=vecT_soil_out.begin();vit!=vecT_soil_out.end();++vit) cout << *vit - 273.15 << " °C" << "\n"; //test

	//cout << "void HpCore::calculate()" << endl;

	//cout << "used dt_comm out of hps = " << dt_comm << " s" << "\n";
	//cout << "HpCore: Actual time step in HpCore::calculate(): " << t << " s"<<endl;

	//cout << "HpCore: Actual counterIter in HpCore at beginning of calculate = " << counterIter  << "\n";

	year = floor(t / (8760 * 3600));
	//cout << "year =  " << year << "\n"; //test

//test
/*	for (int i = 0; i < heatPumps.size(); i++) {
			//cout << "actYear = " << actYear << "\n"; //test
			//cout << "if((actYear == 1) || (actYear == endYear)){" << "\n"; //test

			fstream f;
			//cout << "actual year in if clause ............................. = " << actYear << "\n";
			//cout << "string fileName = ..." << "\n";//test
			string fileName;
			fileName = "hpm/results_" + heatPumps[i].ID + "check_year" 	+ ".txt";

			//cout << "Number of entries in textFile makeTXT = " << vecNameData.size() << "\n";//test
			//for(vector<string>::iterator vit=vecNameData.begin();vit!=vecNameData.end();++vit) cout << *vit << "\n"; //test
			//cout << "f.open(fileName.c_str(), ios::app);" << "\n";//test
			f.open(fileName.c_str(), ios::app); //if existing txt: f.open(fileName.c_str(), ios::app);
			//cout << "write Actual Year = in txt" << "\n"; //test
			//f << "#" << "check year of " << heatPumps[i].ID << " in year "	<< actYear << endl;
			//...
			//...
			////cout << "if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){" << "\n";//test
			//if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){
			//cout << "write in txt t, T_s_out, ... " << "\n";//test
			f << "#" << t << "	" << actYear << "\n";
			//cout << "Teeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeest ... "	<< "\n"; //test
			//cout << "vecTime.size() = " <<  vecTime.size() << "\n";		//test

			f.close();

		}*/

	//cout << "actYear = " << actYear << "\n";//test
	step = floor((t - year * 8760 * 3600) / 3600); //Rounds x downward, returning the largest integral value that is not greater than x.
	//cout << "actual step =  " << step << "\n";

//+++++++++++++++core / calculation rules+++++++++++++++

	////cout << "vecT_soil_out" << "\n";
	//for(vector<double>::iterator vit=vecT_soil_out.begin();vit!=vecT_soil_out.end();++vit) cout << *vit << "\n";

//calculation HP by HP
	for (unsigned int i = 0; i < heatPumps.size(); i++) {

		if(heatPumps[i].simulateHP == true){
			////cout << "actual HP:  " << "'"<< heatPumps[i].ID<< "'" << "\n";

			if(std::fmod(3600,dt_comm) != 0){
				//cout << "Test TEST modulus != 0 -> so Weighting is active (modulus = " << std::fmod(3600,dt_comm) << ")"  << " \n"; //test
				//Weighting for unusual time steps - Assignment of Q_HL_hp[t] -> Q_HL_hp[t=0] = Q_HL_hp[0]; Q_HL_hp[t=3760] = Q_HL_hp[t=3600] = Q_HL_hp[1]; ... ->
				if (heatPumps[i].weightMarker == true) { //test it -> check
					heatPumps[i].Q_HL_hp_time = heatPumps[i].Q_HL_hp_weight;
					heatPumps[i].Q_dhw_hp_time = heatPumps[i].Q_dhw_hp_weight;
					heatPumps[i].Q_COOL_hp_time = heatPumps[i].Q_COOL_hp_weight;
					//Q_DHW_hp_time = ...

					heatPumps[i].weightMarker = false;
				} else {

					if (floor((t + dt_comm) / 3600 - year * 8760) > step) { //test it -> check
						double n = 3600 + 3600 * step - (t - year * 8760 * 3600) ;
						heatPumps[i].Q_HL_hp_time = 1 / dt_comm * (heatPumps[i].vecPreQ_HL_hp[step] * n	+ heatPumps[i].vecPreQ_HL_hp[step + 1] * (dt_comm - n));
						heatPumps[i].Q_dhw_hp_time = 1 / dt_comm * (heatPumps[i].vecPreQ_dhw_hp[step] * n + heatPumps[i].vecPreQ_dhw_hp[step + 1] * (dt_comm - n));
						heatPumps[i].Q_COOL_hp_time = 1 / dt_comm * (heatPumps[i].vecPreQ_COOL_hp[step] * n + heatPumps[i].vecPreQ_COOL_hp[step + 1] * (dt_comm - n));

						heatPumps[i].Q_HL_hp_weight = heatPumps[i].Q_HL_hp_time;
						heatPumps[i].Q_dhw_hp_weight = heatPumps[i].Q_dhw_hp_time;
						heatPumps[i].Q_COOL_hp_weight = heatPumps[i].Q_COOL_hp_time;

						heatPumps[i].weightMarker = true;
					} else {
						heatPumps[i].Q_HL_hp_time = heatPumps[i].vecPreQ_HL_hp[step];
						heatPumps[i].Q_dhw_hp_time = heatPumps[i].vecPreQ_dhw_hp[step];
						heatPumps[i].Q_COOL_hp_time = heatPumps[i].vecPreQ_COOL_hp[step];
					}

				}
			}
			else{
				heatPumps[i].Q_HL_hp_time = heatPumps[i].vecPreQ_HL_hp[step];
				heatPumps[i].Q_dhw_hp_time = heatPumps[i].vecPreQ_dhw_hp[step];
				heatPumps[i].Q_COOL_hp_time = heatPumps[i].vecPreQ_COOL_hp[step];
			}

			//cout << "actual heatPumps[i].Q_HL_hp_time =  " << heatPumps[i].Q_HL_hp_time << "\n";
			//cout << "actual heatPumps[i].Q_dhw_hp_time =  "	<< heatPumps[i].Q_dhw_hp_time << "\n";
			//cout << "actual heatPumps[i].vecPreQ_HL_hp[step] =  " << heatPumps[i].vecPreQ_HL_hp[step] << "\n";
			//cout << "actual heatPumps[i].vecPreQ_dhw_hp[step] =  " << heatPumps[i].vecPreQ_dhw_hp[step] << "\n";

	//+++++++++++++++Assignment of BHEs to HP -> T_soil_out+++++++++++++++
			////cout << "T_soil_out of actual HP before taking the middle = " << heatPumps[i].T_soil_out << " K"<< "\n";
			double T_sum;
			//cout << "HPC: linked bhe size of actual HP = " << heatPumps[i].linked_bhe.size() << "\n";
			//for(vector<string>::iterator vit=heatPumps[i].linked_bhe.begin();vit!=heatPumps[i].linked_bhe.end();++vit) cout << *vit << "\n";
			////cout << "actual HP:  " << "'"<< heatPumps[i].ID<< "'" << "\n";
			//cout << "HPC: all linked bhe size = " << Phy.all_linked_bhe.size() << "\n";
			//for(vector<string>::iterator vit=Phy.all_linked_bhe.begin();vit!=Phy.all_linked_bhe.end();++vit) cout << *vit << "\n";

			for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
				//cout <<  "iterator j = " << j << "\n"; // test
				for (unsigned int k = 0; k < Phy.all_linked_bhe.size(); k++) {
					//cout <<  "iterator k = " << k << "\n"; // test
					if (heatPumps[i].linked_bhe[j] == Phy.all_linked_bhe[k]) {
						//cout << "HPC: T of " << heatPumps[i].linked_bhe[j] << " = " << vecT_soil_out[k] << " K with iterator j = " << j << " and k = " << k << "\n";
						T_sum = T_sum + vecT_soil_out[k];
						////cout << vecT_soil_out[k] << "\n";
						//cout << "T_sum = " << T_sum << "\n";
						break;
					}
				}
			}
			heatPumps[i].T_soil_out = T_sum / heatPumps[i].linked_bhe.size();
			//cout << "HPC: Middled T_soil_out of " << heatPumps[i].ID << " = " << heatPumps[i].T_soil_out - 273.15 << " °C"<< "\n"; //test

			T_sum = 0;

			//cout << "middle T_soil_out of actual HP = " << heatPumps[i].T_soil_out << " K"<< "\n"; //test

	//+++*temperature control T_soil_cell_min > T_soil_cell_crit? +++*
			//cout << "heatPumps[i].crit_soil before if clause= " << heatPumps[i].crit_soil << "\n"; // test
			if(ugModel == 3){
				for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
					//cout << "iterator j = " << j << "\n"; // test
					for (unsigned int k = 0; k < Phy.all_linked_bhe.size(); k++) {
						//cout << "iterator k = " << k << "\n"; // test
						if( (heatPumps[i].linked_bhe[j] == Phy.all_linked_bhe[k]) && (VecT_soil_cell_min[k] <= heatPumps[i].Tsoil_cellcrit)){
							//cout << "HPC: T_soilMin of " << heatPumps[i].linked_bhe[j] << " = "	<< VecT_soil_cell_min[k] << " K with iterator j = " << j << " and k = "	<< k << "\n"; //test
							//cout << "HPC: T_soil_cell_min (= " << VecT_soil_cell_min[k] << " K)" << " of bhe " << Phy.all_linked_bhe[k] << " is <= heatPumps[i].Tsoil_cellcrit (= " << heatPumps[i].Tsoil_cellcrit << " K)" << "\n"; //test
							heatPumps[i].crit_soil = true;
							heatPumps[i].HP_on = false;
							//cout << "heatPumps[i].crit_soil in if clause = " << heatPumps[i].crit_soil << "\n"; // test
							break;
						} else if( (heatPumps[i].linked_bhe[j] == Phy.all_linked_bhe[k]) && (VecT_soil_cell_min[k] > heatPumps[i].Tsoil_cellcrit)){
							heatPumps[i].crit_soil = false;
						}
						//cout << "heatPumps[i].crit_soil in if clause before break= " << heatPumps[i].crit_soil << "\n"; // test
						//cout << "heatPumps[i].crit_soil in if clause after break= " << heatPumps[i].crit_soil << "\n"; // test
					}
				}
			}
			else{
				heatPumps[i].crit_soil = false;
			}
			//heatPumps[i].crit_soil = false;





				//cout << "heatPumps[i].HP_on before HP control = " << heatPumps[i].HP_on << "\n"; //test
				//cout << "heatPumps[i].HP_mode before HP control = " << heatPumps[i].HP_mode << "\n"; //test

	//+++++++++++++++Control+++++++++++++++

			//+++ HPmodeControl
			if(heatPumps[i].app_range != 5){ // only passive cooling (because no active cooling implemented) -> no dhw & no heating!! //5=cooling

				modeControl(i);

				//***for validation test case!!!!
					//heatPumps[i].HP_on = true;
				//***for validation test case!!!!

				//cout << "HP_on of " << heatPumps[i].ID << " before interpolation = " << std::boolalpha << heatPumps[i].HP_on << "\n"; //test
				//cout << "HP_mode of " << heatPumps[i].ID << " before interpolation = " << heatPumps[i].HP_mode << "\n"; //test

				//blockingHP(i) here for speed up simulation time?? -> works, but is not good for code description! //if blocking times: maximal 3 times a day with maximal 2 hours at piece -> 6/24 = 1/4 of the complete calculating time (except control alternative)
/*				if( (heatPumps[i].blockingTime > 0){
					blockingHP(i); //ToDo: delete 'if( (heatPumps[i].blockingTime > 0){' in blockingHP(i)
				}
				if(heatPumps[i].operatingMode != 4){ // not bivalent alternative
					if(heatPumps[i].blockHP == false){
						interpolationHP(i);
						if(heatPumps[i].operatingMode == 0){ //0=monovalent
							controlMonovalent(i);
						}else if( (heatPumps[i].operatingMode == 1) || (heatPumps[i].operatingMode == 2) ){ //1=monoenergetic, 2=bivalent parallel
							controlParallel(i); //after interpolation because it depends on Q_h
						}else if(heatPumps[i].operatingMode == 3){ //3=bivalent teilparallel
							controlPartParallel(i); //after interpolation because it depends on Q_h}
						}
					}
					else{ //bivalent alternative
						heatPumps[i].HP_on = false;
					}

				}
				else{
					interpolationHP(i);
					controlAlternative(i);
				}*/

				//+++interpolationHP+++
				interpolationHP(i); //before all controls, because of Q_h & Q_dhw

				//cout << "Q_h after interpolationHP = " << heatPumps[i].Q_h << " of HP " << heatPumps[i].ID << "\n"; //test
				//cout << "Q_HL_hp_time after control = " << heatPumps[i].Q_HL_hp_time << "\n"; //test
				//cout << "Q_dhw after control = " << heatPumps[i].Q_dhw << "\n"; //test
				//cout << "Q_dhw_hp_time after interpolation = " << heatPumps[i].Q_dhw_hp_time << "\n"; //test


				//+++blocking+++
				blockingHP(i); //placed after interpolation and before controls, because control alternative needs Q_h, also if blockHP == true; parallel and part parallel needs Q_h if blockHP == false;
		/*

				cout << "HPC: heatPumps[i].vecBlockHP[step] = " << heatPumps[i].vecBlockHP[step] << "\n"; //test
				cout << "HPC: heatPumps[i].blockMarker = " << heatPumps[i].blockMarker << "\n"; //test
				cout << "HPC: heatPumps[i].blockCounter = " << heatPumps[i].blockCounter << "\n"; //test
				cout << "HPC: ceil(heatPumps[i].blockingTime / dt_comm) = " << ceil(heatPumps[i].blockingTime / dt_comm) << "\n"; //test
				cout << "HPC: heatPumps[i].blockHP = " << heatPumps[i].blockHP << "\n"; //test
				cout << "HPC: heatPumps[i].HP_on = " << heatPumps[i].HP_on << "\n"; //test
		*/

				//+++control strategies+++

				if(heatPumps[i].operatingMode == 0){ //0=monovalent
					controlMonovalent(i);
				}else if( (heatPumps[i].operatingMode == 1) || (heatPumps[i].operatingMode == 2) ){ //1=monoenergetic, 2=bivalent parallel
					controlParallel(i); //after interpolation because it depends on Q_h
				}else if(heatPumps[i].operatingMode == 3){ //3=bivalent teilparallel
					controlPartParallel(i); //after interpolation because it depends on Q_h}
				}else if(heatPumps[i].operatingMode == 4){ //4=bivalent alternativ
					controlAlternative(i);
				}
				//cout << "Q_hSecHeatGen after control = " << heatPumps[i].Q_hSecHeatGen << "\n"; //test
				//cout << "Q_dhwSecHeatGen after control = " << heatPumps[i].Q_dhwSecHeatGen << "\n"; //test

				//cout << "heatPumps[i].HP_on after HP control = " << heatPumps[i].HP_on << "\n"; //test
				//cout << "heatPumps[i].HP_mode after HP control = " << heatPumps[i].HP_mode << "\n"; //test
			}//end of if(heatPumps[i].app_range != 5){




	//+++++++++++++++COP, V_soil, T_soil in -> this has to be placed after interpolation and after controls! +++++++++++++++
			//after controls
			QsoilTsoilinCOP(i);

			//+++ cooling +++
			cooling(i);

			////cout << "rhoCp_soil = " << heatPumps[i].rhoCp_soil << "\n";//test
	//+++++++++++++++end of COP, V_soil, T_soil+++++++++++++++


	//+++++++++++++++difference between heating and dhw mode+++++++++++++++
			//this has to be placed after calculating COP, V_soil, T_soil and before storage balance
			if (heatPumps[i].HP_mode == 0) { //heating
				heatPumps[i].Q_dhw = 0;
				heatPumps[i].Q_dhwSecHeatGen = 0;

			} else if (heatPumps[i].HP_mode == 1) { //dhw
				heatPumps[i].Q_dhw = heatPumps[i].Q_h;
				heatPumps[i].Q_h = 0;
				heatPumps[i].Q_dhwSecHeatGen = heatPumps[i].Q_hSecHeatGen;
				heatPumps[i].Q_hSecHeatGen = 0;
			}

			//cout << "Q_hSecHeatGen after difference between heating and dhw mode = " << heatPumps[i].Q_hSecHeatGen << "\n"; //test
			//cout << "Q_dhwSecHeatGen after difference between heating and dhw mode = " << heatPumps[i].Q_dhwSecHeatGen << "\n"; //test

			//cout << "HPC: actual T_soil_in of " << heatPumps[i].ID << " = " << heatPumps[i].T_soil_in - 273.15 << " °C" << "\n";//test

			//cout << "HP_on of " << heatPumps[i].ID << " after Q_h interpolation before push back = " << std::boolalpha << heatPumps[i].HP_on << "\n";
			//cout << "HP_mode of " << heatPumps[i].ID << " after Q_h interpolation before push back = " << heatPumps[i].HP_mode << "\n";

	//+++++++++++++++calculate storage balance+++++++++++++++
			if (heatPumps[i].HP_storage != 3) { //3 = no storages
				storageBalance(i);
			}


	//+++++++++++++++energySecHeatGenH & energySecHeatGenDHW+++++++++++++++

			//energy second heat gen
			heatPumps[i].energySecHeatGenH = heatPumps[i].energySecHeatGenH + heatPumps[i].Q_hSecHeatGen * dt_comm; // [J]

			//energy second heat gen
			heatPumps[i].energySecHeatGenDHW = heatPumps[i].energySecHeatGenDHW + heatPumps[i].Q_dhwSecHeatGen * dt_comm; // [J]

	//+++++++++++++++ heatingLoadCoverage +++++++++++++++
			loadCoverage(i);

			if( (heatPumps[i].crit_soil == true) || (heatPumps[i].critInterpolation == true) ){
				heatPumps[i].timeCritSoil = heatPumps[i].timeCritSoil + dt_comm;
			}

			heatPumps[i].critInterpolation = false;
			heatPumps[i].HPmodeLastTimeStep = heatPumps[i].HP_mode; // necessary because of seperation of modeControl(i) and HPonControl(i)


		 }// end of if(heatPumps[i].simulateHP == true)


	} //end of HP[i] loop


	pushBackTXT();


	//cout << "Actual time step in calculate() before t = t + dt_comm; = " << t << " s"<<endl;
	//cout << "t = t + dt_comm" << "\n";//test
	t = t + dt_comm; //(internal = external step width!)
	//cout << "Actual time step in calculate() after t = t + dt_comm; = " << t << " s"<<endl;

	if( (ugModel == 3) || (ugModel == 5) ){ //5 = other iterative model; 3 = MPfluidGeTIS //-> this is only a dummy; if ModHPS is coupled to MPFluidGeTIS, Hps.cpp will not be used!
	counterIter++;
	}
	//cout << "HpCore: Actual counterIter in HP core after calculate = " << counterIter  << "\n";

} //end of function calculate

void HpCore::makeTXT() {
	//!function to make .txt files for every simulation year -> calculates yearly result parameter and stores in vectors

	//cout << "HpCore::makeTXT()" << "\n"; //test

	for (unsigned int i = 0; i < heatPumps.size(); i++) {

		if(heatPumps[i].simulateHP == true){
			//cout << "actYear = " << actYear << "\n"; //test
			//cout << "if((actYear == 1) || (actYear == endYear)){" << "\n"; //test

			fstream f;
			//cout << "actual year in if clause ............................. = " << actYear << "\n";
			//cout << "string fileName = ..." << "\n";//test
			string fileName;
			stringstream stringYear;
			stringYear << actYear;
			string stryear = stringYear.str();
			fileName = "hpm/results_" + heatPumps[i].ID + "_year_" + stryear + ".txt";
			string name = "hpm/results_" + heatPumps[i].ID + "_year_" + stryear;
			vecNameData.push_back(name);
			//cout << "Number of entries in textFile makeTXT = " << vecNameData.size() << "\n";//test
			//for(vector<string>::iterator vit=vecNameData.begin();vit!=vecNameData.end();++vit) cout << *vit << "\n"; //test
			//cout << "f.open(fileName.c_str(), ios::app);" << "\n";//test
			f.open(fileName.c_str(), ios::out); //if existing txt: f.open(fileName.c_str(), ios::app);
			//cout << "write Actual Year = in txt" << "\n"; //test
			f << "#" << "Results of heat pump " << heatPumps[i].ID << " in year "
					<< actYear << endl;
			f << "#" << "Name of heat pump: '" << heatPumps[i].HPname << "'" << endl;
			f << "#" << "Linked bhe to heat pump: " << endl;
			for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
				f << "#" << heatPumps[i].linked_bhe[j] << "\n";
			}
			f << "#" << "Vdot_soil of each BHE: " << endl;
			for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
				f << "#" << heatPumps[i].Vdot_soil_bhe_init << "\n";
			}
			f << "#" << "Simulated time = " << t / 3600 << " h" << endl;
			f << "#" << "Simulated end time in years = " << t_sim_end / 3600 / 8760 << " years" << endl;
			f << "#" << "Summed time of this year, when heat pump was shut down because of subsurface boundary condition  = " << heatPumps[i].timeCritSoil / 3600 << " h" << endl;
			f << "#" << "BlockingTime = " << heatPumps[i].blockingTime / 3600 << " h" << endl;
			f << "#" << "Second Heat generator on during blocking time = " << heatPumps[i].secHeatGenOnInBlockingTime << " (0 = false; 1 = true)" << endl;
			f << "#" << "operatingMode = " << heatPumps[i].operatingMode << " (0=monovalent, 1=monoenergetic, 2=bivalent parallel, 3=bivalent teilparallel, 4=bivalent alternativ)"<< endl;
			f << "#" << "application range = " << heatPumps[i].app_range << " (0=heating, 1=dhw, 2=heating+dhw, 3=heating+cooling, 4=heating+cooling+dhw, 5=cooling, 6=dhw+cooling)" << endl;
			f << "#" << "heating storage volume = " << heatPumps[i].Vol_st * 1000
					<< " l" << ", T_st_min = " << heatPumps[i].T_st_min - 273.15
					<< " °C" << ", T_st_max = " << heatPumps[i].T_st_max - 273.15
					<< " °C" << ", dhw storage volume = "
					<< heatPumps[i].Vol_st_dhw * 1000 << " l" << ", T_st_min_dhw = "
					<< heatPumps[i].T_st_min_dhw - 273.15 << " °C"
					<< ", T_st_max_dhw = " << heatPumps[i].T_st_max_dhw - 273.15
					<< " °C" << endl;
			//...
			//...
			f << "#" << " " << endl;
			//f << "#" << "resHP_on sum_vec = " << sum_vec << endl; //test
			//f << "#" << "heatPumps[j].P_el_soil_pump = " << heatPumps[j].P_el_soil_pump << endl; //test
			//cout << "f << # << middled annual energy demand of soil pump" << "\n";//test
			f << "#" << "Run time of heat pump in this year = "	<< heatPumps[i].t_run / 3600 << " h" << endl;
			f << "#" << "Middled heating load coverage in this year = "	<< heatPumps[i].HLC_year << " %" << endl;
			f << "#" << "Middled dhw load coverage in this year = " << heatPumps[i].DHWLC_year << " %" << endl;
			if ((heatPumps[i].app_range == 0) || (heatPumps[i].app_range == 3)) { //no dhw //++++heatPumps[i].app_range == 0 (heating), 3=heating+cooling++++
				f << "#" << "Middled load coverage (dhw + heating) in this year = "
						<< heatPumps[i].HLC_year << " %" << endl;
			}else if( (heatPumps[i].app_range == 1) || (heatPumps[i].app_range == 6) ){ //no heating	//++++heatPumps[i].app_range == 1 (dhw), 6=dhw+cooling++++
				f << "#" << "Middled load coverage (dhw + heating) in this year = "
						<< heatPumps[i].DHWLC_year << " %" << endl;
			}else if( (heatPumps[i].app_range == 2) || (heatPumps[i].app_range == 4) ){ //heating + dhw
				f << "#" << "Middled load coverage (dhw + heating) in this year = "
						<< (heatPumps[i].HLC_year + heatPumps[i].DHWLC_year) / 2
						<< " %" << endl;
			}
			f << "#" << "Middled coverage ratio heating of heat pump in this year = " << heatPumps[i].coverageRatioHeating << " %" << endl;
			f << "#" << "Middled coverage ratio dhw of heat pump in this year = " << heatPumps[i].coverageRatioDHW << " %" << endl;
			f << "#" << "Middled coverage ratio (heating + dhw) of heat pump in this year = " << heatPumps[i].coverageRatio << " %" << endl;
			f << "#" << "Middled coverage ratio cooling in this year = " << heatPumps[i].coveringRatioCooling << " %" << endl;


			//f << "#" << "Heating energy of heat pump in this year = " << E_h_year / 3600 / 1000 << " kWh/a" << endl;
			f << "#" << "Middled heating power of heat pump = " << heatPumps[i].Qdot_hMiddle << " W" << endl;
			//f << "#" << "heatPumps[i].count_h = " <<  heatPumps[i].count_h << endl; //test
			//f << "#" << "Dhw energy of heat pump in this year = " << E_dhw_year / 1000 / 3600 << " kWh/a" << endl;
			f << "#" << "Middled dhw power of heat pump = " << heatPumps[i].Qdot_dhwMiddle << " W" << endl;
			//f << "#" << "heatPumps[i].count_dhw = " <<  heatPumps[i].count_dhw << endl; //test
			double sum_HPon = 0;
			for (std::vector<bool>::iterator it = heatPumps[i].resHP_on.begin();it != heatPumps[i].resHP_on.begin() + (yearVecSize); ++it)sum_HPon += *it;
			double sum_Qsoil = 0;
			for (std::vector<double>::iterator it = heatPumps[i].resQ_soil.begin();it != heatPumps[i].resQ_soil.begin() + (yearVecSize); ++it)sum_Qsoil += *it;
			heatPumps[i].Qdot_soilMiddle = sum_Qsoil / sum_HPon;
			f << "#" << "Middled soil heat flux to heat pump = " << heatPumps[i].Qdot_soilMiddle << " W" << endl;
			//f << "#" << "CountHPon = " << (heatPumps[i].t_run * 3600 / dt_comm) << " = " << heatPumps[i].count_dhw + heatPumps[i].count_h  << endl; //test
			//f << "#" << "Energy of heating and dhw of heat pump in this year = " <<  E_year / 1000 / 3600 << " kWh/a" << endl;
			//f << "#" << "Electrical energy demand of heat pump in this year = " <<  E_el_year / 1000 / 3600 << " kWh/a" << endl;
			//f << "#" << "Test test P_el_test = " <<  P_el_test << " W" << endl; //test
			f << "#" << "Electrical energy demand of soil pump in this year = "
					<< heatPumps[i].t_run / 3600 * heatPumps[i].P_el_soil_pump / 1000 << " kWh/a" << endl;

			f << "#"
					<< "Seasonal Performance Factor (SPF_WP) in this year (only heat pump) = "
					<< heatPumps[i].JAZ_WP << endl; //JAZ_WP;
			f << "#"
					<< "Seasonal Performance Factor (SPF_WPA) in this year (heat pump + soil pump + second electric heat generator) = "
					<< heatPumps[i].JAZ_WPA << endl; //JAZ_WPA = E_H / (E_el_HP + E_el_soil_pump + HE_g);
			//...
			f << "#" << " " << endl;
			f << "#" << "t = time [s]" << endl;
			f << "#" << "T_s_out = brine temperature out of soil [°C]" << endl;
			f << "#" << "T_crit_s? = Is soil temperature of subsurface model < critical soil temperature? -> 0 = No -> HP = on; 1 = Yes -> HP = off; " << endl;
			f << "#" << "Q_HL = heating load of all buildings linked to heat pump [W]" << endl;
			f << "#" << "Q_dhw = domestic hot water (dhw) load of all buildings linked to heat pump [W]" << endl;
			f << "#" << "HP_on = Is heat pump on? -> 0 = off; 1 = on" << endl;
			f << "#" << "HP_mode = Mode of heat pump -> 0 = heating; 1 = dhw" << endl;
			f << "#" << "Q_h = heat pump heating power depending on T_s_out [W]" << endl;
			f << "#" << "hLC(t) = heating load coverage of heat pump [%]" << endl;
			f << "#" << "Q_dhw = heat pump dhw power depending on T_s_out [W]" << endl;
			f << "#" << "dhwLC(t) = dhw load coverage of heat pump [%]" << endl;
			f << "#" << "P_el = heat pump electric power depending on T_s_out [W]" << endl;
			f << "#" << "COP* = ratio of heating power and electric power of heat pump in actual mode and time step" << endl;
			f << "#" << "T_st = heating storage temperature in [°C]" << endl;
			f << "#" << "T_st_dhw = domestic hot water (dhw) storage temperature in [°C]" << endl;
			f << "#" << "T_s_in = brine inlet temperature -> into soil [°C]" << endl;
			f << "#" << "dT = temperature difference (T_S_out - T_s_in)  [K]" << endl;
			f << "#" << "hBlock? = Is in this time step a blocking hour of HP? -> 0 = No; 1 = Yes -> HP = off; " << endl;
			//...
			f << "#" << " " << endl;
			////cout << "if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){" << "\n";//test
			//if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){
			//cout << "write in txt t, T_s_out, ... " << "\n";//test
			f << "#" << "t:" << "	" << "T_s_out:" << "	" << "T_crit_s?" << "	"
					<< "Q_HL:" << "	" << "Q_dhw:" << "	" << "HP_on:" << "	" << "HP_mode:" << "	"
					<< "Q_h:" << "	" << "hLC(t):" << "	" << "Q_dhw:" << "	" << "dhwLC(t):" << "	"
					<< "P_el:" << "	" << "COP*:" << "	"
					<< "T_st:" << "	" << "T_st_dhw:" << "	"
					<< "T_s_in:" << "	" << "dT:" << "	" << "hBlock?" << "\n";
			//cout << "Teeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeest ... "	<< "\n"; //test
			//cout << "vecTime.size() = " <<  vecTime.size() << "\n";		//test
			for (unsigned int j = 0; j < vecTime.size(); j++) {
				//cout << "write vectors in vecTime, ... " << "\n";//test
				f << setprecision(10) << vecTime[j] << "	"
						<< setprecision(2) << heatPumps[i].resT_soil_out[j] - 273.15 << "	"
						<< heatPumps[i].resVecCrit_soil[j] << "	"
						<< setprecision(5) << heatPumps[i].vecQ_HL_hp_time[j] << "	"
						<< setprecision(5) << heatPumps[i].vecQ_dhw_hp_time[j] << "	"
						<< heatPumps[i].resHP_on[j] << "	"
						<< heatPumps[i].resHP_mode[j] << "	"
						<< setprecision(6) << heatPumps[i].resQ_h[j] << "	"
						<< heatPumps[i].vecHL_Coverage[j] << "	"
						<< setprecision(6) << heatPumps[i].resQ_dhw[j] << "	"
						<< heatPumps[i].vecDHW_Coverage[j] << "	"
						<< setprecision(6) << heatPumps[i].resP_el[j] << "	"
						<< setprecision(5) << heatPumps[i].vecCOP[j] << "	"
						<< setprecision(3) << heatPumps[i].vecT_st[j] - 273.15 << "	"
						<< setprecision(3) << heatPumps[i].vecT_st_dhw[j] - 273.15 << "	"
						<< setprecision(2) << heatPumps[i].resT_soil_in[j] - 273.15 << "	"
						<< setprecision(2) << heatPumps[i].resT_soil_out[j] - heatPumps[i].resT_soil_in[j] << "	"
						<< heatPumps[i].resVecBlockHP[j] << "\n";
			}
			f.close();
		} //end of if(heatPumps[i].simulateHP == true)
	}
}

void HpCore::resetValues() {
	//!function to reset the values of all variables for n time steps (n = number of iterations) when iter command = false -> sets current = valid;


	//cout << "resetValues() in HpCore" << "\n"; //test


	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			heatPumps[i].weightMarker = heatPumps[i].ValidWeightMarker;
			heatPumps[i].T_soil_out = heatPumps[i].ValidT_soil_out;
			heatPumps[i].U_st_useful = heatPumps[i].ValidU_st_useful;
			heatPumps[i].T_st = heatPumps[i].ValidT_st;
			heatPumps[i].U_st_useful_dhw = heatPumps[i].ValidU_st_useful_dhw;
			heatPumps[i].T_st_dhw = heatPumps[i].ValidT_st_dhw;
			heatPumps[i].Q_h = heatPumps[i].ValidQ_h;
			heatPumps[i].Q_dhw = heatPumps[i].ValidQ_dhw;
			heatPumps[i].P_el = heatPumps[i].ValidP_el;
			heatPumps[i].COP = heatPumps[i].ValidCOP;
			heatPumps[i].T_soil_in = heatPumps[i].ValidT_soil_in;
			heatPumps[i].Vdot_soil_bhe = heatPumps[i].ValidVdot_soil_bhe;
			heatPumps[i].HP_mode = heatPumps[i].ValidHP_mode;
			heatPumps[i].HP_on = heatPumps[i].ValidHP_on;
			heatPumps[i].timeCritSoil = heatPumps[i].ValidTimeCritSoil;
			//weighting because of time steps which are not a part of 3600
			heatPumps[i].Q_HL_hp_weight = heatPumps[i].ValidQ_HL_hp_weight;
			heatPumps[i].Q_HL_hp_time = heatPumps[i].ValidQ_HL_hp_time;
			heatPumps[i].Q_dhw_hp_weight = heatPumps[i].ValidQ_dhw_hp_weight;
			heatPumps[i].Q_dhw_hp_time = heatPumps[i].ValidQ_dhw_hp_time;
			//+++blocking time+++
			heatPumps[i].blockCounter = heatPumps[i].ValidBlockCounter;
			heatPumps[i].blockHP = heatPumps[i].ValidBlockHP;
			heatPumps[i].blockMarker = heatPumps[i].ValidBlockMarker;
			heatPumps[i].fullLoad = heatPumps[i].ValidFullLoad;
			//+++bivalent+++
			heatPumps[i].secHeatGen_on = heatPumps[i].ValidSecHeatGen_on;
			heatPumps[i].Q_hSecHeatGen = heatPumps[i].ValidQ_hSecHeatGen;
			heatPumps[i].Q_dhwSecHeatGen = heatPumps[i].ValidQ_dhwSecHeatGen;
			heatPumps[i].energySecHeatGenH = heatPumps[i].ValidEnergySecHeatGenH;
			heatPumps[i].energySecHeatGenDHW = heatPumps[i].ValidEnergySecHeatGenDHW;
			//+++cooling+++
			heatPumps[i].energyCooling = heatPumps[i].ValidEnergyCooling;
			heatPumps[i].Vdot_soilBHEcooling = heatPumps[i].vaildVdot_soilBHEcooling;
		} // end of if(heatPumps[i].simulateHP == true)

	}

	t = t_valid;

	//delete wrong vector entries
	//cout << "vecTime before erase" << "\n"; //test
	//for(vector<double>::iterator vit=vecTime.begin();vit!=vecTime.end();++vit) cout << *vit << "\n"; //test
	vecTime.erase(vecTime.end() - counterIter, vecTime.end()); //test it -> works!!
	//cout << "vecTime after erase" << "\n";//test
	//for(vector<double>::iterator vit=vecTime.begin();vit!=vecTime.end();++vit) cout << *vit << "\n"; //test

	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			heatPumps[i].vecQ_HL_hp_time.erase(heatPumps[i].vecQ_HL_hp_time.end() - counterIter, heatPumps[i].vecQ_HL_hp_time.end());
			heatPumps[i].vecQ_dhw_hp_time.erase(heatPumps[i].vecQ_dhw_hp_time.end() - counterIter, heatPumps[i].vecQ_dhw_hp_time.end());
			heatPumps[i].resT_soil_out.erase(heatPumps[i].resT_soil_out.end() - counterIter, heatPumps[i].resT_soil_out.end());
			heatPumps[i].resT_soil_in.erase(heatPumps[i].resT_soil_in.end() - counterIter, heatPumps[i].resT_soil_in.end());
			heatPumps[i].resVecCrit_soil.erase(heatPumps[i].resVecCrit_soil.end() - counterIter, heatPumps[i].resVecCrit_soil.end());
			heatPumps[i].vecT_st.erase(heatPumps[i].vecT_st.end() - counterIter, heatPumps[i].vecT_st.end());
			heatPumps[i].vecT_st_dhw.erase(heatPumps[i].vecT_st_dhw.end() - counterIter, heatPumps[i].vecT_st_dhw.end());
			heatPumps[i].vecHL_Coverage.erase(heatPumps[i].vecHL_Coverage.end() - counterIter, heatPumps[i].vecHL_Coverage.end());
			heatPumps[i].vecDHW_Coverage.erase(heatPumps[i].vecDHW_Coverage.end() - counterIter, heatPumps[i].vecDHW_Coverage.end());
			heatPumps[i].resQ_h.erase(heatPumps[i].resQ_h.end() - counterIter, heatPumps[i].resQ_h.end());
			heatPumps[i].resQ_dhw.erase(heatPumps[i].resQ_dhw.end() - counterIter, heatPumps[i].resQ_dhw.end());
			heatPumps[i].resP_el.erase(heatPumps[i].resP_el.end() - counterIter, heatPumps[i].resP_el.end());
			heatPumps[i].resQ_soil.erase(heatPumps[i].resQ_soil.end() - counterIter, heatPumps[i].resQ_soil.end());
			heatPumps[i].vecCOP.erase(heatPumps[i].vecCOP.end() - counterIter, heatPumps[i].vecCOP.end());
			heatPumps[i].resHP_mode.erase(heatPumps[i].resHP_mode.end() - counterIter, heatPumps[i].resHP_mode.end());
			heatPumps[i].resHP_on.erase(heatPumps[i].resHP_on.end() - counterIter, heatPumps[i].resHP_on.end());
		} // end of if(heatPumps[i].simulateHP == true){
	}
	counterIter = 0;
}

void HpCore::updateValues() {
	//!function to update the values of all variables when iter command = true -> sets valid = current;

	//cout << "HpCore::updateValues() in HpCore" << "\n"; //test

	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			//+++++valid = current+++++
			//push_back for one year -> every year print in txt and clear vector
			heatPumps[i].ValidWeightMarker = heatPumps[i].weightMarker;
			heatPumps[i].ValidT_soil_out = heatPumps[i].T_soil_out;
			heatPumps[i].ValidU_st_useful = heatPumps[i].U_st_useful;
			heatPumps[i].ValidT_st = heatPumps[i].T_st;
			heatPumps[i].ValidU_st_useful_dhw = heatPumps[i].U_st_useful_dhw;
			heatPumps[i].ValidT_st_dhw = heatPumps[i].T_st_dhw;
			heatPumps[i].ValidQ_h = heatPumps[i].Q_h;
			heatPumps[i].ValidQ_dhw = heatPumps[i].Q_dhw;
			heatPumps[i].ValidP_el = heatPumps[i].P_el;
			heatPumps[i].ValidCOP = heatPumps[i].COP;
			heatPumps[i].ValidT_soil_in = heatPumps[i].T_soil_in;
			heatPumps[i].ValidVdot_soil_bhe = heatPumps[i].Vdot_soil_bhe;
			heatPumps[i].ValidHP_mode = heatPumps[i].HP_mode;
			heatPumps[i].ValidHP_on = heatPumps[i].HP_on;
			heatPumps[i].ValidTimeCritSoil = heatPumps[i].timeCritSoil;
			//weighting because of time steps which are not a part of 3600
			heatPumps[i].ValidQ_HL_hp_weight = heatPumps[i].Q_HL_hp_weight;
			heatPumps[i].ValidQ_HL_hp_time = heatPumps[i].Q_HL_hp_time;
			heatPumps[i].ValidQ_dhw_hp_weight = heatPumps[i].Q_dhw_hp_weight;
			heatPumps[i].ValidQ_dhw_hp_time = heatPumps[i].Q_dhw_hp_time;
			//+++ blocking time+++
			heatPumps[i].ValidBlockCounter = heatPumps[i].blockCounter;
			heatPumps[i].ValidBlockHP = heatPumps[i].blockHP;
			heatPumps[i].ValidBlockMarker = heatPumps[i].blockMarker;
			heatPumps[i].ValidFullLoad = heatPumps[i].fullLoad;
			//+++bivalent+++
			heatPumps[i].ValidSecHeatGen_on = heatPumps[i].secHeatGen_on;
			heatPumps[i].ValidQ_hSecHeatGen = heatPumps[i].Q_hSecHeatGen;
			heatPumps[i].ValidQ_dhwSecHeatGen = heatPumps[i].Q_dhwSecHeatGen;
			heatPumps[i].ValidEnergySecHeatGenH = heatPumps[i].energySecHeatGenH;
			heatPumps[i].ValidEnergySecHeatGenDHW = heatPumps[i].energySecHeatGenDHW;
			//+++cooling+++
			heatPumps[i].ValidEnergyCooling = heatPumps[i].energyCooling;
			heatPumps[i].vaildVdot_soilBHEcooling = heatPumps[i].Vdot_soilBHEcooling;

		}//end of if(heatPumps[i].simulateHP == true)
	}

	t_valid = t;

	//old
	/*

	 //pushBackTXT();
	 cout << "Actual time step before t = t + dt_comm * counterIter; = " << t << " s"<<endl;
	 //cout << "t = t + dt_comm * counterIter" << "\n";//test
	 t = t + dt_comm * counterIter; //(internal = external step width!)
	 cout << "Actual time step after t = t + dt_comm * counterIter; = " << t << " s"<<endl;
	 //cout << "Actual time step in updateValues() before t_valid = t; = " << t << " s"<<endl; //test
	 t_valid = t;
	 //cout << "Actual time step in updateValues() after t_valid = t; = " << t << " s"<<endl; //test
	 */

	/*	for (int i = 0; i < heatPumps.size(); i++) { //test -> not necessary -> only for testing
	 if (t > 0) {
	 heatPumps[i].Q_soilMPF = heatPumps[i].Vdot_soil * heatPumps[i].rhoCp_soil * (heatPumps[i].T_soil_out - heatPumps[i].resT_soil_in[heatPumps[i].resT_soil_in.size() - 1]); //not working for t=0!! //heatPumps[i].resT_soil_in[heatPumps[i].resT_soil_in.size() - 1] = T_soil in of last Timestep -> only later push_back
	 cout << "Q_soil HP = " << heatPumps[i].Q_soil <<endl; //test
	 cout << "heatPumps[i].Q_soilMPF = " << heatPumps[i].Q_soilMPF << endl; //test
	 if (heatPumps[i].Q_soil > heatPumps[i].Q_soilMPF) { //every time step the same -> out of definition -> not necessary
	 countErr++;
	 cout << "heatPumps[i].Q_soil >= heatPumps[i].Q_soilMPF" <<endl; //test

	 }
	 }
	 }*/


	if (vecTime.size() >= yearVecSize) { //test -> works: if (vecTime.size() >= yearVecSize) {
		//cout << "actYear = " << actYear << "; vecTime.size() = " << vecTime.size() << " with last value = " << vecTime[vecTime.size() - 1] << "pushBackYearTXT()" << "\n"; //test
		pushBackYearTXT();
	}

//+++*begin Debugging+++*
/*	if (vecTime.size() >= yearVecSize) {
		debugTXT();
	}*/
//+++*end Debugging+++*


	if (((actYear == 1) && (vecTime.size() >= yearVecSize))	|| ((actYear == endYear) && (vecTime.size() >= yearVecSize))) { //1. year and last year -> checked if (((actYear == 1) && (vecTime.size() >= yearVecSize))	|| ((actYear == endYear) && (vecTime.size() >= yearVecSize))) { //if( ( (actYear == 1) && (t >= (8760 * 3600) ) ) || ( (actYear == endYear) && (t >= t_sim_end) ) ) {
		//cout << "actual time = "<< t << " endYear = " << endYear << " actYear = " << actYear << " vecTime.size() = " << vecTime.size() << " with last value = " << vecTime[vecTime.size() - 1] << "makeTXT()" << "\n"; //test
		makeTXT();
	}

	//if iteration over a year and there are some time steps too much -> take the more time steps in vector for next step
	//cout << "actYear before if (vecTime.size() >= yearVecSize){saveOverhang()} = " << actYear << t <<endl; //test
	if (vecTime.size() >= yearVecSize) { //vector of time is bigger than 8760 * 3600 / dt_comm, because of non-fitting time steps
		//cout << "actYear = " << actYear << " vecTime.size() = " << vecTime.size() << " with last value = " << vecTime[vecTime.size() - 1] << "saveOverhang()" << "\n"; //test
		saveOverhang();
		//cout << "saveOverhang() completed; Teeeeeeeeeeeest" << "\n"; //test
		//cout << "actYear++; Teeeeeeeeeeeeeeeeeeeeeeeest" << "\n"; //test
		actYear++;
		//cout << "actYear after actYear++ = " << actYear << "\n"; //test
	}


	//ToDo: dont need it any more?? What happens if no iteration? -> because of saveOverhang()?
/*	if (vecTime.size() >= yearVecSize) { //vecTime.size() >= 8760 * 3600 / dt_comm
		clearVectors();
	}*/

	//set back counter Iter
	counterIter = 0;
}


//+++++++++++++++modeControl+++++++++++++++
void HpCore::modeControl(int i) {
	//!function to calculate the current mode (heating or dhw) of HP instance

	//++++heatPumps[i].app_range == 0 (heating) || 3=heating+cooling++++
	if( (heatPumps[i].app_range == 0) ||  (heatPumps[i].app_range == 3)){
		//cout << "(heatPumps[i].app_range == 0)" << "\n"; //test
		heatPumps[i].HP_mode = 0; //heating
	}
	else if( (heatPumps[i].app_range == 1) ||  (heatPumps[i].app_range == 6)){ //++++heatPumps[i].app_range == 1 (dhw), 6=dhw+cooling++++
		//cout << "(heatPumps[i].app_range == 1)" << "\n"; //test
		heatPumps[i].HP_mode = 1; //dhw
	}
	else if( (heatPumps[i].app_range == 2) ||  (heatPumps[i].app_range == 4)){  	//+++*heatPumps[i].app_range == 2 (heating + dhw)+++*
		//+++*dhw first till heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw || (heatPumps[i].Q_HL_hp[step] > 0) && (heatPumps[i].T_st <= heatPumps[i].T_st_min))
		//+++*(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)+++*
		if (heatPumps[i].HP_storage == 0) { //if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){ //0 = heating and dhw storages
			//cout << "(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)" << "\n"; //test
			if(heatPumps[i].fullLoad == true){ //ToDo: if HpCore::HPControlLoadStorageFull(int i) is optimized, then fullLoad can be also in last hour of the year -> check if(step +1 < 8760){ for (heatPumps[i].vecPreQ_HL_hp[step +1] == 0)
				//ToDo: if(step +1 < 8760){} //not last time step
				//if(step +1 < 8760){
				//	if((heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] == 0)){ //summer set back
				//		heatPumps[i].HP_mode = 1; //dhw
				//}else if((heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[0] == 0)){ //summer set back{ //last time step
				//
							//		heatPumps[i].HP_mode = 1; //dhw
				//...
				//}
				if((heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] == 0)){ //summer set back
					heatPumps[i].HP_mode = 1; //dhw
				}else if(heatPumps[i].T_st_dhw < heatPumps[i].T_st_max_dhw - heatPumps[i].hysteresis){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep //here '- heatPumps[i].hysteresis' makes sense
					heatPumps[i].HP_mode = 1; //dhw
				}else if(heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw){
					heatPumps[i].HP_mode = 0; //heating
				}else if( (heatPumps[i].Q_HL_hp_time > 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] == 0) && (heatPumps[i].T_st < heatPumps[i].T_st_min + heatPumps[i].hysteresis) ){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep
					heatPumps[i].HP_mode = 0; //heating
				}else if( (heatPumps[i].Q_HL_hp_time > 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] > 0) && (heatPumps[i].T_st < heatPumps[i].T_st_max) ){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep
					heatPumps[i].HP_mode = 0; //heating
				}else if( (heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] > 0) && (heatPumps[i].T_st < heatPumps[i].T_st_max) ){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep
					heatPumps[i].HP_mode = 0; //heating
				}else if(heatPumps[i].T_st >= heatPumps[i].T_st_max){
					heatPumps[i].HP_mode = 1; //dhw
				}
			}//no fullLoad
			else if (heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw + heatPumps[i].hysteresis) { //T_st_min_control_dhw //+++*temperature control T_st_dhw <= T_st_min_control_dhw+++*
				//cout << "heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw" << "\n"; //test
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			} else if(heatPumps[i].Q_HL_hp_time > 0){
				if(heatPumps[i].T_st <= heatPumps[i].T_st_min + heatPumps[i].hysteresis) {
					//+++*temperature control T_st <= T_st_min+++*
					//cout << "heatPumps[i].T_st <= heatPumps[i].T_st_min" << "\n"; //test
					heatPumps[i].HP_mode = 0; //heating
					//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
				}else if(heatPumps[i].T_st >= heatPumps[i].T_st_max){ //here no heatPumps[i].HP_on = false; -> direct after T_st >= T_st,max -> dhw
					heatPumps[i].HP_mode = 1; //dhw
				}
			} //heatPumps[i].Q_HL_hp_time = 0
			else{
				heatPumps[i].HP_mode = 1; //dhw
			}

		} else if (heatPumps[i].HP_storage == 1) { //+++*(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw == 0)+++* //1 = only heating storage
			//cout << "(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw == 0)" << "\n"; //test
			if (heatPumps[i].Q_dhw_hp_time > 0) {
				//cout << "heatPumps[i].Q_dhw_hp[step] > 0" << "\n"; //test
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			} else {
				heatPumps[i].HP_mode = 0; //heating
				//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
			}
		} else if (heatPumps[i].HP_storage == 2) { //2 = only dhw storage //+++*(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw > 0) -> dhw first if T_dhw < T_min_dhw+++*
			//cout << "(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw > 0)" << "\n"; //test
			if (heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw + heatPumps[i].hysteresis) { //T_st_min_control
				heatPumps[i].HP_mode = 1; //dhw
			}
			else if (heatPumps[i].Q_HL_hp_time > 0) {
				//cout << "heatPumps[i].Q_dhw_hp[step] > 0" << "\n"; //test
				heatPumps[i].HP_mode = 0; //heating
				//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
			} else { //heatPumps[i].Q_HL_hp_time == 0
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			}
			//here fullLoad too? -> no not necessary

		} else if (heatPumps[i].HP_storage == 3) { //+++*(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw == 0)+++* //3 = no storages
			//cout << "(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw == 0)" << "\n"; //test
			if (heatPumps[i].Q_dhw_hp_time > 0) {
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			} else {
				heatPumps[i].HP_mode = 0;					//heating
				//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
			}
		}
	}

/*	else if( (heatPumps[i].app_range == 2) ||  (heatPumps[i].app_range == 4)){  	//+++*heatPumps[i].app_range == 2 (heating + dhw)+++*
		//+++*dhw first till heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw || (heatPumps[i].Q_HL_hp[step] > 0) && (heatPumps[i].T_st <= heatPumps[i].T_st_min))
		//+++*(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)+++*
		if (heatPumps[i].HP_storage == 0) { //if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){ //0 = heating and dhw storages
			//cout << "(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)" << "\n"; //test
			if(heatPumps[i].fullLoad == true){
				if((heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] == 0)){ //summer set back
					heatPumps[i].HP_mode = 1; //dhw
				}else if(heatPumps[i].T_st_dhw < heatPumps[i].T_st_max_dhw - heatPumps[i].hysteresis){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep
					heatPumps[i].HP_mode = 1; //dhw
				}else if(heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw){
					heatPumps[i].HP_mode = 0; //heating
				}else if( (heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] > 0) && (heatPumps[i].T_st < heatPumps[i].T_st_max) ){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep
					heatPumps[i].HP_mode = 0; //heating
				}else if(heatPumps[i].T_st >= heatPumps[i].T_st_max){
					heatPumps[i].HP_mode = 1; //dhw
				}
			}//no fullLoad
			else if (heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw + heatPumps[i].hysteresis) { //T_st_min_control_dhw //+++*temperature control T_st_dhw <= T_st_min_control_dhw+++*
				//cout << "heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw" << "\n"; //test
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			} else if(heatPumps[i].Q_HL_hp_time > 0){
				if(heatPumps[i].T_st <= heatPumps[i].T_st_min + heatPumps[i].hysteresis) {
					//+++*temperature control T_st <= T_st_min+++*
					//cout << "heatPumps[i].T_st <= heatPumps[i].T_st_min" << "\n"; //test
					heatPumps[i].HP_mode = 0; //heating
					//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
				}else if ( (heatPumps[i].T_st >= heatPumps[i].T_st_max) && (heatPumps[i].T_st_dhw < heatPumps[i].T_st_max_dhw) ){ //here no heatPumps[i].HP_on = false; -> direct after T_st >= T_st,max -> dhw
					heatPumps[i].HP_mode = 1; //dhw

				}else if(heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw){
					heatPumps[i].HP_mode = 0; //heating
				}
			} //heatPumps[i].Q_HL_hp_time = 0
			else{
				heatPumps[i].HP_mode = 1; //dhw
			}

		} else if (heatPumps[i].HP_storage == 1) { //+++*(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw == 0)+++* //1 = only heating storage
			//cout << "(heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw == 0)" << "\n"; //test
			if (heatPumps[i].Q_dhw_hp_time > 0) {
				//cout << "heatPumps[i].Q_dhw_hp[step] > 0" << "\n"; //test
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			} else {
				heatPumps[i].HP_mode = 0; //heating
				//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
			}
		} else if (heatPumps[i].HP_storage == 2) { //2 = only dhw storage //+++*(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw > 0) -> dhw first if T_dhw < T_min_dhw+++*
			//cout << "(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw > 0)" << "\n"; //test
			if (heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw + heatPumps[i].hysteresis) { //T_st_min_control
				heatPumps[i].HP_mode = 1; //dhw
			}
			else if (heatPumps[i].Q_HL_hp_time > 0) {
				//cout << "heatPumps[i].Q_dhw_hp[step] > 0" << "\n"; //test
				heatPumps[i].HP_mode = 0; //heating
				//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
			} else { //heatPumps[i].Q_HL_hp_time == 0
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			}
		} else if (heatPumps[i].HP_storage == 3) { //+++*(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw == 0)+++* //3 = no storages
			//cout << "(heatPumps[i].Vol_st == 0) && (heatPumps[i].Vol_st_dhw == 0)" << "\n"; //test
			if (heatPumps[i].Q_dhw_hp_time > 0) {
				heatPumps[i].HP_mode = 1; //dhw
				//cout << "heatPumps[i].HP_mode = 1" << "\n"; //test
			} else {
				heatPumps[i].HP_mode = 0;					//heating
				//cout << "heatPumps[i].HP_mode = 0" << "\n"; //test
			}
		}
	}*/


//+++++++++++++++end of modeControl+++++++++++++++
}

//+++++++++++++++blocking time+++++++++++++++
void HpCore::blockingHP(int i) {
	//!function to check if HP is blocked because of blocking times (sets blockHP = true / false, ...)

	if( (heatPumps[i].blockingTime > 0) && (heatPumps[i].vecBlockHP[step] == true) && (heatPumps[i].blockCounter == 0) ){
			heatPumps[i].blockMarker = true;
	}else if( (heatPumps[i].blockingTime > 0) && (heatPumps[i].vecBlockHP[step] == false) && (heatPumps[i].blockMarker == false) ){
			//cout << "HPC: if( (heatPumps[i].vecBlockHP[step] == false) && (heatPumps[i].blockMarker == false) ) " << "\n"; //test
			heatPumps[i].blockCounter = 0;
			//cout << "heatPumps[i].blockCounter = 0; " << "\n"; //test
	}
	if( (heatPumps[i].blockMarker == true) && (heatPumps[i].blockCounter < ceil(heatPumps[i].blockingTime / dt_comm)) ){
		//cout << "HPC: heatPumps[i].blockCounter before if(heatPumps[i].blockCounter < ceil(heatPumps[i].blockingTime / dt_comm) ) = " << heatPumps[i].blockCounter << "\n"; //test
		heatPumps[i].blockHP = true;
		heatPumps[i].blockCounter++;
		//heatPumps[i].HP_on = false;
	}else if( (heatPumps[i].blockMarker == true) && (heatPumps[i].blockCounter == ceil(heatPumps[i].blockingTime / dt_comm)) ){
		heatPumps[i].blockHP = false;
		heatPumps[i].blockMarker = false;
		//cout << "HPC: Test Test heatPumps[i].blockHP = false = " << heatPumps[i].blockHP << "\n"; //test
	}
}

//+++++++++++++++controlMonovalent+++++++++++++++
void HpCore::controlMonovalent(int i) {
	//!control function for monovalent case

	if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
		if(heatPumps[i].blockingTime > 0){
			if(heatPumps[i].blockHP == false){
				HPControlLoadStorageFull(i);
			}else{
				heatPumps[i].HP_on = false;
			}
		}else{
			HPonControl(i);
		}
	}else{
		heatPumps[i].HP_on = false;
	}
}


//+++++++++++++++controlAlternative+++++++++++++++
void HpCore::controlAlternative(int i) {
	//!control function for bivalent alternative case

	if(heatPumps[i].HP_mode == 0){ //heating
		if( (heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) * 1.1 >= heatPumps[i].Q_h){ //1.1 -> second heat generator is on earlier (10 % Extra charge) -> storages should be loaded!!
			//only second heat generator on
			if(heatPumps[i].blockingTime > 0){
				if(heatPumps[i].secHeatGenOnInBlockingTime == true){
					secHeatGenOnControl(i); //only for HP
				}else{
					if(heatPumps[i].blockHP == false){
						secHeatGenControlLoadStorageFull(i); //only for HP
					}else{
						heatPumps[i].secHeatGen_on = false;
					}
				}
			}else{
				secHeatGenOnControl(i);
			}
			if(heatPumps[i].secHeatGen_on == true){
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;

			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}
		else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == true) ){
			//only second heat generator on
			secHeatGenOnControl(i); //second heat generator on, if HP would be on too
			if(heatPumps[i].secHeatGen_on == true){ //only if HP would be on -> then secHeatGen is on too
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == false) ){
			heatPumps[i].Q_hSecHeatGen = 0;
			heatPumps[i].HP_on = false;
		}else if(heatPumps[i].blockHP == false){
			//only HP on
			heatPumps[i].Q_hSecHeatGen = 0;
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
				//Assumption: second heat generator is not switched on, if heatPumps[i].crit_soil == true) || (heatPumps[i].critInterpolation == true); only switched on because of capacities

			}
		}

	}else if(heatPumps[i].HP_mode == 1){ //dhw
		if( (heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) * 1.1 >= heatPumps[i].Q_dhw){
			//only second heat generator on
			//storages should be loaded!!
			if(heatPumps[i].blockingTime > 0){
				if(heatPumps[i].secHeatGenOnInBlockingTime == true){
					secHeatGenOnControl(i);
				}else{
					if(heatPumps[i].blockHP == false){
						secHeatGenControlLoadStorageFull(i);
					}else{
						heatPumps[i].secHeatGen_on = false;
					}
				}
			}else{
				secHeatGenOnControl(i);
			}
			if(heatPumps[i].secHeatGen_on == true){
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;

			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}
		else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == true) ){
			//only second heat generator on
			secHeatGenOnControl(i); //second heat generator on, if HP would be on too
			if(heatPumps[i].secHeatGen_on == true){
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen; // here dhw? no -> or like in Interpolation -> differentiation after control
			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == false) ){
			heatPumps[i].Q_hSecHeatGen = 0;
			heatPumps[i].HP_on = false;
		}else if(heatPumps[i].blockHP == false){
			heatPumps[i].Q_hSecHeatGen = 0;
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}
		}
	}
}


//+++++++++++++++controlPartParallel+++++++++++++++
void HpCore::controlPartParallel(int i) {
	//!control function for bivalent part parallel case

	if(heatPumps[i].HP_mode == 0){ //heating

		if(heatPumps[i].Q_HL_hp_time >= heatPumps[i].shutDownHL){
			//only SecHeatGen
			if(heatPumps[i].blockingTime > 0){
				if(heatPumps[i].secHeatGenOnInBlockingTime == true){
					secHeatGenOnControl(i);
				}else{
					if(heatPumps[i].blockHP == false){
						secHeatGenControlLoadStorageFull(i);
					}else{
						heatPumps[i].secHeatGen_on = false;
					}
				}
			}else{
				secHeatGenOnControl(i);
			}
			if(heatPumps[i].secHeatGen_on == true){ //only if HP would be on -> then secHeatGen is on too
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}
		else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == true) ){
			//only second heat generator
			secHeatGenOnControl(i); //second heat generator on, if HP would be on too
			if(heatPumps[i].secHeatGen_on == true){ //only if HP would be on -> then secHeatGen is on too
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == false) ){
			heatPumps[i].Q_hSecHeatGen = 0;
			heatPumps[i].HP_on = false;
		}else if(heatPumps[i].blockHP == false){
			//here only HP or both possible
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;

			}
			if((heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) * 1.1 >= heatPumps[i].Q_h){
				//Hp + secHeatGen
				if(heatPumps[i].HP_on == false){ //Q_hSecHeatGen only on if HP also on
					heatPumps[i].Q_hSecHeatGen = 0;
				}else{
					heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
				}
			}else{ //only HP
				heatPumps[i].Q_hSecHeatGen = 0;
			}

		}

	}else if(heatPumps[i].HP_mode == 1){ //dhw
		//here no shutDown -> like parallel
		if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == true) ){
			//only secHeatGen
			//second heat generator only in blocking time on, if its necessary because of storage temperatures
			secHeatGenOnControl(i);
			if(heatPumps[i].secHeatGen_on == true){
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == false) ){
			heatPumps[i].Q_hSecHeatGen = 0;
			heatPumps[i].HP_on = false;
		//aaa
		}else if(heatPumps[i].blockHP == false){
			//Hp + secHeatGen or only HP
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}
			if((heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) * 1.1 >= heatPumps[i].Q_dhw){
				//Hp + secHeatGen
				if(heatPumps[i].HP_on == false){ //Q_hSecHeatGen only on if HP also on
					heatPumps[i].Q_hSecHeatGen = 0;
				}else{
					heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
				}
			}else{ //only HP
				heatPumps[i].Q_hSecHeatGen = 0;
			}
		}
		/*}else if( ((heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) * 1.1 < heatPumps[i].Q_dhw) && (heatPumps[i].blockHP == false) ){
			//only HP
			heatPumps[i].Q_hSecHeatGen = 0;
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}
		}else if( ((heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) * 1.1 >= heatPumps[i].Q_dhw) && (heatPumps[i].blockHP == false) ){
			//Hp + secHeatGen
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				//following if clause checks if HP_on = true because if T of storages
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}

			if(heatPumps[i].HP_on == false){ //Q_hSecHeatGen only on if HP also on
				heatPumps[i].Q_hSecHeatGen = 0;
			}else{
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}
		}*/
	}
}


//+++++++++++++++controlParallel+++++++++++++++
void HpCore::controlParallel(int i){
	//!control function for bivalent parallel and monoenergetic case

	//cout << "HpCore::controlParallel(int i) -> start Control" << "\n"; //test
	//cout << "heatPumps[i].HP_mode = " << heatPumps[i].HP_mode << "\n"; //test
	if(heatPumps[i].HP_mode == 0){ //heating
		if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == true) ){
			//second heat generator only in blocking time on, if its necessary because of storage temperatures
			HPonControl(i);
			if(heatPumps[i].HP_on == true){
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			//no stages of second heat generator -> then better storage loading especially in HPControlLoadStorageFull (overload energies are subtracted in storage balance)
//				if(heatPumps[i].Q_HL_hp_time > heatPumps[i].nominalPowerSecHeatGen / 2){
//					heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
//				}else{ //heatPumps[i].Q_HL_hp_time <= heatPumps[i].nominalPower2HeatGen / 2
//					heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen / 2;
//				}
			heatPumps[i].HP_on = false;
		}else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == false) ){
			heatPumps[i].Q_hSecHeatGen = 0;
			heatPumps[i].HP_on = false;
		}else if(heatPumps[i].blockHP == false){
			//HP + secHeatGen or only HP
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}
			if((heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) * 1.1 >= heatPumps[i].Q_h){
				//Hp + secHeatGen
				if(heatPumps[i].HP_on == false){ //Q_hSecHeatGen only on if HP also on
					heatPumps[i].Q_hSecHeatGen = 0;
				}else{
					heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
				}
			}else{ //only HP
				heatPumps[i].Q_hSecHeatGen = 0;
			}
		}
		/*}else if( ((heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) * 1.1 < heatPumps[i].Q_h) && (heatPumps[i].blockHP == false) ){
			//only HP
			heatPumps[i].Q_hSecHeatGen = 0;
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}
		}else if( ((heatPumps[i].Q_HL_hp_time + + heatPumps[i].losses_st) * 1.1 >= heatPumps[i].Q_h) && (heatPumps[i].blockHP == false) ){
			//Hp + secHeatGen
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				//following if clause checks if HP_on = true because if T of storages
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}

			if(heatPumps[i].HP_on == false){ //Q_hSecHeatGen only on if HP also on
				heatPumps[i].Q_hSecHeatGen = 0;
			}else{
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}
		}*/

	}else if(heatPumps[i].HP_mode == 1){ //dhw
		//cout << "in if-claus if heatPumps[i].HP_mode == 1 " << "\n"; //test
		if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == true) ){
			//cout << "if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == true) ){" << "\n"; //test
			//second heat generator only in blocking time on, if its necessary because of storage temperatures
			HPonControl(i);
			if(heatPumps[i].HP_on == true){
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}else{
				heatPumps[i].Q_hSecHeatGen = 0;
			}
			heatPumps[i].HP_on = false;
		}else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == false) ){
			//cout << "}else if( (heatPumps[i].blockHP == true) && (heatPumps[i].secHeatGenOnInBlockingTime == false) ){" << "\n"; //test
			heatPumps[i].Q_hSecHeatGen = 0;
			heatPumps[i].HP_on = false;
		}else if(heatPumps[i].blockHP == false){
			//Hp + secHeatGen or only HP
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}
			if((heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) * 1.1 >= heatPumps[i].Q_dhw){
				//Hp + secHeatGen
				if(heatPumps[i].HP_on == false){ //Q_hSecHeatGen only on if HP also on
					heatPumps[i].Q_hSecHeatGen = 0;
				}else{
					heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
				}
			}else{ //only HP
				heatPumps[i].Q_hSecHeatGen = 0;
			}
		}


/*		}else if( ((heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) * 1.1 < heatPumps[i].Q_dhw) && (heatPumps[i].blockHP == false) ){
			//cout << "}else if( (heatPumps[i].Q_dhw_hp_time < heatPumps[i].Q_dhw) && (heatPumps[i].blockHP == false) ){" << "\n"; //test
			//only HP
			heatPumps[i].Q_hSecHeatGen = 0;
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}
		}else if( ((heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) * 1.1 >= heatPumps[i].Q_dhw) && (heatPumps[i].blockHP == false) ){
			//cout << "}else if( (heatPumps[i].Q_dhw_hp_time >= heatPumps[i].Q_dhw) && (heatPumps[i].blockHP == false) ){" << "\n"; //test
			//Hp + secHeatGen
			if( (heatPumps[i].crit_soil == false) && (heatPumps[i].critInterpolation == false) ){
				//following if clause checks if HP_on = true because if T of storages
				if(heatPumps[i].blockingTime > 0){
					if(heatPumps[i].secHeatGenOnInBlockingTime == true){
						HPonControl(i);
					}else{
						HPControlLoadStorageFull(i);
					}
				}else{
					HPonControl(i);
				}
			}else{
				heatPumps[i].HP_on = false;
			}

			if(heatPumps[i].HP_on == false){ //Q_hSecHeatGen only on if HP also on
				heatPumps[i].Q_hSecHeatGen = 0;
			}else{
				heatPumps[i].Q_hSecHeatGen = heatPumps[i].nominalPowerSecHeatGen;
			}
		}*/
	}
	//cout << "Q_hSecHeatGen = " << heatPumps[i].Q_hSecHeatGen << "\n"; //test
	//cout << "Q_dhwSecHeatGen = " << heatPumps[i].Q_dhwSecHeatGen << "\n"; //test

}

void HpCore::secHeatGenOnControl(int i){
	//!function to calculate the status (on/off) of second heat generator in bivalent cases

	if(heatPumps[i].HP_mode == 0){ //heating
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 1) ){ //with heating storage
			if (heatPumps[i].Q_HL_hp_time > 0) { //for summer set back
				if(heatPumps[i].T_st < heatPumps[i].T_st_min + heatPumps[i].hysteresis){
					heatPumps[i].secHeatGen_on = true;
				}else if(heatPumps[i].T_st >= heatPumps[i].T_st_max){
					heatPumps[i].secHeatGen_on = false;
				}
			}else{ //heatPumps[i].Q_HL_hp_time = 0
				heatPumps[i].secHeatGen_on = false;
			}
		}else if( (heatPumps[i].HP_storage == 2) || (heatPumps[i].HP_storage == 3) ){ //no heating storage
			//for summer set back
			if(heatPumps[i].Q_HL_hp_time > 0) {
				heatPumps[i].secHeatGen_on = true;
			}else{
				heatPumps[i].secHeatGen_on = false;
			}
		}
	}
	else if(heatPumps[i].HP_mode == 1){ //dhw
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 2) ){ //with dhw storage
			if (heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw + heatPumps[i].hysteresis) { //T_st_min_control_dhw
				heatPumps[i].secHeatGen_on = true;
			}else if(heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw){
				heatPumps[i].secHeatGen_on = false;
			}else if( (heatPumps[i].HP_storage == 0) && (heatPumps[i].T_st >= heatPumps[i].T_st_max) ){ //Recharging the DHW storage tank after heating
				heatPumps[i].HP_on = true;
			}
		}else if( (heatPumps[i].HP_storage == 1) || (heatPumps[i].HP_storage == 3) ){ //no dhw storage
			if(heatPumps[i].Q_dhw_hp_time > 0) {
				heatPumps[i].secHeatGen_on = true;
			}else{
				heatPumps[i].secHeatGen_on = false;
			}
		}
	}


}

//+++++++++++++++HPonControl+++++++++++++++
void HpCore::HPonControl(int i) {
	//!function to calculate the status (on/off) of heat pump

	if(heatPumps[i].HP_mode == 0){ //heating
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 1) ){ //with heating storage
			if (heatPumps[i].Q_HL_hp_time > 0) { //for summer set back
				if(heatPumps[i].T_st < heatPumps[i].T_st_min + heatPumps[i].hysteresis){
					heatPumps[i].HP_on = true;
				}else if(heatPumps[i].T_st >= heatPumps[i].T_st_max){
					heatPumps[i].HP_on = false;
				}
			}else{ //heatPumps[i].Q_HL_hp_time = 0
				heatPumps[i].HP_on = false;
			}
		}else if( (heatPumps[i].HP_storage == 2) || (heatPumps[i].HP_storage == 3) ){ //no heating storage
			if(heatPumps[i].Q_HL_hp_time > 0) { //for summer set back
				heatPumps[i].HP_on = true;
			}else{
				heatPumps[i].HP_on = false;
			}
		}
	}
	else if(heatPumps[i].HP_mode == 1){ //dhw
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 2) ){ //with dhw storage
			if (heatPumps[i].T_st_dhw <= heatPumps[i].T_st_min_dhw + heatPumps[i].hysteresis) { //T_st_min_control_dhw
				heatPumps[i].HP_on = true;
			}else if(heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw){
				heatPumps[i].HP_on = false;
			}else if( (heatPumps[i].HP_storage == 0) && (heatPumps[i].T_st >= heatPumps[i].T_st_max) ){ //Recharging the DHW storage tank after heating
				heatPumps[i].HP_on = true;
			}
		}else if( (heatPumps[i].HP_storage == 1) || (heatPumps[i].HP_storage == 3) ){ //no dhw storage
			if(heatPumps[i].Q_dhw_hp_time > 0) {
				heatPumps[i].HP_on = true;
			}else{
				heatPumps[i].HP_on = false;
			}
		}
	}

	//ToDo: integrate cooling mode in modeControl?

//+++++++++++++++end of HPonControl+++++++++++++++
}


//+++++++++++++++ interpolation	+++++++++++++++
void HpCore::interpolationHP(int i) {
	//!function to interpolate within the manufacturer's performance curves -> calculates Q_h and P_el of heat pump or heat pump cascade

	//cout << "T_soil_steps.size() = " << heatPumps[i].T_soil_steps.size() << "\n"; //test
	//for(vector<double>::iterator vit=heatPumps[i].T_soil_steps.begin();vit!=heatPumps[i].T_soil_steps.end();++vit) cout << *vit << "\n"; //test
	//vector<double> xData = heatPumps[i].T_soil_steps;
	//vector<double> yData;
	//+++ Q_h +++
	if (heatPumps[i].HP_mode == 0) { //heating
		//cout << "if heatPumps[i].HP_mode = 0" << "\n"; //test
		heatPumps[i].yData = heatPumps[i].mapQ_h[heatPumps[i].T_h_step - 273.15]; // vector<double> yData = heatPumps[i].Q_h[heatPumps[i].T_h_step-273.15]
		//cout << "yData = heatPumps[i].Q_h[heatPumps[i].T_h_step-273.15] size() = " << heatPumps[i].yData.size() << "\n"; //test
		//cout << "HP step heating of " << heatPumps[i].ID << " before interpolation = " << heatPumps[i].T_h_step-273.15 << "\n";
	} else if (heatPumps[i].HP_mode == 1) { //dhw
		//cout << "if heatPumps[i].HP_mode = 1" << "\n"; //test
		heatPumps[i].yData =	heatPumps[i].mapQ_h[heatPumps[i].T_dhw_step - 273.15];
		//cout << "yData = heatPumps[i].Q_h[heatPumps[i].T_dhw_step-273.15] size() = " << yData.size() << "\n"; //test
		//cout << "HP step dhw of " << heatPumps[i].ID << " before interpolation = " << heatPumps[i].T_dhw_step-273.15 << "\n";
	}
	//cout << "Vector of Q_h of " << heatPumps[i].ID << " = " << "\n"; //test
	//for(vector<double>::iterator vit=yData.begin();vit!=yData.end();++vit) cout << *vit << "\n"; //test
	//int size = heatPumps[i].T_soil_steps.size(); //xData
	unsigned int k = 0; // find left end of interval for interpolation
	////cout << "T_soil_out of actual HP after taking the middle = " << heatPumps[i].T_soil_out << " K"<< "\n"; //test
	//double xL = xData[k], yL = yData[k], xR = xData[k + 1], yR = yData[k + 1]; // points on either side (unless beyond ends)
	//double dydx = (yR - yL) / (xR - xL);                 // gradient
	//iteration of Q_h
	if (heatPumps[i].T_soil_out > heatPumps[i].T_soil_steps[heatPumps[i].T_soil_steps.size() - 1]) {
		//cout << "(heatPumps[i].T_soil_out > xData[size - 1])" << "\n"; //test
		heatPumps[i].critInterpolation = true;
		//cout << " actual T_soil_step = " << heatPumps[i].T_soil_out << " K"<< " is > 20 °C" << "\n"; //test
		//cout << " xData[size - 1] = " << xData[size - 1] << " K"<< " is > 20 °C" << "\n"; //test
	} else if (heatPumps[i].T_soil_out < heatPumps[i].T_soil_steps[0]) {
		//cout << "(heatPumps[i].T_soil_out < xData[0])" << "\n"; //test
		heatPumps[i].critInterpolation = true;
		//cout << " actual T_soil_step = " << heatPumps[i].T_soil_out << " K"<< " is < -5 °C" << "\n"; //test
		//cout << " xData[0] = " << xData[0] << " K"<< " is < -5  °C" << "\n"; //test
	} else { //ToDo:new if clause: else if(heatPumps[i].Q_h == 0){heatPumps[i].HP_on = false; heatPumps[i].Vdot_soil_bhe = 0;}? // sometimes given with HP data //ToDo: test it
		while (heatPumps[i].T_soil_out > heatPumps[i].T_soil_steps[k + 1])
			k++; //heatPumps[i].T_soil_out > xData[k+1] -> error here?? then -> for loop until xData[].size()?
		heatPumps[i].Q_h = heatPumps[i].yData[k] + (heatPumps[i].yData[k + 1] - heatPumps[i].yData[k]) / (heatPumps[i].T_soil_steps[k + 1] - heatPumps[i].T_soil_steps[k]) * (heatPumps[i].T_soil_out - heatPumps[i].T_soil_steps[k]);
		//cout << "Q_h[" << heatPumps[i].T_soil_out<<"]: " << heatPumps[i].Q_h << "\n";
		//cout << "HP_on = " << heatPumps[i].HP_on << "\n";

	}

	if(heatPumps[i].critInterpolation == false){
		//+++ P_el +++
		////cout << "P_el["<<"heatPumps[i].T_st_max-273.15]: " << "\n";
		//for(unsigned int j=0; j < heatPumps[i].P_el[heatPumps[i].T_st_max-273.15].size(); j++) //cout << heatPumps[i].P_el[heatPumps[i].T_st_max-273.15][j] << "\n";
		//yData = heatPumps[i].P_el[heatPumps[i].T_st_max-273.15];
		if (heatPumps[i].HP_mode == 0) { //heating
			heatPumps[i].yData = heatPumps[i].mapP_el[heatPumps[i].T_h_step - 273.15];
			//cout << "HP step heating for P_el of " << heatPumps[i].ID << " before interpolation = " << heatPumps[i].T_h_step-273.15 << " °C" << "\n";
		} else if (heatPumps[i].HP_mode == 1) { //dhw
			heatPumps[i].yData = heatPumps[i].mapP_el[heatPumps[i].T_dhw_step - 273.15];
			//cout << "HP step dhw for P_el of " << heatPumps[i].ID << " before interpolation = " << heatPumps[i].T_dhw_step-273.15 << " °C" <<  "\n";
		}
		//for(unsigned int j=0; j < yData.size(); j++) //cout << heatPumps[i].P_el[heatPumps[i].T_st_max-273.15][j] << "\n";
		//iteration of P_el
		if (heatPumps[i].T_soil_out > heatPumps[i].T_soil_steps[heatPumps[i].T_soil_steps.size() - 1]) {
			//cout << "(heatPumps[i].T_soil_out > xData[size - 1])" << "\n"; //test
			heatPumps[i].critInterpolation = true;
		} else if (heatPumps[i].T_soil_out < heatPumps[i].T_soil_steps[0]) {
			//cout << "(heatPumps[i].T_soil_out < xData[0])" << "\n"; //test
			heatPumps[i].critInterpolation = true;
		} else {
			while (heatPumps[i].T_soil_out > heatPumps[i].T_soil_steps[k + 1])
				k++; //heatPumps[i].T_soil_out > xData[k+1]
			//P_el = yData[k] + ( yData[k+1] - yData[k] ) / ( xR - xL ) * ( x - xL ); // linear interpolation
			heatPumps[i].P_el = heatPumps[i].yData[k] + (heatPumps[i].yData[k + 1] - heatPumps[i].yData[k]) / (heatPumps[i].T_soil_steps[k + 1] - heatPumps[i].T_soil_steps[k]) * (heatPumps[i].T_soil_out - heatPumps[i].T_soil_steps[k]);
			//cout << "P_el["<<heatPumps[i].T_soil_out<<"]: " << heatPumps[i].P_el << "\n";

		}
	}


	if(heatPumps[i].critInterpolation == false){
		//+++ cascade +++
		//cout << "HPC: Test heatPumps[i].Q_h of HP " << heatPumps[i].ID << " before cascadeFactor = " << heatPumps[i].Q_h << " W in heatPumps[i].HP_mode = " << heatPumps[i].HP_mode << "\n"; //test
		if(heatPumps[i].vecCascadeFactor.size() > 1){ //then cascade
			//cout << "HPC: Test heatPumps[i].vecCascadeFactor.size > 1 " << "\n";
			sumFactorCascade = 0;
			if (heatPumps[i].HP_mode == 0) { //heating
				if(heatPumps[i].T_st >= (heatPumps[i].T_st_min + heatPumps[i].T_st_max) / 2){
					for(unsigned int a = 0; a < heatPumps[i].vecCascadeFactor.size(); a++){
						sumFactorCascade = sumFactorCascade + heatPumps[i].vecCascadeFactor[a];
						//ToDo: if fullload -> use complete cascade?? test it!
						if(heatPumps[i].Q_h * sumFactorCascade > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st){
							heatPumps[i].Q_h = heatPumps[i].Q_h * sumFactorCascade;
							//cout << "HPC: Heating power of HP " << heatPumps[i].ID << " = " << heatPumps[i].Q_h << "\n"; //test
							heatPumps[i].P_el = heatPumps[i].P_el * sumFactorCascade;
							heatPumps[i].Vdot_soil_bhe = heatPumps[i].Vdot_soil_bhe_init * sumFactorCascade;
							heatPumps[i].Vdot_soil = heatPumps[i].Vdot_soil_init * sumFactorCascade;
							break;
						}/*else{
							cout << "HPC: Heating power of HP " << heatPumps[i].ID << " is not high enough!" << "\n"; //test
							cout << "HPC: heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st of HP  " << heatPumps[i].ID << " = " << heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st << "\n"; //test
						}*/
					}
				}
			}else if (heatPumps[i].HP_mode == 1) {//dhw
				if(heatPumps[i].T_st_dhw >= (heatPumps[i].T_st_min_dhw + heatPumps[i].T_st_max_dhw) / 2){
					for(unsigned int a = 0; a < heatPumps[i].vecCascadeFactor.size() ; a++){
						sumFactorCascade = sumFactorCascade + heatPumps[i].vecCascadeFactor[a];
						//ToDo: if fullload -> use complete cascade?? test it!
						if(heatPumps[i].Q_h * sumFactorCascade > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw){
							heatPumps[i].Q_h = heatPumps[i].Q_h * sumFactorCascade;
							heatPumps[i].P_el = heatPumps[i].P_el * sumFactorCascade;
							heatPumps[i].Vdot_soil_bhe = heatPumps[i].Vdot_soil_bhe_init * sumFactorCascade;
							heatPumps[i].Vdot_soil = heatPumps[i].Vdot_soil_init * sumFactorCascade;
							break;
						}/*else{
							cout << "HPC: dhw power of HP " << heatPumps[i].ID << " is not high enough!" << "\n"; //test
						}*/
					}
				}
			}
		}else{ //no cascade
			heatPumps[i].Vdot_soil_bhe = heatPumps[i].Vdot_soil_bhe_init;
			heatPumps[i].Vdot_soil = heatPumps[i].Vdot_soil_init;
		}
	}


	//+++++++++++++++difference between heating and dhw mode+++++++++++++++
	//has to stand here too, because of alternative control
	if (heatPumps[i].HP_mode == 1) { //dhw
		heatPumps[i].Q_dhw = heatPumps[i].Q_h;
		//heatPumps[i].Q_h = 0;
		heatPumps[i].Q_dhwSecHeatGen = heatPumps[i].Q_hSecHeatGen;
		//heatPumps[i].Q_hSecHeatGen = 0;
	}
	//+++++++++++++++end difference between heating and dhw mode+++++++++++++++
	//cout << "HPC: Test heatPumps[i].Q_h of HP " << heatPumps[i].ID << " = " << heatPumps[i].Q_h << " W in heatPumps[i].HP_mode = " << heatPumps[i].HP_mode << "\n"; //test
}


//+++++++++++++++VsoilTsoilinCascadeCOP etc.+++++++++++++++
void HpCore::QsoilTsoilinCOP(int i) {
	//!function to calculate current T_soil_in, COP, Vdot_soil

	if (heatPumps[i].HP_on == false) {
		heatPumps[i].Q_h = 0;
		heatPumps[i].P_el = 0;
		heatPumps[i].COP = 0;
		//ToDo: EER = 0; like COP!!
		//cout << "heatPumps[i].T_soil_in = heatPumps[i].T_soil_out" << "\n";//test
		heatPumps[i].T_soil_in = heatPumps[i].T_soil_out;
		heatPumps[i].Vdot_soil = 0;
		heatPumps[i].Vdot_soil_bhe = 0;
		heatPumps[i].Q_soil = 0;

	}else{ //heatPumps[i].HP_on == true
		//cout << "heatPumps[i].HP_on = " << heatPumps[i].HP_on << " of HP " << heatPumps[i].ID << "\n";//test

		//+++++ test validation +++++
		////cout << "Vdot_soil_test_val = " << Vdot_soil_test_val << "\n";//test
		////cout << "heatPumps[i].rhoCp_soil = " << heatPumps[i].rhoCp_soil << "\n";//test
		//heatPumps[i].T_soil_in = heatPumps[i].T_soil_out - Q_soil/ (Vdot_soil_test_val * heatPumps[i].rhoCp_soil); //++++++test validation with measured Vdot
		//+++++ end test validation +++++

		//++++++ Q_soil, V_soil, T_soil_in ++++++
		heatPumps[i].Q_soil = heatPumps[i].Q_h - heatPumps[i].P_el;
		//cout << "heatPumps[i].Q_soil = " << heatPumps[i].Q_soil << "\n";//test
		//cout << "calculate T_soil_in" << "\n";//test
		//commented out for test validation
		heatPumps[i].T_soil_in = heatPumps[i].T_soil_out - heatPumps[i].Q_soil / (heatPumps[i].Vdot_soil * heatPumps[i].rhoCp_soil); //out of Q_soil = Vdot_soil * cp_soil * rho_soil * (T_soil_out - T_soil_in);
		//cout << "T_soil_out of actual HP after taking the middle = " << heatPumps[i].T_soil_out << " K"<< "\n"; //test

		//+++ COP +++
		if (heatPumps[i].P_el > 0) { //necessary?? if HP is on then always P_el is > 0!! -> only to avoid mathematical error
			//cout << "if(heatPumps[i].P_el > 0)" << "\n"; //test
			//cout << "heatPumps[i].P_el = " << heatPumps[i].P_el << "\n";
			heatPumps[i].COP = heatPumps[i].Q_h / heatPumps[i].P_el;
		}else{
			heatPumps[i].COP = 0;
		}
		//cout << "heatPumps[i].COP = " << heatPumps[i].COP << " of HP " << heatPumps[i].ID << "\n";//test
		//cout << "rhoCp_soil = " << heatPumps[i].rhoCp_soil << "\n";
	}
	////cout << "HP_on of " << heatPumps[i].ID << " before T_soil_in interpolation = " << heatPumps[i].HP_on << "\n";
}

//+++++++++++++++Cooling+++++++++++++++
void HpCore::cooling(int i) {
	//!function for direct geothermal cooling (simplified)

	if( (heatPumps[i].app_range == 3) || (heatPumps[i].app_range == 4) || (heatPumps[i].app_range == 5) || (heatPumps[i].app_range == 6) ){ //cooling
		if( (heatPumps[i].coolingType == 0) && (heatPumps[i].Q_COOL_hp_time > 0) && (heatPumps[i].T_soil_out <= heatPumps[i].T_maxPassivCooling) ){ //passive cooling
			heatPumps[i].Q_soil = heatPumps[i].Q_h - heatPumps[i].P_el - heatPumps[i].Q_COOL_hp_time; //Assumption: passive cooling can be done in every situation, except if T_soil_out is too high //Q_soil can be negative because of cooling -> T_soil_in rises
			heatPumps[i].energyCooling = heatPumps[i].energyCooling + dt_comm * heatPumps[i].Q_COOL_hp_time; //full cooling load coverage if (heatPumps[i].T_soil_out <= heatPumps[i].T_maxPassivCooling)
			heatPumps[i].Vdot_soilBHEcooling = heatPumps[i].Vdot_soilBHEcoolingInit;
			if(heatPumps[i].Vdot_soil < heatPumps[i].Vdot_soilCooling){
				heatPumps[i].Vdot_soil = heatPumps[i].Vdot_soilCooling;
				heatPumps[i].Vdot_soil_bhe = heatPumps[i].Vdot_soilBHEcooling;
			}
			//cout << "calculate T_soil_in" << "\n";//test
			//commented out for test validation
			heatPumps[i].T_soil_in = heatPumps[i].T_soil_out - heatPumps[i].Q_soil / ( (heatPumps[i].Vdot_soil + heatPumps[i].Vdot_soilCooling) * heatPumps[i].rhoCp_soil); //Assumption: passive cooling can be done in every situation, except if T_soil_out is too high
		}else{ //if( (heatPumps[i].coolingType == 0) && (heatPumps[i].Q_COOL_hp_time > 0) && (heatPumps[i].T_soil_out > heatPumps[i].T_maxPassivCooling) ){
			//cout << "HPC: In passive Cooling Phase: heatPumps[i].T_soil_out > heatPumps[i].T_maxPassivCooling" << "\n";//test
			heatPumps[i].Vdot_soilBHEcooling = 0;
		}
		//ToDo: cooling type = 1
	}

	//ToDo??: Calculation EER (Energy Efficiency Ratio -> active cooling) like COP!!
}

//+++++++++++++++calculate storage balance+++++++++++++++
void HpCore::storageBalance(int i) {
	//!function for storage tank balances

	//+++ monovalent + bivalent +++
	//storage balance -> also if heatPumps[i].HP_on = false

	if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 1)){ //heating and dhw storages or only heating storage
		//T_st
		heatPumps[i].T_st = heatPumps[i].T_st + dt_comm	* (heatPumps[i].Q_h + heatPumps[i].Q_hSecHeatGen - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) / (cp_w * rho_w * heatPumps[i].Vol_st);
		if(heatPumps[i].T_st > heatPumps[i].T_h_step){
			if(heatPumps[i].operatingMode == 0){ //monovalent
				//only if HP is on
				//cout << "HPcore:: StorageBalance: HP heating power to storage Q_h before deduction = " << heatPumps[i].Q_h << " W" << "\n";
				heatPumps[i].Q_h = heatPumps[i].Q_h - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
				//cout << "HPcore:: StorageBalance: HP heating power to storage Q_h after deduction = " << heatPumps[i].Q_h << " W" << "\n";
				//cout << "HPcore:: StorageBalance: HP -> minus energy from T_st > T_h_step = " << - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) << " J" << "\n";
				heatPumps[i].P_el = heatPumps[i].Q_h / heatPumps[i].COP;
			}else if((heatPumps[i].operatingMode == 1) || (heatPumps[i].operatingMode == 2)){//1=monoenergetic, 2=bivalent parallel
				//if HP and secHeatGen are both on or HP alone
				if(heatPumps[i].Q_hSecHeatGen > 0){ //only if HP and secHeatGen are both on
					if( (heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm) <= heatPumps[i].Q_hSecHeatGen){
						//heatPumps[i].Q_h has not to be decreased
						heatPumps[i].Q_hSecHeatGen = heatPumps[i].Q_hSecHeatGen - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
					}else{ //occurs only as a result of the 10% surcharge when deciding whether to switch on the second heat generator
						//heatPumps[i].Q_h has be decreased
						heatPumps[i].Q_h = heatPumps[i].Q_h - (heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm - heatPumps[i].Q_hSecHeatGen);
						heatPumps[i].P_el = heatPumps[i].Q_h / heatPumps[i].COP;
						//energy = heatPumps[i].Q_hSecHeatGen * dt_comm is not required
						heatPumps[i].energySecHeatGenH = heatPumps[i].energySecHeatGenH - heatPumps[i].Q_hSecHeatGen * dt_comm; //see (l. 596)
					}
				}else{ //only HP
					heatPumps[i].Q_h = heatPumps[i].Q_h - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
					heatPumps[i].P_el = heatPumps[i].Q_h / heatPumps[i].COP;
				}
			}else if(heatPumps[i].operatingMode == 3){//3=bivalent teilparallel
				if(heatPumps[i].HP_on == true){
					if(heatPumps[i].Q_hSecHeatGen > 0){
						//only if HP and secHeatGen are both on
						if( (heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm) <= heatPumps[i].Q_hSecHeatGen){
							heatPumps[i].Q_hSecHeatGen = heatPumps[i].Q_hSecHeatGen - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
						}else{ //occurs only as a result of the 10% surcharge when deciding whether to switch on the second heat generator
							heatPumps[i].Q_h = heatPumps[i].Q_h - (heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm - heatPumps[i].Q_hSecHeatGen);
							heatPumps[i].P_el = heatPumps[i].Q_h / heatPumps[i].COP;
							heatPumps[i].energySecHeatGenH = heatPumps[i].energySecHeatGenH - heatPumps[i].Q_hSecHeatGen * dt_comm;
						}
					}else{ //only HP on
						heatPumps[i].Q_h = heatPumps[i].Q_h - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
						heatPumps[i].P_el = heatPumps[i].Q_h / heatPumps[i].COP;
					}
				}else{ //only secHeatGen
					//cout << "HPcore:: StorageBalance: secHG heating power to storage Q_hSecHeatGen before deduction = " << heatPumps[i].Q_hSecHeatGen << " W" << "\n";
					heatPumps[i].Q_hSecHeatGen = heatPumps[i].Q_hSecHeatGen - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
					//cout << "HPcore:: StorageBalance: secHG heating power to storage Q_hSecHeatGen after deduction = " << heatPumps[i].Q_hSecHeatGen - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm << " W" << "\n";
					//cout << "HPcore:: StorageBalance: secHG -> minus energy from T_st > T_h_step = " << - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) << " J" << "\n";
				}

			}else if(heatPumps[i].operatingMode == 4){//4=bivalent alternativ
				if(heatPumps[i].HP_on == true){ //only HP on
					heatPumps[i].Q_h = heatPumps[i].Q_h - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
					heatPumps[i].P_el = heatPumps[i].Q_h / heatPumps[i].COP;
				}else{ //only secHeatGen
					heatPumps[i].Q_hSecHeatGen = heatPumps[i].Q_hSecHeatGen - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm;
				}
			}
			heatPumps[i].T_st = heatPumps[i].T_h_step;
		}else if (heatPumps[i].T_st < 288.15) {
			heatPumps[i].T_st = 288.15;
		}
		heatPumps[i].U_st_useful = heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_st_min);
	}

	if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 2)){ //heating and dhw storages or only dhw storage
		//T_st_dhw
		heatPumps[i].T_st_dhw = heatPumps[i].T_st_dhw + dt_comm * (heatPumps[i].Q_dhw + heatPumps[i].Q_dhwSecHeatGen - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time) / (cp_w * rho_w * heatPumps[i].Vol_st_dhw);
		if(heatPumps[i].T_st_dhw > heatPumps[i].T_dhw_step){
			if(heatPumps[i].operatingMode == 0){ //monovalent
				//only if HP is on
				heatPumps[i].Q_dhw = heatPumps[i].Q_dhw - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;
				heatPumps[i].P_el = heatPumps[i].Q_dhw / heatPumps[i].COP;
			}else if((heatPumps[i].operatingMode == 1) || (heatPumps[i].operatingMode == 2)){//1=monoenergetic, 2=bivalent parallel
				//if HP and secHeatGen are both on or HP alone
				if(heatPumps[i].Q_dhwSecHeatGen > 0){ //only if HP and secHeatGen are both on
					if( (heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm) <= heatPumps[i].Q_dhwSecHeatGen){
						heatPumps[i].Q_dhwSecHeatGen = heatPumps[i].Q_dhwSecHeatGen - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;
					}else{ //occurs only as a result of the 10% surcharge when deciding whether to switch on the second heat generator
						heatPumps[i].Q_dhw = heatPumps[i].Q_dhw - (heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm - heatPumps[i].Q_dhwSecHeatGen);
						heatPumps[i].P_el = heatPumps[i].Q_dhw / heatPumps[i].COP;
						heatPumps[i].energySecHeatGenDHW = heatPumps[i].energySecHeatGenDHW - heatPumps[i].Q_dhwSecHeatGen * dt_comm;
					}
				}else{ //only HP
					heatPumps[i].Q_dhw = heatPumps[i].Q_dhw - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;
					heatPumps[i].P_el = heatPumps[i].Q_dhw / heatPumps[i].COP;
				}
			}else if(heatPumps[i].operatingMode == 3){//3=bivalent teilparallel
				if(heatPumps[i].HP_on == true){
					//only if HP and secHeatGen are both on
					if(heatPumps[i].Q_dhwSecHeatGen > 0){
						if( (heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm) <= heatPumps[i].Q_dhwSecHeatGen){
							heatPumps[i].Q_dhwSecHeatGen = heatPumps[i].Q_dhwSecHeatGen - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;
						}else{ //occurs only as a result of the 10% surcharge when deciding whether to switch on the second heat generator
							heatPumps[i].Q_dhw = heatPumps[i].Q_dhw - (heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm - heatPumps[i].Q_dhwSecHeatGen);
							heatPumps[i].P_el = heatPumps[i].Q_dhw / heatPumps[i].COP;
							heatPumps[i].energySecHeatGenDHW = heatPumps[i].energySecHeatGenDHW - heatPumps[i].Q_dhwSecHeatGen * dt_comm;
						}
					}else{
						heatPumps[i].Q_dhw = heatPumps[i].Q_dhw - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;
						heatPumps[i].P_el = heatPumps[i].Q_dhw / heatPumps[i].COP;
					}
				}else{ //only secHeatGen
					//cout << "HPcore:: StorageBalance: secHG heating power to storage Q_hSecHeatGen before deduction = " << heatPumps[i].Q_hSecHeatGen << " W" << "\n";
					heatPumps[i].Q_dhwSecHeatGen = heatPumps[i].Q_dhwSecHeatGen - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;
					//cout << "HPcore:: StorageBalance: secHG heating power to storage Q_hSecHeatGen after deduction = " << heatPumps[i].Q_hSecHeatGen - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) / dt_comm << " W" << "\n";
					//cout << "HPcore:: StorageBalance: secHG -> minus energy from T_st > T_h_step = " << - heatPumps[i].Vol_st * rho_w * cp_w * (heatPumps[i].T_st - heatPumps[i].T_h_step) << " J" << "\n";
				}
			}else if(heatPumps[i].operatingMode == 4){//4=bivalent alternativ
				if(heatPumps[i].HP_on == true){ //only HP on
					heatPumps[i].Q_dhw = heatPumps[i].Q_dhw - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;
					heatPumps[i].P_el = heatPumps[i].Q_dhw / heatPumps[i].COP;
				}else{ //only secHeatGen
					heatPumps[i].Q_dhwSecHeatGen = heatPumps[i].Q_dhwSecHeatGen - heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_dhw_step) / dt_comm;

				}
			}
			heatPumps[i].T_st_dhw = heatPumps[i].T_dhw_step;
		}
		else if (heatPumps[i].T_st_dhw < 288.15) {
			heatPumps[i].T_st_dhw = 288.15;
		}
		heatPumps[i].U_st_useful_dhw = heatPumps[i].Vol_st_dhw * rho_w * cp_w * (heatPumps[i].T_st_dhw - heatPumps[i].T_st_min_dhw);

	}



	//cout << "T_st = " << heatPumps[i].T_st << " K"<< "\n"; //test
	//cout << "T_st_dhw = " << heatPumps[i].T_st_dhw << " K"<< "\n"; //test

//+++++++++++++++end of storage balance+++++++++++++++

}




//+++++++++++++++ loadCoverage heating and dhw +++++++++++++++
void HpCore::loadCoverage(int i) {
	//!function to calculate load coverage for heating and cooling in every time step

	if ((heatPumps[i].app_range == 0) || (heatPumps[i].app_range == 3)) { //no dhw
		heatPumps[i].dhwLoadCoverage = 0;
		if (heatPumps[i].Q_HL_hp_time <= 0) {
			heatPumps[i].heatingLoadCoverage = 100;
		} else {
			if (heatPumps[i].HP_storage == 3) { //no storages
				heatPumps[i].heatingLoadCoverage = 100 * heatPumps[i].Q_h
						/ heatPumps[i].Q_HL_hp_time;
				if (heatPumps[i].heatingLoadCoverage > 100) {
					heatPumps[i].heatingLoadCoverage = 100;
				}
			} else if (heatPumps[i].HP_storage == 1) { //heating storage
				if (heatPumps[i].T_st >= heatPumps[i].T_st_min) { //heating storage
					heatPumps[i].heatingLoadCoverage = 100;
				} else {
					heatPumps[i].heatingLoadCoverage = 100
							* (heatPumps[i].Q_h
									+ heatPumps[i].U_st_useful / dt_comm)
							/ heatPumps[i].Q_HL_hp_time;
					if (heatPumps[i].heatingLoadCoverage > 100) {
						heatPumps[i].heatingLoadCoverage = 100;
					} else if (heatPumps[i].heatingLoadCoverage < 0) {
						heatPumps[i].heatingLoadCoverage = 0;
					}
				}
			}
		}
	}else if( (heatPumps[i].app_range == 1) || (heatPumps[i].app_range == 6) ){ //no heating	//++++heatPumps[i].app_range == 1 (dhw), 6=dhw+cooling++++
		heatPumps[i].heatingLoadCoverage = 0;
		if (heatPumps[i].Q_dhw_hp_time <= 0) {
			heatPumps[i].dhwLoadCoverage = 100;
		} else {
			if (heatPumps[i].HP_storage == 3) { //no storages
				heatPumps[i].dhwLoadCoverage = 100 * heatPumps[i].Q_dhw
						/ heatPumps[i].Q_dhw_hp_time;
				if (heatPumps[i].dhwLoadCoverage > 100) {
					heatPumps[i].dhwLoadCoverage = 100;
				}
			} else if (heatPumps[i].HP_storage == 2) {
				if (heatPumps[i].T_st_dhw >= heatPumps[i].T_st_min_dhw) { //dhw storage
					heatPumps[i].dhwLoadCoverage = 100;
				} else {
					heatPumps[i].dhwLoadCoverage = 100 * (heatPumps[i].Q_dhw + heatPumps[i].U_st_useful_dhw / dt_comm) / heatPumps[i].Q_dhw_hp_time;
					if (heatPumps[i].dhwLoadCoverage > 100) {
						heatPumps[i].dhwLoadCoverage = 100;
					} else if (heatPumps[i].dhwLoadCoverage < 0) {
						heatPumps[i].dhwLoadCoverage = 0;
					}
				}
			}
		}
	} else if ((heatPumps[i].app_range == 2) || (heatPumps[i].app_range == 4)) { //heating + dhw
		if (heatPumps[i].HP_storage == 3) { //no storages
			if (heatPumps[i].Q_HL_hp_time <= 0) {
				heatPumps[i].heatingLoadCoverage = 100;
			} else {
				heatPumps[i].heatingLoadCoverage = 100 * heatPumps[i].Q_h
						/ heatPumps[i].Q_HL_hp_time;
				if (heatPumps[i].heatingLoadCoverage > 100) {
					heatPumps[i].heatingLoadCoverage = 100;
				}
			}
			//dhw
			if (heatPumps[i].Q_dhw_hp_time <= 0) {
				heatPumps[i].dhwLoadCoverage = 100;
			} else {
				heatPumps[i].dhwLoadCoverage = 100 * heatPumps[i].Q_dhw
						/ heatPumps[i].Q_dhw_hp_time;
				if (heatPumps[i].dhwLoadCoverage > 100) {
					heatPumps[i].dhwLoadCoverage = 100;
				}
			}
		} else if (heatPumps[i].HP_storage == 0) { //heating + dhw storages
			//heating
			if (heatPumps[i].Q_HL_hp_time <= 0) {
				heatPumps[i].heatingLoadCoverage = 100;
			} else {
				if (heatPumps[i].T_st >= heatPumps[i].T_st_min) { //heating storage
					heatPumps[i].heatingLoadCoverage = 100;
				} else {
					heatPumps[i].heatingLoadCoverage = 100 * (heatPumps[i].Q_h + heatPumps[i].U_st_useful / dt_comm) / heatPumps[i].Q_HL_hp_time;
					if (heatPumps[i].heatingLoadCoverage > 100) {
						heatPumps[i].heatingLoadCoverage = 100;
					} else if (heatPumps[i].heatingLoadCoverage < 0) {
						heatPumps[i].heatingLoadCoverage = 0;
					}
				}
			}
			//dhw
			if (heatPumps[i].Q_dhw_hp_time <= 0) {
				heatPumps[i].dhwLoadCoverage = 100;
			} else {
				if (heatPumps[i].T_st_dhw >= heatPumps[i].T_st_min_dhw) { //dhw storage
					heatPumps[i].dhwLoadCoverage = 100;
				} else {
					heatPumps[i].dhwLoadCoverage = 100 * (heatPumps[i].Q_dhw + heatPumps[i].U_st_useful_dhw / dt_comm) / heatPumps[i].Q_dhw_hp_time;
					if (heatPumps[i].dhwLoadCoverage > 100) {
						heatPumps[i].dhwLoadCoverage = 100;
					} else if (heatPumps[i].dhwLoadCoverage < 0) {
						heatPumps[i].dhwLoadCoverage = 0;
					}
				}
			}
		} else if (heatPumps[i].HP_storage == 1) { // HP_storage = 1 = only heating storage
			//heating
			if (heatPumps[i].Q_HL_hp_time <= 0) {
				heatPumps[i].heatingLoadCoverage = 100;
			} else {
				if (heatPumps[i].T_st >= heatPumps[i].T_st_min) { //heating storage
					heatPumps[i].heatingLoadCoverage = 100;
				} else {
					heatPumps[i].heatingLoadCoverage = 100
							* (heatPumps[i].Q_h
									+ heatPumps[i].U_st_useful / dt_comm)
							/ heatPumps[i].Q_HL_hp_time;
					if (heatPumps[i].heatingLoadCoverage > 100) {
						heatPumps[i].heatingLoadCoverage = 100;
					} else if (heatPumps[i].heatingLoadCoverage < 0) {
						heatPumps[i].heatingLoadCoverage = 0;
					}
				}
			}
			//dhw
			if (heatPumps[i].Q_dhw_hp_time <= 0) {
				heatPumps[i].dhwLoadCoverage = 100;
			} else {
				heatPumps[i].dhwLoadCoverage = 100 * heatPumps[i].Q_dhw
						/ heatPumps[i].Q_dhw_hp_time;
				if (heatPumps[i].dhwLoadCoverage > 100) {
					heatPumps[i].dhwLoadCoverage = 100;
				}
			}
		} else if (heatPumps[i].HP_storage == 2) { // HP_storage = 2 = only dhw storage
			//heating
			if (heatPumps[i].Q_HL_hp_time <= 0) {
				heatPumps[i].heatingLoadCoverage = 100;
			} else {
				heatPumps[i].heatingLoadCoverage = 100 * heatPumps[i].Q_h
						/ heatPumps[i].Q_HL_hp_time;
				if (heatPumps[i].heatingLoadCoverage > 100) {
					heatPumps[i].heatingLoadCoverage = 100;
				}
			}
			//dhw
			if (heatPumps[i].Q_dhw_hp_time <= 0) {
				heatPumps[i].dhwLoadCoverage = 100;
			} else {
				if (heatPumps[i].T_st_dhw >= heatPumps[i].T_st_min_dhw) { //dhw storage
					heatPumps[i].dhwLoadCoverage = 100;
				} else {
					heatPumps[i].dhwLoadCoverage = 100 * (heatPumps[i].Q_dhw + heatPumps[i].U_st_useful_dhw / dt_comm) / heatPumps[i].Q_dhw_hp_time;
					if (heatPumps[i].dhwLoadCoverage > 100) {
						heatPumps[i].dhwLoadCoverage = 100;
					} else if (heatPumps[i].dhwLoadCoverage < 0) {
						heatPumps[i].dhwLoadCoverage = 0;
					}
				}
			}
		}
	}
//+++++++++++++++ end of loadCoverage +++++++++++++++
}



void HpCore::HPControlLoadStorageFull(int i) { //loadStoragesFull
	//!control function for full loading storage tanks in case of blocked HP in next hour -> calls HpCore::LoadStorageFull(int i)

	//ToDo: test with all varieties of storages


//ToDo? step + 1 * ceil(heatPumps[i].loadTime/3600) < 8760
//no sec Heat generator out of control strategy
//here actually also secHeatGen -> but not yet always calculated (z. B. controlAlternative) -> without consideration is conservative view
/*	if( (heatPumps[i].Q_h > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max >= heatPumps[i].T_st) && (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
		heatPumps[i].loadTime = rho_w * cp_w * (heatPumps[i].Vol_st * (heatPumps[i].T_st_max - heatPumps[i].T_st) / (heatPumps[i].Q_h - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) + heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].Q_dhw - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time));
	}else if( (heatPumps[i].Q_h > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max >= heatPumps[i].T_st) && (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw < heatPumps[i].T_st_dhw) ){
		//heatPumps[i].T_st_max_dhw < heatPumps[i].T_st_dhw
		heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st * (heatPumps[i].T_st_max - heatPumps[i].T_st) / (heatPumps[i].Q_h - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) ;
	}else if( (heatPumps[i].Q_h > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max < heatPumps[i].T_st) && (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
		//heatPumps[i].T_st_max < heatPumps[i].T_st
		heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].Q_dhw - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time);
	}
					heatPumps[i].loadTime = rho_w * cp_w * (heatPumps[i].Vol_st * (heatPumps[i].T_st_max - heatPumps[i].T_st) / (heatPumps[i].Q_h - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) + heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].Q_dhw - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time));
					if(heatPumps[i].loadTime <= 0){ //possible because of storage Temp.

					}

	*/

/*	if(step +1 * ceil(heatPumps[i].loadTime/3600) < 8760){ //not for last hour of year //ToDo: test // what about the last value? //ToDo: increase to several time steps? -> depending on the heatPumps[i].timerLoadStFull??
		if(heatPumps[i].vecBlockHP[step+1 * ceil(heatPumps[i].loadTime/3600)] == true){*/

	if(step +1 < 8760){ //not for last hour of year //ToDo: test // what about the last value? //ToDo: increase to several time steps? -> depending on the heatPumps[i].timerLoadStFull??
		if(heatPumps[i].vecBlockHP[step+1] == true){

			LoadStorageFull(i);

			//ToDo? implement loadTime??
			/*if( (heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] == 0) ){ // heatPumps[i].Q_HL_hp_time && heatPumps[i].vecPreQ_HL_hp[step +1] = 0


				if( (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
					heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].Q_dhw - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time);
				}else{
					heatPumps[i].loadTime = 0;
				}
			}else if( (heatPumps[i].Q_h > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max >= heatPumps[i].T_st) && (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
				//normal formula
				heatPumps[i].loadTime = rho_w * cp_w * (heatPumps[i].Vol_st * (heatPumps[i].T_st_max - heatPumps[i].T_st) / (heatPumps[i].Q_h - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) + heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].Q_dhw - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time));
			}else if( (heatPumps[i].Q_h > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max >= heatPumps[i].T_st) && (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw < heatPumps[i].T_st_dhw) ){
				//heatPumps[i].T_st_max_dhw < heatPumps[i].T_st_dhw -> if T_dhw_step > T_st_max_dhw
				heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st * (heatPumps[i].T_st_max - heatPumps[i].T_st) / (heatPumps[i].Q_h - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) ;
			}else if( (heatPumps[i].Q_h > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max < heatPumps[i].T_st) && (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
				//heatPumps[i].T_st_max < heatPumps[i].T_st -> if T_h_step > T_st_max_h
				heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].Q_dhw - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time);
			}else{
				//simplified!!
				//not all cases covered, so the calculation of a shorter recharge time sometimes does not work -> out again for now
				//could be negative or smaller as necessary
				heatPumps[i].loadTime = 3600;
			}
			if(heatPumps[i].loadTime >= 3600){
				//heatPumps[i].loadTime = 3600; //only necessary if loadTime is used anywhere else
				if(t < (step +1) * 3600){ //ToDo: test it
					LoadStorageFull(i);
					//Here difference between operatingMode?? -> secHeatGen on although it is off because of control strategy? -> no, because of usage of second heat gen
				} //end of timerLoadStFull
				else{
					//HPonControl(i); // ToDo: necessary? -> wenn t = (step + 1) * 3600, dann ist doch die WP geblockt!!
					heatPumps[i].fullLoad = false;
				}
			}else{
				if( ( t >= (step +1) * 3600 - heatPumps[i].loadTime) && (t < (step +1) * 3600) ){
					LoadStorageFull(i);
				} //end of timerLoadStFull
				else{
					HPonControl(i);
					heatPumps[i].fullLoad = false;
				}
			}*/


		}
		else{
			HPonControl(i);
			heatPumps[i].fullLoad = false;
		}
	} //end of if(step +1 < 8760)
	else{ //last entry of year
//		ToDo: if(heatPumps[i].vecBlockHP[0] == true){ //first step of each year
//			LoadStorageFull(i);
//		}
//		else{
//			HPonControl(i);
//			heatPumps[i].fullLoad = false;
//		}
		HPonControl(i);
		heatPumps[i].fullLoad = false;
	}

}

void HpCore::LoadStorageFull(int i) { //is used in HpCore::HPControlLoadStorageFull(int i)
	//!function for full loading storage tanks in case of blocked HP in next hour -> HP_on = true/false

	heatPumps[i].fullLoad = true;
	if(heatPumps[i].HP_mode == 0){ //heating
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 1) ){ //with heating storage
			if(heatPumps[i].T_st >= heatPumps[i].T_st_max){
				heatPumps[i].HP_on = false;
			}else if (heatPumps[i].Q_HL_hp_time > 0) { //for summer set back
				if(heatPumps[i].vecPreQ_HL_hp[step +1] > 0) {
					heatPumps[i].HP_on = true;
				}else if(heatPumps[i].T_st < heatPumps[i].T_st_min + heatPumps[i].hysteresis){
					heatPumps[i].HP_on = true;
				}
			}else{ //heatPumps[i].Q_HL_hp_time = 0
				if(heatPumps[i].vecPreQ_HL_hp[step +1] > 0) {
					heatPumps[i].HP_on = true;
				}else{
					heatPumps[i].HP_on = false;
				}
			}
		}else if( (heatPumps[i].HP_storage == 2) || (heatPumps[i].HP_storage == 3) ){ //no heating storage
			if(heatPumps[i].Q_HL_hp_time > 0) { //for summer set back
				heatPumps[i].HP_on = true;
			}else{
				heatPumps[i].HP_on = false;
			}
		}
	}else if(heatPumps[i].HP_mode == 1){ //dhw
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 2) ){ //with dhw storage
			if(heatPumps[i].T_st_dhw < heatPumps[i].T_st_max_dhw){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep
				heatPumps[i].HP_on = true;
			}else if(heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw){
				heatPumps[i].HP_on = false;
			}
		}else if( (heatPumps[i].HP_storage == 1) || (heatPumps[i].HP_storage == 3) ){ //no dhw storage
			if(heatPumps[i].Q_dhw_hp_time > 0) {
				heatPumps[i].HP_on = true;
			}else{
				heatPumps[i].HP_on = false;
			}
		}
	}

}

void HpCore::secHeatGenControlLoadStorageFull(int i) { //loadStoragesFull
	//!control function for full loading storage tanks in case of blocked HP in next hour and bivalent systems -> calls HpCore::secHeatGenLoadStorageFull(int i)

	//only secHeatGen
	if(step +1 < 8760){ //not for last hour of year
		if(heatPumps[i].vecBlockHP[step+1] == true){
			//no difference between Q_dhwSecHeatGen and Q_hSecHeatGen here (after controls) -> so everyTime nominalPowerSecHeatGen
			secHeatGenLoadStorageFull(i);



			//ToDo? implement loadTime??

/*			if( (heatPumps[i].Q_HL_hp_time == 0) && (heatPumps[i].vecPreQ_HL_hp[step +1] == 0) ){ // heatPumps[i].Q_HL_hp_time && heatPumps[i].vecPreQ_HL_hp[step +1] = 0
				//xxxx ToDo:test it
				if( (heatPumps[i].Q_dhw > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
					heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].Q_dhw - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time);
				}else{
					heatPumps[i].loadTime = 0;
				}
			}else if( (heatPumps[i].nominalPowerSecHeatGen > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max >= heatPumps[i].T_st) && (heatPumps[i].nominalPowerSecHeatGen > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
				//normal formula
				heatPumps[i].loadTime = rho_w * cp_w * (heatPumps[i].Vol_st * (heatPumps[i].T_st_max - heatPumps[i].T_st) / (heatPumps[i].nominalPowerSecHeatGen - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) + heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].nominalPowerSecHeatGen - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time));
			}else if( (heatPumps[i].nominalPowerSecHeatGen > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max >= heatPumps[i].T_st) && (heatPumps[i].nominalPowerSecHeatGen > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw < heatPumps[i].T_st_dhw) ){
				//heatPumps[i].T_st_max_dhw < heatPumps[i].T_st_dhw -> if T_dhw_step > T_st_max_dhw
				heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st * (heatPumps[i].T_st_max - heatPumps[i].T_st) / (heatPumps[i].nominalPowerSecHeatGen - heatPumps[i].losses_st - heatPumps[i].Q_HL_hp_time) ;
			}else if( (heatPumps[i].nominalPowerSecHeatGen > heatPumps[i].Q_HL_hp_time + heatPumps[i].losses_st) && (heatPumps[i].T_st_max < heatPumps[i].T_st) && (heatPumps[i].nominalPowerSecHeatGen > heatPumps[i].Q_dhw_hp_time + heatPumps[i].losses_st_dhw) && (heatPumps[i].T_st_max_dhw >= heatPumps[i].T_st_dhw) ){
				//heatPumps[i].T_st_max < heatPumps[i].T_st -> if T_h_step > T_st_max_h
				heatPumps[i].loadTime = rho_w * cp_w * heatPumps[i].Vol_st_dhw * (heatPumps[i].T_st_max_dhw - heatPumps[i].T_st_dhw) / (heatPumps[i].nominalPowerSecHeatGen - heatPumps[i].losses_st_dhw - heatPumps[i].Q_dhw_hp_time);
			}else{
				//could be negative or smaller as necessary
				heatPumps[i].loadTime = 3600;
			}
			if(heatPumps[i].loadTime >= 3600){
				//heatPumps[i].loadTime = 3600; //only necessary if loadTime is used anywhere else
				if(t < (step +1) * 3600){
					secHeatGenLoadStorageFull(i);
					//Here difference between operatingMode?? -> secHeatGen on although it is off because of control strategy? -> no, because of usage of second heat gen
				} //end of timerLoadStFull
				else{ //ToDo: test it
					secHeatGenOnControl(i);
					heatPumps[i].fullLoad = false;
				}
			}else{
				if( ( t >= (step +1) * 3600 - heatPumps[i].loadTime) && (t < (step +1) * 3600) ){
					secHeatGenLoadStorageFull(i);
				} //end of timerLoadStFull
				else{ //ToDo: test it
					secHeatGenOnControl(i);
					heatPumps[i].fullLoad = false;
				}
			}*/


		}else{
			secHeatGenOnControl(i);
			heatPumps[i].fullLoad = false;
		}
	} //end of if(step +1 < 8760)
	else{ //last entry of year
		secHeatGenOnControl(i);
		heatPumps[i].fullLoad = false;
	}

}

void HpCore::secHeatGenLoadStorageFull(int i) { //is used in HpCore::HPControlLoadStorageFull(int i)
	//!function for full loading storage tanks in case of blocked HP in next hour and bivalent systems -> secHeatGen_on = true/false;

	heatPumps[i].fullLoad = true;

	if(heatPumps[i].HP_mode == 0){ //heating
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 1) ){ //with heating storage
			if(heatPumps[i].T_st >= heatPumps[i].T_st_max){
				heatPumps[i].secHeatGen_on = false;
			}else if (heatPumps[i].Q_HL_hp_time > 0) { //for summer set back
				if(heatPumps[i].vecPreQ_HL_hp[step +1] > 0) {
					heatPumps[i].secHeatGen_on = true;
				}else if(heatPumps[i].T_st < heatPumps[i].T_st_min + heatPumps[i].hysteresis){
					heatPumps[i].secHeatGen_on = true;
				}
			}else{ //heatPumps[i].Q_HL_hp_time = 0
				if(heatPumps[i].vecPreQ_HL_hp[step +1] > 0) {
					heatPumps[i].secHeatGen_on = true;
				}else{
					heatPumps[i].secHeatGen_on = false;
				}
			}
		}else if( (heatPumps[i].HP_storage == 2) || (heatPumps[i].HP_storage == 3) ){ //no heating storage
			if(heatPumps[i].Q_HL_hp_time > 0) { //for summer set back
				heatPumps[i].secHeatGen_on = true;
			}else{
				heatPumps[i].secHeatGen_on = false;
			}
		}
	}
	else if(heatPumps[i].HP_mode == 1){ //dhw
		if( (heatPumps[i].HP_storage == 0) || (heatPumps[i].HP_storage == 2) ){ //with dhw storage
			if(heatPumps[i].T_st_dhw < heatPumps[i].T_st_max_dhw){ //fullLoad calculated in HPControlLoadStorageFull(i) -> after modeControl -> modeControl uses fullLoad of last timestep
				heatPumps[i].secHeatGen_on = true;
			}else if(heatPumps[i].T_st_dhw >= heatPumps[i].T_st_max_dhw){
				heatPumps[i].secHeatGen_on = false;
			}
		}else if( (heatPumps[i].HP_storage == 1) || (heatPumps[i].HP_storage == 3) ){ //no dhw storage
			if(heatPumps[i].Q_dhw_hp_time > 0) {
				heatPumps[i].secHeatGen_on = true;
			}else{
				heatPumps[i].secHeatGen_on = false;
			}
		}
	}



}

void HpCore::saveOverhang() {
	//!function to clear all vectors (after pushBackTXT()) in the end of one year + saves overhang if unusual time steps

	//cout << "void HpCore::saveOverhang()" <<endl; //test

	//clears all vectors of pushBackTXT() in the end of one year + saves overhang if unusual time steps
	//nexVecX.push_back.vecX[vecX.size() - (x-i)];


	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			//cout << "for (int i = 0; i < heatPumps.size(); i++)" <<endl; //test
			heatPumps[i].vecQ_HL_hp_time = buildNewVecDouble(heatPumps[i].vecQ_HL_hp_time);
			heatPumps[i].vecQ_dhw_hp_time = buildNewVecDouble(heatPumps[i].vecQ_dhw_hp_time);
			heatPumps[i].resT_soil_out = buildNewVecDouble(heatPumps[i].resT_soil_out);
			heatPumps[i].resT_soil_in = buildNewVecDouble(heatPumps[i].resT_soil_in);
			heatPumps[i].resVecCrit_soil = buildNewVecBool(heatPumps[i].resVecCrit_soil);
			heatPumps[i].vecT_st = buildNewVecDouble(heatPumps[i].vecT_st);
			heatPumps[i].vecT_st_dhw = buildNewVecDouble(heatPumps[i].vecT_st_dhw);
			heatPumps[i].vecHL_Coverage = buildNewVecDouble(heatPumps[i].vecHL_Coverage);
			heatPumps[i].vecDHW_Coverage = buildNewVecDouble(heatPumps[i].vecDHW_Coverage);
			heatPumps[i].resQ_h = buildNewVecDouble(heatPumps[i].resQ_h);
			heatPumps[i].resQ_dhw = buildNewVecDouble(heatPumps[i].resQ_dhw);
			heatPumps[i].resP_el = buildNewVecDouble(heatPumps[i].resP_el);
			heatPumps[i].resQ_soil = buildNewVecDouble(heatPumps[i].resQ_soil);
			heatPumps[i].vecCOP = buildNewVecDouble(heatPumps[i].vecCOP);
			heatPumps[i].resHP_mode = buildNewVecUnsInt(heatPumps[i].resHP_mode);
			heatPumps[i].resHP_on = buildNewVecBool(heatPumps[i].resHP_on);
		} // end of if(heatPumps[i].simulateHP == true)

	}
	//cout << "vecTime = buildNewVecDouble(vecTime);" <<endl; //test
	vecTime = buildNewVecDouble(vecTime);
	//cout << "new vecTime.size() = " << vecTime.size() << endl; //test
}

vector<double> HpCore::buildNewVecDouble(vector<double> inputVec) {
	//cout << "vector<double> HpCore::buildNewVecDouble(vector<double> inputVec)" <<endl; //test
	vector<double> newVec;
/*
	cout << "actYear before if (vecTime.size() > yearVecSize){actYear = actYear - 1;} = " << actYear <<endl; //test
	if (vecTime.size() > yearVecSize) {
		actYear = actYear - 1;
	}
	cout << "actYear after if (vecTime.size() > yearVecSize){actYear = actYear - 1;} = " << actYear <<endl; //test
*/
	unsigned int x = 0; // number of overlapping time steps
	//cout << "x = vecTime.size() - yearVecSize" <<endl; //test
	x = vecTime.size() - yearVecSize; //x = ceil((t-actYear * 8760 * 3600)/dt_comm);
	//cout << "x = " << x <<endl; //test
	//cout << "for(unsigned int i = 0; i < x; i++)" <<endl; //test
	for(unsigned int i = 0; i < x; i++){
		//function all vectors of pushBackTXT() -> 17 new vectors for saving??
/*
		cout << "i = " << i <<endl; //test
		cout << "x = " << x <<endl; //test
		cout << "newVec.push_back(inputVec[inputVec.size() - (x - i)])" <<endl; //test
*/
		newVec.push_back(inputVec[inputVec.size() - (x - i)]);

	}
	return newVec;
}

vector<bool> HpCore::buildNewVecBool(vector<bool> inputVec) {
	//cout << "vector<bool> HpCore::buildNewVecBool(vector<bool> inputVec)" <<endl; //test
	vector<bool> newVec;
	unsigned int x = 0;
	x = vecTime.size() - yearVecSize;
	for(unsigned int i = 0; i < x; i++){
		//function all vectors of pushBackTXT() -> 17 new vectors for saving??
		//nexVecX.push_back.vecX[vecX.size() - (x-i)];
		newVec.push_back(inputVec[inputVec.size() - (x - i)]);

	}
	return newVec;
}

vector<unsigned int> HpCore::buildNewVecUnsInt(vector<unsigned int> inputVec) {
	//cout << "vector<unsigned int> HpCore::buildNewVecUnsInt(vector<unsigned int> inputVec)" <<endl; //test
	vector<unsigned int> newVec;
	unsigned int x = 0;
	x = vecTime.size() - yearVecSize;
	for(unsigned int i = 0; i < x; i++){
		//function all vectors of pushBackTXT() -> 17 new vectors for saving??
		//nexVecX.push_back.vecX[vecX.size() - (x-i)];
		newVec.push_back(inputVec[inputVec.size() - (x - i)]);

	}
	return newVec;
}

void HpCore::pushBackTXT() {
	//!function to push back current variables in vectors (vector size = number of time steps in one year)

	//cout << "pushBackTXT() in HpCore" << "\n"; //test

	//count t for results:

	//cout << "Actual time step in pushBackTXT() before vecTime.push_back(t); = " << t << " s"<<endl; //test
	//cout << "Actual vecTime.size() in pushBackTXT() before vecTime.push_back(t); = " << vecTime.size() <<endl; //test
	vecTime.push_back(t);
	//cout << "Actual vecTime.size() in pushBackTXT() after vecTime.push_back(t); = " << vecTime.size() <<endl; //test

	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			heatPumps[i].vecQ_HL_hp_time.push_back(heatPumps[i].Q_HL_hp_time); //heating load for more than one year: begins at first value after each year!
			heatPumps[i].vecQ_dhw_hp_time.push_back(heatPumps[i].Q_dhw_hp_time);
			heatPumps[i].resT_soil_out.push_back(heatPumps[i].T_soil_out); //push back to result vector
			heatPumps[i].resT_soil_in.push_back(heatPumps[i].T_soil_in);
			heatPumps[i].resVecCrit_soil.push_back(heatPumps[i].crit_soil); //only for result

			heatPumps[i].vecT_st.push_back(heatPumps[i].T_st); //here or before storage balance -> how do i get t=0
			heatPumps[i].vecT_st_dhw.push_back(heatPumps[i].T_st_dhw); //before storage balance

			heatPumps[i].vecHL_Coverage.push_back(heatPumps[i].heatingLoadCoverage);
			heatPumps[i].vecDHW_Coverage.push_back(heatPumps[i].dhwLoadCoverage);

			//cout << "heatPumps[i].resQ_h.push_back(" << heatPumps[i].Q_h << ")" << "\n"; //test
			heatPumps[i].resQ_h.push_back(heatPumps[i].Q_h); //push back to result vector
					/*		if((heatPumps[i].HP_mode == 0)&&(heatPumps[i].HP_on == true)){ //heating
					 heatPumps[i].count_h++; //count_h her not good because of iteration!!
					 }*/
			//cout << "heatPumps[i].resQ_dhw.push_back(" << heatPumps[i].Q_dhw << ")" << "\n"; //test
			heatPumps[i].resQ_dhw.push_back(heatPumps[i].Q_dhw); //push back to result vector
					/*		if((heatPumps[i].HP_mode == 1)&&(heatPumps[i].HP_on == true)){ //count_dhw her not good because of iteration!!
					 heatPumps[i].count_dhw++;
					 }*/
			//cout << "heatPumps[i].resP_el.push_back(" << heatPumps[i].P_el << ")" << "\n"; //test
			heatPumps[i].resP_el.push_back(heatPumps[i].P_el);
			heatPumps[i].resQ_soil.push_back(heatPumps[i].Q_soil);
			//cout << "heatPumps[i].vecCOP.push_back(" << heatPumps[i].COP << ")" << "\n"; //test
			heatPumps[i].vecCOP.push_back(heatPumps[i].COP);

			//cout << "heatPumps[i].resHP_mode.push_back(heatPumps[i].HP_mode)" << "\n";//test
			heatPumps[i].resHP_mode.push_back(heatPumps[i].HP_mode);
			////cout << "HP_on of " << heatPumps[i].ID << " in the end = " << heatPumps[i].HP_on << "\n";
			heatPumps[i].resHP_on.push_back(heatPumps[i].HP_on);
		//Blocking
			heatPumps[i].resVecBlockHP.push_back(heatPumps[i].blockHP);
		//resQ_hSecHeatGen & resQ_dhwSecHeatGen
			heatPumps[i].resQ_hSecHeatGen.push_back(heatPumps[i].Q_hSecHeatGen); //[W]
			heatPumps[i].resQ_dhwSecHeatGen.push_back(heatPumps[i].Q_dhwSecHeatGen); //[W]

		} //end of if(heatPumps[i].simulateHP == true)

	}

}

void HpCore::pushBackYearTXT() {
	//!function to push back yearly result variables in vectors (vector size = number simulated years)

	//cout << "vecYear.size() before pushback = " << vecYear.size() << "\n";//test

	vecYear.push_back(actYear);
	//cout << "vecYear.size() after pushback = " << vecYear.size() << "\n";//test
	//cout << "actual time t = " << t << " s" << "\n";//test
	//cout << "actYear = " << actYear << "\n";//test
	//cout << "value in vecYear = " << vecYear[vecYear.size() - 1] << "\n";//test

	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			//cout << "Actual vecTime.size() in pushBackTXT() after vecTime.push_back(t); = " << vecTime.size() <<endl; //test
			double sum_HPon = 0;
			double sum_vec = 0;

	//+++ t_run +++
			for (std::vector<bool>::iterator it = heatPumps[i].resHP_on.begin();it != heatPumps[i].resHP_on.begin() + (yearVecSize); ++it)sum_HPon += *it; //test it -> or begin()+(yearVecSize)-1?? -> no -> right is: heatPumps[i].resHP_on.begin()+(yearVecSize)
			heatPumps[i].t_run = sum_HPon * dt_comm; //[s]
			//cout << "heatPumps[i].resHP_on.size() = " << heatPumps[i].resHP_on.size() << "\n"; //test
			//cout << "heatPumps[i].t_run = " << heatPumps[i].t_run << "with sum_HPon = " << sum_HPon << "\n"; //test
	/*		if(heatPumps[i].compressor_mode == 0){
				heatPumps[i].t_run = sum_HPon * dt_comm / 3600; //[h/a]
				//cout << "heatPumps[i].t_run = " << heatPumps[i].t_run << "with sum_HPon = " << sum_HPon << "\n"; //test
			}
			else if(heatPumps[i].compressor_mode == 1){
				//heatPumps[i].t_run = heatPumps[i].E_h_year / heatPumps[i].nominalPowerH + heatPumps[i].E_dhw_year / heatPumps[i].nominalPowerDHW; //[h/a]
				//cout << "heatPumps[i].t_run = " << heatPumps[i].t_run << "with sum_HPon = " << sum_HPon << "\n"; //test
			}*/
			heatPumps[i].res_t_run_year.push_back(heatPumps[i].t_run);


			/*double JAZ_WPtest = 0;
			 for (std::vector<double>::iterator it = heatPumps[i].vecCOP.begin();it != heatPumps[i].vecCOP.end(); ++it)sum_vec += *it;
			 if(sum_vec > 0){
			 JAZ_WPtest = sum_vec / sum_HPon; //wrong formula -> calculating with energies like SPF!!!
			 }
			 else{
			 JAZ_WPtest = 0;
			 }
			 cout << "JAZ_WPtest = " << JAZ_WPtest << "\n"; //test
			 //heatPumps[i].vecJAZ_WP.push_back(heatPumps[i].JAZ_WP);*/
			//sum_vec = 0;

	//+++ count_h & count_dhw +++
			//cout << "heatPumps[i].resHP_on.size() = " << heatPumps[i].resHP_on.size() << "\n"; //test
			//cout << "heatPumps[i].resHP_mode.size() = " << heatPumps[i].resHP_mode.size() << "\n"; //test
			//cout << "heatPumps[i].resHP_mode.size() = " << heatPumps[i].resHP_mode.size() << "\n"; //test
			for (unsigned int j = 0; j < yearVecSize; j++) { //test it -> check
				//cout << "j = " << j << "\n";
				if (heatPumps[i].resHP_on[j] == true) {
					//cout << "Teeeeeeeeeeeeeeeest heatPumps[i].resHP_on[j] == true " << "\n"; //test
					if (heatPumps[i].resHP_mode[j] == 0) { //heating
						heatPumps[i].count_h++;
						//cout << "Teeeeeeeeeeeeeeeest heatPumps[i].count_h = " << heatPumps[i].count_h << "\n"; //test
					} else if (heatPumps[i].resHP_mode[j] == 1) { //dhw
						heatPumps[i].count_dhw++;
						//cout << "Teeeeeeeeeeeeeeeest heatPumps[i].count_dhw = " << heatPumps[i].count_dhw << "\n"; //test
					}
				}
			}
			//cout << "heatPumps[i].count_h = " << heatPumps[i].count_h << "\n"; //test
			//cout << "heatPumps[i].count_dhw = " << heatPumps[i].count_dhw << "\n"; //test

	//+++ P_elMiddle,  P_elMiddle +++
			for (std::vector<double>::iterator it = heatPumps[i].resP_el.begin(); it != heatPumps[i].resP_el.begin() + (yearVecSize); ++it)	sum_vec += *it; //W
			heatPumps[i].P_elMiddle = sum_vec / sum_HPon;
			//cout << "heatPumps[i].P_elMiddle = " << heatPumps[i].P_elMiddle << "\n"; //test
			//double P_el_test = sum_vec / sum_HPon; //test
			heatPumps[i].E_el_year = dt_comm * sum_vec; //[J]
			heatPumps[i].resE_el_year.push_back(heatPumps[i].E_el_year);

	//+++ Qdot_hMiddle,  E_h_year +++
			sumVecH = 0;
			for (std::vector<double>::iterator it = heatPumps[i].resQ_h.begin(); it != heatPumps[i].resQ_h.begin() + (yearVecSize); ++it) sumVecH += *it; //[W]
			if(heatPumps[i].count_h > 0){
				heatPumps[i].Qdot_hMiddle = sumVecH / heatPumps[i].count_h; //[W]
			}else{
				heatPumps[i].Qdot_hMiddle = 0;
			}
			//cout << "heatPumps[i].Qdot_hMiddle = " << heatPumps[i].Qdot_hMiddle << "\n"; //test
			heatPumps[i].E_h_year = dt_comm * sumVecH; //[J] //ToDo: check if its working for inverter too -> i think so
			heatPumps[i].resE_h_year.push_back(heatPumps[i].E_h_year); //[J]

	//+++ Qdot_dhwMiddle,  E_dhw_year +++
			sumVecDHW = 0;
			for (std::vector<double>::iterator it = heatPumps[i].resQ_dhw.begin();it != heatPumps[i].resQ_dhw.begin() + (yearVecSize); ++it)sumVecDHW += *it; //W
			if(heatPumps[i].count_dhw > 0){
				heatPumps[i].Qdot_dhwMiddle = sumVecDHW / heatPumps[i].count_dhw;
			}else{
				heatPumps[i].Qdot_dhwMiddle = 0;
			}
			//cout << "heatPumps[i].Qdot_dhwMiddle = " << heatPumps[i].Qdot_dhwMiddle << "\n"; //test
			heatPumps[i].E_dhw_year = dt_comm * sumVecDHW; //[J]
			heatPumps[i].resE_dhw_year.push_back(heatPumps[i].E_dhw_year); //[J]

	//++++Full load hours heatPumps[i].runTimeFullLoad ++++
			//heatPumps[i].runTimeFullLoad = heatPumps[i].t_run * (heatPumps[i].Qdot_hMiddle / heatPumps[i].nominalPowerH * heatPumps[i].count_h / (heatPumps[i].count_h + heatPumps[i].count_dhw) + heatPumps[i].Qdot_dhwMiddle / heatPumps[i].nominalPowerDHW * heatPumps[i].count_dhw / (heatPumps[i].count_h + heatPumps[i].count_dhw) );
			//cout << "heatPumps[i].runTimeFullLoad = " << heatPumps[i].runTimeFullLoad / 3600 << " h of HP " << heatPumps[i].ID << "\n"; //test
	//+++++++++ coverage ratio +++++++++
			calcCR(i);


	//+++ JAZ +++

			heatPumps[i].E_year = heatPumps[i].E_h_year + heatPumps[i].E_dhw_year; //[J]
			heatPumps[i].resE_year.push_back(heatPumps[i].E_year);
			//SPF_WP / JAZ_WP
			if (heatPumps[i].E_el_year == 0) {
				heatPumps[i].JAZ_WP = 0;
			} else {
				heatPumps[i].JAZ_WP = heatPumps[i].E_year / heatPumps[i].E_el_year;
			}
			heatPumps[i].vecJAZ_WP.push_back(heatPumps[i].JAZ_WP);

			//SPF_WPA / JAZ_WPA
			if(heatPumps[i].operatingMode == 1){ //monoenergetic
				if (heatPumps[i].E_el_year + (heatPumps[i].t_run * heatPumps[i].P_el_soil_pump) + heatPumps[i].energySecHeatGenH + heatPumps[i].energySecHeatGenDHW == 0) { //no division 0
					heatPumps[i].JAZ_WPA = 0;
				}else{
					heatPumps[i].JAZ_WPA = (heatPumps[i].E_year + heatPumps[i].energySecHeatGenH + heatPumps[i].energySecHeatGenDHW) / (heatPumps[i].E_el_year + (heatPumps[i].t_run * heatPumps[i].P_el_soil_pump) + heatPumps[i].energySecHeatGenH + heatPumps[i].energySecHeatGenDHW);
				}
			}else{
				if (heatPumps[i].E_el_year + (heatPumps[i].t_run * heatPumps[i].P_el_soil_pump) == 0) { //no division 0
					heatPumps[i].JAZ_WPA = 0;
				} else {
					heatPumps[i].JAZ_WPA = heatPumps[i].E_year / (heatPumps[i].E_el_year + (heatPumps[i].t_run * heatPumps[i].P_el_soil_pump));
				}
			}
			heatPumps[i].vecJAZ_WPA.push_back(heatPumps[i].JAZ_WPA);
			//JAZ_WPHA = E_year / (E_el_year+ (heatPumps[i].t_run * heatPumps[i].P_el_soil_pump) + HE_d + HE_dhw_d); //ToDo: second heat generator with bivalent operation+ ToDo: (HE_g + HE_dhw_g) + HE_d + (HE_dhw_s) + HE_dhw_d ?? -> then still parse and split at buildings on WP -> much work for little change! I also don't know what exactly this involves! -> no
			//ToDo: HE_d + HE_dhw_d parsen ??
			//ToDo: vecJAZ_WPHA

	//+++ HLC and DHWcoverage +++
			sum_vec = 0;
			for (std::vector<double>::iterator it = heatPumps[i].vecHL_Coverage.begin(); it != heatPumps[i].vecHL_Coverage.begin() + (yearVecSize); ++it)sum_vec += *it;
			//cout << "sum_vec HL-Cover = " << sum_vec << "\n"; //test
			//cout << "heatPumps[i].vecHL_Coverage.size() = " << heatPumps[i].vecHL_Coverage.size() << "\n"; //test
			heatPumps[i].HLC_year = ceil( sum_vec / heatPumps[i].vecHL_Coverage.size() ); //[%]
			heatPumps[i].resVecHL_Coverage.push_back(heatPumps[i].HLC_year); //not size because of untypical time steps
			sum_vec = 0;
			for (std::vector<double>::iterator it =
					heatPumps[i].vecDHW_Coverage.begin();
					it != heatPumps[i].vecDHW_Coverage.begin() + (yearVecSize);
					++it)
				sum_vec += *it;
			heatPumps[i].DHWLC_year = ceil (sum_vec / heatPumps[i].vecDHW_Coverage.size() ); //[%]
			heatPumps[i].resVecDHW_Coverage.push_back(heatPumps[i].DHWLC_year);
	//+++ end of HLC and DHWcoverage +++


			//cout << "heatPumps[i].count_h = " << heatPumps[i].count_h << "\n"; //for testing of heating energy
			//cout << "heatPumps[i].count_dhw = " << heatPumps[i].count_dhw << "\n"; //for testing of dhw energy


			//bivalent Systems
			heatPumps[i].energySecHeatGenH = 0;
			heatPumps[i].energySecHeatGenDHW = 0;

		} //end of if(heatPumps[i].simulateHP == true)
	}

}

void HpCore::debugTXT() {
	//!optional function to create a .txt file for debugging

	cout << "HpCore::debugTXT() for actual year = " << actYear << "\n";
	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			fstream f;
			//cout << "actual year in if clause ............................. = " << actYear << "\n";
			//cout << "string fileName = ..." << "\n";//test
			string fileName;
			//fileName = "hpm/results_" + heatPumps[i].ID + "_debug" + ".txt";
			stringstream stringYear;
			stringYear << actYear;
			string stryear = stringYear.str();
			fileName = "hpm/results_" + heatPumps[i].ID + "_debug" + "_year_" + stryear + ".txt";

			f.open(fileName.c_str(), ios::out); //if existing txt: f.open(fileName.c_str(), ios::app);
			//cout << "write Actual Year = in txt" << "\n"; //test
			f << "#" << "Debug heat pump " << heatPumps[i].ID << " in year "	<< actYear << endl;
			f << "#" << "Name of heat pump: '" << heatPumps[i].HPname << "'"
					<< endl;
			f << "#" << "Linked bhe to heat pump: " << endl;
			for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
				f << "#" << heatPumps[i].linked_bhe[j] << "\n";
			}
			f << "#" << "Vdot_soil of each BHE: " << endl;
			for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
				f << "#" << heatPumps[i].Vdot_soil_bhe_init << "\n";
			}
			f << "#" << "Simulated time = " << t / 3600 << " h" << endl;
			f << "#" << "Simulated end time in years = " << t_sim_end / 3600 / 8760
					<< " years" << endl;
			f << "#" << "Summed time of this year, when heat pump was shut down because of subsurface boundary condition  = " << heatPumps[i].timeCritSoil / 3600 << " h" << endl;
			f << "#" << "BlockingTime = " << heatPumps[i].blockingTime / 3600 << " h" << endl;
			f << "#" << "Second Heat generator on during blocking time = " << heatPumps[i].secHeatGenOnInBlockingTime << " (0 = false; 1 = true)" << endl;
			f << "#" << "operatingMode = " << heatPumps[i].operatingMode << " (0=monovalent, 1=monoenergetic, 2=bivalent parallel, 3=bivalent teilparallel, 4=bivalent alternativ)"<< endl;
			f << "#" << "application range = " << heatPumps[i].app_range
					<< " (0=heating, 1=dhw, 2=heating+dhw, 3=heating+cooling, 4=heating+cooling+dhw, 5=cooling, 6=dhw+cooling)"
					<< endl;
			f << "#" << "heating storage volume = " << heatPumps[i].Vol_st * 1000
					<< " l" << ", T_st_min = " << heatPumps[i].T_st_min - 273.15
					<< " °C" << ", T_st_max = " << heatPumps[i].T_st_max - 273.15
					<< " °C" << ", dhw storage volume = "
					<< heatPumps[i].Vol_st_dhw * 1000 << " l" << ", T_st_min_dhw = "
					<< heatPumps[i].T_st_min_dhw - 273.15 << " °C"
					<< ", T_st_max_dhw = " << heatPumps[i].T_st_max_dhw - 273.15
					<< " °C" << endl;
			f << "#" << " " << endl;
			//f << "#" << "resHP_on sum_vec = " << sum_vec << endl; //test
			//f << "#" << "heatPumps[j].P_el_soil_pump = " << heatPumps[j].P_el_soil_pump << endl; //test
			//cout << "f << # << middled annual energy demand of soil pump" << "\n";//test
			f << "#" << "Run time of heat pump in this year = "	<< heatPumps[i].t_run / 3600 << " h" << endl;
			f << "#" << "Middled heating load coverage in this year = "	<< heatPumps[i].HLC_year << " %" << endl;
			f << "#" << "Middled dhw load coverage in this year = "
					<< heatPumps[i].DHWLC_year << " %" << endl;
			if ((heatPumps[i].app_range == 0) || (heatPumps[i].app_range == 3)) { //no dhw //++++heatPumps[i].app_range == 0 (heating), 3=heating+cooling++++
				f << "#" << "Middled load coverage (dhw + heating) in this year = "
						<< heatPumps[i].HLC_year << " %" << endl;
			}else if( (heatPumps[i].app_range == 1) || (heatPumps[i].app_range == 6) ){ //no heating	//++++heatPumps[i].app_range == 1 (dhw), 6=dhw+cooling++++
				f << "#" << "Middled load coverage (dhw + heating) in this year = "
						<< heatPumps[i].DHWLC_year << " %" << endl;
			}else if( (heatPumps[i].app_range == 2) || (heatPumps[i].app_range == 4) ){ //heating + dhw
				f << "#" << "Middled load coverage (dhw + heating) in this year = "
						<< (heatPumps[i].HLC_year + heatPumps[i].DHWLC_year) / 2
						<< " %" << endl;
			}

			f << "#" << "Middled coverage ratio heating of heat pump in this year = " << heatPumps[i].coverageRatioHeating << " %" << endl;
			f << "#" << "Middled coverage ratio dhw of heat pump in this year = " << heatPumps[i].coverageRatioDHW << " %" << endl;
			f << "#" << "Middled coverage ratio (heating + dhw) of heat pump in this year = " << heatPumps[i].coverageRatio << " %" << endl;
			f << "#" << "Middled coverage ratio cooling in this year = " << heatPumps[i].coveringRatioCooling << " %" << endl;

			//f << "#" << "Heating energy of heat pump in this year = " << E_h_year / 3600 / 1000 << " kWh/a" << endl;
			f << "#" << "Middled heating power of heat pump = " << heatPumps[i].Qdot_hMiddle << " W" << endl;
			//f << "#" << "heatPumps[i].count_h = " <<  heatPumps[i].count_h << endl; //test
			//f << "#" << "Dhw energy of heat pump in this year = " << E_dhw_year / 1000 / 3600 << " kWh/a" << endl;
			f << "#" << "Middled dhw power of heat pump = " << heatPumps[i].Qdot_dhwMiddle << " W" << endl;
			//f << "#" << "heatPumps[i].count_dhw = " <<  heatPumps[i].count_dhw << endl; //test
			heatPumps[i].Qdot_soilMiddle = (sumVecH + sumVecDHW) / (heatPumps[i].count_dhw + heatPumps[i].count_h) - heatPumps[i].P_elMiddle;
			f << "#" << "Middled soil heat flux to heat pump = "
					<< heatPumps[i].Qdot_soilMiddle << " W" << endl;
			//f << "#" << "CountHPon = " << (heatPumps[i].t_run * 3600 / dt_comm) << " = " << heatPumps[i].count_dhw + heatPumps[i].count_h  << endl; //test
			//f << "#" << "Energy of heating and dhw of heat pump in this year = " <<  E_year / 1000 / 3600 << " kWh/a" << endl;
			//f << "#" << "Electrical energy demand of heat pump in this year = " <<  E_el_year / 1000 / 3600 << " kWh/a" << endl;
			//f << "#" << "Test test P_el_test = " <<  P_el_test << " W" << endl; //test
			f << "#" << "Electrical energy demand of soil pump in this year = "
					<< heatPumps[i].t_run / 3600 * heatPumps[i].P_el_soil_pump / 1000 << " kWh/a" << endl;

			f << "#"
					<< "Seasonal Performance Factor (SPF_WP) in this year (only heat pump) = "
					<< heatPumps[i].JAZ_WP << endl; //JAZ_WP;
			f << "#"
					<< "Seasonal Performance Factor (SPF_WPA) in this year (heat pump + soil pump + second electric heat generator) = "
					<< heatPumps[i].JAZ_WPA << endl; //JAZ_WPA = E_H / (E_el_HP + E_el_soil_pump + HE_g);

			f << "#" << " " << endl;
			f << "#" << " " << endl;
			//...
			f << "#" << " " << endl;
			f << "#" << "t = time [s]" << endl;
			f << "#" << "T_s_out = brine temperature out of soil [°C]" << endl;
			f << "#" << "T_crit_s? = Is soil temperature of subsurface model < critical soil temperature? -> 0 = No -> HP = on; 1 = Yes -> HP = off; " << endl;
			f << "#" << "Q_HL = heating load of all buildings linked to heat pump [W]" << endl;
			f << "#" << "Q_dhw = domestic hot water (dhw) load of all buildings linked to heat pump [W]" << endl;
			f << "#" << "HP_on = Is heat pump on? -> 0 = off; 1 = on" << endl;
			f << "#" << "HP_mode = Mode of heat pump -> 0 = heating; 1 = dhw" << endl;
			f << "#" << "Q_h = heat pump heating power depending on T_s_out [W]" << endl;
			f << "#" << "hLC(t) = heating load coverage of heat pump [%]" << endl;
			f << "#" << "Q_dhw = heat pump dhw power depending on T_s_out [W]" << endl;
			f << "#" << "dhwLC(t) = dhw load coverage of heat pump [%]" << endl;
			f << "#" << "P_el = heat pump electric power depending on T_s_out [W]" << endl;
			f << "#" << "COP* = ratio of heating power and electric power of heat pump in actual mode and time step" << endl;
			f << "#" << "T_st = heating storage temperature in [°C]" << endl;
			f << "#" << "T_st_dhw = domestic hot water (dhw) storage temperature in [°C]" << endl;
			f << "#" << "T_s_in = brine inlet temperature -> into soil [°C]" << endl;
			f << "#" << "dT = temperature difference (T_S_out - T_s_in)  [K]" << endl;
			f << "#" << "hBlock? = Is in this time step a blocking hour of HP? -> 0 = No; 1 = Yes -> HP = off; " << endl;
			f << "#" << "Q_hSecHGen = SecHeatGen heating power depending on T_s_out [W] " << endl;
			f << "#" << "Q_dhwSecHGen = SecHeatGen dhw power depending on T_s_out [W] " << endl;
			//...
			f << "#" << " " << endl;
			////cout << "if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){" << "\n";//test
			//if((heatPumps[i].Vol_st > 0) && (heatPumps[i].Vol_st_dhw > 0)){
			//cout << "write in txt t, T_s_out, ... " << "\n";//test
			f << "#" << "t:" << "	" << "T_s_out:" << "	" << "T_crit_s?" << "	"
					<< "Q_HL:" << "	" << "Q_dhw:" << "	" << "HP_on:" << "	" << "HP_mode:" << "	"
					<< "Q_h:" << "	" << "hLC(t):" << "	" << "Q_dhw:" << "	" << "dhwLC(t):" << "	"
					<< "P_el:" << "	" << "COP*:" << "	"
					<< "T_st:" << "	" << "T_st_dhw:" << "	"
					<< "T_s_in:" << "	" << "dT:" << "	" << "hBlock?" << "	" //<< "\n";
					<< "Q_hSecHGen" << "	"  //only for testing
					<< "Q_dhwSecHGen" << "\n";//only for testing
			cout << "DEBUG::Teeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeest ... "	<< "\n"; //test
			//cout << "vecTime.size() = " <<  vecTime.size() << "\n";		//test
			for (unsigned int j = 0; j < vecTime.size(); j++) {
				//cout << "write vectors in vecTime, ... " << "\n";//test
				f << setprecision(10) << vecTime[j] << "	"
						<< setprecision(2) << heatPumps[i].resT_soil_out[j] - 273.15 << "	"
						<< heatPumps[i].resVecCrit_soil[j] << "	"
						<< setprecision(5) << heatPumps[i].vecQ_HL_hp_time[j] << "	"
						<< setprecision(5) << heatPumps[i].vecQ_dhw_hp_time[j] << "	"
						<< heatPumps[i].resHP_on[j] << "	"
						<< heatPumps[i].resHP_mode[j] << "	"
						<< setprecision(6) << heatPumps[i].resQ_h[j] << "	"
						<< heatPumps[i].vecHL_Coverage[j] << "	"
						<< setprecision(6) << heatPumps[i].resQ_dhw[j] << "	"
						<< heatPumps[i].vecDHW_Coverage[j] << "	"
						<< setprecision(6) << heatPumps[i].resP_el[j] << "	"
						<< setprecision(5) << heatPumps[i].vecCOP[j] << "	"
						<< setprecision(3) << heatPumps[i].vecT_st[j] - 273.15 << "	"
						<< setprecision(3) << heatPumps[i].vecT_st_dhw[j] - 273.15 << "	"
						<< setprecision(2) << heatPumps[i].resT_soil_in[j] - 273.15 << "	"
						<< setprecision(2) << heatPumps[i].resT_soil_out[j] - heatPumps[i].resT_soil_in[j] << "	"
						<< heatPumps[i].resVecBlockHP[j] << "	" //<< "\n";
						<< setprecision(6) << heatPumps[i].resQ_hSecHeatGen[j] << "	" //only for testing
						<< setprecision(6) << heatPumps[i].resQ_dhwSecHeatGen[j] << "\n"; //only for testing
			}
			f.close();

		} // end of if(heatPumps[i].simulateHP == true)

	}
	cout << "DEBUG::end of debug "	<< "\n"; //test
}

//void HpCore::clearVectors() { //not necessary -> because of saveOverhang
//	//cout << "clear vectors" << "\n"; //test
//	vecTime.clear();
//
//	for (unsigned int i = 0; i < heatPumps.size(); i++) {
//		if(heatPumps[i].simulateHP == true){
//			heatPumps[i].resHP_on.clear();
//			heatPumps[i].vecCOP.clear();
//			heatPumps[i].vecHL_Coverage.clear();
//			heatPumps[i].vecDHW_Coverage.clear();
//			heatPumps[i].resP_el.clear();
//			heatPumps[i].resQ_h.clear();
//			heatPumps[i].resQ_dhw.clear();
//			heatPumps[i].resQ_soil.clear();
//			heatPumps[i].resT_soil_out.clear();
//			heatPumps[i].vecT_st.clear();
//			heatPumps[i].vecT_st_dhw.clear();
//			heatPumps[i].vecQ_HL_hp_time.clear();
//			heatPumps[i].vecQ_dhw_hp_time.clear();
//			heatPumps[i].resHP_mode.clear();
//			heatPumps[i].resT_soil_in.clear();
//			heatPumps[i].resVecCrit_soil.clear();
//			heatPumps[i].resVecBlockHP.clear();
//		} //end of if(heatPumps[i].simulateHP == true){
//	}
//
//}

void HpCore::makeTXTend() {
	//!function to make .txt result files

	//std::fstream("results.txt", std::ios::out);

	fstream f;

	cout << "HpCore::makeTXTend()" << "\n";//test
	//cout << "endYear = " << endYear << "\n";//test

//+++* test validation +++*
	/*

	 //plot data out of "SeW_Nov2016_validation.txt" and "results_validation.txt" in one plot
	 f.open("hpm/results_validation.txt", ios::out);
	 f << "Sequence: " << endl;
	 for(int i =0; i < Phy.all_linked_bhe.size(); i++){
	 f << Phy.all_linked_bhe[i] << "\n";
	 }
	 f << "Vdot_soil of sequence: " << endl;
	 for(int i =0; i < Phy.VecV_soil_bhe_nominal.size(); i++){
	 f << Phy.VecV_soil_bhe_nominal[i] << "\n";
	 }
	 for(int j =0; j < heatPumps.size(); j++){
	 double sum_vec = 0;
	 for(std::vector<bool>::iterator it = heatPumps[j].resHP_on.begin(); it != heatPumps[j].resHP_on.end(); ++it) sum_vec += *it;
	 heatPumps[j].t_run = sum_vec * dt_comm / 3600; //[h]
	 f << "#" << heatPumps[j].ID << ", storage volume = " << heatPumps[j].Vol_st << " m³" << ", T_st_min = " << heatPumps[j].T_st_min << " K" << ":" << ", T_st_max = " << heatPumps[j].T_st_max << " K" << ":" << ", Run time of heat pump = " << heatPumps[j].t_run << " h" << ":" << "\n";
	 f << "#" << "t:" << " " << "T_soil_out(t):" << " " << "T_soil_in:" << " " << "deltaT_soil" << " " << "Q_h(T_soil_out):" << " " << "P_el(T_soil_out):" << "\n";
	 for(int i = 0; i < heatPumps[j].resT_soil_out.size(); i++){
	 double deltaT_soil = heatPumps[j].resT_soil_out[i] - heatPumps[j].resT_soil_in[i];
	 f << vecTime[i] << "	"  << heatPumps[j].resT_soil_out[i] - 273.15 << "	 " << heatPumps[j].resT_soil_in[i] - 273.15 << "	 " << deltaT_soil << "	" << heatPumps[j].resQ_h[i] << "	" << heatPumps[j].resP_el[i]  << "\n";
	 }
	 f << " " << "\n";
	 }
	 f.close();
	 */

//+++* end test validation +++*
//+++++ txt_year -> ONE txt for results of vec_year!!
	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			string fileName;
			fileName = "hpm/results_" + heatPumps[i].ID + ".txt"; //
			//cout << "f.open(fileName.c_str(), ios::out);" << "\n";//test
			f.open(fileName.c_str(), ios::out); //if existing txt: f.open(fileName.c_str(), ios::app);
			double sum_vec = 0;
			for (std::vector<double>::iterator it = heatPumps[i].res_t_run_year.begin(); it != heatPumps[i].res_t_run_year.end(); ++it) sum_vec += *it;
			heatPumps[i].t_runMiddled = sum_vec / heatPumps[i].res_t_run_year.size();
			sum_vec = 0;
			for (std::vector<double>::iterator it =	heatPumps[i].vecCoverageRatioHeating.begin();it != heatPumps[i].vecCoverageRatioHeating.end(); ++it)sum_vec += *it;
			heatPumps[i].middledHCR = round( sum_vec / heatPumps[i].vecCoverageRatioHeating.size() ); //ToDo: check if its working for inverter too
			sum_vec = 0;
			for (std::vector<double>::iterator it =	heatPumps[i].vecCoverageRatioDHW.begin();it != heatPumps[i].vecCoverageRatioDHW.end(); ++it)sum_vec += *it;
			heatPumps[i].middledDHWCR = round( sum_vec / heatPumps[i].vecCoverageRatioDHW.size() );
			sum_vec = 0;
			for (std::vector<double>::iterator it =	heatPumps[i].vecCoverageRatio.begin();it != heatPumps[i].vecCoverageRatio.end(); ++it)sum_vec += *it;
			heatPumps[i].middledCR = round( sum_vec / heatPumps[i].vecCoverageRatio.size() );
			sum_vec = 0;
			for (std::vector<double>::iterator it =	heatPumps[i].vecCoverageRatioCooling.begin();it != heatPumps[i].vecCoverageRatioCooling.end(); ++it)sum_vec += *it;
			heatPumps[i].middledCRcooling = round( sum_vec / heatPumps[i].vecCoverageRatioCooling.size() );
			sum_vec = 0;
			for (std::vector<double>::iterator it =	heatPumps[i].vecCoverageRatioSecHeatGen.begin();it != heatPumps[i].vecCoverageRatioSecHeatGen.end(); ++it)sum_vec += *it; //test
			heatPumps[i].middledCRsecHeatGen = round( sum_vec / heatPumps[i].vecCoverageRatioSecHeatGen.size() ); //test

			//cout << "test heatPumps[i].vecCoverageRatioSecHeatGen of HP " << heatPumps[i].ID << "\n"; //test
			//for(vector<double>::iterator vit=heatPumps[i].vecCoverageRatioSecHeatGen.begin();vit!=heatPumps[i].vecCoverageRatioSecHeatGen.end();++vit) cout << *vit << "\n"; //test

		/*	sum_vec = 0; //not used
			for (std::vector<double>::iterator it = heatPumps[i].resE_h_year.begin(); it != heatPumps[i].resE_h_year.end(); ++it) sum_vec += *it;
			double E_heating_middled = sum_vec / endYear; //ToDo: unused variable
			sum_vec = 0;
			for (std::vector<double>::iterator it = heatPumps[i].resE_dhw_year.begin(); it != heatPumps[i].resE_dhw_year.end(); ++it) sum_vec += *it;
			double E_dhw_middled = sum_vec / endYear;
			sum_vec = 0;
			for (std::vector<double>::iterator it = heatPumps[i].resE_year.begin(); it != heatPumps[i].resE_year.end(); ++it) sum_vec += *it;
			double E_heating_dhw_middled = sum_vec / endYear;
			//E_heating_dhw_middled = E_heating_middled + E_dhw_middled; //test
			sum_vec = 0;
			for (std::vector<double>::iterator it = heatPumps[i].resE_el_year.begin(); it != heatPumps[i].resE_el_year.end(); ++it) sum_vec += *it;
			double E_el_middled = sum_vec / endYear;*/
			sum_vec = 0;
			for (std::vector<double>::iterator it = heatPumps[i].vecJAZ_WP.begin(); it != heatPumps[i].vecJAZ_WP.end(); ++it) sum_vec += *it;
			heatPumps[i].JAZ_WPmiddled = sum_vec / endYear;
			sum_vec = 0;
			for (std::vector<double>::iterator it = heatPumps[i].vecJAZ_WPA.begin(); it != heatPumps[i].vecJAZ_WPA.end(); ++it)sum_vec += *it;
			heatPumps[i].JAZ_WPAmiddled = sum_vec / endYear;

			f << "#" << "Results of heat pump " << heatPumps[i].ID << endl;
			f << "#" << "Name of heat pump: '" << heatPumps[i].HPname << "'"
					<< endl;
			f << "#" << "Heating energy of linked buildings per year: " << heatPumps[i].buildEnergyHeating  /1000 /3600 << " kWh" << endl;
			f << "#" << "DHW energy of linked buildings per year: " << heatPumps[i].buildEnergyDHW /1000 /3600 << " kWh" << endl;
			f << "#" << "Linked bhe to heat pump: " << endl;
			for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
				f << "#" << heatPumps[i].linked_bhe[j] << "\n";
			}
			f << "#" << "Vdot_soil of each BHE: " << endl;
			for (unsigned int j = 0; j < heatPumps[i].linked_bhe.size(); j++) {
				f << "#" << heatPumps[i].Vdot_soil_bhe_init << "\n";
			}
			f << "#" << "Simulated time = " << t / 3600 << " h" << endl;
			f << "#" << "Simulated end time in years = " << t_sim_end / 3600 / 8760
					<< " years" << endl;
			f << "#" << "BlockingTime = " << heatPumps[i].blockingTime / 3600 << " h" << endl;
			f << "#" << "Second Heat generator on during blocking time = " << heatPumps[i].secHeatGenOnInBlockingTime << " (0 = false; 1 = true)" << endl;
			f << "#" << "operating mode = " << heatPumps[i].operatingMode
							<< " (0=monovalent, 1=monoenergetic, 2=bivalent parallel, 3=bivalent teilparallel, 4=bivalent alternativ)"
							<< endl;
			f << "#" << "application range = " << heatPumps[i].app_range
					<< " (0=heating, 1=dhw, 2=heating+dhw, 3=heating+cooling, 4=heating+cooling+dhw, 5=cooling, 6=dhw+cooling)"
					<< endl;
			f << "#" << "heating storage volume = " << heatPumps[i].Vol_st * 1000
					<< " l" << ", T_st_min = " << heatPumps[i].T_st_min - 273.15
					<< " °C" << ", T_st_max = " << heatPumps[i].T_st_max - 273.15
					<< " °C" << ", dhw storage volume = "
					<< heatPumps[i].Vol_st_dhw * 1000 << " l" << ", T_st_min_dhw = "
					<< heatPumps[i].T_st_min_dhw - 273.15 << " °C"
					<< ", T_st_max_dhw = " << heatPumps[i].T_st_max_dhw - 273.15
					<< " °C" << endl;
			//...
			//...
			f << "#" << endl;
			f << "#" << "Middled run time of heat pump over all years = " << heatPumps[i].t_runMiddled / 3600 << " h" << endl;
			f << "#" << "Summed time over all years, when heat pump was shut down because of subsurface boundary condition  = " << heatPumps[i].timeCritSoil / 3600 << " h" << endl;
			f << "#" << "Middled heating coverage ratio over all years = " << heatPumps[i].middledHCR << " %" << endl;
			f << "#" << "Middled dhw coverage ratio over all years = " << heatPumps[i].middledDHWCR << " %" << endl;
			f << "#" << "Middled coverage ratio (dhw + heating) over all years = " << heatPumps[i].middledCR << " %" << endl;
			f << "#" << "Middled coverage ratio (dhw + heating) of second heat generator over all years = " << heatPumps[i].middledCRsecHeatGen << " %" << endl;
			f << "#" << "Middled coverage ratio cooling over all years = " << heatPumps[i].middledCRcooling << " %" << endl;

			//f << "#" << "Middled heating energy of heat pump over all years = " << E_heating_middled << " kWh/a" << endl;
			//f << "#" << "Middled dhw energy of heat pump over all years = " << E_dhw_middled << " kWh/a" << endl;
			//f << "#" << "Middled energy of heating and dhw over all years = " << E_heating_dhw_middled << " kWh/a" << endl;
			//f << "#" << "Middled electric energy of heat pump over all years = " << E_el_middled << " kWh/a" << endl;
			f << "#"
					<< "Middled Seasonal Performance Factor (SPF_WP) of heat pump over all years = "
					<< heatPumps[i].JAZ_WPmiddled << endl;
			//f << "#" << "Test test SCOP = E_heating_dhw_middled / E_el_middled = " << E_heating_dhw_middled / E_el_middled << endl; //test
			f << "#" << "Middled energy demand of soil pump over all years = "
					<< heatPumps[i].t_runMiddled / 3600 * heatPumps[i].P_el_soil_pump / 1000 << " kWh/a" << endl;
			f << "#"
					<< "Middled Seasonal Performance Factor (SPF_WPA) of heat pump including soil pump and second heat generator over all years = "
					<< heatPumps[i].JAZ_WPAmiddled << endl;
			//...
			f << "#" << " " << endl;
			f << "#" << "y = year [a]" << endl;
			f << "#" << "t_run = run time of HP [h]" << endl;
			f << "#" << "E_h = Heating energy of heat pump in this year [kWh]"
					<< endl;
			f << "#" << "HCR = annual heating coverage ratio of heat pump [%]" << endl;
			//f << "#" << "hLC = annual middled heating load coverage of heat pump [%]" << endl;
			f << "#" << "HCRsecHG = annual heating coverage ratio of second heat generator[%]" << endl;
			f << "#" << "E_dhw = Dhw energy of heat pump in this year [kWh]" << endl;
			f << "#" << "dhwCR = annual DHW coverage ratio of heat pump [%]" << endl;
			//f << "#" << "dhwLC = annual middled dhw load coverage of heat pump [%]" << endl;
			f << "#" << "dhwCRsecHG = annual dhw coverage ratio of second heat generator[%]" << endl;
			f << "#" << "CR = annual coverage ratio (heating + dhw) of heat pump [%]" << endl;
			f << "#" << "CRsecHG = annual coverage ratio (heating + dhw) of second heat generator[%]" << endl;
			f << "#"
					<< "E_h+dhw = Energy of heating and dhw of heat pump in this year [kWh]"
					<< endl;
			f << "#"
					<< "E_el = Electrical energy demand of heat pump in this year [kWh]"
					<< endl;
			f << "#"
					<< "SPF = annual middled Seasonal Performance Factor (SPF) (heating+dhw)"
					<< endl;
			//f << "#" << "E_sPump = Energy demand of soil pump in this year [kWh]" << endl;
			f << "#"
					<< "SPF_WPA = annual middled Seasonal Performance Factor (SPF) (heating+dhw) including soil pump and second heat generator"
					<< endl;
			//f << "#" << "dT_s = annual middled brine temperature difference (out - in) [K]" << endl;
			// ...
			f << "#" << " " << endl;
			f << "#" << " " << endl;
			//cout << "write in txt t, T_s_out, ... " << "\n";//test
	/*
			cout << "heatPumps[i].vecCoverageRatio.size() = " << heatPumps[i].vecCoverageRatio.size() << "\n"; //test
			cout << "heatPumps[i].vecCoverageRatioHeating.size() = " << heatPumps[i].vecCoverageRatioHeating.size() << "\n"; //test
			cout << "heatPumps[i].vecCoverageRatioDHW.size() = " << heatPumps[i].vecCoverageRatioDHW.size() << "\n"; //test
			cout << "heatPumps[i].vecCoverageRatioSecHeatGen.size() = " << heatPumps[i].vecCoverageRatioSecHeatGen.size() << "\n"; //test
			cout << "heatPumps[i].vecCoverageRatioHeatingSecHeatGen.size() = " << heatPumps[i].vecCoverageRatioHeatingSecHeatGen.size() << "\n"; //test
			cout << "heatPumps[i].vecCoverageRatioDHWsecHeatGen.size() = " << heatPumps[i].vecCoverageRatioDHWsecHeatGen.size() << "\n"; //test
	*/

			f << "#" << "y:" << "	" << "t_run:" << "	" << "E_h:" << "	" << "HCR:" << "	" << "HCRsecHG:" //<< "	" << "hLC:"
					<< "	" << "E_dhw:" << "	" << "dhwCR:" << "	" << "dhwCRsecHG:" << "	" //<< "dhwLC:" << "	"
					<< "E_h+dhw:" << "	"
					<< "CR:" << "	"
					//<< "H+dhwLC:" << "	"
					<< "CRsecHG:" << "	"
					<< "CRcool:" << "	"
					<< "E_el:" << "	" << "SPF_WP:" << "	"
					//<< "E_sPump:" << "	"
					<< "SPF_WPA:" << "\n";
			for (unsigned int j = 0; j < vecYear.size(); j++) {
				//cout << "vecYear.size() ... " << vecYear.size() << "\n";//test
				//cout << "write vectors in vecTime, ... " << "\n";//test
				f << vecYear[j] << "	" << heatPumps[i].res_t_run_year[j] / 3600 << "	"
						<< heatPumps[i].resE_h_year[j] /1000 /3600 << "	"
						<< heatPumps[i].vecCoverageRatioHeating[j] << "	"
						<< heatPumps[i].vecCoverageRatioHeatingSecHeatGen[j] << "	"
						//<< heatPumps[i].resVecHL_Coverage[j] << "	"
						<< heatPumps[i].resE_dhw_year[j] /1000 /3600 << "	"
						<< heatPumps[i].vecCoverageRatioDHW[j] << "	"
						<< heatPumps[i].vecCoverageRatioDHWsecHeatGen[j] << "	"
						//<< heatPumps[i].resVecDHW_Coverage[j] << "	"
						<< heatPumps[i].resE_year[j] /1000 /3600 << "	"
						<< heatPumps[i].vecCoverageRatio[j] << "	"
						<< heatPumps[i].vecCoverageRatioSecHeatGen[j] << "	"
						<< heatPumps[i].vecCoverageRatioCooling[j] << "	"
						<< heatPumps[i].resE_el_year[j] /1000 /3600 << "	"
						<< heatPumps[i].vecJAZ_WP[j] << "	"
						<< heatPumps[i].vecJAZ_WPA[j] << "\n";
			}
			f.close();
		}// end of if(heatPumps[i].simulateHP == true){
	}




}

void HpCore::calcCR(int i){
	//!function to calculate yearly covering ratio (heating and dhw) of Heat pump and second heat generator (in case of bivalent systems)

	if(heatPumps[i].operatingMode == 0){//monovalent
		//+++heatPumps[i].coverageRatioHeating+++
		if(heatPumps[i].buildEnergyHeating > 0){
			//Coverage via energy difficult, because HP provides more energy than building needs, because storage losses are added and energy that is not taken into account because T_max is exceeded! -> Storage losses simplified for 365 days considered
			heatPumps[i].coverageRatioHeating = round(heatPumps[i].E_h_year / (heatPumps[i].buildEnergyHeating + 365 * 24 * 3600 * heatPumps[i].losses_st) * 100 ) ; //[J/J]
			//heatPumps[i].coverageRatioHeating = ceil( heatPumps[i].E_h_year / heatPumps[i].buildEnergyHeating * 100 );
			if(heatPumps[i].coverageRatioHeating >= 100){ //ToDo: delete?
				heatPumps[i].coverageRatioHeating = 100;
			}else if(heatPumps[i].coverageRatioHeating <= 0){ //ToDo: delete?
				heatPumps[i].coverageRatioHeating = 0;
			}
		}
		else{
			heatPumps[i].coverageRatioHeating = 0;
		}

		heatPumps[i].vecCoverageRatioHeating.push_back(heatPumps[i].coverageRatioHeating);

		//+++ coverage ratio dhw +++
		if(heatPumps[i].buildEnergyDHW > 0){
			heatPumps[i].coverageRatioDHW = round(heatPumps[i].E_dhw_year / (heatPumps[i].buildEnergyDHW + 365 * 24 * 3600 * heatPumps[i].losses_st_dhw) * 100); //[J/J]
			//heatPumps[i].coverageRatioDHW = round(heatPumps[i].E_dhw_year / heatPumps[i].buildEnergyDHW * 100); //[J/J]
			if(heatPumps[i].coverageRatioDHW >= 100){
				heatPumps[i].coverageRatioDHW = 100;
			}else if(heatPumps[i].coverageRatioDHW <= 0){
				heatPumps[i].coverageRatioDHW = 0;
			}
		}
		else{
			heatPumps[i].coverageRatioDHW = 0;
		}

		heatPumps[i].vecCoverageRatioDHW.push_back(heatPumps[i].coverageRatioDHW);

		//+++ coverage ratio (h + dhw) +++
		if ((heatPumps[i].app_range == 0) || (heatPumps[i].app_range == 3)) { //no dhw
			heatPumps[i].coverageRatio = heatPumps[i].coverageRatioHeating;
		} else if( (heatPumps[i].app_range == 1) || (heatPumps[i].app_range == 6) ){ //no heating	//++++heatPumps[i].app_range == 1 (dhw), 6=dhw+cooling++++
			heatPumps[i].coverageRatio = heatPumps[i].coverageRatioDHW;
		} else if( (heatPumps[i].app_range == 2) || (heatPumps[i].app_range == 4) ){ // dhw + heating
			if( (heatPumps[i].buildEnergyDHW + heatPumps[i].buildEnergyHeating) > 0){
				//heatPumps[i].coverageRatio = round( (heatPumps[i].E_dhw_year + heatPumps[i].E_h_year) / (heatPumps[i].buildEnergyDHW  + heatPumps[i].buildEnergyHeating + 365 * 24 * heatPumps[i].losses_st_dhw / 1000 + 365 * 24 * heatPumps[i].losses_st / 1000) * 100); // both a= and B) nearly the same
				heatPumps[i].coverageRatio = round( (heatPumps[i].E_dhw_year + heatPumps[i].E_h_year) / (heatPumps[i].buildEnergyDHW + 365 * 24 * 3600 * heatPumps[i].losses_st + heatPumps[i].buildEnergyHeating + 365 * 24 * 3600 * heatPumps[i].losses_st_dhw) * 100);
				if(heatPumps[i].coverageRatio >= 100){ //ToDo: delete this function for debugging??
					heatPumps[i].coverageRatio = 100;
				}
				else if(heatPumps[i].coverageRatio <= 0){
					heatPumps[i].coverageRatio = 0;
				}
			}else{
				heatPumps[i].coverageRatio = 0;
			}

		}
		heatPumps[i].vecCoverageRatio.push_back(heatPumps[i].coverageRatio);

		heatPumps[i].coverageRatioHeatingSecHeatGen = 0;
		heatPumps[i].coverageRatioDHWsecHeatGen = 0;
		heatPumps[i].coverageRatioSecHeatGen = 0;
		heatPumps[i].vecCoverageRatioHeatingSecHeatGen.push_back(heatPumps[i].coverageRatioHeatingSecHeatGen);
		heatPumps[i].vecCoverageRatioDHWsecHeatGen.push_back(heatPumps[i].coverageRatioDHWsecHeatGen);
		heatPumps[i].vecCoverageRatioSecHeatGen.push_back(heatPumps[i].coverageRatioSecHeatGen);


	}else{
	//+++bivalent//

	//+++heatPumps[i].coverageRatioHeating+++
		if(heatPumps[i].buildEnergyHeating > 0){
			//Coverage via energy -> HP provides more energy than the building needs, since storage losses are added!
			//heatPumps[i].coverageRatioHeatingSecHeatGen = round(heatPumps[i].energySecHeatGenH / (heatPumps[i].buildEnergyHeating + 8760 * 3600 * heatPumps[i].losses_st) * 100) ; //[J/J]
			heatPumps[i].coverageRatioHeatingSecHeatGen = round(heatPumps[i].energySecHeatGenH / (heatPumps[i].buildEnergyHeating + 365 * 24 * 3600 * heatPumps[i].losses_st) * 100) ; //[J/J]
			if(heatPumps[i].coverageRatioHeatingSecHeatGen >= 100){
				heatPumps[i].coverageRatioHeatingSecHeatGen = 100;
			}else if(heatPumps[i].coverageRatioHeatingSecHeatGen <= 0){
				heatPumps[i].coverageRatioHeatingSecHeatGen = 0;
			}
			//heatPumps[i].//heatPumps[i].coverageRatioDHW = round(heatPumps[i].E_h_year / (heatPumps[i].buildEnergyHeating + 8760 * 3600 * heatPumps[i].losses_st_dhw) * 100); //[J/J]
			heatPumps[i].coverageRatioHeating = round(heatPumps[i].E_h_year / (heatPumps[i].buildEnergyHeating + 365 * 24 * 3600 * heatPumps[i].losses_st) * 100) ; //[J/J]
			if(heatPumps[i].coverageRatioHeating >= 100){
				heatPumps[i].coverageRatioHeating = 100;
			}else if(heatPumps[i].coverageRatioHeating <= 0){
				heatPumps[i].coverageRatioHeating = 0;
			}
			if( (heatPumps[i].coverageRatioHeatingSecHeatGen + heatPumps[i].coverageRatioHeating) > 100 ){ //delete this funktion for debugging??
				//heatPumps[i].coverageRatioSecHeatGen = 100 - heatPumps[i].coverageRatio;
				heatPumps[i].coverageRatioHeating = 100 - heatPumps[i].coverageRatioHeatingSecHeatGen;
			}
		}
		else{
			heatPumps[i].coverageRatioHeating = 0;
			heatPumps[i].coverageRatioHeatingSecHeatGen = 0;
		}
		heatPumps[i].vecCoverageRatioHeating.push_back(heatPumps[i].coverageRatioHeating);
		heatPumps[i].vecCoverageRatioHeatingSecHeatGen.push_back(heatPumps[i].coverageRatioHeatingSecHeatGen);

	//+++ coverage ratio dhw +++
		if(heatPumps[i].buildEnergyDHW > 0){
			//heatPumps[i].coverageRatioDHWsecHeatGen = round(heatPumps[i].energySecHeatGenDHW / (heatPumps[i].buildEnergyDHW + 365 * 24 * heatPumps[i].losses_st_dhw / 1000) * 100 ); //[kWh/kWh]
			heatPumps[i].coverageRatioDHWsecHeatGen = round(heatPumps[i].energySecHeatGenDHW / (heatPumps[i].buildEnergyDHW + 365 * 24 * 3600 * heatPumps[i].losses_st_dhw) * 100 ); //[kWh/kWh]
			if(heatPumps[i].coverageRatioDHWsecHeatGen >= 100){
				heatPumps[i].coverageRatioDHWsecHeatGen = 100;
			}else if(heatPumps[i].coverageRatioDHWsecHeatGen <= 0){
				heatPumps[i].coverageRatioDHWsecHeatGen = 0;
			}
			//heatPumps[i].coverageRatioDHW = round(heatPumps[i].E_dhw_year / (heatPumps[i].buildEnergyDHW + 365 * 24 * heatPumps[i].losses_st_dhw / 1000) * 100); //[kWh/kWh]
			heatPumps[i].coverageRatioDHW = round(heatPumps[i].E_dhw_year / (heatPumps[i].buildEnergyDHW + 365 * 24 * 3600 * heatPumps[i].losses_st_dhw) * 100); //[kWh/kWh]
			if(heatPumps[i].coverageRatioDHW >= 100){
				heatPumps[i].coverageRatioDHW = 100;
			}else if(heatPumps[i].coverageRatioDHW <= 0){
				heatPumps[i].coverageRatioDHW = 0;
			}
			if( (heatPumps[i].coverageRatioDHWsecHeatGen + heatPumps[i].coverageRatioDHW) > 100 ){ //delete this function for debugging??
				//heatPumps[i].coverageRatioSecHeatGen = 100 - heatPumps[i].coverageRatio;
				heatPumps[i].coverageRatioDHW = 100 - heatPumps[i].coverageRatioDHWsecHeatGen;
			}
		}
		else{
			heatPumps[i].coverageRatioDHW = 0;
			heatPumps[i].coverageRatioDHWsecHeatGen = 0;
		}

		heatPumps[i].vecCoverageRatioDHW.push_back(heatPumps[i].coverageRatioDHW);
		heatPumps[i].vecCoverageRatioDHWsecHeatGen.push_back(heatPumps[i].coverageRatioDHWsecHeatGen);

		//+++ coverage ratio (h + dhw) +++
		if ((heatPumps[i].app_range == 0) || (heatPumps[i].app_range == 3)) { //no dhw
			heatPumps[i].coverageRatio = heatPumps[i].coverageRatioHeating;
			heatPumps[i].coverageRatioSecHeatGen = heatPumps[i].coverageRatioHeatingSecHeatGen;
		} else if( (heatPumps[i].app_range == 1) || (heatPumps[i].app_range == 6) ){ //no heating	//++++heatPumps[i].app_range == 1 (dhw), 6=dhw+cooling++++
			heatPumps[i].coverageRatio = heatPumps[i].coverageRatioDHW;
			heatPumps[i].coverageRatioSecHeatGen = heatPumps[i].coverageRatioDHWsecHeatGen;
		} else if( (heatPumps[i].app_range == 2) || (heatPumps[i].app_range == 4) ){ // dhw + heating
			if( (heatPumps[i].buildEnergyDHW + heatPumps[i].buildEnergyHeating) > 0){

				//cout << "heatPumps[i].energySecHeatGenH = " << heatPumps[i].energySecHeatGenH << " J" << "\n"; //test
				//cout << "heatPumps[i].buildEnergyHeating = " << heatPumps[i].buildEnergyHeating << " J" << "\n"; //test
				//cout << "heatPumps[i].energySecHeatGenDHW = " << heatPumps[i].energySecHeatGenDHW << " J" << "\n"; //test
				//cout << "heatPumps[i].buildEnergyDHW = " << heatPumps[i].buildEnergyDHW << " J" << "\n"; //test
				//heatPumps[i].coverageRatioSecHeatGen = round( (heatPumps[i].energySecHeatGenDHW + heatPumps[i].energySecHeatGenH) / (heatPumps[i].buildEnergyDHW  + heatPumps[i].buildEnergyHeating + 365 * 24 * heatPumps[i].losses_st_dhw / 1000 + 365 * 24 * heatPumps[i].losses_st / 1000) ); // both a= and B) nearly the same
				heatPumps[i].coverageRatioSecHeatGen = round( (heatPumps[i].energySecHeatGenDHW + heatPumps[i].energySecHeatGenH) / (heatPumps[i].buildEnergyDHW + 365 * 24 * 3600 * heatPumps[i].losses_st_dhw + heatPumps[i].buildEnergyHeating + 365 * 24 * 3600 * heatPumps[i].losses_st) * 100 );
				if(heatPumps[i].coverageRatioSecHeatGen >= 100){ //delete this function for debugging??
					heatPumps[i].coverageRatioSecHeatGen = 100;
				}
				else if(heatPumps[i].coverageRatioSecHeatGen <= 0){
					heatPumps[i].coverageRatioSecHeatGen = 0;
				}
				//heatPumps[i].coverageRatio = round( (heatPumps[i].E_dhw_year + heatPumps[i].E_h_year) / (heatPumps[i].buildEnergyDHW  + heatPumps[i].buildEnergyHeating + 365 * 24 * heatPumps[i].losses_st_dhw / 1000 + 365 * 24 * heatPumps[i].losses_st / 1000) * 100 ); // both a= and B) nearly the same
				heatPumps[i].coverageRatio = round( (heatPumps[i].E_dhw_year + heatPumps[i].E_h_year) / (heatPumps[i].buildEnergyDHW + 365 * 24 * 3600 * heatPumps[i].losses_st_dhw + heatPumps[i].buildEnergyHeating + 365 * 24 * 3600 * heatPumps[i].losses_st) * 100 );
				if(heatPumps[i].coverageRatio >= 100){ //ToDo: delete this function for debugging??
					heatPumps[i].coverageRatio = 100;
				}
				else if(heatPumps[i].coverageRatio <= 0){
					//cout << "heatPumps[i].coverageRatio = 0;" << "\n"; //test
					heatPumps[i].coverageRatio = 0;
				}
				if( (heatPumps[i].coverageRatioSecHeatGen + heatPumps[i].coverageRatio) > 100 ){ //ToDo: delete this funktion for debugging??
					//heatPumps[i].coverageRatioSecHeatGen = 100 - heatPumps[i].coverageRatio;
					heatPumps[i].coverageRatio = 100 - heatPumps[i].coverageRatioSecHeatGen;
				}
			}else{
				heatPumps[i].coverageRatio = 0;
				heatPumps[i].coverageRatioSecHeatGen = 0;
			}

		}
		heatPumps[i].vecCoverageRatio.push_back(heatPumps[i].coverageRatio);
		heatPumps[i].vecCoverageRatioSecHeatGen.push_back(heatPumps[i].coverageRatioSecHeatGen);

	} //end bivalent

	//+++ cooling +++
	if( (heatPumps[i].app_range == 3) || (heatPumps[i].app_range == 4) || (heatPumps[i].app_range == 5) || (heatPumps[i].app_range == 6) ){ //cooling
		if(heatPumps[i].buildEnergyCooling > 0){
			heatPumps[i].coveringRatioCooling = round(heatPumps[i].energyCooling / heatPumps[i].buildEnergyCooling * 100 ) ; //[J/J]
			if(heatPumps[i].coveringRatioCooling >= 100){ //ToDo: delete?
				heatPumps[i].coveringRatioCooling = 100;
			}else if(heatPumps[i].coveringRatioCooling <= 0){ //ToDo: delete?
				heatPumps[i].coveringRatioCooling = 0;
			}
		}
		else{
			heatPumps[i].coveringRatioCooling = 0;
		}

	}

	heatPumps[i].vecCoverageRatioCooling.push_back(heatPumps[i].coveringRatioCooling);

}

void HpCore::resPlot() {
	//!function to plot the results automatically in diagrams (PDF)

	//ToDo: possibly only call with command from GeoWPSys? -> in own batch or AppImage?

	cout << "HpCore::resPlot" << "\n";
	//GNU_Executer::use_gnu("a", "fr", "01kuat");

	/*
	 * DOCUMENTATION
	 *
	 * Setting the value range of the y-axes:
	 * The y-axis value range is defined in the write_gnu_exe_one_axis/write_gnu_exe_two_axis/write_gnu_exe_2x2 functions in UseGNU.cpp.
	 * By default, the value range goes over [0:*]. If data is not displayed, for example because it is negative, the range must be changed.
	 * -> search for "yrange" or "y2range" (depending on the axis) in the used function
	 * -> change the 0 for the lower bound and the * for the upper bound
	 *
	 * If the error "all points y value undefined" appears, it can be because the range is set wrong. set yrange [*:*]' should avoid the error.
	 *
	 * Links:
	 * http://www.gnuplotting.org/
	 * http://www.bersch.net/gnuplot-doc/gnuplot.html
	 * https://i.stack.imgur.com/AjKc6.png
	 * https://www.ecosia.org/images?q=linetype+gnuplot&tt=mzl#id=A193766545128747D607E2E10EE14001EC1ED056
	 * https://www3.physik.uni-stuttgart.de/studium/praktika/ap/pdf_dateien/Allgemeines/BeschreibungGnuplot.pdf
	 */

	string name;



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++ standard ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (unsigned int i = 0; i < vecNameData.size(); i++) {
		name = vecNameData[i];
		GNU_Executer exec; //use of GNU_Executers:
		//exec.set_outputpath_pdf(name + ".pdf"); //Specify the diagram path and name
		exec.set_outputpath_pdf(name + ".pdf"); //Specify the diagram path and name
		//exec.set_compression_size( ceil(3600/dt_comm) );// uses command every from GnuPlot //ToDo: calculate with time steps? 3600/dt and round?
		exec.set_two_axes(1); //1 = two axes; 0 = one axe
		exec.set_outputpath_exe("hpm/gnuplot_skript_standard.txt"); //location of the gnuplot_script (will be deleted at the end)
		exec.set_x_unit("d"); // Defines the unit of the x-axis. Default is seconds. Possible are: "s"/"sec"/"seconds", "m"/"min"/"minutes", "h"/"hours", "d"/"days", "w"/"weeks", "M"/"months".
		exec.select_header("none"); //What text is displayed Default = title [none, title, description, both]
		/*		for (int j = 0; j < heatPumps.size(); j++) {
					string findString = vecNameData[i];
					if (findString.find(heatPumps[j].ID) != std::string::npos) {
						exec.set_description_variables(heatPumps[j].HPname,							//Passes all variables for the description in the description text
								heatPumps[j].linked_bhe,
								heatPumps[j].Vdot_soil_bhe_init,
														t,
														t_sim_end,
														heatPumps[j].app_range,
														heatPumps[j].Vol_st,
														heatPumps[j].T_st_min,
														heatPumps[j].T_st_max,
														heatPumps[j].Vol_st_dhw,
														heatPumps[j].T_st_min_dhw,
														heatPumps[j].T_st_max_dhw,
														heatPumps[j].t_run,
														heatPumps[j].P_el_soil_pump,
														heatPumps[j].JAZ_WP,
														heatPumps[j].JAZ_WPA);
					}
				}*/
		exec.set_global_timestep(dt_comm);
		exec.set_delete_all_files(0); // 0 = do not delete; 1 = delete (default); deletes all txt-files for running the code
		//set_bmargin/set_tmargin/set_lmargin/set_rmargin
		//Gives additional border at the bottom/top/left/right
		//The default margin can also be reduced by negative values
		// bmargin -> bottom margin -> lower edge
		// tmargin -> top margin -> upper edge
		// lmargin -> left margin -> left edge
		exec.set_lmargin(-1.5);
		// rmargin -> right margin -> right edge
		//Parameter:
		//double		margin						Size of the additional margin in number of lines
		exec.set_bmargin(-1.5); //botton
		exec.set_tmargin(-1); // top
		//Setting up the offset of the labels
		exec.set_LabelOffset("x" , "y", -1); //The x-label is to be shifted by -1 in the y-direction
		exec.set_LabelOffset("y" , "x", -2); //The y-label is to be shifted by -1 in the x-direction
		exec.set_LabelOffset("y2" , "x", 3.5); //The y2 label is to be shifted by 1 in the x direction
		//set_yrange
		//Parameter:
		//int			axe							For which y-axis should the value range be set? 1 or 2 possible
		//int			from						Lower limit
		//int			to							Upper limit
		//ACHTUNG: If lower and upper limit are equal, the range of values goes from 0 to a limit chosen by gnuplot. If the function is not executed this is also the case.
		exec.set_yrange(1, -6, 15);
		//set_y_tic_distance & set_y2_tic_distance
		//Parameter:
		//double		tic_distance				Specifies the distance in the scale of the Y-axis
		exec.set_y_tic_distance(5);
		exec.set_y2_tic_distance(1000);
		//info-Header
		//variables only if exec.select_header("description")
		//add-line -> Adds a curve to the diagram
		//Parameter:
		//	bool	second_axis			Should the values be plotted on the first or second y-axis?
		//	int		x_values			Column number in which the x-values are located
		//	int		y_values			Column number in which the y-values are located
		//	string	label				Labeling the curve
		//	string	data_path			Path behind which the data file.txt is located
		//	int		aggregation			How many values are combined into one value	->Default= 1
		//	int 	which_aggregation	Which aggregation is used?								->Default = 0 -> mean over all values, 1 -> mean over all greater than 0
		//	string	lines_or_points		Lines or points											->Default= lines ([lines, points, linespoints])
		//	int		type				Lines or point type 									->Default= Solid 1 or point 0 ([0;1] or [0;15])
		//	int		size				Line thickness or point diameter 						->Default= 0 or 0,5 [0:] //see more: https://www.ecosia.org/images?q=linetype+gnuplot&tt=mzl#id=A193766545128747D607E2E10EE14001EC1ED056
		//	string	linecolor			Color 													->Default= Deposited color scheme //see more: https://www.ecosia.org/images?q=linetype+gnuplot&tt=mzl#id=A193766545128747D607E2E10EE14001EC1ED056
		exec.add_line(0, 1, 2, "Soleaustrittstemperatur", name + ".txt", 1, 0, "lines", 1, 3, "dark-violet"); //"T_S_o_l_e_,_a_u_s"
		exec.add_line(0, 1, 17, "Sole-Temperaturdifferenz", name + ".txt", 1, 0, "lines", 1, 0.5, "orange"); //"deltaT_S_o_l_e"//size = 0.5
		for (unsigned int j = 0; j < heatPumps.size(); j++) {
			string findString = vecNameData[i];
			if (findString.find(heatPumps[j].ID) != std::string::npos) {
				if( (heatPumps[j].app_range == 0) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 3) || (heatPumps[j].app_range == 4) ){ // app_range with heating
					exec.add_line(1, 1, 4, "Heizleistungsbedarf", name + ".txt", 1, 1, "lines", 1, 0.5); //size = 0.5
					if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 1)) {
						exec.set_yrange(1, -6, heatPumps[j].T_h_step -273.15 + 2);
						exec.add_line(0, 1, 14, "Puffer-Temperatur",	name + ".txt", 1, 1, "lines", 1, 0.5, "red"); //"T_P_u_f_f_e_r_s_p_e_i_c_h_e_r" //size = 0.5
					}
				}
				if ((heatPumps[j].app_range == 1) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 4) || (heatPumps[j].app_range == 6) ) { // app_range with dhw
					exec.add_line(1, 1, 5, "TWW-Bedarf", name + ".txt", 1, 1, "lines", 1, 0.5); //"TWW-Bedarf"//size = 0.5
					if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 2)) {
						if(heatPumps[j].T_dhw_step > heatPumps[j].T_h_step){
							exec.set_yrange(1, -6, heatPumps[j].T_dhw_step -273.15 + 2);
						}
						exec.add_line(0, 1, 15, "TWW-Speicher-Temperatur",	name + ".txt", 1, 1, "lines", 1, 0.5, "navy"); //"T_T_W_W_-_S_p_e_i_c_h_e_r"//size = 0.5 //ceil(3600/dt_comm/12) = Medium over 5 min
					}
				}
			}
		}

		//For more detailed plotting settings e.g.
		//Plotting_Settings plt_set = exec.get_plt_set();
		//	plt_set.set_style("line");
		//	plt_set.set_title("Title");
		//	plt_set.set_xlable("x lable");
		//	plt_set.set_ylable("y label");
		//	exec2.set_plt_set(plt_set);

		//Uses GNU-Executer to plot all added lines
		//Parameter title, x_lable, y_lable, y2_lable
		exec.use_gnu("", "Zeit [d]", "Temperatur [°C]",	"Leistungsbedarf [W]");

	}

	//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++ standard comppressed++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
		for (unsigned int i = 0; i < vecNameData.size(); i++) {
			name = vecNameData[i];
			GNU_Executer exec0;
			//exec.set_outputpath_pdf(name + ".pdf");
			exec0.set_outputpath_pdf(name + "compressed.pdf");
			//exec.set_compression_size( ceil(3600/dt_comm) );// uses command every from GnuPlot //ToDo: calculate with time steps? 3600/dt and round?
			exec0.set_two_axes(1); //1 = two axes; 0 = one axe
			exec0.set_outputpath_exe("hpm/gnuplot_skript_standard_compressed.txt"); //location of the gnuplot_skript (will be deleted at the end)
			exec0.set_x_unit("d"); // Defines the unit of the x-axis. Default is seconds. Possible are: "s"/"sec"/"seconds", "m"/"min"/"minutes", "h"/"hours", "d"/"days", "w"/"weeks", "M"/"months".
			exec0.select_header("none"); //What text is displayed Default = title [none, title, description, both]
			exec0.set_global_timestep(dt_comm);
			exec0.set_delete_all_files(0); // 0 = do not delete; 1 = delete (default); deletes all txt-files for running the code
			exec0.set_lmargin(-1.5);
			exec0.set_bmargin(-1.5); //botton
			exec0.set_tmargin(-1); // top
			exec0.set_LabelOffset("x" , "y", -1); //The x-label is to be shifted by -1 in the y-direction
			exec0.set_LabelOffset("y" , "x", -2); //The y-label is to be shifted by -1 in the x-direction
			exec0.set_LabelOffset("y2" , "x", 3.5); //The y2 label is to be shifted by 1 in the x direction
			exec0.set_yrange(1, -6, 15);
			exec0.set_y_tic_distance(5);
			exec0.set_y2_tic_distance(1000);
			exec0.add_line(0, 1, 2, "Soleaustrittstemperatur", name + ".txt", ceil(3600/dt_comm)*24, 0, "lines", 1, 3, "dark-violet"); //"T_S_o_l_e_,_a_u_s"
			exec0.add_line(0, 1, 17, "Sole-Temperaturdifferenz", name + ".txt", ceil(3600/dt_comm)*24, 0, "lines", 1, 3, "orange"); //"deltaT_S_o_l_e"//size = 0.5
			for (unsigned int j = 0; j < heatPumps.size(); j++) {
				string findString = vecNameData[i];
				if (findString.find(heatPumps[j].ID) != std::string::npos) {
					if( (heatPumps[j].app_range == 0) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 3) || (heatPumps[j].app_range == 4) ){ // app_range with heating
						exec0.add_line(1, 1, 4, "Heizleistungsbedarf", name + ".txt", ceil(3600/dt_comm)*24, 1, "lines", 1, 0.5); //size = 0.5
						if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 1)) {
							exec0.set_yrange(1, -6, heatPumps[j].T_h_step -273.15 + 2);
							exec0.add_line(0, 1, 14, "Puffer-Temperatur",	name + ".txt", ceil(3600/dt_comm), 1, "lines", 1, 0.5, "red"); //"T_P_u_f_f_e_r_s_p_e_i_c_h_e_r" //size = 0.5
						}
					}
					if ((heatPumps[j].app_range == 1) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 4) || (heatPumps[j].app_range == 6) ) { // app_range with dhw
						exec0.add_line(1, 1, 5, "TWW-Bedarf", name + ".txt", ceil(3600/dt_comm)*24, 1, "lines", 1, 3); //"TWW-Bedarf"//size = 0.5
						if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 2)) {
							if(heatPumps[j].T_dhw_step > heatPumps[j].T_h_step){
								exec0.set_yrange(1, -6, heatPumps[j].T_dhw_step -273.15 + 2);
							}
							exec0.add_line(0, 1, 15, "TWW-Speicher-Temperatur",	name + ".txt", ceil(3600/dt_comm), 1, "lines", 1, 0.5, "navy"); //"T_T_W_W_-_S_p_e_i_c_h_e_r"//size = 0.5 //ceil(3600/dt_comm/12) = Medium over 5 min
						}
					}
				}
			}
			exec0.use_gnu("", "Zeit [d]", "Temperatur [°C]",	"Leistungsbedarf [W]");

		}




//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++ 2x2 ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++

	//4 week diagrams (15. - 21.02. ; 01. - 07.05. ; 25. - 31.07.; 01. - 07.12.)
/*	for (unsigned int i = 0; i < vecNameData.size(); i++) {
		name = vecNameData[i];
		GNU_Executer exec1;
			exec1.set_outputpath_pdf(name +"2x2" + ".pdf");
			exec1.set_two_axes(1); //1 = two axes; 0 = one axe
			exec1.set_x_unit("d");
			exec1.select_header("none");											//What text is displayed Default = title [none, title, description, both]
			exec1.set_outputpath_exe("hpm/gnuplot_skript2x2.txt");
			exec1.set_global_timestep(dt_comm);
			exec1.set_delete_all_files(0); // 0 = do not delete; 1 = delete (default); deletes all txt-files for running the code
			//Create week file
			//Parameter:
			//string				path_year_file				File path of simulation results over one year
			//double				d_t							Time step
			exec1.create_2by2_week_files(name + ".txt");
			exec1.set_yrange(1, -6, 15);
			exec1.set_y_tic_distance(5);
			exec1.set_y2_tic_distance(0.5);
			exec1.set_yrange(2, 0, 7);
			exec1.set_lmargin(0);
			exec1.set_bmargin(-2); //bottom
			exec1.set_tmargin(-1.5); // top
			exec1.set_LabelOffset("x" , "y", -1); //The x-label is to be shifted by -1 in the y-direction
			exec1.set_LabelOffset("y" , "x", -2); //The y-label is to be shifted by -1 in the x-direction
			exec1.set_LabelOffset("y2" , "x", 5);
			exec1.add_line(0, 1, 2, "Soleaustrittstemperatur", "", 1, 1, "lines", 1, 3, "orange"); //0, 1, 2, "T_S_o_l_e_,_a_u_s", name + ".txt"
			for (unsigned int j = 0; j < heatPumps.size(); j++) {
				string findString = vecNameData[i];
				if (findString.find(heatPumps[j].ID) != std::string::npos) {
					if (heatPumps[j].app_range != 1) {
						if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 1)) {
							exec1.set_yrange(1, -6, heatPumps[j].T_h_step -273.15 + 2);
							exec1.add_line(0, 1, 14, "Puffer-Temperatur", "", 1, 1, "lines", 1, 1.5, "red");
						}
					}
					if ((heatPumps[j].app_range == 1) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 4)) {
						if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 2)) {
							if(heatPumps[j].T_dhw_step > heatPumps[j].T_h_step){
								exec1.set_yrange(1, -6, heatPumps[j].T_dhw_step -273.15 + 2);
							}
							exec1.add_line(0, 1, 15, "TWW-Speicher-Temperatur", "", 1, 1, "lines", 1, 1.5, "navy");
						}
						//exec1.add_line(1, 1, 10, "Q_D_H_W", name + ".txt"); //too much
					}
//					if (heatPumps[j].app_range != 1) {
//						//exec1.add_line(1, 1, 8, "Q_H", name + ".txt"); //too much
//					}

					exec1.add_line(1, 1, 13, "COP*", "", 1, 1, "lines", 1, 0.5, "grey70"); //, "points", 7, 0.05
				}
			}

			//exec1.add_line(1, 1, 4, "Heizleistungsbedarf", "", "lines"); //exec.add_line(1, 1, 4, "Heizleistungsbedarf", name + ".txt");
			exec1.use_gnu("Multiplot of " + name, "Zeit [d]", "Temperatur [°C]", "COP* [-]");

	}*/


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++ day 01.02. ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (unsigned int i = 0; i < vecNameData.size(); i++) {
		name = vecNameData[i];
		GNU_Executer exec2;
		exec2.set_global_timestep(dt_comm);
		//create_timeslot_file
		//Parameter:
		//string		path_year_file				Path to the annual record
		//string		path_day_file				Path at which the new record is to be created
		//int			d_start						start day
		//int 			m_start						start month
		//int			d_end						End day
		//int			m_end						End month
		//int			timestep					time step
		exec2.create_timeslot_file(name + ".txt", name +  "day_file.txt", 1, 2, 2, 2);
		exec2.set_outputpath_pdf(name + "day.pdf"); 			//Specify the diagram path and name
		exec2.set_two_axes(1);//1 = two axes; 0 = one axe
		string path1 = "hpm/gnuplot_skript_day_file.txt";
		exec2.set_outputpath_exe(path1);				//location of the gnuplot_skript (will be deleted at the end)
		exec2.set_x_unit("h");													//Defines the unit of the x-axis. Default is seconds. Possible are: "s"/"sec"/"seconds", "m"/"min"/"minutes", "h"/"hours", "d"/"days", "w"/"weeks", "M"/"months".
		exec2.select_header("none");											//What text is displayed Default = title [none, title, description, both]
		exec2.set_delete_all_files(0); // 0 = do not delete; 1 = delete (default); deletes all txt-files for running the code
		exec2.set_lmargin(-0.5);
		exec2.set_bmargin(0); //botton
		exec2.set_tmargin(-2); // top
		exec2.set_LabelOffset("x" , "y", -1.5); //The x-label is to be shifted by -1 in the y-direction
		exec2.set_LabelOffset("y" , "x", -3); //The y-label is to be shifted by -1 in the x-direction
		exec2.set_LabelOffset("y2" , "x", +3); //The y2 label is to be shifted by 1 in the x direction
		exec2.set_yrange(1, -6, 15);
		exec2.set_y_tic_distance(5);
		exec2.set_y2_tic_distance(1000);
		exec2.add_line(0, 1, 2, "Soleaustrittstemperatur", name +  "day_file.txt", 1, 1, "lines", 1, 3, "dark-yellow"); //0, 1, 2, "T_S_o_l_e_,_a_u_s", name + ".txt"
		for (unsigned int j = 0; j < heatPumps.size(); j++) {
			string findString = vecNameData[i];
			if (findString.find(heatPumps[j].ID) != std::string::npos) {
				if( (heatPumps[j].app_range == 0) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 3) || (heatPumps[j].app_range == 4) ){ // app_range with heating
					if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 1)) {
						exec2.set_yrange(1, -6, heatPumps[j].T_h_step -273.15 + 2);
						exec2.add_line(0, 1, 14, "Puffer-Temperatur", name +  "day_file.txt", 1, 1, "lines", 1, 2, "red");
					}
				}
				if ((heatPumps[j].app_range == 1) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 4) || (heatPumps[j].app_range == 6) ) { // app_range with dhw
					if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 2)) {
						if(heatPumps[j].T_dhw_step > heatPumps[j].T_h_step){
							exec2.set_yrange(1, -6, heatPumps[j].T_dhw_step -273.15 + 2);
						}
						exec2.add_line(0, 1, 15, "TWW-Speicher-Temperatur", name +  "day_file.txt", 1, 1, "lines", 1, 2, "dark-violet");
					}
					exec2.add_line(1, 1, 5, "TWW-Bedarf", name +  "day_file.txt", 1, 1, "lines", 1, 2, "blue");
					exec2.add_line(1, 1, 10, "WP-Leistung-TWW", name + "day_file.txt", 1, 1, "lines", 1, 1, "web-blue");
				}
				if( (heatPumps[j].app_range == 0) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 3) || (heatPumps[j].app_range == 4) ){ // app_range with heating
					exec2.add_line(1, 1, 4, "Heizleistungsbedarf", name +  "day_file.txt", 1, 1, "lines", 1, 2, "grey20"); //exec.add_line(1, 1, 4, "Heizleistungsbedarf", name + ".txt");
					exec2.add_line(1, 1, 8, "WP-Leistung-Heizung", name + "day_file.txt", 1, 1, "lines", 1, 1, "orange");
				}
			}
		}
		exec2.use_gnu("Tagesplot (01. Feb)", "Zeit [h]", "Temperatur [°C]", "Leistung [W]");
	}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++ hour ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (unsigned int i = 0; i < vecNameData.size(); i++) {
		name = vecNameData[i];
		GNU_Executer exec3;
		exec3.set_delete_all_files(0);
		exec3.set_global_timestep(dt_comm);
		//create_hour_file
		//Parameter:
		//string		path_year_file				Path to the annual record
		//string		path_hour_file				Path at which the new record is to be created
		//int			hour						Start hour [0-8760]
		//int 			amount_hour					Number of hours to plot
		exec3.create_hour_file(name + ".txt", name + "hour_file.txt", 24 * (31) + 10, 4); //Creates record for hour 500 to 502
		exec3.set_outputpath_pdf(name + "_hour.pdf"); 			//Specify the diagram path and name
		exec3.set_two_axes(1);	//1 = two axes; 0 = one axe
		exec3.set_outputpath_exe("hpm/gnuplot_skript_hour_file.txt");			//location of the gnuplot_skript (will be deleted at the end)
		exec3.set_x_unit("h");													//Defines the unit of the x-axis. Default is seconds. Possible are: "s"/"sec"/"seconds", "m"/"min"/"minutes", "h"/"hours", "d"/"days", "w"/"weeks", "M"/"months".
		exec3.select_header("title");											//What text is displayed Default = title [none, title, description, both]
		exec3.set_lmargin(-0.5);
		exec3.set_bmargin(0); //botton
		exec3.set_tmargin(-1); // top
		exec3.set_LabelOffset("x" , "y", -1.5); //The x-label is to be shifted by -1 in the y-direction
		exec3.set_LabelOffset("y" , "x", -2.5); //The y-label is to be shifted by -1 in the x-direction
		exec3.set_LabelOffset("y2" , "x", +3); //The y2 label is to be shifted by 1 in the x direction
		exec3.set_yrange(1, -6, 15);
		exec3.set_y_tic_distance(5);
		exec3.set_y2_tic_distance(1000);
		exec3.add_line(0, 1, 2, "Soleaustrittstemperatur", name +  "hour_file.txt", 1, 1, "lines", 1, 3, "dark-yellow"); //0, 1, 2, "T_S_o_l_e_,_a_u_s", name + ".txt"
		for (unsigned int j = 0; j < heatPumps.size(); j++) {
			string findString = vecNameData[i];
			if (findString.find(heatPumps[j].ID) != std::string::npos) {
				if( (heatPumps[j].app_range == 0) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 3) || (heatPumps[j].app_range == 4) ){ // app_range with heating
					if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 1)) {
						exec3.set_yrange(1, -6, heatPumps[j].T_h_step -273.15 + 2);
						exec3.add_line(0, 1, 14, "Puffer-Temperatur", name +  "hour_file.txt", 1, 1, "lines", 1, 2.5, "red");
					}
					exec3.add_line(1, 1, 4, "Heizleistungsbedarf", name +  "hour_file.txt", 1, 1, "lines", 1, 2.5, "grey20"); //exec.add_line(1, 1, 4, "Heizleistungsbedarf", name + ".txt");
					exec3.add_line(1, 1, 8, "WP-Leistung-Heizung", name + "hour_file.txt", 1, 1, "lines", 1, 0.5, "orange");
				}
				if ((heatPumps[j].app_range == 1) || (heatPumps[j].app_range == 2) || (heatPumps[j].app_range == 4) || (heatPumps[j].app_range == 6) ) { // app_range with dhw
					if ((heatPumps[j].HP_storage == 0) || (heatPumps[j].HP_storage == 2)) {
						if(heatPumps[j].T_dhw_step > heatPumps[j].T_h_step){
							exec3.set_yrange(1, -6, heatPumps[j].T_dhw_step -273.15 + 2);
						}
						exec3.add_line(0, 1, 15, "TWW-Speicher-Temperatur", name +  "hour_file.txt", 1, 1, "lines", 1, 2.5, "dark-violet");
						//exec.add_line(1, 1, 6, "Q_D_H_W", name + ".txt");
					}
					exec3.add_line(1, 1, 5, "TWW-Bedarf", name +  "hour_file.txt", 1, 1, "lines", 1, 2.5, "blue");
					exec3.add_line(1, 1, 10, "WP-Leistung-TWW", name + "hour_file.txt", 1, 1, "lines", 1, 0.5, "web-blue");
				}
			}
		}
		exec3.use_gnu("Stundenplot (01. Feb 10:00 bis 14:00)", "Zeit [h]", "Temperatur [°C]", "Leistung [W]");
	}



//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++ COP* first and last year ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
	for (unsigned int i = 0; i < heatPumps.size(); i++) {
		if(heatPumps[i].simulateHP == true){
			GNU_Executer exec4; //Verwendung des GNU_Executers:
			exec4.set_outputpath_pdf("hpm/results_Tsoil_COP_" + heatPumps[i].ID + ".pdf"); //Specify the diagram path and name
			//exec4.set_compression_size( floor(3600/dt_comm) );// uses command every from GnuPlot //ToDo: calculate with time steps? 3600/dt and round?
			exec4.set_two_axes(1); //1 = two axes; 0 = one axe
			exec4.set_outputpath_exe("hpm/gnuplot_skript_COP.txt"); //location of the gnuplot_skript (will be deleted at the end)
			exec4.set_x_unit("d");
			stringstream stringFirstYear;
			stringstream stringLastYear;
			stringFirstYear << vecYear[0];
			stringLastYear << vecYear[vecYear.size() - 1];
			//cout << "vecYear.size()" << vecYear.size() << "\n"; //test vecYear.size()
			//for(vector<unsigned int>::iterator vit=vecYear.begin();vit!=vecYear.end();++vit) cout << *vit << "\n"; //test
			string strFirstYear = stringFirstYear.str();
			string strLastYear = stringLastYear.str();
			//cout << "strLastYear = " << strLastYear << "\n"; //test vecYear.size()
			exec4.select_header("none"); //
			exec4.set_global_timestep(dt_comm);
			exec4.set_delete_all_files(0); // 0 = do not delete; 1 = delete (default); deletes all txt-files for running the code
			exec4.set_lmargin(-1.5);
			exec4.set_bmargin(-1.5); //bottom
			exec4.set_tmargin(-1.5); // top
			exec4.set_LabelOffset("x" , "y", -1); //The x-label is to be shifted by -1 in the y-direction
			exec4.set_LabelOffset("y" , "x", -2); //The y-label is to be shifted by -1 in the x-direction
			exec4.set_LabelOffset("y2" , "x", +2.5); //The y2 label is to be shifted by 1 in the x direction
			exec4.set_yrange(1, -6, 12);
			exec4.set_yrange(2, 0, 7);
			exec4.set_y_tic_distance(2);
			exec4.set_y2_tic_distance(0.5);
			//exec4.set_x_tic_distance(2);
			//exec4.add_line(1, 1, 13, "COP_J_a_h_r_1_M_i_t_t_e_l", "hpm/results_" + heatPumps[i].ID + "_year_" + strFirstYear + ".txt", ceil(3600/dt_comm) , "lines", 1, 0.5, "red"); //, "points", 7, 0.05 // Error
			exec4.add_line(0, 1, 2, "T_S_o_l_e_,_a_u_s_,_l_e_t_z_t_e_s_ _J_a_h_r", "hpm/results_" + heatPumps[i].ID + "_year_" + strLastYear + ".txt", ceil(3600/dt_comm), 0, "lines", 2, 4, "orange-red");
			//exec4.add_line(1, 1, 13, "TEST", "hpm/results_" + heatPumps[i].ID + "_year_" + strLastYear + ".txt", 1000, 1, "lines", 1, 0.5, "grey");
			//exec4.add_line(1, 1, 13, "TEST2", "hpm/results_" + heatPumps[i].ID + "_year_" + strFirstYear + ".txt", 1000, 1, "lines", 1, 0.5, "yellow");
			exec4.add_line(0, 1, 2, "T_S_o_l_e_,_a_u_s_,_e_r_s_t_e_s_ _J_a_h_r", "hpm/results_" + heatPumps[i].ID + "_year_" + strFirstYear + ".txt", ceil(3600/dt_comm), 0, "lines", 1, 3, "red"); //"results_HP01_year_1.txt"
			exec4.add_line(1, 1, 13, "COP*_l_e_t_z_t_e_s_,_J_a_h_r_,_T_a_g_e_s_m_i_t_t_e_l",	"hpm/results_" + heatPumps[i].ID + "_year_" + strLastYear + ".txt", ceil(3600/dt_comm)*24, 1, "lines", 2, 5, "grey10");
			exec4.add_line(1, 1, 13, "COP*_e_r_s_t_e_s_,_J_a_h_r_,_T_a_g_e_s_m_i_t_t_e_l",	"hpm/results_" + heatPumps[i].ID + "_year_" + strFirstYear + ".txt", ceil(3600/dt_comm)*24, 1, "lines", 1, 1.5, "navy");
			exec4.add_line(1, 1, 13, "COP*_l_e_t_z_t_e_s_,_J_a_h_r",	"hpm/results_" + heatPumps[i].ID + "_year_" + strLastYear + ".txt", ceil(3600/dt_comm), 1,  "points", 7, 0.1, "grey"); //, "points", 7, 0.05 //ToDo: points visible/big enough in legend -> https://stackoverflow.com/questions/20245287/custom-legend-or-text-gnuplot
			//x-tics: http://www.gnuplot.info/docs_4.2/node295.html
			exec4.add_line(1, 1, 13, "COP*_e_r_s_t_e_s_,_J_a_h_r", "hpm/results_" + heatPumps[i].ID + "_year_" + strFirstYear + ".txt", ceil(3600/dt_comm), 1,  "points", 7, 0.1, "skyblue"); //, "points", 7, 0.05
			exec4.use_gnu("h-Mittel der Temperaturen, h-Mittel der WP-Effizienz und Soleaustrittstemperaturen von " + heatPumps[i].ID, "Zeit [d]" , "Temperatur [°C]", "COP* [-]"); //exec4.use_gnu("Soleaustrittstemperaturen und COP des ersten und letzten Jahres von "+ heatPumps[i].ID, "Zeit", "Temperatur [°C]", "COP [-]");
		}//end of if(heatPumps[i].simulateHP == true)

	}


//+++++++++++++++++++++++++++++++++++++++++++++++++++++++++ t_run &&	hLC || dhwLC ??r ++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++++
//ToDo? Bar chart?

/*	for (unsigned int i = 0; i < heatPumps.size(); i++) {
 * 		if(heatPumps[i].simulateHP == true){
 * 			GNU_Executer exec5; //use of GNU_Executers:
			exec5.set_outputpath_pdf("hpm/endResults_t_run_" + heatPumps[i].ID + ".pdf"); //Specify the diagram path and name
			exec5.set_two_axes(1); //1 = two axes; 0 = one axe
			exec5.set_outputpath_exe("hpm/gnuplot_skript_endResults_t_run_.txt"); //location of the gnuplot_skript (will be deleted at the end)
			exec5.set_x_unit("a"); //ToDo
			exec5.select_header("none"); //
			exec5.set_global_timestep(1); //? //ToDo
			exec5.set_delete_all_files(0); // 0 = do not delete; 1 = delete (default); deletes all txt-files for running the code
			exec5.set_bmargin(-1.5);
			exec5.set_tmargin(-1);
			//exec5.set_yrange(1, -6, 12);
			exec5.set_yrange(2, 0, 100);
			//set_y_tic_distance & set_y2_tic_distance
			//Parameter:
			//double		tic_distance				Specifies the distance in the scale of the Y-axis
			exec5.set_y_tic_distance(500);
			exec5.set_y2_tic_distance(5);
			exec5.add_line(0, 1, 2, "Volllaststunden", "hpm/results_" + heatPumps[i].ID + ".txt", 1, 1, "linespoints", 1, 2.5, "dark-green");
			exec5.add_line(1, 1, 4, "Heizlastdeckung", "hpm/results_" + heatPumps[i].ID + ".txt", 1, 1, "linespoints", 1, 3, "red");
			exec5.add_line(1, 1, 6, "TWW-Lastdeckung", "hpm/results_" + heatPumps[i].ID + ".txt", 1, 1, "linespoints", 1, 3, "navy");
			exec5.use_gnu("Volllaststunden und Lastdeckung von " + heatPumps[i].ID, "Zeit [a]" , "Volllaststunden [h]", "Lastdeckung [%]"); //exec4.use_gnu("Soleaustrittstemperaturen und COP des ersten und letzten Jahres von "+ heatPumps[i].ID, "Zeit", "Temperatur [°C]", "COP [-]");
 * 		}
 *
	}*/

}


void HpCore::writeResToXML() {
	cout << "HPC: writeResToXML" << "\n"; //test


// load the XLM file
file<> xmlFile("../import/xml_file.xml");
xml_document<> xmlDoc;
xmlDoc.parse<0>(xmlFile.data());

// // Finden Sie den gewünschten XML-Knoten
//root node
xml_node<> * rootNode = xmlDoc.first_node("getisData");
string testHPID; 
unsigned int hpInXML = 0;
string valueStr;
const char* valueCStr;
//const char* testJAZ;
for (xml_node<> * HP_node = rootNode->first_node("HeatPumpCollection")->first_node("heat_pump"); HP_node; HP_node = HP_node->next_sibling()){
	testHPID = HP_node->first_attribute()->next_attribute("heat_pump:id")->value();
	cout << "HPC: writeResToXML: ---------------> heat_pump:id = " << testHPID << "\n"; //test	
	cout << "HPC: writeResToXML: ---------------> hpInXML = " << hpInXML << "\n"; //test
	cout << "HPC: writeResToXML: ---------------> heatPumps[hpInXML].JAZ_WPAmiddled = " << heatPumps[hpInXML].JAZ_WPAmiddled << "\n"; //test

	valueStr = to_string(heatPumps[hpInXML].JAZ_WPAmiddled);
	valueCStr = valueStr.c_str();
	//cout << "-> valueCStr = " << valueCStr << endl;


	xml_node<>* JAZwpaNode = xmlDoc.allocate_node(node_element, "heat_pump:ResSCOP_WPA"); //xml_node<> * JAZwpaNode = HP_node->first_node("heat_pump:ResSCOP_WPA");
	//testJAZ = JAZwpaNode->value();
	//cout << "HPC: writeResToXML: ---------------> testJAZ before change = " << testJAZ << "\n"; //test	
	JAZwpaNode->value(xmlDoc.allocate_string(valueCStr));
	xml_node<> * targetNode = HP_node->first_node("heat_pump:ResSCOP_WPA");
	HP_node->remove_node(targetNode); 
	xml_node<> * JAZwphaNode = HP_node->first_node("heat_pump:ResSCOP_WPHA");
	HP_node->insert_node(JAZwphaNode, JAZwpaNode); //appends a new node at the beginning of each HP_node //rootNode->first_node("HeatPumpCollection")->first_node("heat_pump")->insert_node(rootNode->first_node("HeatPumpCollection")->first_node("heat_pump")->first_node(), JAZwpaNode); //appends a new node at the beginning of HP_node (only first sibling!!) //xmlDoc.append_node(JAZwpaNode); //appends a new node at end of XML //ToDo: andere Funktionen testen: clone_node(); void insert_node(xml_node< Ch > *where, xml_node< Ch > *child);
	//testJAZ = JAZwpaNode->value();
	//cout << "HPC: writeResToXML: ---------------> testJAZ after insert_node = " << testJAZ << "\n"; //test

	valueStr = to_string(heatPumps[hpInXML].middledCR);
	valueCStr = valueStr.c_str();
	//cout << "-> valueCStr = " << valueCStr << endl;
	cout << "HPC: writeResToXML: ---------------> heatPumps[hpInXML].middledCR = " << heatPumps[hpInXML].middledCR << "\n"; //test

	xml_node<>* coverRatioNode = xmlDoc.allocate_node(node_element, "heat_pump:ResCoverRatioTotal"); //xml_node<> * JAZwpaNode = HP_node->first_node("heat_pump:ResSCOP_WPA");
	coverRatioNode->value(xmlDoc.allocate_string(valueCStr));
	xml_node<> * targetCRNode = HP_node->first_node("heat_pump:ResCoverRatioTotal");
	HP_node->remove_node(targetCRNode); 
	xml_node<> * crSecHeatGenNode = HP_node->first_node("heat_pump:ResCRatioTotal2HeatGen");
	HP_node->insert_node(crSecHeatGenNode, coverRatioNode); //appends a new node at the beginning of each HP_node //rootNode->first_node("HeatPumpCollection")->first_node("heat_pump")->insert_node(rootNode->first_node("HeatPumpCollection")->first_node("heat_pump")->first_node(), JAZwpaNode); //appends a new node at the beginning of HP_node (only first sibling!!) //xmlDoc.append_node(JAZwpaNode); //appends a new node at end of XML //ToDo: andere Funktionen testen: clone_node(); void insert_node(xml_node< Ch > *where, xml_node< Ch > *child);



	hpInXML = hpInXML+1;
}


//save xmlDoc into new XML file 
ofstream outFile("../run/hpm/xml_file.xml");
outFile << xmlDoc;
outFile.close();


}
