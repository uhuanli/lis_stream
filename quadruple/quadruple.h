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
class qnlist{
public:
	qnlist(int _n);
	~qnlist();
	void run(int run_method, string _data_f);
	void run_debug();
	void run_stream();
	void run_microsoft();
	void run_sloperange();

	int update(double _ins);
	int construction(vector<Vtype>& ivec);

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
	double count_lis();

	string mingap_one_str(stringstream & _ss);
	string maxgap_one_str(stringstream & _ss);
	string minwid_one_str(stringstream & _ss);
	string maxwid_one_str(stringstream & _ss);
	string range_str(stringstream & _ss, int Li, int Ui, double Lv, double Uv);
	string slope_str(stringstream & _ss, double m_slope);
	string range_nocolor_str(stringstream & _ss, int Li, int Ui, double Lv, double Uv);
	string slope_nocolor_str(stringstream & _ss, double m_slope);

#ifdef DEMO
	double moving_avg;
	double max_gap;
	double min_gap;
	double max_wei;
	double min_wei;
	double max_wid;
	double min_wid;
	/* lis_len, num_new */
	void demo(string _dateset, int _winsz);
	double movingAVG(vector<double> _data, int _winsz, int _i);
	void write_all(vector<vector<double> >& allvec, int _winsz, string _dataset);
	void write_pair
	(string _file, vector<double>& _data, vector<double>& _value, int _winsz, string _dataset);
#endif

	void set_range(int Li, int Ui, double Lv, double Uv);
	void set_slope(double _slope);

	int get_timestamp();

	const static int ENUM = 0;
	const static int MAXGAP_ONE = 1;
	const static int MINGAP_ONE = 2;
	const static int MAXWID_ONE = 3;
	const static int MINWID_ONE = 4;
	const static int MAXWEI_ONE = 5;
	const static int MINWEI_ONE = 6;
	const static int LIS_COUNT = 101;

private:
	int timestamp;
	int win_size;
	int insert(item* _ins);
	int find_ins_pos(double _val);
	item* remove();
	int remove_adjust();
	int h_adjust();
	int v_adjust();

	/* tail of leftpart & head of the _rightpart */
	int remove_divide(item** _left, item** _right);
	/* tail of leftpart & head of the _rightpart */
	int remove_join(item** _left, item** _right);
	/* tail of leftpart & head of the _rightpart */
	int vopt_adjust(item** _left, item** _right);

	int update_rlen_tail();

	string starting_str(item* it_first, stringstream & _ss);
	item* rn_of_dn(item* it);
	string ending_str(item* it_last);
	void ending_str(stringstream& _ss, item* it_last, int& _cur_num_lis);

	string compute_str(int run_method);
	string get_method(int _i);

	void range_color(int Li, int Ui, double Lv, double Uv);
	void slope_color(double m_slope);
	void no_black_str(stringstream & _ss);
	bool range_satisfy
	(item* it_ai, item* it_ak, int Li, int Ui, double Lv, double Uv);
	bool slope_satisfy(item* it_ai, item* it_ak, double m_slope);

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

#ifdef OPT_SAVE
	int pre_len;
	int v_save;
	int v_total;
	int v_dn;
	int v_un;
	int avg_len;
#endif

#ifdef NUM_LIS
	double num_lis;
	double num_new;
#endif

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

#ifdef QN_SEQ
	Vtype* buf;
	int buf_h;
	int buf_t;
#endif

	int gtmpibegin;
	int gtmpiend;
	int gtmpimid;
	int gtmpi;
};


#endif /* ORTHOGONAL_H_ */
