/*
 * PlottingSettings.cpp
 *
 *  Created on: 05.11.2019
 *      Author: Tobias
 */

#include <iostream>
#include "PlottingSettings.h"
#include <vector>
#include <fstream>
#include <limits>
#include <string>
#include <utility> // std::pair
#include <stdexcept> // std::runtime_error
#include <sstream> // std::stringstream

using namespace std;



//Constructor
Plotting_Settings::Plotting_Settings() {
	this->_title = "";
	this->_xlable = "";
	this->_ylable = "";
	this->_y2lable = "";
	this->_x_unit = "d";
	this->show_description = 0;
	this->show_title = 1;
	this->_global_timestep = 60;

	this->_x_font = "Myriad Pro";
	this->_y_font = "Myriad Pro";
	this->_x_font_size = 30;
	this->_y_font_size = 30;
	this->_x_tic_size = 5;
	this->_y_tic_size = 5;
	this->_x_tic_font_size = 20;
	this->_y_tic_font_size = 20;


	this->y_tic_distance = 0;
	this->y2_tic_distance = 0;

	this->additional_bmargin = 0;
	this->additional_tmargin = 0;
	this->additional_lmargin = 0;
	this->additional_rmargin = 0;

	this->set_description_values("","","","","","","","","","","","","","","","","");

	this->set_pathes_2x2("", 0);
	this->set_pathes_2x2("", 1);
	this->set_pathes_2x2("", 2);
	this->set_pathes_2x2("", 3);

	this->set_offset_2x2(0, 0);
	this->set_offset_2x2(0, 1);
	this->set_offset_2x2(0, 2);
	this->set_offset_2x2(0, 3);


}

Plotting_Settings::Plotting_Settings(string title, string xlable, string ylable, string y2lable){
	this->_title = title;
	this->_xlable = xlable;
	this->_ylable = ylable;
	this->_y2lable = y2lable;
	this->_x_unit = "d";
	this->show_description = 0;
	this->show_title = 1;
	this->_global_timestep = 60;

	this->_x_font = "Myriad Pro";
	this->_y_font = "Myriad Pro";
	this->_x_font_size = 30;
	this->_y_font_size = 30;
	this->_x_tic_size = 5;
	this->_y_tic_size = 5;
	this->_x_tic_font_size = 20;
	this->_y_tic_font_size = 20;

	this->y_tic_distance = 0;
	this->y2_tic_distance = 0;

	this->additional_bmargin = 0;
	this->additional_tmargin = 0;
	this->additional_lmargin = 0;
	this->additional_rmargin = 0;

	this->set_description_values("","","","","","","","","","","","","","","","","");

	this->set_pathes_2x2("", 0);
	this->set_pathes_2x2("", 1);
	this->set_pathes_2x2("", 2);
	this->set_pathes_2x2("", 3);

	this->set_offset_2x2(0, 0);
	this->set_offset_2x2(0, 1);
	this->set_offset_2x2(0, 2);
	this->set_offset_2x2(0, 3);

};


//Destructor
Plotting_Settings::~Plotting_Settings() {
	// TODO? Auto-generated destructor stub
}


// SET FUNCTIONS
void Plotting_Settings::set_title(string title){
		_title = title;
		}
void Plotting_Settings::set_xlable(string xlable){
		_xlable = xlable;
	}
void Plotting_Settings::set_ylable(string ylable){
		_ylable = ylable;
	}

void Plotting_Settings::set_y2lable(string y2lable){
		_y2lable = y2lable;
	}

void Plotting_Settings::set_x_font(string font){
		_x_font = font;
}
void Plotting_Settings::set_y_font(string font){
		_y_font = font;
}
void Plotting_Settings::set_x_font_size(int size){
		_x_font_size = size;
}
void Plotting_Settings::set_y_font_size(int size){
		_y_font_size = size;
}
void Plotting_Settings::set_x_tic_size(int size){
		_x_tic_size = size;
}
void Plotting_Settings::set_y_tic_size(int size){
		_y_tic_size = size;
}
void Plotting_Settings::set_x_tic_font_size(int size){
		_x_tic_font_size = size;
}
void Plotting_Settings::set_y_tic_font_size(int size){
		_y_tic_font_size = size;
}

void Plotting_Settings::set_x_unit(string x_unit){
		_x_unit = x_unit;
	}

void Plotting_Settings::set_show_title(bool show_title){
		this->show_title = show_title;
	}

void Plotting_Settings::set_show_description(bool show_description){
		this->show_description = show_description;
	}

void Plotting_Settings::set_description_values(vector<string> desc_values){
		this->_description_values = desc_values;
	}

void Plotting_Settings::set_description_values(string name,
		string linked_to_building,
		string linked_to_bhe,
		string v_soil,
		string sim_time,
		string end_time,
		string appl_range,
		string heating_storage_vol,
		string t_min,
		string t_max,
		string dhw_storage,
		string t_min_dhw,
		string t_max_dhw,
		string run_time_per_year,
		string energy_demand,
		string scop_only_hp,
		string scop_all)
		{


		//Transfers all values into a vector
		vector<string> dummy;
		dummy.push_back(name);
		dummy.push_back(linked_to_building);
		dummy.push_back(linked_to_bhe);
		dummy.push_back(v_soil);
		dummy.push_back(sim_time);
		dummy.push_back(end_time);
		dummy.push_back(appl_range);
		dummy.push_back(heating_storage_vol);
		dummy.push_back(t_min);
		dummy.push_back(t_max);
		dummy.push_back(dhw_storage);
		dummy.push_back(t_min_dhw);
		dummy.push_back(t_max_dhw);
		dummy.push_back(run_time_per_year);
		dummy.push_back(energy_demand);
		dummy.push_back(scop_only_hp);
		dummy.push_back(scop_all);


		this->_description_values = dummy;
	}

void Plotting_Settings::set_global_timestep(double global_ts){
	this->_global_timestep = global_ts;
}

void Plotting_Settings::set_pathes_2x2(string path, int i){
	this->_pathes_2x2[i] = path;
}

void Plotting_Settings::set_offset_2x2(unsigned long offset, int i){
	this->_offset_2x2[i] = offset;
}

void Plotting_Settings::set_y_tic_distance(double y_tic_distance){
	this->y_tic_distance = y_tic_distance;
}

void Plotting_Settings::set_y2_tic_distance(double y2_tic_distance){
	this->y2_tic_distance = y2_tic_distance;
}

void Plotting_Settings::set_bmargin(double margin){
	this->additional_bmargin = margin;
}

void Plotting_Settings::set_tmargin(double margin){
	this->additional_tmargin = margin;
}

void Plotting_Settings::set_lmargin(double margin){
	this->additional_lmargin = margin;
}

void Plotting_Settings::set_rmargin(double margin){
	this->additional_rmargin = margin;
}

//GET FUNCTIONS
string Plotting_Settings::get_title(){
	return _title;
}

string Plotting_Settings::get_x_lable(){
	return _xlable;
}

string Plotting_Settings::get_y_lable(){
	return _ylable;
}

string Plotting_Settings::get_y2_lable(){
	return _y2lable;
}

string Plotting_Settings::get_x_font(){
	return _x_font;
}
string Plotting_Settings::get_y_font(){
	return _y_font;
}
int Plotting_Settings::get_x_font_size(){
	return _x_font_size;
}
int Plotting_Settings::get_y_font_size(){
	return _y_font_size;
}
int Plotting_Settings::get_x_tic_size(){
	return _x_tic_size;
}
int Plotting_Settings::get_y_tic_size(){
	return _y_tic_size;
}
int Plotting_Settings::get_x_tic_font_size(){
	return _x_tic_font_size;
}
int Plotting_Settings::get_y_tic_font_size(){
	return _y_tic_font_size;
}

string Plotting_Settings::get_x_unit(){
	return _x_unit;
}

vector<Plotting_Settings::linesettings> Plotting_Settings::get_plottings(){
	return _plottings;
}

bool Plotting_Settings::get_show_title(){
	return this->show_title;
}

bool Plotting_Settings::get_show_description(){
	return this->show_description;
}

vector<string> Plotting_Settings::get_description_values(){
	return this->_description_values;
}

double Plotting_Settings::get_global_time_step(){
	return this->_global_timestep;
}

string Plotting_Settings::get_pathes_2x2(int i){
	return this->_pathes_2x2[i];
}

unsigned long Plotting_Settings::get_offset_2x2(int i){
	return this->_offset_2x2[i];
}

double Plotting_Settings::get_y_tic_distance(){
	return this->y_tic_distance;
}

double Plotting_Settings::get_y2_tic_distance(){
	return this->y2_tic_distance;
}

double Plotting_Settings::get_bmargin(){
	return this->additional_bmargin;
}

double Plotting_Settings::get_tmargin(){
	return this->additional_tmargin;
}

double Plotting_Settings::get_lmargin(){
	return this->additional_lmargin;
}

double Plotting_Settings::get_rmargin(){
	return this->additional_rmargin;
}


// other Functions
void Plotting_Settings::append_line(bool second_axis,
									int x_data,
									int y_data,
									string title,
									string datafile,
									int aggregation,
									int which_aggregation,
									string points_or_lines,
									int type,
									float size,
									string linecolor){

	//Determine length of the plot without header (-43) TODO: Rewrite that the header length is not hardcoded
	int numLines = -43;
	ifstream file(datafile.c_str());
	string unused;
	while ( getline(file, unused) )
		++numLines;


	//Determine offset (Offset: The files year02 and year03 do not start at t=0 but at the time after one year. Therefore the
	//time elapsed until the year must be subtracted.
	float number = 0;
	long int offset = 0;
	int time_step = 0;
	ifstream inFile(datafile.c_str());

	if(!inFile.good() && datafile != ""){
		cout << "The file " << datafile.c_str() << " was not found. The process was canceled." << endl;
		throw exception();
	}

	string firstWord;

	while (inFile >> firstWord)
	{
		sscanf(firstWord.c_str(), "%e", &number); //String of the first word is transferred to number
		if (number !=0){
			offset = number;
			break;
		}

	    inFile.ignore(numeric_limits<streamsize>::max(), '\n');
	}

	time_step = this->get_global_time_step();

	if(time_step == 0){
		cout << "ATTENTION: The time_step was not set. [GNU_Executer::set_timestep(double)]. Timestep was set to 60s.." << endl;
		time_step = 60;
		this->set_global_timestep(60);
	}

	//Start difference is always one timestep too high
	if(offset == this->get_global_time_step()){
		offset = offset - this->get_global_time_step();
	}


	//Consider aggregation
	//If aggregation > 1 a new input file must be written that aggregates the data

	if(aggregation > 1){

		//Read data
		std::vector<std::pair<int, std::vector<double> > > read_in_data = this->read_plotting_values(datafile);


		//Process data with aggregation over all values
		std::vector<std::pair<int, std::vector<double> > > agg_data = this->aggregate_data(read_in_data, aggregation, which_aggregation);




		//Create new file name
		datafile = datafile.substr(0, datafile.size() - 4) + "_agg_" + title + ".txt";
		//cout << datafile << endl;
		//save files
		this->write_data_to_file(agg_data, datafile);

	}



	//int pos = int(_plottings.end());
	linesettings dummy;
	dummy.x_data = x_data;
	dummy.y_data = y_data;
	dummy.title = title;
	dummy.datafile = datafile;
	dummy.linecolor = linecolor;

	//DEFAULT settings for lines and points
	if(points_or_lines == "lines" || points_or_lines == "line"){
		if(size == -1){
			dummy.size = 0;
		}
		else{
			dummy.size = size;
		}

		if(type == -1){
			dummy.type = 1;
		}
		else{
			dummy.type = type;
		}
	}
	else if(points_or_lines == "points" || points_or_lines == "point"){
		if(size == -1){
			dummy.size = 0.5;
		}
		else{
			dummy.size = size;
		}

		if(type == -1){
			dummy.type = 0;
		}
		else{
			dummy.type = type;
		}
	}
	else if(points_or_lines == "linespoints"){
		if(size == -1){
			dummy.size = 0.5;
		}
		else{
			dummy.size = size;
		}

		if(type == -1){
			dummy.type = 2;
		}
		else{
			dummy.type = type;
		}
	}
	else{
		dummy.size = size;
		dummy.type = type;
	}


	if(dummy.size == -1){
		dummy.size = 1;
	}
	if(dummy.type == -1){
		dummy.type = 1;
	}
	dummy.x_plot_length = numLines;
	dummy.time_steps = time_step;
	dummy.second_axis = second_axis;
	dummy.offset = offset;
	dummy.line_or_points = points_or_lines;


	_plottings.push_back(dummy);


}

std::vector<std::pair<int, std::vector<double> > > Plotting_Settings::read_plotting_values(string path){


    // Create a vector of <string, int vector> pairs to store the result
    std::vector<std::pair<int, std::vector<double> > > result;

	std::ifstream resultFile(path.c_str());

	if(!resultFile.is_open()) throw std::runtime_error("Could not open file");

	// Helper vars
	std::string line, colname;
	double val;

	// Read the column names
	if(resultFile.good())
	{
		//Comment line Not found
		bool commentary_line = true;
		//Search for first line without comment
		while(commentary_line){

			//Iterate over lines
			std::getline(resultFile, line);
			//Check if line starts with #
			if(line[0] != '#'){
				commentary_line = false;
			}
		}

		// Create a stringstream from line
		std::stringstream ss(line);

		int colIdx = 1;
		// Extract each column name
		while(std::getline(ss, colname, '\t')){

			// Initialize and add <colname, int vector> pairs to result
			result.push_back({colIdx, std::vector<double> {}});
			//increase ColumnIndex
			colIdx++;


		}
	}

	// Read data, line by line
	while(std::getline(resultFile, line))
	{
		// Create a stringstream of the current line
		std::stringstream ss;
		ss << (line);

		// Keep track of the current column index
		int colIdx = 0;

		// Extract each integer
		while(ss >> val){


			// Add the current integer to the 'colIdx' column's values vector
			result.at(colIdx).second.push_back(val);

			// If the next token is a comma, ignore it and move on
//			if(ss.peek() == ',') ss.ignore();

			// Increment the column index
			colIdx++;
		}
	}

	// Close file
	resultFile.close();





	return result;

}

std::vector<std::pair<int, std::vector<double> > > Plotting_Settings::aggregate_data(std::vector<std::pair<int, std::vector<double> > > data,
																					int aggregation,
																					int which_aggregation = 0){


	std::vector<std::pair<int, std::vector<double> > > result;

	switch (which_aggregation){
	case 0:
		//----Iterate over each record----
		for(unsigned int i= 0; i < data.size(); i ++){

			std::pair<int, std::vector<double> > dummy_column;
			dummy_column.first = i + 1;

			result.push_back(dummy_column);

			//Aggregation counter
			int a = 0;
			double sum = 0;

			//Iterate over values of one column
			for(unsigned int j = 0; j < data.at(i).second.size() ; j++){

				a++;
				sum = sum + data.at(i).second.at(j);

				if( a >= aggregation){
					result.at(i).second.push_back(sum / a);

					a = 0;
					sum = 0;
				}


			}
		}
		break;


	case 1:
		//----Iteration only over values greater than zero----
		for(unsigned int i= 0; i < data.size(); i ++){

			std::pair<int, std::vector<double> > dummy_column;
			dummy_column.first = i + 1;

			result.push_back(dummy_column);

			//Aggregation counter
			int a = 0;
			int a_over_zero = 0;
			double sum = 0;

			//Iteration over values of a column
			for(unsigned int j = 0; j < data.at(i).second.size() ; j++){

				//Only values greater than zero
				if(data.at(i).second.at(j) > 0){
					a_over_zero++;
					sum = sum + data.at(i).second.at(j);
				}

				a++;

				if( a >= aggregation){

					//exclude a==0
					if(a_over_zero==0){
						result.at(i).second.push_back(0);

					}

					else{
						result.at(i).second.push_back(sum / a_over_zero);
					}


					a = 0;
					a_over_zero = 0;
					sum = 0;
				}


			}
		}

		break;

	default:
		cout << "For which_aggregation = " << which_aggregation << " is no bill implemented.";
		break;

	}

	return result;
}

void Plotting_Settings::write_data_to_file(std::vector<std::pair<int, std::vector<double> > > data, string path){

	ofstream file;
	file.open(path.c_str());


	//iterate over lines
	for(unsigned int j = 0; j < data.at(0).second.size(); j++){

		//column iteration
		for(unsigned int i = 0; i < data.size(); i++){

			//stream value
			file << data.at(i).second.at(j);

			//set tab stop
			file << "\t";

		}

		//new line
		file << "\n";
	}

	file.close();

}








