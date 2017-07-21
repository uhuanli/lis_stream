/*
 * range.cpp
 *
 *  Created on: 2016Äê8ÔÂ9ÈÕ
 *      Author: liyouhuan
 */
#include "range.h"
#include "../datastream/datastream.h"
#include "../quadruple/quadruple.h"

range::range(int _winsz, int _Li, int _Ui, double _Lv, double _Uv){
	this->win_size = _winsz;
	this->Li = _Li;
	this->Ui = _Ui;
	this->Lv = _Lv;
	this->Uv = _Uv;
	this->timestamp = 0;
	this->lis_len = 0;
	this->buf = new Vtype[_winsz];
	this->buf_h = 0;
	this->buf_t = 0;
	this->pool_used = 0;
	this->item_pool = new ritem*[_winsz];
	for(int i = 0; i < this->win_size; i ++){
		this->item_pool[i] = NULL;
	}
}
range::~range(){
	delete[] this->item_pool;
	delete[] this->buf;
}

void range::run(string _data_f){
	this->t_insert.initial();
	this->t_remove.initial();
	this->t_update.initial();
	this->t_compute.initial();
	this->t_total.initial();

	string log_f = util::exp_home + "runningtime/";
	string file_name;
	stringstream label_type;
	{
		int i1 = _data_f.rfind('/');
		int i2 = _data_f.rfind('.');
		string _datatype = _data_f.substr(i1+1, i2-i1-1);
		stringstream _ss;
		_ss << "[rlis]";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		label_type << "rlis\t" << "R";
		label_type << this->Li << "," << this->Ui << "-";
		label_type << this->Lv << "," << this->Uv;
		label_type << "\t" << _datatype << "\t" << this->win_size;
	}
	this->run_log.open(log_f.c_str(), ios::out);
	if(! this->run_log){
		cout << log_f << " can not be opened" << endl;
		system("pause");
	}

	util::init_space = util::get_space();

	datastream ds(_data_f);
	while(ds.hasnext() && ds.timestamp() < this->win_size)
	{
		int ai = ds.next();
		{
			this->update(ai);
		}
	}

	while(ds.hasnext())
	{
		this->t_total.begin();
		this->t_update.begin();
		int ai = ds.next();
		{
			this->update(ai);
		}

		this->t_update.end();

		this->t_compute.begin();
		util::log(this->range_str().c_str(), "");
		this->t_compute.end();

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
}
void range::run_debug(){

}
void range::run_stream(){

}
void range::run_microsoft(int _winsz){
	cout << "begin run_rangemicrosoft" << endl;
	cout << Li << ", " << Ui << ", " << Lv << "," << Uv << endl;

	string microsoft = "microsoft_stock.dat";
	string powerusage= "powerusage.dat";
	string geneseq = "gene_seq.dat";
	string synthetic = "synthetic_part.dat";
	string dataset = microsoft;
	datastream ds(dataset);
	qnlist otg(_winsz);
	int count_diff = 0;
	int count_total = 0;
	while(ds.hasnext())
	{
		double d_next = ds.next();
		int ai = ((int)(d_next));
		{
			stringstream _ss;
			_ss <<"\n\n*****"<< ds.timestamp() << ": ai = " << ai << "\t" << d_next << endl;
			util::log(_ss);
			if(ds.timestamp() == 61){
				cout << "in" << endl;
			}
		}
		this->update(ai);
		otg.update(ai);
		stringstream _ss;
		string range_lis = this->range_str();
		string otg_lis;
		{
			stringstream _tmp_ss;
			otg.enum_str(_tmp_ss);
			otg_lis = _tmp_ss.str();
		}
		_ss << "\n\n++++++++++++++" << endl;
		_ss << otg.seq_str() << endl;
		_ss << "range_str:\n" << this->to_str() << endl;
		_ss << "rlis: [" << range_lis << "]" << endl;
		_ss << "ot: [" << otg_lis << "]" << endl;

		int slope_len = this->LISlength();
		int otg_len = otg.LISlength();
		if(slope_len != otg_len){
//			cout << "different at " << this->timestamp << endl;
//			_ss << "different at " << this->timestamp << endl << endl;
//			util::log(_ss);
			count_diff ++;
		}
		util::log(_ss);

		count_total ++;
		if(count_total % 100 == 0) cout << count_total << endl;
		if(count_total > 2000) break;
	}

	cout << "dataset: " << dataset << endl;
	cout << "end run_mhmicrosoft and there are " << count_diff << " diffs" << endl;
	cout << count_diff << "/" << count_total << " = " << (count_diff+0.0)/count_total << endl;
	cout.flush();
}

int range::update(Vtype _ins){
	this->pool_used = 0;
	for(int i = 0; i < this->win_size; i ++){
		delete this->item_pool[i];
		this->item_pool[i] = NULL;
	}
	this->lis_len = 0;

	if(this->timestamp >= this->win_size)
	{
		this->buf_h ++;
	}
	this->buf[this->buf_t] = _ins;
	this->buf_t ++;
	this->buf_h %= this->win_size;
	this->buf_t %= this->win_size;
	this->timestamp ++;

	this->construct();

	return 0;
}
int range::construction(vector<Vtype>& ivec){
	this->pool_used = 0;
	for(int i = 0; i < this->win_size; i ++){
		delete this->item_pool[i];
		this->item_pool[i] = NULL;
	}
	this->lis_len = 0;
	this->timestamp = ivec.size();
	this->buf_h = 0;
	this->buf_t = ivec.size() - 1;
	for(int i = 0; i < (int)ivec.size(); i ++){
		this->buf[i] = ivec[i];
	}

	this->construct();

	return 0;
}

int range::LISlength(){
	return this->lis_len;
}
string range::to_str(){
	stringstream _ss;

	int num_item = this->win_size;
	if(this->timestamp < this->win_size){
		num_item = this->timestamp;
	}

	for(int i = 0; i < num_item; i ++){
		_ss << this->get_item(i)->to_str() << endl;
	}
	_ss << endl;

	return _ss.str();
}
int range::to_size(){
	int sz = sizeof(range);

	sz += sizeof(ritem*) * this->win_size;
	sz += sizeof(ritem*) * this->win_size;
	sz += sizeof(int) * this->win_size;

	/* map */
	sz += (3*sizeof(int*)+sizeof(ritem*))*this->win_size;

	ritem* sit = new ritem(-1, -1, this->win_size);
	sz += sit->to_size() * this->win_size;
	delete sit;
	return sz;
}
string range::range_str(){
	stringstream _ss;
	S = new ritem*[this->lis_len];
	ritem* it = this->get_item(0);
	int num_item = -1;
	if(this->timestamp<this->win_size){
		num_item = this->timestamp;
	}else{
		num_item = this->win_size;
	}

	for(int i = 1; i < num_item; i ++)
	{
		if(it->rank < this->get_item(i)->rank){
			it = this->get_item(i);
		}
	}

	int _tmp_i = 0;
	while(it != NULL){
		S[_tmp_i] = it;
		it = it->backtrack;
		_tmp_i ++;
	}
	_ss << S[this->lis_len-1]->val;
	for(int i = this->lis_len-2; i >= 0; i --){
		_ss << "\t" << S[i]->val;
	}
//	_ss << endl;
	delete[] S;
	return _ss.str();
}

int range::get_timestamp(){
	return this->timestamp;
}

//private:

Vtype range::get_buf(int _i){
	return this->buf[(this->buf_h+_i) % this->win_size];
}
void range::new_item(ritem* _it){
	this->item_pool[this->pool_used] = _it;
	this->pool_used ++;
}
ritem* range::get_item(int _i){
	if(this->pool_used <= _i || _i < 0){
		cout << "error pool" << _i << endl;
		system("pause");
	}
	return this->item_pool[_i];
}
int range::construct(){
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	this->v2rank.clear();
	for(int i = 0; i < sz; i ++)
	{
		int i_val = this->get_buf(i);
		int itime = i;
		ritem* it = new ritem(i_val, itime, this->win_size);
		this->new_item(it);

		int i_del = i-this->Ui-1;
		if(i_del >= 0){
			this->rmq_delete(i_del);
		}
		int i_ins = i-this->Li;
		if(i_ins >= 0){
			this->rmq_insert(i_ins);
		}

		double range_lv = i_val-this->Uv;
		double range_rv = i_val-this->Lv;
		ritem* backtrack = this->rmq_query(range_lv, range_rv);
		if(backtrack == NULL)
		{
			it->rank = 1;
		}
		else
		{
			it->rank = backtrack->rank + 1;
			it->backtrack = backtrack;
		}

		if(this->lis_len < it->rank){
			this->lis_len = it->rank;
		}
	}

	return 0;
}

bool range::rmq_insert(int _i){
	ritem* _rt = this->get_item(_i);
	this->v2rank.insert(pair<double, ritem*>(_rt->val, _rt));
	return true;
}
bool range::rmq_delete(int _i){
	ritem* _rt = this->get_item(_i);
	multimap<double,ritem*>::iterator rt_del
		= this->v2rank.find(_rt->val);
	while(rt_del != this->v2rank.end() && rt_del->first == _rt->val)
	{
		if(rt_del->second->timestamp == _rt->timestamp){
			this->v2rank.erase(rt_del);
			return true;
		}
	}

	return false;
}
ritem* range::rmq_query(double v_l, double v_r){
	ritem* rt_ret = NULL;
	multimap<double,ritem*>::iterator it_l
		= this->v2rank.lower_bound(v_l);
	multimap<double, ritem*>::iterator it_r
		= this->v2rank.upper_bound(v_r);
	int max_rank = -1;
	while(it_l != it_r && it_l != this->v2rank.end())
	{
		if(max_rank < it_l->second->rank){
			max_rank = it_l->second->rank;
			rt_ret = it_l->second;
		}
		it_l ++;
	}
	return rt_ret;
}

void range::log_running(){
	if(this->timestamp == 1)
	{
		this->run_log << "ts";
		this->run_log << "	total_sum	total_cur";
		this->run_log << "	compu_sum	compu_cur";
		this->run_log << "	updat_sum	updat_cur";
		this->run_log << "	inser_sum	inser_cur";
		this->run_log << "	remov_sum	remov_cur";
		this->run_log << endl;
	}

	this->run_log << this->timestamp;
	this->run_log << "\t" << this->t_total.getsum() << "\t" << this->t_total.getcur();
	this->run_log << "\t" << this->t_compute.getsum() << "\t" << this->t_compute.getcur();
	this->run_log << "\t" << this->t_update.getsum() << "\t" << this->t_update.getcur();
	this->run_log << "\t" << this->t_insert.getsum() << "\t" << this->t_insert.getcur();
	this->run_log << "\t" << this->t_remove.getsum() << "\t" << this->t_remove.getcur();
	this->run_log << endl;

	this->run_log.flush();
}
string range::sum_running(){
	stringstream _ss;
	_ss << "" << this->t_total.getsum() << "\t";
	_ss << "" << this->t_compute.getsum() << "\t";
	_ss << "" << this->t_update.getsum() << "\t";
	_ss << "" << this->t_insert.getsum() << "\t";
	_ss << "" << this->t_remove.getsum() << "\t";
	_ss << "" << this->to_size() << "\t";
	_ss << "" << this->throughput_total() << "\t";
	_ss << "" << this->throughput_update() << "\t";
	_ss << "" << this->t_total.getavg() << "\t";
	_ss << "" << this->t_compute.getavg() << "";
	return _ss.str();
 }

long long int range::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}
long long int range::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}



