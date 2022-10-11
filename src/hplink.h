/*
 * hplink.h
 *
 *  Created on: 12.12.2017
 *      Author: Sebastian Weck-Ponten
 */

#ifndef HPLINK_H_
#define HPLINK_H_

#include "hpcore.h" //included because of connection to Erics model
using namespace std;

//!Class for linking HpCore (core clas of ModHPS) to hps or a coupled subsurface model


class HPLink {
public:
	HPLink();

	bool initialize(); //void initialize();
	double parse_dt_comm();
	double parse_t_end();
	unsigned int parse_ugModel();
	void get_dt_comm(double);
	vector<double> sendVbheNominal();
	vector<string> sendBHEseq();
	void getIterCommand(bool); //get iteration command from MPFluid for implicit calculation -> no influence if coupling with static subsurface calculation of GeTIS
	void getTsoil_out(vector<double>);
	void getTsoil_min(vector<double>);
	//void getVsoil_test(double); //****validation test****
	void hpCalc();
	vector<double> sendT();
	vector<double> sendV();
	double sendCurrentTime();
	void postprocessing();



private:
	HpCore Core;

	vector<double> vecT_soil_in; //vector for hpLink
	vector<double> VecV_soil_bhe;

	friend class HPCore;

};

#endif /* HPLINK_H_ */
