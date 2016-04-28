/*
 * main.cpp
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#include "util/util.h"
#include "datastream/datastream.h"
#include "quadruple/orthogonal.h"

void construction_main(string _data_f, stringstream& _ss, int win_size);

int main(int argc, char* args[])
{
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

		_data_f = _data_home + str_dat[data_set];

		long long int total_begin = util::get_time_cur();
		if(util::run_mode == 1){
			int run_method;
			_ss >> run_method;
			orthogonal otg(win_size);
			otg.run(run_method, _data_f);
		}
		else
		if(util::run_mode == 5)/*Construction*/
		{
			construction_main(_data_f, _ss, win_size);
		}
	}
	else
	{
		cout << "console" << endl;
	}
}

void construction_main(string _data_f, stringstream& _ss, int win_size){
	datastream ds(_data_f);
	vector<int> Vds;
	while(ds.hasnext()){
		Vds.push_back(ds.next());
		if(Vds.size() >= win_size) break;
	}

	cout << "read data: " << Vds.size() << "\t" << "win=" << win_size << "\t";

	int run_work = 0;
	_ss >> run_work;

	long long int tcost = 0, isize;
	if(run_work == 0)
	{/*orthogonal*/
		tcost = 0;
		for(int i = 0; i < 5; i ++)
		{
			orthogonal otg(Vds.size());
			long long int tbegin = util::get_time_cur();
			otg.construction(Vds);
			long long int tend = util::get_time_cur();
			tcost += util::cal_time(tend, tbegin);
			isize = otg.to_size();
		}
		cout << "otg\tspace\t" << isize/1000.0 << "\ttime\t";
		cout << "(" << Vds.size() << ", " << tcost/1000.0 << ")" << endl;
	}
}




