/*
 * dynamicprogram.cpp
 *
 *  Created on: 2016Äê2ÔÂ15ÈÕ
 *      Author: liyouhuan
 */
#include "dynamicprogram.h"
#include "../util/util.h"
#include "../datastream/datastream.h"
#include "../quadruple/quadruple.h"

// public
dynprogram::dynprogram(int _winsz)
{
	this->win_size = _winsz;
	this->timestamp = 0;
	this->lis_len = 0;
	this->buf = new int[_winsz];
	this->buf_h = 0;
	this->buf_t = 0;
	this->run_method = -1;
	this->pool_used = 0;
	this->item_pool = new ditem*[_winsz];
	for(int i = 0; i < this->win_size; i ++){
		this->item_pool[i] = NULL;
	}
}
dynprogram::~dynprogram()
{
	delete[] this->item_pool;
	delete[] this->buf;

	this->htail = NULL;
	this->buf = NULL;
	this->item_pool = NULL;
}
// _method: 0-enum, 1-length
void dynprogram::run(string _data_f, int _method)
{
	this->t_insert.initial();
	this->t_remove.initial();
	this->t_update.initial();
	this->t_compute.initial();
	this->t_total.initial();

	string log_f = util::exp_home + "runningtime/";
	string file_name;
	stringstream label_type;
	string _datatype;
	{
		int i1 = _data_f.rfind('/');
		int i2 = _data_f.rfind('.');
		_datatype = _data_f.substr(i1+1, i2-i1-1);
		stringstream _ss;
		_ss << "[dp]";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		label_type << "dp\t" << "enum";
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
		this->update(ai);
	}

	while(ds.hasnext())
	{
		this->t_total.begin();
		this->t_update.begin();
		int ai = ds.next();
		this->update(ai);

		this->t_update.end();
		this->t_compute.begin();
		util::log(this->enum_str().c_str(), "");
		this->t_compute.end();

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
}
void dynprogram::run_dpmicrosoft()
{
	cout << "begin run_canmicrosoft" << endl;
	int winsize = 10;
	string file = "microsoft_stock.dat";
	datastream ds(file);
	orthogonal otg(winsize);
	while(ds.hasnext())
	{
		int ai = ds.next();
		otg.update(ai);
		this->update(ai);
		string otg_enum = "";
		{
			stringstream _tmp_ss;
			otg.enum_str(_tmp_ss);
			otg_enum = _tmp_ss.str();
		}

		string dp_enum = this->enum_str();
		stringstream _ss;
		_ss << "\n\n++++++++++++++" << endl;
		_ss << otg.seq_str() << endl;
		_ss << "otg at " << ds.timestamp() << endl;
		_ss << "[" << otg_enum << "]" << endl;
		_ss << "dp" << endl;
		_ss << "[" << dp_enum << "]" << endl;
		if(otg_enum != dp_enum){
			util::log(_ss);
			util::log(this->to_str().c_str(), "");
			cout << "diff" << endl;
		}
	}

	cout << "end run_dpmicrosoft" << endl;
}

string dynprogram::to_str()
{
	stringstream _ss;
	_ss << " time: " << this->timestamp << endl;
	_ss << " lis: " << this->lis_len << endl;
	for(int i = 0; i < this->pool_used; i ++)
	{
		ditem* dit = this->get_item(i);
		_ss << "\t" << i << "-[" << dit->to_str() << "]\n";
	}
	_ss << endl;
	return _ss.str();
}
int dynprogram::to_size()
{
	int sz = 0;
	sz += sizeof(int) * 4;
	sz += sizeof(ditem*) * this->win_size;
	sz += sizeof(int);
	sz += sizeof(int) * this->win_size;
	ditem* dit = new ditem(0, 0, this->win_size);
	sz += dit->to_size() * this->win_size;
	delete dit;

	return sz;
}
string dynprogram::seq_str()
{
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
string dynprogram::enum_str()
{
	stringstream _ss;
	ditem** S = new ditem*[this->lis_len];

	for(int i = this->pool_used-1; i >= 0 ; i --)
	{
		ditem* dit = this->get_item(i);
		if(dit->rlen == this->lis_len)
		{
			dit->enumlis(this->lis_len, 0, S, _ss);
		}
	}

	delete[] S;
	return _ss.str();
}

// private
int dynprogram::get_buf(int _i){
	return this->buf[(this->buf_h+_i) % this->win_size];
}
ditem* dynprogram::get_item(int _i){
	if(_i < 0 || _i > this->pool_used){
		cout << "err ditem" << endl;
		exit(0);
	}

	return this->item_pool[_i];
}
void dynprogram::new_item(ditem* _it){
	this->item_pool[this->pool_used] = _it;
	this->pool_used ++;
}
int dynprogram::update(int _ins)
{
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
int dynprogram::construction(vector<int>& ivec){
	this->pool_used = 0;
	for(int i = 0; i < this->win_size; i ++){
		delete this->item_pool[i];
		this->item_pool[i] = NULL;
	}
	this->lis_len = 0;
	this->timestamp = ivec.size();
	this->buf_h = 0;
	this->buf_t = ivec.size() - 1;
	for(int i = 0; i < ivec.size(); i ++){
		this->buf[i] = ivec[i];
	}

	this->construct();

	return 0;
}
int dynprogram::construct(){
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	this->pool_used = 0;
	for(int i = 0; i < sz; i ++){
		ditem* it = new ditem(this->get_buf(i), i, this->win_size);
		this->new_item(it);
	}

	this->lis_len = 0;

	for(int i = 0; i < sz; i ++)
	{
		ditem* it = this->get_item(i);
		int _max_pre_rlen = 0;
		for(int j = 0; j < i; j ++)
		{
			ditem* jt = this->get_item(j);
			if(jt->val > it->val)
				continue;

			if(_max_pre_rlen < jt->rlen)
			{
				_max_pre_rlen = jt->rlen;
			}
		}

		it->rlen = _max_pre_rlen + 1;
		if(this->lis_len < it->rlen){
			this->lis_len = it->rlen;
		}

		for(int j = 0; j < i; j ++)
		{
			ditem* jt = this->get_item(j);
			if(_max_pre_rlen == jt->rlen && it->val >= jt->val)
			{
				it->add_pre(jt);
			}
		}
	}

	return 0;
}
int dynprogram::find_ins_pos(int _val){
	int ibegin = 0;
	int iend = this->lis_len;

	int imid = 0;
	while(ibegin < iend)
	{
		if(htail[ibegin]->val > _val) return ibegin;
		if(htail[iend-1]->val <= _val) return iend;

		imid = (ibegin + iend)/2;
		if(htail[imid]->val <= _val)
		{
			ibegin = imid + 1;
		}
		else
		if(htail[imid-1]->val > _val)
		{
			iend = imid - 1;
		}
		else
		{
			return imid;
		}
	}

	return ibegin;

	return 0;
}
void dynprogram::log_running(){
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
string dynprogram::sum_running(){
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
long long int dynprogram::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}
long long int dynprogram::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

