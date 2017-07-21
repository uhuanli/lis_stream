/*
 * range.h
 *
 *  Created on: 2016Äê8ÔÂ9ÈÕ
 *      Author: liyouhuan
 */

#ifndef RANGE_H_
#define RANGE_H_
#include "ritem.h"
#include "../util/runtime.h"
#include <map>

class range{
public:
	range(int _winsz, int _Li, int _Ui, double _Lv, double _Uv);
	~range();

	void run(string _data_f);
	void run_debug();
	void run_stream();
	void run_microsoft(int _winsz);

	int update(Vtype _ins);
	int construction(vector<Vtype>& ivec);

	int LISlength();
	string to_str();
	int to_size();
	string range_str();


	int get_timestamp();

private:
	int Li;
	int Ui;
	double Lv;
	double Uv;
	int win_size;
	int timestamp;

	Vtype get_buf(int _i);
	void new_item(ritem* _it);
	ritem* get_item(int _i);
	int construct();

	multimap<double, ritem*> v2rank;
	bool rmq_insert(int _i);
	bool rmq_delete(int _i);
	ritem* rmq_query(double v_l, double v_r);

	ritem** item_pool;
	int pool_used;

	ritem** S;

	int lis_len;

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



#endif /* RANGE_H_ */
