/*
 * PlottingSettings.h
 *
 *  Created on: 05.11.2019
 *      Author: Tobias
 */
#include <vector>
#ifndef PLOTTINGSETTINGS_H_
#define PLOTTINGSETTINGS_H_
using namespace std;

//!Class for the most important settings of automatic plotting

class Plotting_Settings {

	public:
		string _title;
		string _xlable;
		string _ylable;
		string _y2lable;
		string _x_font;
		string _y_font;
		int _x_font_size;
		int _y_font_size;
		int _x_tic_size;
		int _y_tic_size;
		int _x_tic_font_size;
		int _y_tic_font_size;

		string _x_unit;
		bool show_title;
		bool show_description;
		double _global_timestep;

		vector<string> _description_values;

		string _pathes_2x2[4];
		unsigned long _offset_2x2[4];

		double additional_bmargin;
		double additional_lmargin;
		double additional_rmargin;
		double additional_tmargin;

		struct linesettings {
				string linecolor;
				string title;
				string datafile;
				string style;
				string line_or_points;
				int  x_data;
				int y_data;
				int type;
				int x_plot_length;
				int time_steps;
				long unsigned int offset;
				float size;
				bool second_axis;
			};

		vector<linesettings> _plottings;

		double y_tic_distance;
		double y2_tic_distance;



		//Constructor and deconstructor
		Plotting_Settings();
		Plotting_Settings(string title, string xlable, string ylable, string y2lable);
		virtual ~Plotting_Settings();



		// Set Functions
		void set_title(string title);
		void set_xlable(string xlable);
		void set_ylable(string ylable);
		void set_y2lable(string y2lable);

		void set_x_font(string font);
		void set_y_font(string font);
		void set_x_font_size(int size);
		void set_y_font_size(int size);
		void set_x_tic_size(int size);
		void set_y_tic_size(int size);
		void set_x_tic_font_size(int size);
		void set_y_tic_font_size(int size);

		void set_x_unit(string x_unit);
		void set_plottings(linesettings ls);
		void set_show_title(bool show_title);
		void set_show_description(bool sow_description);
		void set_description_values(vector<string>);
		void set_description_values(string, string, string, string, string, string, string, string, string, string, string, string, string, string, string, string, string);
		void set_global_timestep(double global_ts);
		void set_pathes_2x2(string, int);
		void set_offset_2x2(unsigned long, int);
		void set_y_tic_distance(double);
		void set_y2_tic_distance(double);
		void set_bmargin(double);
		void set_tmargin(double);
		void set_lmargin(double);
		void set_rmargin(double);


		// Get Functions
		string get_title();
		string get_x_lable();
		string get_y_lable();
		string get_y2_lable();

		string get_x_font();
		string get_y_font();
		int get_x_font_size();
		int get_y_font_size();
		int get_x_tic_size();
		int get_y_tic_size();
		int get_x_tic_font_size();
		int get_y_tic_font_size();

		string get_x_unit();
		vector<linesettings> get_plottings();
		bool get_show_title();
		bool get_show_description();
		vector<string> get_description_values();
		double get_global_time_step();
		string get_pathes_2x2(int);
		unsigned long get_offset_2x2(int);
		double get_y_tic_distance();
		double get_y2_tic_distance();
		double get_bmargin();
		double get_tmargin();
		double get_lmargin();
		double get_rmargin();

		//other Functions
		void append_line(bool second_axis, int x_data, int y_data, string title, string datafile, int aggregation, int which_aggregation, string points_or_lines, int type = 0, float size = 0,
									string linecolor = "");

		//Function to read in the data to be plotted
		std::vector<std::pair<int, std::vector<double> > > read_plotting_values(string path);

		//Function to aggregate the data
		std::vector<std::pair<int, std::vector<double> > > aggregate_data(std::vector<std::pair<int, std::vector<double> > > data,
																			int aggregation,
																			int which_aggregation);

		//Function to write aggregated values to file
		void write_data_to_file(std::vector<std::pair<int, std::vector<double> > > data, string path);
};

#endif /* PLOTTINGSETTINGS_H_ */
