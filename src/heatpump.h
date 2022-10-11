/*
 * heatpump.h
 *
 *  Created on: 05.12.2017
 *      Author: Sebastian Weck-Ponten
 */

#ifndef HEATPUMP_H_
#define HEATPUMP_H_

using namespace std;

//!Class for heat pump instances


class HeatPump {
public:
	HeatPump(); //default-constructor


private:
//+++ general +++
	bool simulateHP;
	string ID;
	string HPname;
	unsigned int operatingMode; //(0=monovalent, 1=monoenergetic, 2=bivalent parallel, 3=bivalent teilparallel, 4=bivalent alternativ)
	unsigned int app_range; //(0=heating, 1=dhw, 2=heating+dhw, 3=heating+cooling, 4=heating+cooling+dhw, 5=cooling, 6=dhw+cooling)
	unsigned int compressor_mode; //0=fixed-speed = taktender Betrieb; 1=modulating = modulierend
	unsigned int HP_mode; // 0=heating; 1=dhw; //-> for iteration of Q_h //ToDo:2 = active cooling //passive cooling as a mode necessary?? -> Assumption: passive cooling can be done in every situation, except if T_soil_out is too high
	unsigned int HPmodeLastTimeStep;
	bool HP_on; //true = 1 = HP on; false = 0 = HP off

//+++ assignment +++
    vector<string> linked_buildings;
    vector<string> linked_bhe; //list<string> linked_bhe; //vector<string> linked_bhe;

    vector<double> vecPreQ_HL_hp; //vector of all heating loads (HL of all buildings which are linked to HP) for the hole year (calculated at Assignment of buildings to HP) [W at every hour]
    vector<double> vecPreQ_dhw_hp; //vector of all dhw load (HL of all buildings which are linked to HP) for the hole year (calculated at Assignment of buildings to HP) [W at every hour]

    double Q_HL_hp_time; //actual heating load of all linked buildings to HP -> out of vecPreQ_HL_hp or weighting if unusual time steps
    double Q_dhw_hp_time; //actual dhw load of all linked buildings to HP -> out of vecPreQ_dhw_hp or weighting if unusual time steps

    double buildEnergyHeating; //sum over HL [W] -> * dt_comm  -> [J]
    double buildEnergyDHW; //sum over HL [W] -> * dt_comm  -> [J]

//+++ interpolation HP values +++
    bool critInterpolation;
    vector<double> T_soil_steps; //T_soil_out steps for HP (-5, 0, 5, 10, 15, 20) °C
    vector<double> yData;
    map<int,vector<double> > mapQ_h;
    map<int,vector<double> > mapP_el;
   	double nominalPowerH; //for calculation of full load hours of inverter HPs + for timerLoadStFull + cascades factor, ... //also for calculating timerLoadStFull
   	double nominalPowerDHW; //for calculation of full load hours of inverter HPs
    //Cascade
    vector<double> vecCascadeFactor; //vector off all cascadeFactors -> vecCascadeFactor.size() defines number of HPs in cascade

//+++ cooling +++
    unsigned int coolingType; // 0 = passive; 1 = active
    double T_maxPassivCooling;
    double Vdot_soilCooling; //necessary for calculating T_soil_in
    double Vdot_soilBHEcoolingInit; //parsed
    double Vdot_soilBHEcooling; // 0 or =Vdot_soilBHEcoolingInit
    double vaildVdot_soilBHEcooling; // 0 or =Vdot_soilBHEcoolingInit
    vector<double> vecPreQ_COOL_hp; //vector of all cooling loads (Cooling of all buildings which are linked to HP) for the hole year (calculated at Assignment of buildings to HP) [W at every hour]
    double Q_COOL_hp_time;
    double buildEnergyCooling;
    double energyCooling;
    double ValidEnergyCooling;
    double coveringRatioCooling;
    vector<double> vecCoverageRatioCooling; //[%] //vector of yearly values for coverage ratio cooling
    double middledCRcooling; // [%] middled coverage ratio cooling over all years

//+++ bivalent: +++
   	bool secHeatGen_on; //necessary for alternative and part parallel (same as HP_on)
   	bool secHeatGenOnInBlockingTime; // 0 = false; 1 = true
    double nominalPowerSecHeatGen; //nominal power of second heat generator
    double shutDownHL; //heating load of linked buildings at shut down point (Abschaltpunkt) for bivalent part parallel systems
    double Q_hSecHeatGen; //for bivalent or monoenergetic HP: actual heating power of second heat generator (two stages -> 50 % or 100 %)
    double Q_dhwSecHeatGen; //for bivalent or monoenergetic HP: actual heating power of second heat generator (two stages -> 50 % or 100 %)
    //double Q_storageSecHeatGen; //heat flux which goes in storages for bivalentStorageBalance
    double energySecHeatGenH; //summed energy heating over one year which comes from second heat generator [J] !! only for txt recalculate in kWh
    double energySecHeatGenDHW; //summed energy dhw over one year which comes from second heat generator [J] !!
    vector<double> resQ_hSecHeatGen; //only for testing
    vector<double> resQ_dhwSecHeatGen; //only for testing
    double coverageRatioHeatingSecHeatGen;
    vector<double> vecCoverageRatioHeatingSecHeatGen; //vector of all yearly coverage ratio of second heat generator
    double coverageRatioDHWsecHeatGen;
    vector<double> vecCoverageRatioDHWsecHeatGen; //vector of all yearly coverage ratio of second heat generator
    double coverageRatioSecHeatGen; //coverage ratio of second heat generator for one year
    vector<double> vecCoverageRatioSecHeatGen; //vector of all yearly coverage ratio of second heat generator
    double middledCRsecHeatGen; //ToDo: -> write in XML -> ResCRatioTotalSecHeatGen;

//+++ HP results +++
	double Q_h; // heat flux (heating) of HP (condenser) of actual time step -> f(T_soil_out, T_h_step) -> [W]
	unsigned int count_h; //counter for heat pump in heating mode -> count_h * dt = t heating in a year -> necessary for Qdot_hMiddle
	double Qdot_hMiddle; //middle of Q_h over year -> [W]
	double Q_dhw; // [W]
	unsigned int count_dhw; //counter for heat pump in dhw mode -> count_dhw * dt = t dhw in a year -> necessary for Qdot_dhwMiddle
	double Qdot_dhwMiddle; //middle of Q_dhw -> [W]
	//int count_cool; //counter for heat pump in dhw mode -> count_dhw * dt = t dhw in a year -> necessary for Qdot_coolMiddle
	double P_el; // [W] // electric power of HP (compressor)
	double P_elMiddle; //middle of P_el over year -> [W]
	double Q_soil; // heat flux (subsoil) of HP (evaporator) -> [W]
	double Qdot_soilMiddle; //middle of Q_soil over year -> [W]

	double heatingLoadCoverage; //load coverage(power not energy!!) heating in % of every time step
	double dhwLoadCoverage; //load coverage (power not energy!!) dhw in % of every time step

	double COP; // calculated efficient ratio of every time step (not real COP)

//+++ storage balance +++
	unsigned int HP_storage; //variable for storage balance calculation modus; app_range is necessary!! -> 0 = heating and dhw storages; 1 = only heating storage; 2 = only dhw storage; 3 = no storages;
	double Vol_st; //[m³]
	double losses_st; // [W] // static storage losses from heat pump manufacturer
	double T_st; //calculated storage temperature in time step -> for result
	double T_st_min; //= T_min_heating_system -> parse to building.cpp -> minimal storage temperature
	double T_st_max; //= T_heating_system ->parse to building.cpp -> maximal storage temperature
	double T_h_step; //=Tmax_heating_system
	double Vol_st_dhw; //[m³]
	double losses_st_dhw; // [W] // static storage losses from heat pump manufacturer
	double T_st_dhw; //calculated dhw storage temperature in time step -> for result
	double T_st_min_dhw; //-> parse!
	double T_st_max_dhw; //= heatpump:T_dhw-> parse T_dhw! = DHW supply temperature -> for HP control
	double T_dhw_step; //for linear interpolation of HP + storage balance! = step of HP B0/W55 -> parse = heat_pump:Tmax_dhw -> check at geoportal -> if HP-model is connected to another program or stand alone, then check it in parser
	double U_st_useful; //useful energy in storage (dhw and heating) calculated with T_st_dhw and T_st //necessary! -> only with T_st? -> only 0% or 100% -> only through U_st_useful statement about degree of coverage!! [J] -> necessary for dhwLoadCoverage heatingLoadCoverage
	double U_st_useful_dhw; //[J] -> necessary for dhwLoadCoverage
	double hysteresis; //-> XML: heat_pump:hysteresis [K]

//+++ linking to subsurface +++
	double T_soil_in; // = BHE_in -> in the subsurface
    double T_soil_out; // = BHE_out -> from the subsurface -> influence on Q_h, P_el, COP, ...
	double Vdot_soil; // can be changed because of cooling or cascade [m³/s]
	double Vdot_soil_init; //parsed // V_soil out of HP data // 2,2 m³/h = 6,11*10^-4 m³/s // [m³/s]
	double Vdot_soil_bhe; //actual volume stream of each linked bhe; = Vdot_soil_bhe_init if HP is on; = 0 if HP is off
	double Vdot_soil_bhe_init; //Vdot_soil per linked BHE of each HP -> value out of HP data / n_EWS -> out of parser
	double rhoCp_soil; //parsed -> [kg/m³] //

	double Tsoil_cellcrit; // Preset minimum temperature in the cells of the subsoil -> condition for switching off the heat pump. //-> in heatpump!!
	bool crit_soil; //parameter for critical subsoil situations (freezing, ...) -> dependant of  -> true = HP off; false = HP on
	double timeCritSoil; //summed time if  A) crit soil boundary (= 0 °C subsurface cell temperature) or B) T < T_soil_steps[0] || T > T_soil_steps[T_soil_steps.size() -1]

	vector <double> VecV_soil_bhe_nominal; // used in hplink -> before HP calculation send volume stream of each BHE in strict sequence (sendBHEseq() -> all_linked_bhe) to subsurface model

//+++ Physics +++
	vector<string> VecAllBuildings; //vector of all buildings out of parser // actually not used in hpCore only in Parser for information
	vector<string> all_linked_bhe; //vector of all BHEs out of parser
	//vector<double> T_amb; -> At the moment not necessary

//++for inverter++


//+++ unusual time steps +++
    bool weightMarker; //weighting because of time steps which are not a part of 3600
    double Q_HL_hp_weight; //Weighting for unusual time steps -> if time step is no part of 3600
    double Q_dhw_hp_weight; //Weighting for unusual time steps -> if time step is no part of 3600
    double Q_COOL_hp_weight; //Weighting for unusual time steps -> if time step is no part of 3600
    // -> influences Q_HL_hp_time or Q_dhw_hp_time -> and vecQ_HL_hp_time or vecQ_dhw_hp_time



//+++ parsed in outs for txt +++
	double P_el_soil_pump; //0 W if one is integrated in HP and in P_el out of STIEBEL data!! -> used for E_el_year -> JAZ -> [W]

//+++ results for txt +++
   	double t_run; //yearly run time of HP [s]
   	//double runTimeFullLoad; //ToDo?? full load hours -> for cascades and inverter other calculation? -> new calculation for inverter
   	//++
	vector<double> res_t_run_year; //vector of run time of HP for every year [s]
	vector<double> resQ_h; //result vector of Q_h [W] of every time step -> //[W]
	vector<double> resQ_dhw; //result vector of Q_dhw [W] of every time step -> //[W]
	double E_h_year; //[J]
	vector<double> resE_h_year; //vector of E_h_year of all years [J]
	double E_dhw_year; //[J]
	vector<double> resE_dhw_year; //vector of E_dhw_year of all years [J]
	double E_year; //E_h_year + E_dhw_year -> [J]
	vector<double> resE_year; //vector of Q_h + Q_h_dhw of HP for every year [J]
	vector<double> resP_el; //[W]
	double E_el_year; //[J]
	vector<double> resE_el_year; //vector of E_el of HP for every year [J]
	vector<double> resQ_soil; // vector of Q_soil of HP for every year [W]
	vector<double> vecT_st; //[K]
	vector<double> vecT_st_dhw; //[K]
	vector<double> vecCOP; //filled vector with COP of time step
	vector<bool> resVecCrit_soil; // HP off because of crit soil Temp -> vector of bool
	vector<double> resT_soil_in; //vector of all T_soil_in of one Year //[K]
	vector<double> resT_soil_out; //vector of all T_soil_out of one Year //[K]
	vector<bool> resHP_on; //vector of all status of HP (on / off)
	vector<unsigned int> resHP_mode; //vector of all modus of HP (heating / DHW / cooling?) //ToDo: cooling here, if active cooling?
	vector<double> vecHL_Coverage; //[%] -> in every time step in first and last year
	double HLC_year; //annual middled heating load coverage [%]
	vector<double> resVecHL_Coverage; //[%] -> annual middled of each year -> in txt_end
	vector<double> vecDHW_Coverage; //[%] -> in every time step in first and last year; with U_st_useful_dhw
	double DHWLC_year; //annual middled DHW load coverage [%]
	vector<double> resVecDHW_Coverage; //[%] -> annual middled of each year -> in txt_end
	vector<double> vecHLdhw_Coverage; //[%] -> with U_st_useful_dhw
	double JAZ_WP; // JAZ middled over hole run time of one year -> only HP and soil pump if integrated in HP
	double JAZ_WPA; // JAZ middled over hole run time of one year -> HP + soil Pump + second heat generator (bivalent systems)
	//double JAZ_WPHA; // JAZ middled over hole run time of one year ToDo: implement/ calculate??
	vector<double> vecJAZ_WP; // JAZ of each year: one entry from 0 - 8759 h, second from 8760 - 17519, ...
	vector<double> vecJAZ_WPA; // JAZ of each year: one entry from 0 - 8759 h, second from 8760 - 17519, ...
	//vector<double> vecJAZ_WPHA; // JAZ of each year: one entry from 0 - 8759 h, second from 8760 - 17519, ...
//*relevant results for dimensioning tool*
	double t_runMiddled; //run time of HP middled over all years [s] (full load hours)//ToDo: implement for inverter
	double JAZ_WPmiddled; // -> only HP and soil pump if integrated in HP //ToDo: -> write in XML
	double JAZ_WPAmiddled; // -> HP + soil Pump + second heat generator (bivalent systems) //ToDo: -> write in XML
	//double JAZ_WPHAmiddled; // ToDo: calculate?? //ToDo: -> write in XML

    vector<double> vecQ_HL_hp_time; //vector of heating loads (Q_HL_hp_time) of actual HP in actual timestep t (weighting included if unusual time steps) (RESULT for txt) -> //heating load for more than one year: begins at first value after each year! //only necessary for integration of unusual time steps!
    vector<double> vecQ_dhw_hp_time; //vector of dhw loads (Q_dhw_hp_time) of actual HP in actual timestep t (weighting included if unusual time steps) //only necessary for integration of unusual time steps!

    double coverageRatioHeating; //[%] // yearly value for heating coverage ratio //ToDo: implement for inverter
    vector<double> vecCoverageRatioHeating; //[%] //vector of yearly values for heating coverage ratio
    double coverageRatioDHW; //[%] // yearly value for dhw coverage ratio //ToDo: implement for inverter
    vector<double> vecCoverageRatioDHW; //[%] //vector of yearly values for dhw coverage ratio
    double coverageRatio; //[%] // yearly value for coverage ratio (dhw + heating) //ToDo: implement for inverter
    vector<double> vecCoverageRatio; //[%] //vector of yearly values for coverage ratio (dhw + heating)

	//ToDo? coverageRatio heating element in monovalent systems

	//double middledHLC; // [%] middled heating load coverage over all years
	double middledHCR; // [%] middled heating coverage ratio over all years //ToDo: implement for inverter //ToDo: -> write in XML ResCoverRatioH
	//double middledDHWLC; // [%] middled DHW load coverage over all years
	double middledDHWCR; // [%] middled DHW coverage ratio over all years //ToDo: implement for inverter //ToDo: -> write in XML ResCoverRatioDHW
	double middledCR; // [%] middled coverage ratio (dhw + heating) over all years  //ToDo: implement for inverter //ToDo: -> write in XML ResCoverRatioTotal

//++++++++ blocking ++++++++
	bool blockHP; //block in this time step (yes or no)
	bool ValidBlockHP;
	bool blockMarker; //blocking marker -> if this is on, then HP is blocked -> necessary for blocking time >= 3600
	bool fullLoad; //actually storages are in full load mode
	unsigned int blockCounter; //??
	vector <bool> resVecBlockHP; //result vector of blockHP
	double blockingTime; // Blocking time at a piece -> parse XML: <heat_pump:blockingTime uom="s">7200</heat_pump:blockingTime>
	//double timerLoadStFull; //timer for loading the storages full -> not necessary any more //ToDo: implement in GeoWPSys //calculated out of V_st and nominalPowerH [s]?
	//double loadTime; //only for HPControlLoadStorageFull(int i) -> calculated in HPControlLoadStorageFull(int i) not necessary to save //function for reducing the load time under 3600 s if storages are full loaded -> not implemented yet
	vector <bool> vecBlockHP; // 8760 values -> year-vector of hours: every start of blocking time is true -> same procedure for every day of the year (like dhw_load)
	//ToDo?? middledRunPeriod // middled on/off-period of HP in each Year or numberOnOffcycles



//+++ValidVariables for iteration with MPFluidGeTIS
	double ValidT_soil_out;
	bool Validcrit_soil;
	double ValidU_st_useful;
	double ValidT_st;
	double ValidU_st_useful_dhw;
	double ValidT_st_dhw;
	double ValidQ_h;
	double ValidQ_dhw;
	double ValidP_el;
	double ValidCOP;
	double ValidT_soil_in;
	double ValidVdot_soil_bhe;
	unsigned int ValidHP_mode;
	bool ValidHP_on;
	double ValidQ_HL_hp_weight;
	double ValidQ_HL_hp_time;
	double ValidQ_dhw_hp_weight;
	double ValidQ_dhw_hp_time;
	double ValidTimeCritSoil;
	//weighting
    bool ValidWeightMarker; //weighting because of time steps which are not a part of 3600 for iteration
    //blocking time
	bool ValidBlockMarker;
	unsigned int ValidBlockCounter;
	bool ValidFullLoad; //HP and secHeatGen are in full load modus
	//bivalent
	bool ValidSecHeatGen_on;
	double ValidQ_hSecHeatGen;
	double ValidEnergySecHeatGenH;
	double ValidEnergySecHeatGenDHW;
	double ValidQ_dhwSecHeatGen;


//+++ make txt +++
	string fileNameTXTfirstYear;
	string fileNameTXTlastYear;
	string fileNameTXT_end;

	friend class HpCore;
	friend class Parser;
	friend class HPLink;

};

#endif /* HEATPUMP_H_ */
