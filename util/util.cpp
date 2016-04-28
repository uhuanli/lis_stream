/*
 * util.cpp
 *
 *  Created on: 2015-2-2
 *      Author: liyouhuan
 */
#include "util.h"
#include "stopwords.h"
#include "windows.h"
#include "psapi.h"
int util::WIN_SIZE;
int util::SEQ_MEM_SIZE;
ofstream util::flog;
ofstream util::run_track;
ofstream util::f_num_track;
char util::buf[util::buf_size];
int util::PARALLEL_NUM = 1;
double util::ACCURRACY;
string util::exp_home = "./";
int util::run_mode = 0;
int util::update_times = 4000;
bool util::total_on_off = false;
bool util::isconsole = false;
double util::init_space = 0;

/*  */
long long int util::get_time_cur()
{
	LARGE_INTEGER m_nTime;
	QueryPerformanceCounter(&m_nTime); //获取当前时间
	return m_nTime.QuadPart;
}
/*micro second*/
long util::cal_time(long long int _t1, long long int _t2){
	return (_t1-_t2)*1000.0*1000.0 / (util::ACCURRACY+0.0);
}

string util::space_str()
{
//	return "";


	stringstream _ss;

//	_ss << "total_vir\t" << "total_phy\t" << "virtual\t" << "physical" << endl;
//	_ss << total_vir << "\t";
//	_ss << total_phy << "\t";
//	_ss << virtuallMemUsedByMe << "\t";

	_ss << fixed << setprecision(3) << (util::get_space()/1000000.0);


	return _ss.str();

}
double util::get_space(){
	PROCESS_MEMORY_COUNTERS pmc;
	GetProcessMemoryInfo(GetCurrentProcess(), &pmc, sizeof(pmc));
	MEMORYSTATUSEX memInfo;
	memInfo.dwLength = sizeof(MEMORYSTATUSEX);
	GlobalMemoryStatusEx(&memInfo);


	SIZE_T virtuallMemUsedByMe = pmc.WorkingSetSize;
	SIZE_T physMemUsedByMe = pmc.WorkingSetSize;
	DWORDLONG total_vir = memInfo.ullTotalVirtual;
	DWORDLONG total_phy = memInfo.ullTotalPhys;
	return (physMemUsedByMe)+0.0;
}
double util::space_inc(){
	double _s = util::get_space();
	return ((_s - util::init_space)/1000)/1000.0;
}

bool util::draw_lots(double prob){
	int _n = rand() % 1000;
	return _n < (prob * 1000);
}

void util::get_rand(int _min, int _max, int _num, vector<int>& vrand)
{
	vrand.clear();

	int interval = _max - _min;
	if(interval < 1){
		cout << "err: max and min" << endl;
		exit(0);
	}
	for(int i = 0; i < _num; i ++){
		int _r = _min + (rand() % interval);
		vrand.push_back(_r);
	}
}

void util::log(const char* str, const char* lat)
{
	if(!util::flog_on){
		return;
	}
	util::flog << str << lat;
	util::flog.flush();
}

void util::log(stringstream & _ss)
{
	if(!util::flog_on){
		return;
	}
	util::flog << _ss.str();
	util::flog.flush();
}

void util::initial(){

	srand((unsigned)time(NULL));

	LARGE_INTEGER m_nFreq;
	QueryPerformanceFrequency(&m_nFreq);
	util::ACCURRACY = (double)m_nFreq.QuadPart;
	util::exp_home = "./";
	util::update_times = 4000;

	string runlog_f = util::exp_home + "log/";
	string flog_f = runlog_f + "flog.log";
	util::flog.open(flog_f.c_str(), ios::out);
	if(! util::flog){
		cout << "log file err： " << flog_f << endl;
		exit(0);
	}

	string run_track_f = runlog_f + "ftrack.log";
	util::run_track.open(run_track_f.c_str(), ios::out);
	if(! util::run_track){
		cout << "track err" << endl;
		exit(0);
	}

	string num_track_f = runlog_f + "num_track.log";
	util::f_num_track.open(num_track_f.c_str(), ios::out);
	if(! util::f_num_track){
		cout << "num_track err" << endl;
		exit(0);
	}

}

void util::track(const char* str, const char* lat)
{
	if(! util::write_track) return;

	util::run_track << str << lat;
	util::run_track.flush();
}

void util::track(stringstream & _ss)
{
	if(! util::write_track) return;

	util::run_track << _ss.str();
	util::run_track.flush();
}

void util::num_track(const char* str, int _num)
{
	util::f_num_track << str << " = " << _num << endl;
	util::f_num_track.flush();
}

void util::num_track(stringstream & _ss)
{
	if(! util::on_num_track){
		return;
	}
	util::f_num_track << _ss.str();
	util::f_num_track.flush();
}
