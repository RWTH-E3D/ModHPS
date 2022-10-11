/*
 * hpcore.h
 *
 *  Created on: 10.04.2018
 *      Author: Sebastian Weck-Ponten
 */

#ifndef HPCORE_H_
#define HPCORE_H_

#include "heatpump.h"
#include "building.h"


using namespace std;

//!core class of ModHPS
/*!
HpCore contains the calculation core of the code and the most important functions for storage tank balances and control strategies.
*/

class HpCore {
public:
	HpCore();
	bool coreParse();
	void assignment();
	void sendVsoilNominal();
	void setInitValues();
	void makeTXTinitial();
	void makeTXT();
	void resetValues();
	void updateValues();
	void saveOverhang();
	vector<double> buildNewVecDouble(vector<double>);
	vector<bool> buildNewVecBool(vector<bool>);
	vector<unsigned int> buildNewVecUnsInt(vector<unsigned int>);
	void pushBackTXT();
	void pushBackYearTXT();
	void debugTXT();
	//void clearVectors(); //not necessary because of saveOverhang()
	void sequence();
	void calculate();
	void makeTXTend();
	void resPlot();

	void modeControl(int i); //control strategy for mode heating or dhw or cooling -> dependent of app_range, HP_storage, T_st, T_st_dhw, Q_HL_hp_time and Q_dhw_hp_time
	void HPonControl(int i); //control strategy for HP: on or off -> dependent of app_range, HP_storage, T_st, T_st_dhw, Q_HL_hp_time, Q_dhw_hp_time and HP_mode!! this Control is for monovalent systems sufficient
	void controlMonovalent(int i); //control strategy of monovalent systems
	void controlAlternative(int i); //control strategy for second heat generator in alternative mode: on or off -> dependent of 	HPonControl (if HP is on because of ...)? -> only if operating_mode not 0!!
	void controlPartParallel(int i); //control strategy for second heat generator in part parallel mode: on or off -> dependent of 	HPonControl (if HP is on because of ...)? -> only if operating_mode not 0!!
	void controlParallel(int i); //control strategy for second heat generator in parallel or monoenergetic mode: on or off -> dependent of 	HPonControl (if HP is on because of ...)? -> only if operating_mode not 0!!
	void interpolationHP(int i);
	void QsoilTsoilinCOP(int i);
	void cooling(int i);
	void storageBalance(int i);
	void calcCR(int i);
	void loadCoverage(int i);

	//+++blocking time+++
	void blockingHP(int i);

	void HPControlLoadStorageFull(int i); //heat pump loads storages full if heatPumps[i].timerLoadStFull is aktive (not loaded of second heat generator!!)
	void LoadStorageFull(int i);

	void secHeatGenOnControl(int i); //necessary for alternative and part parallel //only second heat generator on && only called in modus heating
	void secHeatGenControlLoadStorageFull(int i); //necessary for alternative and part parallel //only second heat generator on && only called in modus heating
	void secHeatGenLoadStorageFull(int i); //necessary for alternative and part parallel //only second heat generator on && only called in modus heating

	//ToDo: void writeResToXML();

private:
//+++ time +++
	double t; //time [s]
	double t_valid; //time [s] for coupling my model to MPFluidGeTIS
	double dt_comm; //time step
	unsigned int yearVecSize; //= floor(8760 * 3600 / dt_comm);
	double t_sim_end; // end time
	unsigned int endYear; //t_sim_end in years -> starts with 0
	unsigned int step; //step of vector Q_HL // = floor((t - year * 8760 * 3600) / 3600); -> starts with 0
	unsigned int year; //actual year -> starts with 0
	unsigned int actYear; //actual year -> starts with 1

//+++ cascade +++
	double sumFactorCascade;

//+++ results +++
	vector<double> vecTime;
	vector<unsigned int> vecYear;
	vector<double> vecT_soil_out; //vector out of T_soil_out
	vector <string> vecNameData;
	//vector <string> vecNameTexDataEnd;

	vector <unsigned int> testVecYear; //test

	//double Vdot_soil_test_val; //+++*validation test+++*

	vector<HeatPump> heatPumps; //vector of objects
	vector<Building> buildings;

	HeatPump Phy;
	HeatPump HP;
	Building Build;

	double rho_w; //[kg/m³] water
	double cp_w; //[J/kgK] water

    vector<double> VecT_soil_cell_min; // = vector of minimal temperatures of soil cells of each BHE
//+++ Coupling with subsurface models
    unsigned int ugModel; // 0 = const. soil temperature; 1 = decreasing soil temperature; 2 = soil temperature data set; 3 = MPFluidGeTIS (numerical); 4 = analytical GeTIS; 5 = other iterative model; 6 = other non-iterative model; 7 = district heating network model)" << "\n";
    unsigned int counterIter;
//+++ implicit calculation MPFluid +++
   	bool iterCommand; //iteration command from MPFluid for implicit calculation (False = resetValues; True = updateValues -> iteration successful) -> no influence if coupling with Düber
   	//unsigned int countErr; //counter for Error of soil heat flux check //test -> not necessary -> only for testing
   	double sumVecH;
   	double sumVecDHW;

	friend class HPLink;
	friend class Parser;
	friend class Building; //old: friend class HpPhysics;
	friend class HeatPump;

};

#endif /* HPCORE_H_ */
