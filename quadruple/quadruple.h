/*
 * orthogonal.h
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#ifndef ORTHOGONAL_H_
#define ORTHOGONAL_H_

#include "../quadruple/item.h"
#include "../util/runtime.h"
class orthogonal{
public:
	orthogonal(int _n);
	~orthogonal();
	void run(int run_method, string _data_f);
	void run_debug();
	void run_stream();
	void run_microsoft();
	void run_sloperange();

	int update(int _ins);
	int construction(vector<int>& ivec);

	int LISlength();
	string to_str();
	int to_size();
	string all_str();
	string seq_str();
	string enum_str(stringstream & _ss);
	string maxweight_str(stringstream & _ss);
	string minweight_str(stringstream & _ss);
	string maxgap_str(stringstream & _ss);
	string mingap_str(stringstream & _ss);
	string maxfirst_str(stringstream & _ss);
	string minfirst_str(stringstream & _ss);
	string maxlast_str(stringstream & _ss);
	string minlast_str(stringstream & _ss);

	string mingap_one_str(stringstream & _ss);
	string maxgap_one_str(stringstream & _ss);
	string minwid_one_str(stringstream & _ss);
	string maxwid_one_str(stringstream & _ss);
	string range_str(stringstream & _ss, int Li, int Ui, double Lv, double Uv);
	string slope_str(stringstream & _ss, double m_slope);

	void set_range(int Li, int Ui, double Lv, double Uv);
	void set_slope(double _slope);

	int get_timestamp();

private:
	int timestamp;
	int win_size;
	int insert(item* _ins);
	int find_ins_pos(int _val);
	item* remove();
	int h_adjust();
	int v_adjust();
	int update_rlen_tail();

	string starting_str(item* it_first, stringstream & _ss);
	item* rn_of_dn(item* it);
	string ending_str(item* it_last);
	void ending_str(stringstream& _ss, item* it_last);

	string compute_str(int run_method);
	string get_method(int _i);

	void range_color(int Li, int Ui, double Lv, double Uv);
	void slope_color(double m_slope);
	void no_black_str(stringstream & _ss);

	int update_rmmost();
	int update_lmmost();
	int update_inLIS();
	/*  */
	item** hlist;
	item** htail;
	item** tmp_left;
	item** tmp_right;
	item** S;

	item* _it_tmp;
	item* _ah_tmp;
	item* _ax_tmp;
	item* _ay_tmp;

	int rli;
	int rui;
	double rlv;
	double ruv;
	double rslope;

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

	int* buf;
	int buf_h;
	int buf_t;
};


#endif /* ORTHOGONAL_H_ */
