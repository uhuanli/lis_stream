/*
 * slis.h
 *
 *  Created on: 2016Äê8ÔÂ7ÈÕ
 *      Author: liyouhuan
 */

#ifndef SLIS_H_
#define SLIS_H_
#include "sitem.h"
#include "../util/runtime.h"

class slope{
public:
	slope(int win_sz, double _slope);
	~slope();
	void run(string _data_f);
	void run_debug();
	void run_stream();
	void run_microsoft(int _winsz);

	int update(Vtype _ins);
	int construction(vector<Vtype>& ivec);

	int LISlength();
	string to_str();
	int to_size();
	string slope_str();

	int get_timestamp();

private:
	int timestamp;
	int win_size;
	double m_slope;

	Vtype get_buf(int _i);
	void new_item(sitem* _it);
	int construct();
	int insert(Vtype _ins);
	int remove();
	int find_rank_j(Vtype _val, int _i);

	bool check_slope(Vtype v1, int t1, Vtype v2, int t2);

	sitem** htail;
	int lis_len;

	sitem** item_pool;
	int pool_used;

	sitem** S;

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



#endif /* SLIS_H_ */
