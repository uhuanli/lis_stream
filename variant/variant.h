/*
 * variants.h
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#ifndef VARIANT_H_
#define VARIANT_H_
#include "vitem.h"
#include "../util/runtime.h"
class variant{
public:
	variant(int _n);
	~variant();
	void run(int run_method, string _data_f, const bool _update_vertical);
	void runmicrosoft();
	int update(Vtype _ins);
	int update_vertical(Vtype _ins);
	int construction(vector<Vtype>& ivec);
	string to_str();
	int to_size();
	string all_str();
	string seq_str();
	string maxweight_str(bool _vertical);
	string minweight_str(bool _vertical);
	string maxgap_str(bool _vertical);
	string mingap_str(bool _vertical);
	string maxgap_one_str(bool _vertical);
	string mingap_one_str(bool _vertical);
	string maxwidth_one_str(bool _vertical);
	string minwidth_one_str(bool _vertical);

	string maxweight_str_ori();
	string minweight_str_ori();
	string maxgap_str_ori();
	string mingap_str_ori();
	string maxgap_one_str_ori();
	string mingap_one_str_ori();
	string minwidth_one_str_ori();
	string maxwidth_one_str_ori();

private:
	int timestamp;
	int win_size;

	int find_ins_pos(Vtype _val);
	string compute_str(int run_method, const bool _vertical);
	string get_method(int _i);
	Vtype get_buf(int _i);
	void new_item(vitem* _it);
	vitem* getRM(int _i, vitem* _cur);

	int h_adjust();
	int v_adjust();
	int insert(Vtype _ins);
	int remove();

	string one_str(vitem* start);
	int most_rm();
	int most_lm();

	/*  */
	vitem** hlist;
	vitem** htail;

	vitem** item_pool;
	int pool_used;

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


#endif /* VARIANT_H_ */
