/*
 * use_gnu.cpp
 *
 *  Created on: 05.11.2019
 *      Author: Tobias
 */
#include <iostream>
#include <fstream>
#include <sstream>
#include <memory>
#include "PlottingSettings.h"
#include "UseGNU.h"
#include <vector>
#include <cstdlib>
#include <math.h>
#include <limits>


using namespace std;


//help functions
string tostring(double Number){
	string Result;
	ostringstream convert;
	convert << Number;
	Result = convert.str();
	return Result;
}

//copy data
bool copy_file(string From, string To, std::size_t MaxBufferSize = 1048576){
    std::ifstream is(From.c_str(), std::ios_base::binary);
    std::ofstream os(To.c_str(), std::ios_base::binary);

    std::pair<char*,std::ptrdiff_t> buffer;
    buffer = std::get_temporary_buffer<char>(MaxBufferSize);

    //Note that exception() == 0 in both file streams,
    //so you will not have a memory leak in case of fail.
    while(is.good() and os)
    {
       is.read(buffer.first, buffer.second);
       os.write(buffer.first, is.gcount());
    }

    std::return_temporary_buffer(buffer.first);

    if(os.fail()) return false;
    if(is.eof()) return true;
    return false;
	}



//Constructor
GNU_Executer::GNU_Executer(){
	this->_format = 0;
	this->_two_axes = 0;
	this->_outputpath_exe = "";
	this->_outputpath_pdf = "";

	this->compression_size = 1;

	this->delete_all_files = 1;
	this->plt_set.set_global_timestep(60);

	this->y1range[0] = 0;
	this->y1range[1] = 0;

	this->y2range[0] = 0;
	this->y2range[1] = 0;

	this->xLabelOffsetx = 0;
	this->xLabelOffsety = -1;
	this->yLabelOffsetx = -5;
	this->yLabelOffsety = 0;
	this->y2LabelOffsetx = 5;
	this->y2LabelOffsety = 0;

}



//GETTER AND SETTER functions

//Getter functions
Plotting_Settings GNU_Executer::get_plt_set(){
	return this->plt_set;
}

string GNU_Executer::get_outputpath_pdf(){
	return this->_outputpath_pdf;
}

string GNU_Executer::get_outputpath_exe(){
	return this->_outputpath_exe;
}

bool GNU_Executer::get_two_axes(){
	return this->_two_axes;
}

int GNU_Executer::get_format(){
	return this->_format;
}

int GNU_Executer::get_xtic_size(){
	return this->plt_set.get_x_tic_size();
}

int GNU_Executer::get_xtic_font_size(){
	return this->plt_set.get_x_tic_font_size();
}

int GNU_Executer::get_xfont_size(){
	return this->plt_set.get_x_font_size();
}

string GNU_Executer::get_xfont(){
	return this->plt_set.get_x_font();
}

int GNU_Executer::get_ytic_size(){
	return this->plt_set.get_y_tic_size();
}

int GNU_Executer::get_ytic_font_size(){
	return this->plt_set.get_y_tic_font_size();
}

int GNU_Executer::get_yfont_size(){
	return this->plt_set.get_y_font_size();
}

string GNU_Executer::get_yfont(){
	return this->plt_set.get_y_font();
}

string GNU_Executer::get_x_unit(){
	return this->plt_set.get_x_unit();
}

double GNU_Executer::get_global_timestep(){
	return this->plt_set.get_global_time_step();
}

int GNU_Executer::get_max_line_title_length(){
	int max_string_size = 0;

	for(unsigned int i = 0; i < this->plt_set.get_plottings().size(); i++){
		int count = 0;
		string line_name = this->plt_set.get_plottings()[i].title;
		while (line_name[count] != '\0'){
			count++;
		}

		if (count > max_string_size){
			max_string_size = count;
		}

	}

	return max_string_size;
}

bool GNU_Executer::get_delete_all_files(){
	return this->delete_all_files;
}

double GNU_Executer::get_y_tic_distance(){
	return this->plt_set.get_y_tic_distance();
}

double GNU_Executer::get_y2_tic_distance(){
	return this->plt_set.get_y2_tic_distance();
}

double GNU_Executer::get_bmargin(){
	return this->plt_set.get_bmargin();
}

double GNU_Executer::get_tmargin(){
	return this->plt_set.get_tmargin();
}

double GNU_Executer::get_lmargin(){
	return this->plt_set.get_lmargin();
}

double GNU_Executer::get_rmargin(){
	return this->plt_set.get_rmargin();
}


//Setter functions
void GNU_Executer::set_plt_set(Plotting_Settings plt_set){
	this-> plt_set = plt_set;
}

void GNU_Executer::set_outputpath_pdf(string outputpath_pdf){
	this->_outputpath_pdf = outputpath_pdf;
}

void GNU_Executer::set_outputpath_exe(string outputpath_exe){
	this->_outputpath_exe = outputpath_exe;
}

void GNU_Executer::set_compression_size(int compression_size){
	this->compression_size = compression_size;
}

void GNU_Executer::set_two_axes(bool two_axes){
	this->_two_axes = two_axes;
}

void GNU_Executer::set_format(int format){
	this->_format = format;
}


void GNU_Executer::set_tic_size(int size){
	this->plt_set.set_x_tic_size(size);
	this->plt_set.set_y_tic_size(size);
}

void GNU_Executer::set_tic_font_size(int size){
	this->plt_set.set_x_tic_font_size(size);
	this->plt_set.set_y_tic_font_size(size);
}

void GNU_Executer::set_font_size(int size){
	this->plt_set.set_x_font_size(size);
	this->plt_set.set_y_font_size(size);
}

void GNU_Executer::set_font(string font){
	this->plt_set.set_x_font(font);
	this->plt_set.set_y_font(font);
}


void GNU_Executer::set_xtic_size(int size){
	this->plt_set.set_x_tic_size(size);
}

void GNU_Executer::set_xtic_font_size(int size){
	this->plt_set.set_x_tic_font_size(size);
}

void GNU_Executer::set_xfont_size(int size){
	this->plt_set.set_x_font_size(size);
}

void GNU_Executer::set_xfont(string font){
	this->plt_set.set_x_font(font);
}

void GNU_Executer::set_ytic_size(int size){
	this->plt_set.set_y_tic_size(size);
}

void GNU_Executer::set_ytic_font_size(int size){
	this->plt_set.set_y_tic_font_size(size);
}

void GNU_Executer::set_yfont_size(int size){
	this->plt_set.set_y_font_size(size);
}

void GNU_Executer::set_yfont(string font){
	this->plt_set.set_y_font(font);
}



void GNU_Executer::set_x_unit(string x_unit){
	this->plt_set.set_x_unit(x_unit);
}

void GNU_Executer::set_global_timestep(double ts){
	this->plt_set.set_global_timestep(ts);
}

void GNU_Executer::set_delete_all_files(bool value){
	this->delete_all_files = value;
}

void GNU_Executer::set_yrange(int axe, double from, double to){
	if(axe == 1){
		this->y1range[0] = from;
		this->y1range[1] = to;
	}

	else if(axe == 2){
		this->y2range[0] = from;
		this->y2range[1] = to;
	}

	else{
		cout << "yrange could not be set because axis 1 or 2 was not selected." << endl;
	}
}

void GNU_Executer::set_y_tic_distance(double y_distance){
	this->plt_set.set_y_tic_distance(y_distance);
}

void GNU_Executer::set_y2_tic_distance(double y2_distance){
	this->plt_set.set_y2_tic_distance(y2_distance);
}

void GNU_Executer::set_bmargin(double margin){
	return this->plt_set.set_bmargin(margin);
}

void GNU_Executer::set_tmargin(double margin){
	return this->plt_set.set_tmargin(margin);
}

void GNU_Executer::set_lmargin(double margin){
	return this->plt_set.set_lmargin(margin);
}

void GNU_Executer::set_rmargin(double margin){
	return this->plt_set.set_rmargin(margin);
}

void GNU_Executer::set_LabelOffset(string label, string direction, double value){
	if(label == "x"){
		if(direction == "x"){
			this->xLabelOffsetx = value;
		}
		else{
			this->xLabelOffsety = value;
		}
	}

	else if (label == "y"){
		if(direction == "x"){
			this->yLabelOffsetx = value;
		}
		else{
			this->yLabelOffsety = value;
		}
	}

	else if (label == "y2"){
		if(direction == "x"){
			this->y2LabelOffsetx = value;
		}
		else{
			this->y2LabelOffsety = value;
		}

	}
}

//other functions
//Writes commands for plot with a y-axis
void GNU_Executer::write_gnu_exe_one_axis(bool safe_in_pdf, string output_path_pdf, string output_path_exe){


	//Determine the longest plot (only in case data files of different length are used)
	Plotting_Settings::linesettings line;
	double x_range = 0;
	for(unsigned int i = 0; i < plt_set.get_plottings().size(); i++){
		line = plt_set.get_plottings()[i];


		if (line.x_plot_length * line.time_steps > x_range){
			x_range = line.x_plot_length * line.time_steps;
		}
	}



	//Conversion from seconds to x_unit
	int x_unit_divider;

	if(this->get_x_unit() == "s"){
		x_unit_divider = 1;
	}

	else if(this->get_x_unit() == "min"){
		x_unit_divider = 60;
	}

	else if(this->get_x_unit() == "h"){
		x_unit_divider = 3600;
	}

	else if(this->get_x_unit() == "d"){
		x_unit_divider = 3600*24;
	}

	else if(this->get_x_unit() == "w"){
		x_unit_divider = 3600*24*7;
	}

	else if(this->get_x_unit() == "m"){
		x_unit_divider = 3600*24*30.5;
	}

	else if(this->get_x_unit() == "a"){
		x_unit_divider = 1;
	}

	else{
		cout << "No available unit was selected for the x-axis. The unit was set to seconds." << endl;
		x_unit_divider = 1;
	}


	x_range = x_range/x_unit_divider;


	//Calculates the divider. Divider is a number 10ex that specifies the length of the x_range.
	//It is needed to get reasonable roundings of the x_tics
	string str_x_range;
	stringstream ss;
	ss << int(round(x_range));
	str_x_range = ss.str();
	int divider;
	if (str_x_range.length() >= 3){
		divider = pow(10, str_x_range.length()-3);
	}
	else{
		divider = 1;
	}


	//Set x_tic distances
	double d_x_range = x_range / divider;
	d_x_range = d_x_range/5;
	d_x_range = round(d_x_range);
	int x_tic_name = int(d_x_range * divider);
	int x_tic = x_tic_name*x_unit_divider;


	//dynamic set
	ofstream exe_file;
	exe_file.open(this->get_outputpath_exe().c_str());



	//Determination of the RWTH color scheme
	//	vector<string> color_scheme;
	//	color_scheme.push_back("#00549F");
	//	color_scheme.push_back("#407FB7");
	//	color_scheme.push_back("#8EBAE5");
	//	color_scheme.push_back("#C7DDF2");
	//	color_scheme.push_back("#E8F1FA");
	//	unsigned int color_counter = 0;



	//Extend RWTH color scheme
	vector<string> color_scheme;
	color_scheme.push_back("#00549F");
	color_scheme.push_back("#BDCD00");
	color_scheme.push_back("#F6A800");
	color_scheme.push_back("#CC071E");
	color_scheme.push_back("#006165");
	color_scheme.push_back("#0098A1");
	color_scheme.push_back("#57AB27");
	color_scheme.push_back("#A11035");
	color_scheme.push_back("#612158");
	color_scheme.push_back("#7A6FAC");
	unsigned int color_counter = 0;


	//Start of GNUPLOT SCRIPTS
	//static set
	exe_file << "set format x \"%10.0f\"" << endl;


	if(this->get_x_unit() == "Jahre" || this->get_x_unit() == "a"){
		exe_file << "set xrange [1:*]" << endl;
	}
	else{
		exe_file << "set xrange [0:" << x_range*x_unit_divider << "]" << endl;
	}




	if(this->y1range[0] == this->y1range[1]){
		exe_file << "set yrange [0:*]" << endl;
	}
	else{
		exe_file << "set yrange [" << this->y1range[0] << ":" << this->y1range[1] << "]" << endl;
	}



	if(this->get_x_unit() != "Jahre" && this->get_x_unit() != "a"){
		exe_file << "set xtics (\"0\" 0";
		for(int i = 1; i < 6; i++){
			exe_file << ", \"" << x_tic_name*i << "\" " << x_tic*i;
		}
		exe_file << ")"<< endl;
		exe_file << "set mxtics 3"<< endl;
	}

	//Settings for axes-, legend-, label-, titel- und tic-font as well as size
	exe_file << "set xtics scale " << this->plt_set.get_x_tic_size() << "font \"" << this->plt_set.get_x_font() << ", " << this->get_xtic_font_size()<< "\"" << endl;
	exe_file << "set xlabel offset "<< this->xLabelOffsetx << "," << this->xLabelOffsety << " font \"" << this->plt_set.get_x_font() << ", " << this->plt_set.get_x_font_size()<< "\"" << endl;
	exe_file << "set ytics scale " << this->plt_set.get_y_tic_size() << "font \"" << this->plt_set.get_y_font() << ", " << this->get_ytic_font_size()<< "\"" << endl;
	exe_file << "set ylabel offset "<< this->yLabelOffsetx << "," << this->yLabelOffsety << " font \"" << this->plt_set.get_y_font() << ", " << this->plt_set.get_y_font_size()<< "\"" << endl;
	exe_file << "set key font '" << this->get_xfont() << ",20'" << endl;

	exe_file << "set ytics nomirror"<< endl;

	if(this->get_y_tic_distance() != 0){
		exe_file << "set ytics "<< this->get_y_tic_distance() << endl;
	}




	//PDF output depending on Title
	//no TITEL and DESCRIPTION
	if(this->plt_set.get_show_title() == 0 && this->plt_set.get_show_description() == 0){

		exe_file << "set key at screen 0.5,0.075 horizontal center" << endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "' " << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "' " << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 15 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 5 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 15 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,1" << endl;
		exe_file << "set terminal pdf enhanced size 11.7,8.3" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}

	//only TITEL
	if(this->plt_set.get_show_title() == 1 && this->plt_set.get_show_description() == 0){

		exe_file << "set key at screen 0.5,0.075 horizontal center" << endl;
		exe_file << "set title " << "\"\\n\"" <<endl;
		exe_file << "set label \"" << this->make_header() << "\"" << " at graph 0,1.1 left font '" << this->get_xfont() << ",30'" <<endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "' " << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "' " << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 15 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 10 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 15 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,1" << endl;
		exe_file << "set terminal pdf enhanced size 11.7,8.3" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}
	//only DESCRIPTION
	if(this->plt_set.get_show_title() == 0 && this->plt_set.get_show_description() == 1){
		exe_file << "set key at screen 0.5,0.3 horizontal center" << endl;
		exe_file << "set title " << "\"\\n\"" <<endl;
		exe_file << "set label \"" << this->make_header() << "\"" << " at screen 0.13,0.95 left font '" << this->get_xfont() << ",10'" <<endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "' " << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "' " << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 5 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 0 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 35 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,0.7" << endl;
		exe_file << "set terminal pdf enhanced size 8.3,11.7" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}

	//DESCRIPTION and TITEL
	if(this->plt_set.get_show_title() == 1 && this->plt_set.get_show_description() == 1){
		exe_file << "set key at screen 0.5,0.3 horizontal center" << endl;
		exe_file << "set label \"" << this->plt_set.get_title() << "\" at screen 0.08, 0.96 font '" << this->get_xfont() << ",30'" <<endl;
		exe_file << "set label \"" << this->make_header() << "\"" << " at screen 0.13,0.95 left font '" << this->get_xfont() << ",10'" <<endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "' " << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "' " << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 5 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 0 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 35 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,0.7" << endl;
		exe_file << "set terminal pdf enhanced size 8.3,11.7" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}

	//List the lines to be plotted
	exe_file << "plot ";

	for(unsigned int i = 0; i < plt_set.get_plottings().size(); i++){

		//complicated vector query, so that the last added plot is also "behind" the layer
		line = plt_set.get_plottings()[plt_set.get_plottings().size() -1  -i];
		//exe_file << "set style " << line.style << "\n";
		/*exe_file << "'"<< line.datafile
					<<"' using ($" << line.x_data << "-" << line.offset << "):" << line.y_data
					<< " title " << "'" << line.title << "'"
					<< " with " <<line.line_or_points;*/


		//test
		exe_file << "'"<< line.datafile
							<<"' every " << this->compression_size
							<<" using ($" << line.x_data << "-" << line.offset << "):" << line.y_data
							<<" title " << "'" << line.title << "'"
							<<" with " <<line.line_or_points;



		//linetype supplement
		if(line.line_or_points == "lines" || line.line_or_points == "linespoints"){
			exe_file << " dashtype " << line.type << " ";
		}

		//pointtype supplement
		if(line.line_or_points == "points" || line.line_or_points == "linespoints"){
			exe_file << " pointtype " << line.type << " ";

		}

		//linewidth supplement
		if(line.line_or_points == "lines" || line.line_or_points == "linespoints"){
			exe_file << " linewidth " << line.size << " ";
		}

		//pointwidth supplement
		if(line.line_or_points == "points" || line.line_or_points == "linespoints"){
			exe_file << " pointsize " << line.size << " ";
		}

		//linecolor supplement
		if(line.linecolor != ""){
			exe_file << " linecolor rgb '" << line.linecolor << "' ";
		}
		else if(color_counter<color_scheme.size()){
			exe_file << " linecolor rgb '" << color_scheme[color_counter] << "' ";
			color_counter++;
		}




		//comma between the plotting commands of the lines
		if(i != plt_set.get_plottings().size() - 1){
			exe_file << ", ";
		}
		else{
			exe_file << "\n" << endl;
		}


	}


	exe_file.close();


}

//writes commands for plot with two y-axes
void GNU_Executer::write_gnu_exe_two_axes(bool safe_in_pdf, string output_path_pdf, string output_path_exe){


	//Determine the longest plot (only in case data files of different length are used)
	Plotting_Settings::linesettings line;
	double x_range = 0;
	for(unsigned int i = 0; i < plt_set.get_plottings().size(); i++){
		line = plt_set.get_plottings()[i];

		if (line.x_plot_length * line.time_steps > x_range){
			x_range = line.x_plot_length * line.time_steps;
		}
	}



	//Conversion from seconds to x_unit
	int x_unit_divider;

	if(this->get_x_unit() == "s"){
		x_unit_divider = 1;
	}

	else if(this->get_x_unit() == "min"){
		x_unit_divider = 60;
	}

	else if(this->get_x_unit() == "h"){
		x_unit_divider = 3600;
	}

	else if(this->get_x_unit() == "d"){
		x_unit_divider = 3600*24;
	}

	else if(this->get_x_unit() == "w"){
		x_unit_divider = 3600*24*7;
	}

	else if(this->get_x_unit() == "m"){
		x_unit_divider = 3600*24*30.5;
	}

	else if(this->get_x_unit() == "a"){
		x_unit_divider = 1;
	}

	else{
		cout << "No available unit was selected for the x-axis. The unit was set to seconds." << endl;
		x_unit_divider = 1;
	}


	x_range = x_range/x_unit_divider;


	//Determine the divider. Divider is a number 10ex, which specifies the length of the x_range.
	//It is needed to get reasonable roundings of the x_tics
	string str_x_range;
	stringstream ss;
	ss << int(round(x_range));
	str_x_range = ss.str();
	int divider;
	if (str_x_range.length() >= 3){
		divider = pow(10, str_x_range.length()-3);
	}
	else{
		divider = 1;
	}


	//Set the x_tic distances
	double d_x_range = x_range / divider;
	d_x_range = d_x_range/5;
	d_x_range = round(d_x_range);
	int x_tic_name = int(d_x_range * divider);
	int x_tic = x_tic_name*x_unit_divider;



	//dynamic set
	ofstream exe_file;
	exe_file.open(this->get_outputpath_exe().c_str());



	//Determination of the RWTH color scheme
	//	vector<string> color_scheme;
	//	color_scheme.push_back("#00549F");
	//	color_scheme.push_back("#407FB7");
	//	color_scheme.push_back("#8EBAE5");
	//	color_scheme.push_back("#C7DDF2");
	//	color_scheme.push_back("#E8F1FA");
	//	unsigned int color_counter = 0;



	//Extended RWTH color scheme
	vector<string> color_scheme;
	color_scheme.push_back("#00549F");
	color_scheme.push_back("#BDCD00");
	color_scheme.push_back("#F6A800");
	color_scheme.push_back("#CC071E");
	color_scheme.push_back("#006165");
	color_scheme.push_back("#0098A1");
	color_scheme.push_back("#57AB27");
	color_scheme.push_back("#A11035");
	color_scheme.push_back("#612158");
	color_scheme.push_back("#7A6FAC");
	unsigned int color_counter = 0;

	/*
	//High contrast color palette
	vector<string> color_scheme;
	color_scheme.push_back("#EF4444"); //red
	color_scheme.push_back("#009F75"); //green
	color_scheme.push_back("#FAA31B"); //orange
	color_scheme.push_back("#88C6ED"); //light blue
	color_scheme.push_back("#FFF000"); //yellow
	color_scheme.push_back("#394BA0"); //dark blue
	color_scheme.push_back("#82C341"); //light green
	color_scheme.push_back("#D54799"); //violett
	color_scheme.push_back("#FF00B3"); //pink
	color_scheme.push_back("#000000"); //black
	unsigned int color_counter = 0;*/


	//start of GNUPLOT SCRIPTS
	//static set
	exe_file << "set format x \"%10.0f\"" << endl;


	if(this->get_x_unit() == "Jahre" || this->get_x_unit() == "a"){
		exe_file << "set xrange [1:*]" << endl;
	}
	else{
		exe_file << "set xrange [0:" << x_range*x_unit_divider << "]" << endl;
	}


	if(this->y1range[0] == this->y1range[1]){
		exe_file << "set yrange [0:*]" << endl;
	}
	else{
		exe_file << "set yrange [" << this->y1range[0] << ":" << this->y1range[1] << "]" << endl;
	}

	if(this->y2range[0] == this->y2range[1]){
		exe_file << "set y2range [0:*]" << endl;
	}
	else{
		exe_file << "set y2range [" << this->y2range[0] << ":" << this->y2range[1] << "]" << endl;
	}


	if(this->get_x_unit() != "Jahre" && this->get_x_unit() != "a"){
		exe_file << "set xtics (\"0\" 0";
		for(int i = 1; i < 6; i++){
			exe_file << ", \"" << x_tic_name*i << "\" " << x_tic*i;
		}
		exe_file << ")"<< endl;
		exe_file << "set mxtics 3"<< endl;
	}



	exe_file << "set ytics nomirror"<< endl;
	if(this->get_y_tic_distance() != 0){
		exe_file << "set ytics "<< this->get_y_tic_distance() << endl;
	}

	if(this->get_y2_tic_distance() != 0){
		exe_file << "set y2tics "<< this->get_y2_tic_distance() << endl;
	}


	//Settings for axes-, legends-, label-, titel- and tic-font as well as size
	exe_file << "set xtics scale " << this->plt_set.get_x_tic_size() << "font \"" << this->plt_set.get_x_font() << ", " << this->get_xtic_font_size()<< "\"" << endl;
	exe_file << "set xlabel offset "<< this->xLabelOffsetx << "," << this->xLabelOffsety << " font \"" << this->plt_set.get_x_font() << ", " << this->plt_set.get_x_font_size()<< "\"" << endl;
	exe_file << "set ytics scale " << this->plt_set.get_y_tic_size() << "font \"" << this->plt_set.get_y_font() << ", " << this->get_ytic_font_size()<< "\"" << endl;
	exe_file << "set ylabel offset "<< this->yLabelOffsetx << "," << this->yLabelOffsety << " font \"" << this->plt_set.get_y_font() << ", " << this->plt_set.get_y_font_size()<< "\"" << endl;
	exe_file << "set y2tics scale " << this->plt_set.get_y_tic_size() << "font \"" << this->plt_set.get_y_font() << ", " << this->get_ytic_font_size()<< "\"" << endl;
	exe_file << "set y2label offset "<< this->y2LabelOffsetx << "," << this->y2LabelOffsety << " font \"" << this->plt_set.get_y_font() << ", " << this->plt_set.get_y_font_size()<< "\"" << endl;
	exe_file << "set key font '" << this->get_xfont() << ",20'" << endl;


	//PDF output depending on Title
	//NO TITEL and DESCRIPTION
	if(this->plt_set.get_show_title() == 0 && this->plt_set.get_show_description() == 0){

		exe_file << "set key at screen 0.5,0.075 horizontal center" << endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "'" << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "'" << endl;
		exe_file << "set y2label '" << this->plt_set.get_y2_lable() << "'" << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 15 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 5 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 15 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,1" << endl;
		exe_file << "set terminal pdf enhanced size 11.7,8.3" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}
	//only TITEL
	if(this->plt_set.get_show_title() == 1 && this->plt_set.get_show_description() == 0){

		exe_file << "set key at screen 0.5,0.075 horizontal center" << endl;
		exe_file << "set title " << "\"\\n\"" <<endl;
		exe_file << "set label \"" << this->make_header() << "\"" << " at graph 0,1.1 left font \"" << this->plt_set.get_x_font() << ", 30\"" << endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "'" << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "'" << endl;
		exe_file << "set y2label '" << this->plt_set.get_y2_lable() << "'" << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 15 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 10 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 15 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,1" << endl;
		exe_file << "set terminal pdf enhanced size 11.7,8.3" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}
	//only DESCRIPTION
	if(this->plt_set.get_show_title() == 0 && this->plt_set.get_show_description() == 1){
		exe_file << "set key at screen 0.5,0.3 horizontal center" << endl;
		exe_file << "set title " << "\"\\n\"" <<endl;
		exe_file << "set label \"" << this->make_header() << "\"" << " at screen 0.13,0.95 left font \"" << this->plt_set.get_x_font() << ", 10\"" << endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "'" << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "'" << endl;
		exe_file << "set y2label '" << this->plt_set.get_y2_lable() << "'" << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 15 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 0 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 35 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,0.7" << endl;
		exe_file << "set terminal pdf enhanced size 8.3,11.7" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}

	//DESCRIPTION and TITEL
	if(this->plt_set.get_show_title() == 1 && this->plt_set.get_show_description() == 1){
		exe_file << "set key at screen 0.5,0.3 horizontal center" << endl;
		exe_file << "set label \"" << this->plt_set.get_title() << "\" at screen 0.08, 0.96 font '" << this->get_xfont() << ",30'" <<endl;
		exe_file << "set label \"" << this->make_header() << "\"" << " at screen 0.13,0.95 left font '" << this->get_xfont() << ",10'" <<endl;
		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "'" << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "'" << endl;
		exe_file << "set y2label '" << this->plt_set.get_y2_lable() << "'" << endl;
		exe_file << "set lmargin 15 + " << this->get_lmargin()<< endl;
		exe_file << "set rmargin 15 + " << this->get_rmargin()<< endl;
		exe_file << "set tmargin 0 + " << this->get_tmargin() << endl;
		exe_file << "set bmargin 35 + " << this->get_bmargin() << endl;
		exe_file << "set size 1,0.7" << endl;
		exe_file << "set terminal pdf enhanced size 8.3,11.7" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
	}

	//List the lines to be plotted
	exe_file << "plot ";

	for(unsigned int i = 0; i < plt_set.get_plottings().size(); i++){

		//complicated vector query, so that the last added plot is also "behind" the layer
		line = plt_set.get_plottings()[plt_set.get_plottings().size() -1  -i];
		//exe_file << "set style " << line.style << "\n";
		exe_file << "'"<< line.datafile
					<<"' every " << this->compression_size
					<<" using ($" << line.x_data << "-" << line.offset << "):" << line.y_data
					<<" title " << "'" << line.title << "'"
					<<" with " <<line.line_or_points;



		//linetype supplement
		if(line.line_or_points == "lines" || line.line_or_points == "linespoints"){
			exe_file << " dashtype " << line.type << " ";
		}

		//pointtype supplement
		if(line.line_or_points == "points" || line.line_or_points == "linespoints"){
			exe_file << " pointtype " << line.type << " ";

		}

		//linewidth supplement
		if(line.line_or_points == "lines" || line.line_or_points == "linespoints"){
			exe_file << " linewidth " << line.size << " ";
		}

		//pointwidth supplement
		if(line.line_or_points == "points" || line.line_or_points == "linespoints"){
			exe_file << " pointsize " << line.size << " ";
		}

		//linecolor supplement
		if(line.linecolor != ""){
			exe_file << " linecolor rgb '" << line.linecolor << "' ";
		}
		else if(color_counter<color_scheme.size()){
			exe_file << " linecolor rgb '" << color_scheme[color_counter] << "' ";
			color_counter++;
		}



		//Specify on which y-axis the line should be plotted
		if(line.second_axis){
			exe_file << " axis x1y2 ";
		}
		else{
			exe_file << " axis x1y1 ";
		}


		//comma between the plotting commands of the lines
		if(i != plt_set.get_plottings().size() - 1){
			exe_file << ", ";
		}
		else{
			exe_file << "\n" << endl;
		}


	}


	exe_file.close();

}

//writes commands for 2x2 plot
void GNU_Executer::write_gnu_exe_2x2(bool safe_in_pdf, string output_path_pdf, string output_path_exe){


	//Determine the x_range for one week
		Plotting_Settings::linesettings line;
		double x_range = 7*24*60*60;




		//Conversion from seconds to x_unit
		int x_unit_divider;

		if(this->get_x_unit() == "s"){
			x_unit_divider = 1;
		}

		else if(this->get_x_unit() == "min"){
			x_unit_divider = 60;
		}

		else if(this->get_x_unit() == "h"){
			x_unit_divider = 3600;
		}

		else if(this->get_x_unit() == "d"){
			x_unit_divider = 3600*24;
		}

		else if(this->get_x_unit() == "w"){
			x_unit_divider = 3600*24*7;
		}

		else if(this->get_x_unit() == "m"){
			x_unit_divider = 3600*24*30.5;
		}

		else{
			cout << "No available unit was selected for the x-axis. The unit was set to seconds." << endl;
			x_unit_divider = 1;
		}


		x_range = x_range/x_unit_divider;


		//Determine the divider. Divider is a number 10ex, which specifies the length of the x_range.
		//It is needed to get reasonable roundings of the x_tics
		string str_x_range;
		stringstream ss;
		ss << int(round(x_range));
		str_x_range = ss.str();
		int divider;
		if (str_x_range.length() >= 3){
			divider = pow(10, str_x_range.length()-3);
		}
		else{
			divider = 1;
		}


		//Set x_tic distances
		double d_x_range = x_range / divider;
		d_x_range = d_x_range/5;
		d_x_range = round(d_x_range);
		int x_tic_name = int(d_x_range * divider);
		int x_tic = x_tic_name*x_unit_divider;



		//dynamic set
		ofstream exe_file;
		exe_file.open(this->get_outputpath_exe().c_str());




		//exe_file << "set terminal pdf" << endl;


		//Determination of the RWTH color scheme
		//	vector<string> color_scheme;
		//	color_scheme.push_back("#00549F");
		//	color_scheme.push_back("#407FB7");
		//	color_scheme.push_back("#8EBAE5");
		//	color_scheme.push_back("#C7DDF2");
		//	color_scheme.push_back("#E8F1FA");
		//	unsigned int color_counter = 0;



		//Extended RWTH color palette
		vector<string> color_scheme;
		color_scheme.push_back("#00549F");
		color_scheme.push_back("#BDCD00");
		color_scheme.push_back("#F6A800");
		color_scheme.push_back("#CC071E");
		color_scheme.push_back("#006165");
		color_scheme.push_back("#0098A1");
		color_scheme.push_back("#57AB27");
		color_scheme.push_back("#A11035");
		color_scheme.push_back("#612158");
		color_scheme.push_back("#7A6FAC");
		unsigned int color_counter = 0;


		//- 21.- 28. march
		//- 20.- 27. june
		//- 22.- 29. september
		//- 21.- 28. december
		string month_names[4] = {"15.- 21. Februar", "01.- 07. Mai", "25.- 31. Juli", "01.- 07. Dezember"};


		//Plotting Settings
		//static set;
		exe_file << "set terminal pdf enhanced size 11.7, 8.3" << endl;
		exe_file << "set output '"<< this->get_outputpath_pdf()<< "'" << endl;
		if(this->get_plt_set().get_show_title() == 1){

			exe_file << "set tmargin 5 + " << this->get_tmargin() << endl;
			exe_file << "set bmargin 10 + " << this->get_bmargin() << endl;
			exe_file << "set lmargin 10 + " << this->get_lmargin()<< endl;
			exe_file << "set rmargin 12 + " << this->get_rmargin()<< endl;
			exe_file << "set key default" << endl;
			exe_file << "set label '" << this->plt_set.get_title() <<"' at screen 0.5,0.95 center front font \"" << this->plt_set.get_x_font() << ", 25\"" << endl;
			exe_file << "set key horizontal" << endl;
			exe_file << "set key center at screen 0.50, 0.52" << endl;

		}

		if(this->get_plt_set().get_show_title() == 0){
			exe_file << "set tmargin 3 + " << this->get_tmargin() << endl;
			exe_file << "set bmargin 10 + " << this->get_bmargin() << endl;
			exe_file << "set lmargin 10 + " << this->get_lmargin()<< endl;
			exe_file << "set rmargin 12 + " << this->get_rmargin()<< endl;
			exe_file << "set key default" << endl;
			exe_file << "set key horizontal" << endl;
			exe_file << "set key center at screen 0.50, 0.53" << endl;
		}


		//Settings for axis, legend, label, title and tic font and size
		exe_file << "set xtics scale " << this->plt_set.get_x_tic_size() << "font \"" << this->plt_set.get_x_font() << ", " << this->get_xtic_font_size()<< "\"" << endl;
		exe_file << "set xlabel offset "<< this->xLabelOffsetx << "," << this->xLabelOffsety << " font \"" << this->plt_set.get_x_font() << ", " << this->plt_set.get_x_font_size()<< "\"" << endl;
		exe_file << "set ylabel offset "<< this->yLabelOffsetx << "," << this->yLabelOffsety << " font \"" << this->plt_set.get_y_font() << ", " << this->plt_set.get_y_font_size()<< "\"" << endl;
		exe_file << "set key font '" << this->get_xfont() << ",20'" << endl;

		exe_file << "set xlabel '" << this->plt_set.get_x_lable() << "' font \"" << this->plt_set.get_x_font() << ", " << this->get_xfont_size()<< "\"" << endl;
		exe_file << "set ylabel '" << this->plt_set.get_y_lable() << "' font \"" << this->plt_set.get_y_font() << ", " << this->get_yfont_size()<< "\"" << endl;
		exe_file << "set multiplot layout 2,2 title '""' font \",40\"" << endl;
		exe_file << "set format x \"%10.0f\"" << endl;
		exe_file << "set xrange [0:" << x_range*x_unit_divider << "]" << endl;
		exe_file << "set xtics (\"0\" 0";
		for(int i = 1; i < 8; i++){
			exe_file << ", \"" << x_tic_name*i << "\" " << x_tic*i;
		}
		exe_file << ")"<< endl;

		if(this->y1range[0] == this->y1range[1]){
			exe_file << "set yrange [0:*]" << endl;
		}
		else{
			exe_file << "set yrange [" << this->y1range[0] << ":" << this->y1range[1] << "]" << endl;
		}

		exe_file << "set mxtics 3"<< endl;
		//exe_file << "set ytics nomirror"<< endl;
		//Specify on which y-axis the line should be plotted
		if(this->get_two_axes()){
			exe_file << "set y2tics " << this->get_y2_tic_distance() << " scale " << this->plt_set.get_y_tic_size() << " font \"" << this->plt_set.get_y_font() << ", " << this->get_ytic_font_size()<< "\"" << endl;
			exe_file << "set y2label '" << this->plt_set.get_y2_lable() << "' font \"" << this->plt_set.get_y_font() << ", " << this->get_yfont_size()<< "\"" << endl;
			exe_file << "set y2range[*:*]" << endl;
		}


		if(this->get_y_tic_distance() != 0){
			exe_file << "set ytics "<< this->get_y_tic_distance() << " scale " << this->plt_set.get_y_tic_size() << "font \"" << this->plt_set.get_y_font() << ", " << this->get_ytic_font_size()<< "\" nomirror" << endl;
		}

		if(this->get_y2_tic_distance() != 0){
			exe_file << "set y2tics "<< this->get_y2_tic_distance() << endl;
		}

		exe_file << "set ytics "<< this->get_y_tic_distance() << " scale " << this->plt_set.get_y_tic_size() << "font \"" << this->plt_set.get_y_font() << ", " << this->get_ytic_font_size()<< "\" nomirror" << endl;
		//Iteration over month
		for(unsigned int j = 0; j < 4; j++){
			exe_file << "#" << endl;
			exe_file << "set title '" << month_names[j] << "' font \"" << this->plt_set.get_y_font() << "\"" << endl;
			exe_file << "plot ";



			//Iteration over plottings
			for(unsigned int i = 0; i < plt_set.get_plottings().size(); i++){
				//complicated vector query, so that the last added plot is also "behind" the layer
				line = plt_set.get_plottings()[plt_set.get_plottings().size() -1  -i];

				string teststring(this->plt_set.get_pathes_2x2(j));

				//exe_file << "set style " << line.style << "\n";
				exe_file << "'"<< teststring
							<<"' every " << this->compression_size
							<<" using ($" << line.x_data << "-" << this->plt_set.get_offset_2x2(j) << "):" << line.y_data //Anstatt Null offset abziehen
							<< " title " << "'" << line.title << "'"
							<< " with " <<line.line_or_points;



				//linetype supplement
				if(line.line_or_points == "lines" || line.line_or_points == "linespoints"){
					exe_file << " dashtype " << line.type << " ";
				}

				//pointtype supplement
				if(line.line_or_points == "points" || line.line_or_points == "linespoints"){
					exe_file << " pointtype " << line.type << " ";

				}

				//linewidth supplement
				if(line.line_or_points == "lines" || line.line_or_points == "linespoints"){
					exe_file << " linewidth " << line.size << " ";
				}

				//pointwidth supplement
				if(line.line_or_points == "points" || line.line_or_points == "linespoints"){
					exe_file << " pointsize " << line.size << " ";
				}

				//linecolor supplement
				if(line.linecolor != ""){
					exe_file << " linecolor rgb '" << line.linecolor << "' ";
				}
				else if(color_counter<color_scheme.size()){
					exe_file << " linecolor rgb '" << color_scheme[i] << "' ";
				}


				if(this->_two_axes){
					//Specify on which y-axis the line should be plotted
					if(line.second_axis){
						exe_file << " axis x1y2 ";
					}
					else{
						exe_file << " axis x1y1 ";
					}
				}



				//comma between the plotting commands of the lines
				if(i != plt_set.get_plottings().size() - 1){
					exe_file << ", ";
				}
				else{
					exe_file << "\n" << endl;
				}
			}


		}


		exe_file << "unset multiplot" << endl;


		exe_file.close();

}

//Executes commands in gnuplot_skript
void GNU_Executer::execute_file(string exe_path){
	string command = "gnuplot -persist ";
	command.append(exe_path);
	int success = system(command.c_str());
}

//deletes gnuplot_script
void GNU_Executer::delete_exe_file(string delete_path){
	remove(delete_path.c_str());
}

//Executes GNU_Executer
int GNU_Executer::use_gnu(string title, string x_label, string y_label, string y2_label, bool safe_in_pdf){


	//Setting the axis labels and heading
	this->plt_set.set_title(title);
	this->plt_set.set_xlable(x_label);
	this->plt_set.set_ylable(y_label);
	this->plt_set.set_y2lable(y2_label);


	if (this->get_format() == 0){
		//writes the gnuplot scripts for single plots
		//Arguments of write_gnu_exe functions:
		//plt_set, safe_in_pdf
		if(this->get_two_axes()){
			write_gnu_exe_two_axes(1, this->_outputpath_pdf, this->_outputpath_exe);
		}
		else{
			write_gnu_exe_one_axis(1, this->_outputpath_pdf, this->_outputpath_exe);
		}

		//Executes gnu scripts
		execute_file(this->_outputpath_exe);

		//deletes GNU script
		//delete_exe_file(this->_outputpath_exe);
	}

	else if (this->get_format() == 1){
		//writes the gnuplot scripts for 2x2
		this->write_gnu_exe_2x2(1, this->_outputpath_pdf, this->_outputpath_exe);
		this->execute_file(this->_outputpath_exe);
	}

	else{
		cout << "Wrong format: " << this->get_format() << " not assigned." << endl;
	}


	//delete all text files

	if(this->delete_all_files){
		this->delete_exe_file(this->_outputpath_exe);

		for(unsigned int i = 0; i<4; i++){
			this->delete_exe_file(this->plt_set.get_pathes_2x2(i));
		}
	}


	return 0;
}

//Adds a curve to the plotting settings
void GNU_Executer::add_line(bool second_axis,
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


	//Adds a new line to the plt.set attribute
	this->plt_set.append_line(second_axis,
							x_data,
							y_data,
							title,
							datafile,
							aggregation,
							which_aggregation,
							points_or_lines,
							type,
							size,
							linecolor);
}

//Sets the option header
void GNU_Executer::select_header(string header){
	//Different cases for the parameter:
	if(header == "none"){
		this->plt_set.set_show_title(0);
		this->plt_set.set_show_description(0);
	}
	if(header == "title"){
		this->plt_set.set_show_title(1);
		this->plt_set.set_show_description(0);
	}
	if(header == "description"){
		this->plt_set.set_show_title(0);
		this->plt_set.set_show_description(1);
	}
	if(header == "both"){
		this->plt_set.set_show_title(1);
		this->plt_set.set_show_description(1);
	}
}

//Creates the header depending on the header option
string GNU_Executer::make_header(){


	string header;
	//Distinction between cases
	//only TITEL
	if(this->plt_set.get_show_title() == 1 && this->plt_set.get_show_description() == 0){
		header = this->plt_set.get_title();
	}

	//only DESCRIPTION
	if(this->plt_set.get_show_title() == 0 && this->plt_set.get_show_description() == 1){
		header = "Input Data:\\n"
				"\\nName of heat pump:"
				"\t\t\t\t\t\t\t\t\t\t\t"
				+ this->plt_set.get_description_values()[0]
				+"\\nLinked buildings to heat pump:" + "\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[1] +
				"\\nLinked bhe to heat pump:" + "\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[2] +
				"\\nV_s_o_i_l of each BHE:" + "\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[3] + "\t\t\t\t" + "[m^3/h]"
				"\\nSimulated time =" + "\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[4] + "\t\t\t\t" + "[s]"
				"\\nSimulated end time in years =" + "\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[5] + "\t\t\t\t" + "[y]"
				"\\napplication range =" + "\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[6] +
				"\\nheating storage volume =" + "\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[7] + "\t\t\t\t" + "[m^3]"
				"\\nT_s_t_,_ _m_i_n =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[8] + "\t\t\t\t" + "[°C]"
				"\\nT_s_t_,_ _m_a_x =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[9] + "\t\t\t\t" + "[°C]"
				"\\ndhw storage volume =" + "\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[10] + "\t\t\t\t" + "[m^3]"
				"\\nT_s_t_,_ _m_i_n_,_ _d_h_w =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[11] + "\t\t\t\t" + "[°C]"
				"\\nT_s_t_,_ _m_a_x_,_ _d_h_w =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[12] + "\t\t\t\t" + "[°C]"
				"\\nRun time of heat pump in this year =" + "\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[13] + "\t\t\t" + "[h]"
				"\\nElectrical energy demand of soil pump in this year =" + "\t\t\t\t\t" + this->plt_set.get_description_values()[14] + "\t\t\t" + "[kWh]"
				"\\nSCOP in this year (only heat pump) =" + "\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[15] +
				"\\nSCOP in this year (heat pump + soil pump + second heat generator) =" + "\t" + this->plt_set.get_description_values()[16];
	}

	//both
	if(this->plt_set.get_show_title() == 1 && this->plt_set.get_show_description() == 1){
		header = "\\n\\n\\n"
				"Input Data:\\n"
				"\\nName of heat pump:"
				"\t\t\t\t\t\t\t\t\t\t\t"
				+ this->plt_set.get_description_values()[0]
				+"\\nLinked buildings to heat pump:" + "\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[1] +
				"\\nLinked bhe to heat pump:" + "\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[2] +
				"\\nV_s_o_i_l of each BHE:" + "\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[3] + "\t\t\t\t" + "[m^3/h]"
				"\\nSimulated time =" + "\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[4] + "\t\t\t\t" + "[s]"
				"\\nSimulated end time in years =" + "\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[5] + "\t\t\t\t" + "[y]"
				"\\napplication range =" + "\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[6] +
				"\\nheating storage volume =" + "\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[7] + "\t\t\t\t" + "[m^3]"
				"\\nT_s_t_,_ _m_i_n =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[8] + "\t\t\t\t" + "[°C]"
				"\\nT_s_t_,_ _m_a_x =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[9] + "\t\t\t\t" + "[°C]"
				"\\ndhw storage volume =" + "\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[10] + "\t\t\t\t" + "[m^3]"
				"\\nT_s_t_,_ _m_i_n_,_ _d_h_w =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[11] + "\t\t\t\t" + "[°C]"
				"\\nT_s_t_,_ _m_a_x_,_ _d_h_w =" + "\t\t\t\t\t\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[12] + "\t\t\t\t" + "[°C]"
				"\\nRun time of heat pump in this year =" + "\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[13] + "\t\t\t" + "[h]"
				"\\nElectrical energy demand of soil pump in this year =" + "\t\t\t\t\t" + this->plt_set.get_description_values()[14] + "\t\t\t" + "[kWh]"
				"\\nSCOP in this year (only heat pump) =" + "\t\t\t\t\t\t\t\t" + this->plt_set.get_description_values()[15] +
				"\\nSCOP in this year (heat pump + soil pump + second heat generator) =" + "\t" + this->plt_set.get_description_values()[16];
	}





	return header;

}

//Save the variables of headers in plotting_settings
void GNU_Executer::set_description_variables(string HPname,
		vector<string> linked_buildings,
		vector<string> linked_bhe,
		double Vdot_soil_bhe_init,
		double t,
		double t_sim_end,
		unsigned int app_range,
		double Vol_st,
		double T_st_min,
		double T_st_max,
		double Vol_st_dhw,
		double T_st_min_dhw,
		double T_st_max_dhw,
		double t_run,
		double P_el_soil_pump,
		double JAZ_WP,
		double JAZ_WPA)
		{

		string all_linked_buildings;
		for(unsigned int i=0; i<linked_buildings.size(); i++){
			all_linked_buildings = all_linked_buildings + linked_buildings[i];
			if(i < linked_buildings.size() - 1){
				all_linked_buildings = all_linked_buildings + ", ";
			}
		}

		string all_linked_bhe;
		for(unsigned int i=0; i<linked_bhe.size(); i++){
			all_linked_bhe = all_linked_bhe + linked_bhe[i] + " ";
			if(i < linked_bhe.size() - 1){
				all_linked_bhe = all_linked_bhe + ", ";
			}
		}



		vector<string> application_range;
		application_range.push_back("heating");
		application_range.push_back("dhw");
		application_range.push_back("heating+dhw");
		application_range.push_back("heating+cooling");
		application_range.push_back("heating+cooling+dhw");

		vector<string> dummy;

		ostringstream s;

		dummy.push_back(HPname);
		dummy.push_back(all_linked_buildings);
		dummy.push_back(all_linked_bhe);
		s.str("");
		s << Vdot_soil_bhe_init;
		dummy.push_back(s.str());
		s.str("");
		s << t;
		dummy.push_back(s.str());
		s.str("");
		s << t_sim_end / 3600 / 8760;
		dummy.push_back(s.str());
		dummy.push_back(application_range[app_range]);
		s.str("");
		s << Vol_st;
		dummy.push_back(s.str());
		s.str("");
		s << T_st_min;
		dummy.push_back(s.str());
		s.str("");
		s << T_st_max;
		dummy.push_back(s.str());
		s.str("");
		s << Vol_st_dhw;
		dummy.push_back(s.str());
		s.str("");
		s << T_st_min_dhw;
		dummy.push_back(s.str());
		s.str("");
		s << T_st_max_dhw;
		dummy.push_back(s.str());
		s.str("");
		s << t_run;
		dummy.push_back(s.str());
		s.str("");
		s << P_el_soil_pump;
		dummy.push_back(s.str());
		s.str("");
		s << JAZ_WP;
		dummy.push_back(s.str());
		s.str("");
		s << JAZ_WPA;
		dummy.push_back(s.str());



		this->plt_set.set_description_values(dummy);
}

//Creates a record from start and end date
bool GNU_Executer::create_timeslot_file(string path_year_file, string path_timeslot_file, int d_begin, int m_begin, int d_end, int m_end){

	//Function for transforming Year_file in week_file

	//Calc startdate
	int days_per_month[12] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};


	unsigned int start_days = -1;
	unsigned int end_days = -1;

	unsigned long start_seconds = 0;
	unsigned long end_seconds = 0;

	unsigned int start_line = 0;
	unsigned int end_line = 0;

	//Calc days to begin of timeslot
	for(int i=0; i < m_begin - 1; i++){
		start_days = start_days + days_per_month[i];
	}
	start_days = start_days + d_begin;

	//Calc days to end of timeslot
	for(int i=0; i < m_end -1; i++){
		end_days = end_days + days_per_month[i];
	}
	end_days = end_days + d_end;


	//Convert days to seconds
	start_seconds = start_days * 24 * 60 * 60;
	end_seconds = end_days * 24 * 60 * 60;


	//Convert seconds to line
	start_line = round(start_seconds / this->get_global_timestep());
	end_line = round(end_seconds / this->get_global_timestep());





	//STREAMING
	ofstream outputstream(path_timeslot_file.c_str());
	ifstream inputstream(path_year_file.c_str());

	if(!inputstream.good()){
		cout << "The file " << path_year_file.c_str() << " was not found. The process was aborted." << endl;
		throw exception();
	}


	string text;
	unsigned int line_counter = 0;

	while (getline(inputstream, text)) {

		char first_letter = text.at(0);

		//Writes data in week file
		if (first_letter != '#'){
			//cout << line_counter << "\t" << start_line << "\t" << end_line << endl;
			if(line_counter >= start_line && line_counter <= end_line){
				outputstream << text;
				outputstream << endl;
			}
			line_counter++;
		}


		//Writes header in week file
		else{
			outputstream << text;
			outputstream << endl;
		}

	  }

	inputstream.close();
	outputstream.close();

	return 0;



}

//Creates a record for one hour
bool GNU_Executer::create_hour_file(string path_year_file, string path_hour_file, int hour, int amount_hour){
	//Function for transforming Year_file in week_file



	unsigned long start_seconds = 0;
	unsigned long end_seconds = 0;

	unsigned int start_line = 0;
	unsigned int end_line = 0;


	//Convert days to seconds
	start_seconds = hour * 60 * 60;
	end_seconds = start_seconds + amount_hour * 3600;


	//Convert seconds to line
	start_line = round(start_seconds / this->get_global_timestep());
	end_line = round(end_seconds / this->get_global_timestep());





	//STREAMING
	ofstream outputstream(path_hour_file.c_str());
	ifstream inputstream(path_year_file.c_str());

	if(!inputstream.good()){
		cout << "The file " << path_year_file.c_str() << " was not found. The process was aborted." << endl;
		throw exception();
	}


	string text;
	unsigned int line_counter = 0;

	while (getline(inputstream, text)) {

		char first_letter = text.at(0);

		//Writes data in week file
		if (first_letter != '#'){
			//cout << line_counter << "\t" << start_line << "\t" << end_line << endl;
			if(line_counter >= start_line && line_counter <= end_line){
				outputstream << text;
				outputstream << endl;
			}
			line_counter++;
		}


		//Writes header in week file
		else{
			outputstream << text;
			outputstream << endl;
		}

	  }

	inputstream.close();
	outputstream.close();

	return 0;

}

//Creates four records for the four weeks of the beginnings of the seasons
void GNU_Executer::create_2by2_week_files(string path_year_file){
	//Function creates a PDF with 4 charts over a week during the year
	//The solstices and equinoxes were taken as the dates:
	//- 21.- 28. march
	//- 20.- 27. june
	//- 22.- 29. september
	//- 21.- 28. december


	//OFF SET calculation
	//The offset is composed of two parts:
	//1) The period from 1 January to the selected week
	//2) The time of the elapsed years

	unsigned int start_days[4] = {46-1, 122-1, 207-1, 336-1};
	unsigned int end_days[4];

	for(unsigned int i = 0; i < 4; i++){
		end_days[i] = start_days[i] + 7;
	}



	unsigned long start_seconds[4];
	unsigned long end_seconds[4];

	unsigned int start_line[4];
	unsigned int end_line[4];


	//Convert days to lines
	for(unsigned int i = 0; i < 4; i++){
		start_seconds[i] = start_days[i] * 24 * 60 * 60;
		end_seconds[i] = end_days[i] * 24 * 60 * 60;

		start_line[i] = round(start_seconds[i] / this->get_global_timestep());
		end_line[i] = round(end_seconds[i] / this->get_global_timestep());
	}



	//Determine offset (Offset: The files year02 and year03 do not start at t=0 but at the time after one year.
	//time elapsed up to the year to be deducted.
	float number = 0;
	long int offset = 0;

	ifstream inFile(path_year_file.c_str());

	if(!inFile.good()){
		cout << "The file " << path_year_file.c_str() << " was not found. The process was aborted." << endl;
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


	//Set offset
	for(int i = 0; i < 4; i++){
		this->plt_set.set_offset_2x2(start_seconds[i] + offset, i);
	}



	//STREAMING Data files
	string w1 = path_year_file + "_march";
	string w2 = path_year_file + "_june";
	string w3 = path_year_file + "_september";
	string w4 = path_year_file + "_december";
	string out_pathes[4] = {w1, w2, w3, w4};










	ofstream outputstream;
	ifstream inputstream;



	for(int i = 0; i < 4; i++){
		this->plt_set.set_pathes_2x2(out_pathes[i], i);


		outputstream.open(out_pathes[i].c_str(), ofstream::out);
		inputstream.open(path_year_file.c_str());
		string text = "";
		unsigned int line_counter = 0;

		while (getline(inputstream, text)) {

				char first_letter = text.at(0);

				//Writes data in week file
				if (first_letter != '#'){
					//cout << line_counter << "\t" << start_line << "\t" << end_line << endl;
					if(line_counter >= start_line[i] && line_counter <= end_line[i]){
						outputstream << text;
						outputstream << endl;
					}
					line_counter++;
				}


				//Writes header in week file
				else{
					outputstream << text;
					outputstream << endl;
				}

			  }
			outputstream.close();
			inputstream.close();


	}


	//Format is set equal to 1 (1= 2x2 plot)
	this->set_format(1);





}



