/*
 * dynamicprogram.h
 *
 *  Created on: Feb 6, 2016
 *      Author: liyouhuan
 */

#ifndef DYNPROGRAM_DYNAMICPROGRAM_H_
#define DYNPROGRAM_DYNAMICPROGRAM_H_

#include "ditem.h"
#include "../util/runtime.h"

class dynprogram
{
public:
	dynprogram(int _winsz);
	~dynprogram();
	// _method: 0-enum, 1-length
	void run(string _data_f, int _method);
	void run_dpmicrosoft();
	int update(Vtype _ins);
	int construction(vector<Vtype>& ivec);

	string to_str();
	int to_size();
	string seq_str();
	string enum_str();

private:
	Vtype get_buf(int _i);
	ditem* get_item(int _i);
	void new_item(ditem* _it);
	int construct();
	int find_ins_pos(Vtype _val);

	int run_method;
	int win_size;
	int timestamp;
	int lis_len;

	ditem** htail;

	ditem** item_pool;
	int pool_used;

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



#endif /* DYNPROGRAM_DYNAMICPROGRAM_H_ */
