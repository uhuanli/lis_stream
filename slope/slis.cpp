/*
 * slis.cpp
 *
 *  Created on: 2016Äê8ÔÂ7ÈÕ
 *      Author: liyouhuan
 */
#include "slis.h"
#include "../datastream/datastream.h"
#include "../quadruple/quadruple.h"
slope::slope(int win_sz, double _slope){
	this->win_size = win_sz;
	this->m_slope = _slope;
	this->timestamp = 0;
	this->htail = new sitem*[win_sz];
	this->lis_len = 0;
	this->buf = new int[win_sz];
	this->buf_h = 0;
	this->buf_t = 0;
	this->pool_used = 0;
	this->item_pool = new sitem*[win_sz];
	for(int i = 0; i < this->win_size; i ++){
		this->item_pool[i] = NULL;
		this->htail[i] = NULL;
	}
}
slope::~slope(){
	delete[] this->htail;
	delete[] this->item_pool;
	delete[] this->buf;
}
void slope::run(string _data_f){
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
		_ss << "[slis]";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		label_type << "slis\t" << "m-"<< this->m_slope;
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
		util::log(this->slope_str().c_str(), "");
		this->t_compute.end();

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
}

void slope::run_debug(){

}
void slope::run_stream(){

}
void slope::run_microsoft(int _winsz){
	cout << "begin run_mhmicrosoft" << endl;

	string microsoft = "microsoft_stock.dat";
	string powerusage= "powerusage.dat";
	string geneseq = "gene_seq.dat";
	string synthetic = "synthetic_part.dat";
	string dataset = powerusage;
	datastream ds(dataset);
	int winsize = 10;
	orthogonal otg(_winsz);
	int count_diff = 0;
	int count_total = 0;
	while(ds.hasnext())
	{
		double d_next = ds.next();
		int ai = ((int)(d_next));
		{
			stringstream _ss;
			_ss << "ai = " << ai << "\t" << d_next << endl;
			util::log(_ss);
		}
		this->update(ai);
		otg.update(ai);
		stringstream _ss;
		string slope_lis = this->slope_str();
		string otg_lis;
		{
			stringstream _tmp_ss;
			otg.enum_str(_tmp_ss);
			otg_lis = _tmp_ss.str();
		}
		_ss << "\n\n++++++++++++++" << endl;
		_ss << otg.seq_str() << endl;
		_ss << "mh_str:\n" << this->to_str() << endl;
		_ss << "mh: [" << slope_lis << "]" << endl;
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

int slope::update(int _ins){
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
int slope::construction(vector<int>& ivec){
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

int slope::LISlength(){
	return this->lis_len;
}
string slope::to_str(){
	stringstream _ss;
	if(this->buf_t > this->buf_h)
	{
		for(int i = this->buf_h; i < this->buf_t; i ++)
		{
			_ss << " " << this->buf[i];
		}
		_ss << endl;
	}
	else
	{
		for(int i = this->buf_h; i < this->win_size; i ++){
			_ss << " " << this->buf[i];
		}
		for(int i = 0; i < this->buf_t; i ++){
			_ss << " " << this->buf[i];
		}
	}
	return _ss.str();
}
int slope::to_size(){
	int sz = sizeof(slope);

	sz += sizeof(sitem*) * this->win_size;
	sz += sizeof(sitem*) * this->win_size;
	sz += sizeof(sitem*) * this->win_size;
	sz += sizeof(int) * this->win_size;

	sitem* sit = new sitem(-1, -1, this->win_size);
	sz += sit->to_size() * this->win_size;//Pool
	sz += sit->to_size() * this->win_size;//htail
	delete sit;
	return sz;
}
string slope::slope_str(){
	stringstream _ss;
	S = new sitem*[this->lis_len];
	int _tmp_i = 0;
	sitem* it = this->htail[this->lis_len-1];
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

int slope::get_timestamp(){
	return this->timestamp;
}

//private:

int slope::get_buf(int _i){
	return this->buf[(this->buf_h+_i) % this->win_size];
}
void slope::new_item(sitem* _it){
	this->item_pool[this->pool_used] = _it;
	this->pool_used ++;
}
int slope::construct(){
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		int i_val = this->get_buf(i);
		int itime = i;
		if(this->timestamp > this->win_size)
		{
			itime = this->timestamp-this->win_size+i;
		}

		sitem* it = new sitem(i_val, itime, this->win_size);
		int itail = this->find_rank_j(it->val, itime);

		this->new_item(it);
		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			this->htail[itail] = it;
		}
		if(itail >= 1){
			it->backtrack = this->htail[itail-1];
		}
		else
		{
			it->backtrack = NULL;
		}
	}

	return 0;
}
int slope::insert(int _ins){
	return 0;
}
int slope::remove(){
	return 0;
}
int slope::find_rank_j(int _val, int _i){
	int i_ins = 0;
	for(int i = 0; i < this->lis_len; i ++)
	{
		int ival = this->htail[i]->val;
		int itime = this->htail[i]->timestamp;
		bool slopeok = this->check_slope(_val, _i, ival, itime);
		if(slopeok){
			i_ins = i+1;
		}
	}

	return i_ins;
}

bool slope::check_slope(int v1, int t1, int v2, int t2)
{
	if(t1 <= t2){
		cout << "error" << endl;
		system("pause");
	}
	double s = (v1-v2)/(t1-t2+0.0);
	return (s>=this->m_slope);
}

void slope::log_running(){
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
string slope::sum_running(){
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

long long int slope::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}
long long int slope::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}





