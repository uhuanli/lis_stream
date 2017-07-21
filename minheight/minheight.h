/*
 * minheight.h
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#ifndef MINHEIGHT_H_
#define MINHEIGHT_H_

#include "mh_item.h"
#include "../util/runtime.h"
#include "../util/util.h"
#include<iostream>
using namespace std;

class minheight{
public:
	minheight(int _winsz);
	~minheight();
	void run(string _data_f, const bool _update_vertical);
	void run_mhdebug();
	void run_mhstream();
	void run_mhmicrosoft(int _winsz);

	string to_str();
	int to_size();
	string seq_str();
	string minheight_str();
	int update(Vtype _ins);
	int update_vertical(Vtype _ins);
	int construction(vector<Vtype>& ivec);
private:
	Vtype get_buf(int _i);
	void new_item(mh_item* _it);
	int construct();
	int find_ins_pos(Vtype _val);
	int get_minheight();

	int h_adjust();
	int v_adjust();
	int insert(Vtype _ins);
	int remove();
	int build_pmost();

	int win_size;
	int timestamp;

	mh_item** htail;
	int lis_len;

	mh_item** item_pool;
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



#endif /* MINHEIGHT_H_ */
