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
#include <windows.h>
#include "psapi.h"
using namespace std;

typedef double Vtype;

//#define RLEN 1
//#define QN_SEQ 1
//#define IND_TAIL 1
//#define QN_PREV 1
#define OPT_SAVE 1
#define NUM_LIS 1

//#define NO_ENUM 1
//#define ENUMSTR 1

//#define DN_OPT 1
//#define TRACK_CRASH 1
#define LOG 1
//#define DEBUG_TRACK 1

//#define DEADCYCLE 1
//#define STDLOG 1

//#define DEMO 1
#define ENABLE_MAX_LIS_NUM 1
#define COUNT_LISNUM 1


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

	static const long long int MAX_LIS_NUM = 1*1000*1000;

	static double ACCURRACY;

	static int run_mode;
	static int update_times;

	static bool opt_update;
	static bool isconsole;

	static string exp_home;
	static string dataset;
	static ofstream flog;
	static ofstream run_track;
	const static bool on_num_track = false;
	static ofstream f_num_track;

	static const bool debug_total = false;
	static bool total_on_off;
	static const bool flog_on = true;
	static const bool write_track = false;
	static const bool testdp = false;

	static LARGE_INTEGER m_nTime;
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
	static void init_log(string _data_set);
	static void track(const char* str, const char* lat);
	static void track(stringstream & _ss);
	static void num_track(const char* str, int _num);
	static void num_track(stringstream & _ss);

};


#endif /* UTIL_H_ */
