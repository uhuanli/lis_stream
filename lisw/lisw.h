/*
 * lisw.h
 *
 *  Created on: 2016Äê7ÔÂ26ÈÕ
 *      Author: liyouhuan
 */

#ifndef LISW_LISW_H_
#define LISW_LISW_H_

#include "../util/runtime.h"
#include "lw_item.h"

class lisw{
public:
	lisw(int _winsz);
	~lisw();

	void run(string _data_f, int _method);
	void run_lwdebug();
	void run_lwstream();
	void run_lwmicrosoft();

	int LISlength() const;
	int to_size() const;
	int update(Vtype _ins);
	int construction(vector<Vtype>& ivec);

	string to_str() const;
	string lis_str() const;
	string seq_str();
	string pool_str() const;

private:
	int insert(Vtype _new_v);
	bool build_parents(int _ins_pos);
	/*
	 * decrease each in d by 1
	 * delete expiry 0
	 * if delete happen, delete ele 0 in alpha
	 * decrease each in alpha by 1
	 */
	bool remove();
	int decline_d();
	bool decline_alpha();

	int find_ins_pos(Vtype _val);
	int construct();
	Vtype get_buf(int _i);

	lw_item* get_item(int _i) const;
	bool addpool(lw_item* _lw_i);
	bool removepool();
	int pool_head;
	int pool_tail;

	int win_size;
	int timestamp;

	int lis_len;

	lw_item** principle_row;

	lw_item** item_pool;

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




#endif /* LISW_LISW_H_ */
