//============================================================================
// Name        : hps.cpp
// Author      : Sebastian Weck-Ponten
// Version     :
// Copyright   : Your copyright notice
//Description : Heat pump system model including storage tanks and control strategies
//============================================================================
//#include "heatpump.h"


//!hps top hirarchy level
/*!
hps contains the top hierarchy level on which the time loop is executed and all classes of the code are called.
hps is used as long as ModHPS is the master model in case of a coupling with an subsurface model.
If ModHPS is the slave model, hps can be skipped. In addition, T_soil,out is defined within hps in case ModHPS simulates without a coupled subsurface model.
*/

#include "include.h"
#include <ctime>

//#define PY_SSICE_T_CLEAN //for coupling with analytical soil model GeTIS
//#include "Python.h" //for coupling with analytical soil model GeTIS
//#include </usr/include/python2.7/Python.h>

using namespace std;




int main() {

	HPLink Model;

	using namespace std;
	clock_t begin = clock();


//********************* initialize  *********************
	bool check = Model.initialize();
	if(check  == false){
		return false;
	}

//********************* get dt_com from subsoil model *********************
	//get dt_comm
	//double dt = 5;
	//Model.get_dt_comm(dt); // only when HP is the slave otherwise parse!!
	//cout << "HPS: delta time step in hps = " << dt << "\n";


//********************* parse dt_comm, dt_ug, t_end, ugModel *********************
	double dt = Model.parse_dt_comm();
	cout << "HPS: delta time step in hps after parse dt_comm = " << dt << "\n";

	vector<string> bhe_sequence = Model.sendBHEseq();
	//cout << "HPS: How many bhes? "<< bhe_sequence.size() << "\n";
	//cout << "HPS: Sequence of all_linked_bhe:" << "\n";
	//for(vector<string>::iterator vit=bhe_sequence.begin();vit!=bhe_sequence.end();++vit) cout << *vit << "\n";

	//V_soil_bhe(HP) / #bhe(HP)
	vector<double> vecVnom = Model.sendVbheNominal();
	//cout << "HPS: V_soil_bhe_nominal size = " << vecVnom.size() << "\n";
	//cout << "HPS: V_soil_bhe_nominal of all bhes are:" << "\n";
	//for(vector<double>::iterator vit=vecVnom.begin();vit!=vecVnom.end();++vit) cout << *vit << "\n";

	double t_end = Model.parse_t_end();
	cout << "HPS: t_end in hps after parse t_end = " << t_end << "\n";
	//dt = 15; //test
	//dt = 3600; //test
	//double t_end = 3600 * 8760 * 2; //s //comment out for validation test
	//cout << "HPS: t_end out of hps= " << t_end / 3600 / 8760 << " a" << "\n";

	unsigned int ugModelType = Model.parse_ugModel();
	cout << "HPS: ugModelType which is coupled to heat pump = " << ugModelType << " (0 = const. soil temperature; 1 = decreasing soil temperature; 2 = soil temperature data set; 3 = MPFluidGeTIS (numerical); 4 = analytical GeTIS; 5 = other iterative model; 6 = other non-iterative model; 7 = district heating network model)" << "\n";

	//********************* dt_ug  *********************
	double dt_ug;

	if( (ugModelType == 0) || (ugModelType == 1) || (ugModelType == 4) ){ //0 = const. soil temperature; 1 = decreasing soil temperature; 4 = analytical soil model GeTIS
		dt_ug = dt;
	}else if(ugModelType == 3){ // 3 = MPfluidGeTIS //-> this is only a dummy; if ModHPS is coupled to MPFluidGeTIS, Hps.cpp will not be used!
		dt_ug = 1800; // has to be a multiple of dt
	}


	//dt_ug = 1800; // has to be a multiple of dt

//****test validation****
/*

//get data out of "../import/SeW_Nov2016_validation.txt" only column 1 and 2
	vector<double> T_out_test;
	//vector<double> V_soil_test; //validation with measured Vdot


	double a,b,c,d,e,f;
	ifstream myfile ("../import/SeW_Nov2017_validierung_dt_17500_Vdot.txt"); //"../import/SeW_Nov2016_validierung.txt"
	 if (myfile.is_open())
	 {
	    while (myfile >> a >> b >> c >> d >> e >> f)
	    {
	      //cout << b << '\n';
	    	double result = c + 273.15;
	    	T_out_test.push_back(result);
	    	//V_soil_test.push_back(f); //validation with measured Vdot
	    }
		//single line
	    myfile >> a >> b;
		//cout << b << '\n';
	    //T_out_test.push_back(b);


	    myfile.close();
	 }

	 else cout << "HPS: Unable to open file" <<"\n";

	 cout << "HPS: Vector T_out_test with vector size = " << T_out_test.size() << '\n';
	 //for(vector<double>::iterator vit=T_out_test.begin();vit!=T_out_test.end();++vit)  cout << *vit<< "\n";

	 double t_end = T_out_test.size() * 60; //++++validation++++

*/

//***** end test validation *****


	int vecSize = vecVnom.size();
	vector<double> T_MPF_out(vecSize, 273.15+10);
/*	vector<double> T_MPF_out;
	double input;
	for(int i=0; i < vecVnom.size(); i++){//test
		input = 273.15 + 8 + i;
		T_MPF_out.push_back(input);
	}*/

	if(ugModelType == 0){ //0 = const. soil temperature;
		T_MPF_out.assign(vecSize, 273.15+2);
	}

	vector<double> T_MPF_min = T_MPF_out;

/*	input = 0;
	for(int i=0; i < vecVnom.size(); i++){//test
		input = 273.15 + 3 + i;
		T_MPF_min.push_back(input);
	}*/
	//vector<double> T_MPF_min(vecSize, 273.15+8);
	//cout << "HPS: T_MPF_out of first time step:" << "\n";
	//for(vector<double>::iterator vit=T_MPF_out.begin();vit!=T_MPF_out.end();++vit) cout << *vit << "\n";

	//***** test validation *****
	//double V_test_res; //validation with measured Vdot
	//***** end test validation *****

	//bool HeatFluxCheck; //test -> not necessary -> only for testing

	//double res = T_MPF_out[0];



	bool iter;
	iter = true;
	unsigned int iterationSize;

	if( (ugModelType == 3) || (ugModelType == 5) ){ //5 = other iterative model; 3 = MPfluidGeTIS //-> this is only a dummy; if ModHPS is coupled to MPFluidGeTIS, Hps.cpp will not be used!
		//int counter = 0;
		iterationSize = 3; //iteration size for MPFluid
		//unsigned int countIter = 0;
	}
	else { //
		 iterationSize = 1; //if no Iteration choose iterationSize = 1
	}





///////////////////////////////////////////////////////////////////////////

//for coupling with analytical soil model GeTIS
	//Initialize the python instance
	//Py_Initialize();
//for coupling with analytical soil model GeTIS

	//here the call of ModHPS is faked from a subsurface model with different time step size!!! (Example GeTIS)
	unsigned int timeSteps = ceil(t_end/dt_ug);
	unsigned int n = ceil(dt_ug / dt);

	double res;
	vector<double> vecV_send;
	vector<double> vecT_send;

//time loop over Model.hpCalc(), Model.sendT(),	Model.sendV() and get T_soil_out & T_soil_min

	for (unsigned int time = 0; time < timeSteps; time++) { //subsurface model time steps
		//cout << "HPS: actual time step in hps = " << time << "\n"; //test
		//cout <<"Teeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeest " <<time<< " "<< timeSteps << endl; //test
		// updates


		//******input code of analytical soil model GeTIS here******


		//Run a simple string
		//PyRun_SimpleString("from time import time,ctime\n""print('Today is',ctime(time()))\n");
		//-> error:
		/*/tmp/ccW89gIR.o: In Funktion `main':	hps.cpp:(.text+0x2b2): Nicht definierter Verweis auf `Py_Initialize'
		hps.cpp:(.text+0x2c1): Nicht definierter Verweis auf `PyRun_SimpleStringFlags'
		collect2: error: ld returned 1 exit status*/

		//Run a simple file
/*		FILE* PScriptFile = fopen("simple_example.py", "r");
		if(PScriptFile){
			PyRun_SimpleFile(PScriptFile, "simple_example.py");
			fclose(PScriptFile);
		}*/
		//-> error:
		/*/tmp/ccW89gIR.o: In Funktion `main':	hps.cpp:(.text+0x2b2): Nicht definierter Verweis auf `Py_Initialize'
		hps.cpp:(.text+0x2c1): Nicht definierter Verweis auf `PyRun_SimpleStringFlags'
		collect2: error: ld returned 1 exit status*/

		if (ugModelType == 1){ //1 = decreasing soil temperature;
			//loop for decreasing T_Soil_out
			if (time > 0) {
				res = T_MPF_out[0] - dt_ug / (t_end) * (10-0); // (8+3) = from 8 °C to -3 °C (if starts from 8) //old: res = T_MPF_out[0] - dt / (t_end) * (8+6) = from 8 to -6 -> HP turned off in last year

				T_MPF_out.assign(vecSize, res);
				T_MPF_min.assign(vecSize, res+5.5); //T_MPF_min.assign(vecSize, res+5.5);
	/*			if(T_MPF_out[0]<= 273.15-5){
					res=res+3;
					T_MPF_out.assign(vecSize, res);
				}*/
			}
		}



		for (unsigned int it = 0; it < iterationSize; it++) {
			//cout <<"Teeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeeest it" <<it<< " "<< iterationSize << endl; //test
			// UG

			// WP -> # = dt_ug / dt

			for (unsigned int j = 0; j < n; j ++) {

				//Model.sendV();
				vecV_send = Model.sendV(); //send V_soil to subsurface model
				//cout << "HPS: Actual VecV_soil_bhe:" << "\n";
				//for(vector<double>::iterator vit=vecV_send.begin();vit!=vecV_send.end();++vit)  cout << *vit<< "\n";
				//Model.sendT();
				vecT_send = Model.sendT(); //send T_soil_in to subsurface model
				//cout << "HPS: Actual vecT_soil_in:" << "\n";
				//for(vector<double>::iterator vit=vecT_send.begin();vit!=vecT_send.end();++vit)  cout << *vit<< "\n";

				//call analytical soil model GeTIS
				//xxx
/*				if(ugModelType == 4){

				}*/

				//get T_soil_min
				Model.getTsoil_min(T_MPF_min);
				//get T_soil_out
				Model.getTsoil_out(T_MPF_out);


				//calculate
				//cout << "HPS: Actual countIter for iteration step beforeeeeeeeeeeeeeeeeeeeeeee hpCalc = " << countIter  << "\n";
				Model.hpCalc(); //start hpCalc

			}

			if( (ugModelType == 3) || (ugModelType == 5) ){ //5 = other iterative model; 3 = MPfluidGeTIS //-> this is only a dummy; if ModHPS is coupled to MPFluidGeTIS, Hps.cpp will not be used!
				if(it == iterationSize -1){
					iter = true;
				}
				else{
					iter = false;
				}
				//cout << "HPS: Actual iter: " << std::boolalpha << iter << "\n";
			 }
			 Model.getIterCommand(iter); // iter = false only if coupling with MPFluidGeTIS!!!

		}


	}




///////////////////////////////////////////////////////////////////////////


	//****validation test****
			/*

					vector<double> V_test;
					if(time/dt <= T_out_test.size()){
					double res =  T_out_test[time/dt];
					//V_test_res = V_soil_test[time/dt]; // for validation with measured Vdot
					T_MPF_out.assign(vecSize, res);
					}
					else{
						T_MPF_out.assign(vecSize, 0);
					}
					//get T_soil_min for validation
					Model.getTsoil_min(T_MPF_min);
					//get T_soil_out for validation
					Model.getTsoil_out(T_MPF_out);
					//get_V_soil_test for validation with measured Vdot
					//Model.getVsoil_test(V_test_res);
			*/

	//****end validation test****


//**************old

/*

//++++start loop++++
	for(double time = 0; time < t_end; time+dt){

		//HeatFluxCheck = Model.CheckHeatFluxSoil(); //test -> not necessary -> only for testing


//commented out for validation test



		//double resTest = 293.149999999; //Test
		//T_MPF_out.assign(vecSize, resTest); //Test


		if(iter == true){
			time = time + dt;
		}

	}
*/


//****end loop****



	Model.postprocessing();

	clock_t end = clock(); //test
	double elapsed_secs = double(end - begin) / CLOCKS_PER_SEC; //test
	cout << "HPS: Simulation time = elapsed_secs = " << elapsed_secs << endl;

	return true;
}
