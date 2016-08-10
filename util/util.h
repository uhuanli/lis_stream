/*
 * util.h
 *
 *  Created on: 2015-2-2
 *      Author: liyouhuan
 */

#ifndef UTIL_H_
#define UTIL_H_

#include<iostream>
#include<sstream>
#include<fstream>
#include<stdio.h>
#include<string.h>
#include<stdlib.h>
#include<vector>
#include<map>
#include<set>
#include<time.h>
#include<math.h>
#include<iomanip>
using namespace std;

class RuntimeLis;
class RuntimeDense;

class util
{
public:
	static int WIN_SIZE;
	static int SEQ_MEM_SIZE;
	static const int buf_size = 1000*1000;
	static char buf[buf_size];
	static int PARALLEL_NUM;


	static double ACCURRACY;

	static int run_mode;
	static int update_times;

	static bool opt_update;
	static bool isconsole;

	static string exp_home;
	static ofstream flog;
	static ofstream run_track;
	const static bool on_num_track = false;
	static ofstream f_num_track;

	static const bool debug_total = false;
	static bool total_on_off;
	static const bool flog_on = true;
	static const bool write_track = false;
	static const bool testdp = false;

	static long long int get_time_cur();
	static long cal_time(long long int _t1, long long int _t2);

	static double init_space;
	static string space_str();
	static double get_space();
	static double space_inc();

	static bool draw_lots(double prob);
	static void get_rand(int _min, int _max, int _num, vector<int>& vrand);
	static void log(const char* str, const char* lat);
	static void log(stringstream & _ss);
	static void initial();
	static void track(const char* str, const char* lat);
	static void track(stringstream & _ss);
	static void num_track(const char* str, int _num);
	static void num_track(stringstream & _ss);

};


#endif /* UTIL_H_ */
