/*
 * canonical.h
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#ifndef CANONICAL_H_
#define CANONICAL_H_
#include "citem.h"
#include "../util/runtime.h"
class canonical{
public:
	canonical(int _winsz);
	~canonical();
	void run(string _data_f, int _constrained);
	void run_candebug();
	void run_canmicrosoft();

	int update(int _ins);
	int construction(vector<Vtype>& ivec);
	string to_str();
	int to_size();
	string seq_str();
	string enum_str();
	string minh_str();
	string maxh_str();
	string minw_str();
	string maxw_str();
	string minwid_str();
	string maxwid_str();

	const static int maxgap = 1;
	const static int mingap = 2;
	const static int minwid = 3;
	const static int maxwid = 4;
	const static int minwei = 5;
	const static int maxwei = 6;

private:

	int insert(citem* it);
	/*
	 * recalculate pmax
	 * update inD for each
	 * update pred
	 * update succ
	 * */
	int remove();
	int pmax_update();
	int horizontal();
	int pred_update();
	int succ_update(citem* _del);

	string compute_str(int constrained_method);
	string get_method(int constrained);

	int win_size;
	int timestamp;
	int lis_len;

	citem** hlist;
	citem** htail;
	citem** pmax;

	fstream run_log;
	void log_running();
	string sum_running();
	long long int throughput_total();
	long long int throughput_update();
	runtime t_insert;
	runtime t_remove;
	runtime t_update;
	runtime t_compute;
	runtime t_total;

	Vtype* buf;
	int buf_h;
	int buf_t;
};


#endif /* CANONICAL_H_ */
