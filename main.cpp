/*
 * main.cpp
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#include "util/util.h"
#include "datastream/datastream.h"
#include "canonical/canonical.h"
#include "minheight/minheight.h"
#include "variant/variant.h"
#include "dynprogram/dynamicprogram.h"
#include "lisw/lisw.h"
#include "slope/slis.h"
#include "range/range.h"
#include "quadruple/quadruple.h"

//#define TEST 1

void construction_main(string _data_f, stringstream& _ss, int win_size);
void console_run();

int main(int argc, char* args[])
{

	if(false)
	{
		qnlist qnl(10);
		cout << sizeof(qnl) << ",tosz:" << qnl.to_size() << endl << endl;
		return 0;
	}
//	cout << "::::main" << endl;
	stringstream _ss;
	for(int i = 1; i < argc; i ++)
	{
		_ss << args[i] << " ";
	}
	int win_size = 15;
	util::run_mode = 6;
	int data_set = 0;

	util::initial();

	if(argc >= 2)
	{
		_ss >> win_size;
#ifdef TEST
		cerr << "this is test, input key to confirm" << endl;
		win_size = 10;
		system("pause");
#endif
		_ss >> util::run_mode;
		_ss >> data_set;
		string _data_f;
		string _data_home = util::exp_home + "dataset/";
		string str_dat[10] = {
				"microsoft_stock.dat",
				"c_rand.dat",
				"powerusage.dat",
				"synthetic_trans.dat",
				"gene_seq.dat"
		};

		if(data_set < 0 || data_set > 4){
			cout << "error data set" << endl;
			exit(-1);
		}

		util::dataset = str_dat[data_set];
		_data_f = _data_home + str_dat[data_set];
		util::init_log(util::dataset);

		if(util::run_mode == 0 || util::run_mode == 1){
			int run_method;
			_ss >> run_method;
#ifdef NO_ENUM
			{
				if(run_method == 0) return 0;
			}
#endif
			qnlist qnl(win_size);
			{
				if(run_method == 9 || run_method == 7){
					double _slope;
					_ss >> _slope;
					qnl.set_slope(_slope);
				}
				if(run_method == 10 || run_method == 8){
					int Li, Ui;double Lv, Uv;
					_ss >> Li >> Ui >> Lv >> Uv;
					qnl.set_range(Li, Ui, Lv, Uv);
				}
			}
			qnl.run(run_method, _data_f);
		}
		else
		if(util::run_mode == 2){
#ifdef NO_ENUM
			{
				return 0;
			}
#endif
			canonical cnl(win_size);
			cnl.run(_data_f, 0);
		}
		else
		if(util::run_mode == 3){
			minheight mh(win_size);
			int update_mode;
			_ss >> update_mode;
			{
				if(update_mode ==1){

				}
			}
			mh.run(_data_f, (update_mode==1));
		}
		else
		if(util::run_mode == 4){
			int run_method, update_mode;
			_ss >> run_method;
			_ss >> update_mode;
			variant vrt(win_size);
			vrt.run(run_method, _data_f, (update_mode==1));
		}
		else
		if(util::run_mode == 5)/*Construction*/
		{
			construction_main(_data_f, _ss, win_size);
		}
		else
		if(util::run_mode == 6)/* LISSET-Post */
		{
#ifdef NO_ENUM
			{
				return 0;
			}
#endif
			int constrained_method = 0;
			_ss >> constrained_method;
			canonical cnl(win_size);
			cnl.run(_data_f, constrained_method);
		}
		else
		if(util::run_mode == 7)/* Dynamic Program */
		{
#ifdef NO_ENUM
			{
				return 0;
			}
#endif
			/* 0 for n^2, 1 for nlogn */
			int run_alg = 0;
			dynprogram dp(win_size);
			dp.run(_data_f, run_alg);
		}
		else
		if(util::run_mode == 8)/* LISW */
		{
			/* 0 for length only, 1 for an LIS */
			int run_alg = 1;
			lisw lw(win_size);
			lw.run(_data_f, run_alg);
		}
		else
		if(util::run_mode == 9)/* Slope */
		{
			double slope_m;
			_ss >> slope_m;
			slope slis(win_size, slope_m);
			slis.run(_data_f);
		}
		else
		if(util::run_mode == 10)/* Range */
		{
			int Li, Ui;
			double Lv, Uv;
			_ss >> Li >> Ui >> Lv >> Uv;
			range rlis(win_size, Li, Ui, Lv, Uv);
			rlis.run(_data_f);
		}

	}
	else
	{
		console_run();
	}
}

void console_run()
{
	util::init_log("console");
	util::isconsole = true;
	if(util::isconsole)  /* for lisw */
	{


//		variant vrt(10);
//		vrt.runmicrosoft();
		int data_set = 0;
		string _data_f;
		string _data_home = util::exp_home + "dataset/";
		string str_dat[10] = {
				"microsoft_stock.dat",
				"c_rand.dat",
				"powerusage.dat",
				"synthetic_trans.dat",
				"gene_seq.dat"
		};

		if(data_set < 0 || data_set > 4){
			cout << "error data set" << endl;
			exit(-1);
		}

		util::dataset = str_dat[data_set];
		_data_f = _data_home + str_dat[data_set];
		int winsz = 500;
		util::update_times = 11;
		qnlist qnl(winsz);
		runtime _r;
		_r.begin();
#ifdef DEMO
		qnl.demo("microsoft_stock.dat", winsz);
#else
		qnl.run(qnlist::LIS_COUNT, _data_f);
#endif
		_r.end();
//		cout << (int)_r.getsum() << endl;

//		range rlis(winsz, Li, Ui, Lv, Uv);
//		rlis.run_microsoft(winsz);
//		minheight mh(winsz);
//		mh.run_mhmicrosoft(winsz);
//			cout << lw.to_size() << endl;
	}

	util::isconsole = true;
//		qnlist qnl(10);
//		canonical cnl(500);
//		minheight mh(1200);
	string stock_dat = "D:/Lab/experiments/lis_constraints/dataset/microsoft_stock.dat";
	string crand_dat = "D:/Lab/experiments/lis_constraints/dataset/c_rand.dat";
//		qnl.run(0, stock_dat);
//		cnl.run(stock_dat, 0);
//		mh.run(stock_dat);
}




void construction_main(string _data_f, stringstream& _ss, int win_size){
	datastream ds(_data_f);
	vector<Vtype> Vds;
	while(ds.hasnext()){
		Vds.push_back(ds.next());
		if((int)Vds.size() >= win_size) break;
	}

	cout << "read data: " << Vds.size() << "\t" << "win=" << win_size << "\t";

	int run_work = 0;
	_ss >> run_work;

	long long int tcost = 0, isize;
	if(run_work == 0)
	{/*qnlist*/
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			qnlist qnl(Vds.size());
			long long int tbegin = util::get_time_cur();
			qnl.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = qnl.to_size();
		}
		cout << "qnl\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
	else
	if(run_work == 1)
	{/*lisset*/
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			canonical cnl(Vds.size());
			long long int tbegin = util::get_time_cur();
			cnl.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = cnl.to_size();
		}
		cout << "cnl\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
	else
	if(run_work == 2)
	{/*mhlis*/
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			minheight mh(Vds.size());
			long long int tbegin = util::get_time_cur();
			mh.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = mh.to_size();
		}
		cout << "mh\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
	else
	if(run_work == 3)
	{/*variants*/
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			variant var(Vds.size());
			long long int tbegin = util::get_time_cur();
			var.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = var.to_size();
		}
		cout << "var\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
	else
	if(run_work == 4)
	{/* dynamicprogram */
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			dynprogram dp(Vds.size());
			long long int tbegin = util::get_time_cur();
			dp.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = dp.to_size();
		}
		cout << "dp\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
	else
	if(run_work == 5)
	{/* lisw */
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			lisw lw(Vds.size());
			long long int tbegin = util::get_time_cur();
			lw.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = lw.to_size();
		}
		cout << "lisw\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
	else
	if(run_work == 6)
	{/* slope */
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			slope slis(Vds.size(), 0);
			long long int tbegin = util::get_time_cur();
			slis.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = slis.to_size();
		}
		cout << "slis\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
	else
	if(run_work == 7)
	{/* range */
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			range rlis(Vds.size(), 0, 10000, 0, 1000000);
			long long int tbegin = util::get_time_cur();
			rlis.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = rlis.to_size();
		}
		cout << "range\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
}
