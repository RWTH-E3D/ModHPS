/*
 * UseGNU.h
 *
 *  Created on: 06.12.2019
 *      Author: Tobias
 */

#ifndef USEGNU_H_
#define USEGNU_H_

#include <iostream>
#include "PlottingSettings.h"
#include <vector>

using namespace std;

//!Class for defining plotting attributes for Gnuplot

class GNU_Executer {

	public:
		Plotting_Settings plt_set;

		string _outputpath_pdf;
		string _outputpath_exe;

		int compression_size;



		//Creates execution script for one axis
		void write_gnu_exe_one_axis(bool safe_in_pdf, string output_path_pdf, string output_path_exe);

		//Creates execution script for two axes
		void write_gnu_exe_two_axes(bool safe_in_pdf, string output_path_pdf, string output_path_exe);

		//Creates execution script for 2x2
		void write_gnu_exe_2x2(bool safe_in_pdf, string output_path_pdf, string output_path_exe);




		//Constructor
		GNU_Executer();

		//Boolean: use two axes?
		bool _two_axes;

		//Output format-> 0= SinglePlot, 1=2x2
		int _format;

		//Should all text_files deleted?
		bool delete_all_files;

		double y1range[2];
		double y2range[2];

		double xLabelOffsetx;
		double xLabelOffsety;
		double yLabelOffsetx;
		double yLabelOffsety;
		double y2LabelOffsetx;
		double y2LabelOffsety;


		//Executes execution script
		void execute_file(string exe_path);

		//Deletes all exe_files
		void delete_exe_file(string delete_path);

		//Plots diagrams
		//Parameter: title, x_lable, y_lable, y2_lable
		int use_gnu(string, string, string, string, bool safe_in_pdf = 1);


		//Adds a line in diagram
		void add_line(bool second_axis,
					int x_data,
					int y_data,
					string title,
					string datafile,
					int aggregation = 1,
					int which_aggregation = 0,
					string points_or_lines = "lines",
					int type = -1,
					float size = -1,
					string linecolor = "");


		//Gives the possibility to select which header is displayed via bools Plotting_Settings._show_title/description
		void select_header(string header="title");

		string make_header();

		//getter functions
		Plotting_Settings get_plt_set();

		string get_outputpath_pdf();

		string get_outputpath_exe();

		bool get_two_axes();

		int get_format();

		int get_xtic_size();

		int get_xtic_font_size();

		int get_xfont_size();

		string get_xfont();

		int get_ytic_size();

		int get_ytic_font_size();

		int get_yfont_size();

		string get_yfont();

		string get_x_unit();

		double get_global_timestep();

		int get_max_line_title_length();

		bool get_delete_all_files();

		double get_y_tic_distance();

		double get_y2_tic_distance();

		double get_bmargin();
		double get_tmargin();
		double get_lmargin();
		double get_rmargin();


		//Setter functions
		void set_plt_set(Plotting_Settings plt);

		void set_outputpath_pdf(string outputpath_pdf);

		void set_outputpath_exe(string outputpath_exe);

		void set_compression_size(int compression_size);

		void set_two_axes(bool two_axes);

		void set_format(int format);

		void set_tic_size(int size);

		void set_tic_font_size(int size);

		void set_font_size(int size);

		void set_font(string font);

		void set_xtic_size(int size);

		void set_xtic_font_size(int size);

		void set_xfont_size(int size);

		void set_xfont(string font);

		void set_ytic_size(int size);

		void set_ytic_font_size(int size);

		void set_yfont_size(int size);

		void set_yfont(string font);

		void set_x_unit(string x_unit);

		void set_description_variables(string, vector<string>, vector<string>, double, double, double, unsigned int, double, double, double, double, double, double, double, double, double, double);

		void set_global_timestep(double ts);

		void set_delete_all_files(bool del_files);

		bool create_timeslot_file(string path_year_file, string path_week_file, int d_begin, int m_begin, int d_end, int m_end);

		bool create_hour_file(string path_year_file, string path_hour_file, int hour, int amount_hour);

		void create_2by2_week_files(string);

		void set_yrange(int axe, double from, double to);

		void set_y_tic_distance(double);
		void set_y2_tic_distance(double);

		void set_bmargin(double);
		void set_tmargin(double);
		void set_lmargin(double);
		void set_rmargin(double);

		void set_LabelOffset(string, string, double);
};






#endif /* USEGNU_H_ */
