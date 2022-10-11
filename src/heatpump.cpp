/*
 * heatpump.cpp
 *
 *  Created on: 05.12.2017
 *      Author: Sebastian Weck-Ponten
 */

#include "include.h"

using namespace std;


HeatPump::HeatPump() {
	simulateHP = true;
	ID = "HP01";
	HPname = "WPF 00";
	t_run = 0.0;
	//runTimeFullLoad = 0.0;
	Q_h = 0.0;
	count_h = 0;
	Qdot_hMiddle = 0.0;
	Q_dhw = 0.0;
	count_dhw = 0;
	Qdot_dhwMiddle = 0.0;
	P_el = 0.0;
	P_elMiddle = 0.0;
	Qdot_soilMiddle = 0.0;
	Q_soil = 0.0;
	COP = 0.0;
	heatingLoadCoverage = 0.0;
	HLC_year = 0.0;
	DHWLC_year = 0.0;
	dhwLoadCoverage = 0.0;
	Q_HL_hp_weight = 0.0;
	Q_HL_hp_time = 0.0;
	buildEnergyHeating = 0.0;
	coverageRatioHeating = 0.0;
	Q_dhw_hp_weight = 0.0;
	Q_dhw_hp_time = 0.0;
	buildEnergyDHW = 0.0;
	coverageRatioDHW = 0.0;
	coverageRatio = 0.0;
	operatingMode = 0;
	app_range = 0;
	compressor_mode = 0;
	T_soil_in = 273.15;
	T_soil_out = 273.15;
	Vdot_soil = 0.0;
	Vdot_soil_init = 0.0;
	Vdot_soil_bhe = 0.0;
	P_el_soil_pump = 0.0;
	Vol_st = 0.0;
	losses_st = 0.0;;
	Vol_st_dhw = 0.0;
	losses_st_dhw = 0.0;
	T_st_dhw = 328.15;
	T_st_min_dhw = 273.15;
	T_st_max_dhw = 273.15;
	hysteresis = 0.0;
	//Physics:
	Tsoil_cellcrit = 268.15; //-5 °C
	rhoCp_soil = 10;
	//storage
	HP_storage = 0;
	T_st = 273.15;
	T_st_min = 273.15 + 0; //+ 26;
	T_st_max = 273.15 + 0; //+ 35;
	T_h_step = 273.15 + 0;
	T_dhw_step = 273.15 + 55;
	U_st_useful = 0.0;
	U_st_useful_dhw = 0.0;
	//heating_hyst = 0;
	HP_mode = 1;
	HPmodeLastTimeStep = 1;
	HP_on = true;
	crit_soil = false;
	timeCritSoil = 0.0;

	JAZ_WP = 0.0;
	JAZ_WPA = 0.0;
	//JAZ_WPHA = 0.0;
	//***Valid
	ValidT_soil_out = 278.15;
	Validcrit_soil = false;
	ValidU_st_useful = 0.0;
	ValidT_st = 278.15;
	ValidU_st_useful_dhw = 0.0;
	ValidT_st_dhw = 278.15;
	ValidQ_h = 0.0;
	ValidQ_dhw = 0.0;
	ValidP_el = 0.0;
	ValidCOP = 0.0;
	ValidT_soil_in = 278.15;
	ValidVdot_soil_bhe = 0.0;
	ValidHP_mode = 1;
	ValidHP_on = true;
	ValidTimeCritSoil = 0.0;
	ValidQ_HL_hp_weight = 0.0;
	ValidQ_HL_hp_time = 0.0;
	ValidQ_dhw_hp_weight = 0.0;
	ValidQ_dhw_hp_time = 0.0;
	ValidWeightMarker = false;
	weightMarker = false;
	//Q_soilMPF = 0;
	Vdot_soil_bhe_init = 0.0;
	E_h_year = 0.0;
	E_dhw_year = 0.0;
	E_year = 0.0;
	E_el_year = 0.0;
	//***results for dimensioning tool
	t_runMiddled = 0.0;
	middledHCR = 0.0;
	middledDHWCR = 0.0;
	middledCR = 0.0;
	JAZ_WPmiddled = 0.0;
	JAZ_WPAmiddled = 0.0;
	//JAZ_WPHAmiddled = 0.0;
	//***blocking time***
	//blockCounter = 0;
	//loadTime = 0.0;
	blockHP = false;
	ValidBlockHP = false;
	blockMarker = false;
	ValidBlockMarker = false;
	fullLoad = false;
	ValidFullLoad = false;
	blockingTime = 0.0;
	blockCounter = 0;
	ValidBlockCounter = 0;
	//***inverter***
	nominalPowerH = 0.0; //also for calculating timerLoadStFull
	nominalPowerDHW = 0.0;
	//***bivalent***
	secHeatGen_on = false;
	ValidSecHeatGen_on = false;
	nominalPowerSecHeatGen = 0.0;
	shutDownHL = 0.0;
	Q_hSecHeatGen = 0.0;
	ValidQ_hSecHeatGen = 0.0;
	energySecHeatGenH = 0.0;
	ValidEnergySecHeatGenH = 0.0;
	energySecHeatGenDHW = 0.0;
	ValidEnergySecHeatGenDHW = 0.0;
	Q_dhwSecHeatGen = 0.0;
	ValidQ_dhwSecHeatGen = 0.0;
	coverageRatioHeatingSecHeatGen = 0.0;
	coverageRatioDHWsecHeatGen = 0.0;
	coverageRatioSecHeatGen = 0.0;
	middledCRsecHeatGen = 0.0;
	secHeatGenOnInBlockingTime = 0;
	//+++cooling+++
	coolingType = 0;
	T_maxPassivCooling = 273.15;
	Q_COOL_hp_time = 0.0;
	Q_COOL_hp_weight = 0.0;
	buildEnergyCooling = 0.0;
	energyCooling = 0.0;
	ValidEnergyCooling = 0.0;
	Vdot_soilBHEcoolingInit = 0.0;
	Vdot_soilCooling = 0.0;
	Vdot_soilBHEcooling = 0.0;
	vaildVdot_soilBHEcooling = 0.0;
	coveringRatioCooling = 0.0;
	middledCRcooling = 0.0;
	//+++critInterpolation+++
	critInterpolation = false;
}





