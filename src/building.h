/*
 * building.h
 *
 *  Created on: 11.04.2018
 *      Author: wp
 */


#ifndef BUILDING_H_
#define BUILDING_H_

using namespace std;

//!Class for building instances

class Building {
public:
	Building();

private:
	string buildingID;
	vector<string> linked_hp;
	double Tmin_heating_system; //[K] = T_st_min
	double Tmax_heating_system; //[K] = HP stage e.g. Q_HL(35) or Q_HL(55)
	double T_heating_system; //[K] = T_st_max
	unsigned int HL_type; //0 = building:heating_load; 1 = building:heating_load_corr_T_heating_limit; 2 = building:heating_load_no_heating_summer
	vector<double> Q_HL; //heating loads as vector
	vector<double> coolingLoad; //cooling loads as vector
	vector<double> Q_dhw_load; //DHW demand [W] as vector

	friend class Parser;
	friend class HpCore;

};

#endif /* BUILDING_H_ */
