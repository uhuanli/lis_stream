/*
 * canonical.cpp
 *
 *  Created on: 2015Äê9ÔÂ23ÈÕ
 *      Author: liyouhuan
 */
#include "canonical.h"
#include "../datastream/datastream.h"
//public:
canonical::canonical(int _winsz){
	this->win_size = _winsz;
	this->lis_len = 0;
	this->timestamp = 0;

	this->hlist = new citem*[this->win_size];
	this->htail = new citem*[this->win_size];
	this->pmax = new citem*[this->win_size];
	for(int i = 0; i < this->win_size; i ++){
		this->hlist[i] = NULL;
		this->htail[i] = NULL;
		this->pmax[i] = NULL;
	}

	this->buf_h = 0;
	this->buf_t = 0;
	this->buf = new Vtype[this->win_size];
}
canonical::~canonical(){
	delete[] this->buf;
	delete[] this->hlist;
	delete[] this->htail;
	delete[] this->pmax;
}

void canonical::run(string _data_f, int _constrained){
	string log_f = util::exp_home + "runningtime/";
	string file_name;
	stringstream label_type;
	string method[] = {
			"enum",
			"maxgap_one",
			"mingap_one",
			"maxwid_one",
			"minwid_one",
			"maxwei_one",
			"minwei_one"
	};
	{
		int i1 = _data_f.rfind('/');
		int i2 = _data_f.rfind('.');
		string _datatype = _data_f.substr(i1+1, i2-i1-1);
		stringstream _ss;
		_ss << "[cnl]";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		label_type << "cnl\t" << method[_constrained];
		label_type << "\t" << _datatype << "\t" << this->win_size;
	}
	this->run_log.open(log_f.c_str(), ios::out);
	if(! this->run_log){
		cerr << log_f << " can not be opened" << endl;
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
		util::log(this->compute_str(_constrained).c_str(), "");
		this->t_compute.end();

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
}

string canonical::compute_str(int constrained_method){
	stringstream _ss_comp;
	switch(constrained_method)
	{
		case 0:
		{
			_ss_comp << "--enum:" << endl;
			_ss_comp << this->enum_str() << endl;
			break;
		}
		case canonical::mingap:
		{
			_ss_comp << "--mingap_one:" << endl;
			_ss_comp << this->minh_str() << endl;
			break;
		}
		case canonical::maxgap:
		{
			_ss_comp << "--maxgap_one:" << endl;
			_ss_comp << this->maxh_str() << endl;
			break;
		}
		case canonical::minwei:
		{
			_ss_comp << "--minwei:" << endl;
			_ss_comp << this->minw_str() << endl;
			break;
		}
		case canonical::maxwei:
		{
			_ss_comp << "--maxwei:" << endl;
			_ss_comp << this->maxw_str() << endl;
			break;
		}
		case canonical::minwid:
		{
			_ss_comp << "--minwei:" << endl;
			_ss_comp << this->minwid_str() << endl;
			break;
		}
		case canonical::maxwid:
		{
			_ss_comp << "--maxwei:" << endl;
			_ss_comp << this->maxwid_str() << endl;
			break;
		}
		default :
		{
			cout << "err canonical constrained" << endl;
			system("pause");
			exit(0);
			break;
		}
	}

	return _ss_comp.str();
}

string canonical::get_method(int constrained){

	return "cnl deprecated";
}

void canonical::run_candebug(){
	cout << "begin run_candebug" << endl;

	util::initial();
	int winsize = 10;
	string debug = "debug.dat";
	datastream ds(debug);
//	orthogonal otg(winsize);
//	canonical  cnl(winsize);
	while(ds.hasnext())
	{
		int ai = ds.next();
//		otg.update(ai);
		this->update(ai);
//		string otg_enum = otg.enum_str();
		string cnl_enum = this->enum_str();
		stringstream _ss;
		_ss << "\n\n++++++++++++++" << endl;
//		_ss << otg.seq_str() << endl;
		_ss << "at " << ds.timestamp() << endl;
//		_ss << "[" << otg_enum << "]" << endl;
		_ss << "cnl" << endl;
		_ss << "[" << cnl_enum << "]" << endl;
//		if(otg_enum != cnl_enum){
//			util::log(_ss);
//			cout << "diff" << endl;
//		}
	}

	cout << "end run_candebug" << endl;
}

void canonical::run_canmicrosoft(){
	cout << "begin run_canmicrosoft" << endl;

	util::initial();
	int winsize = 10;
	string file = "microsoft_stock.dat";
	datastream ds(file);
//	orthogonal otg(winsize);
//	canonical  cnl(winsize);
	while(ds.hasnext())
	{
		int ai = ds.next();
//		otg.update(ai);
		this->update(ai);
//		string otg_enum = otg.enum_str();
		string cnl_enum = this->enum_str();
		stringstream _ss;
		_ss << "\n\n++++++++++++++" << endl;
//		_ss << otg.seq_str() << endl;
		_ss << "otg at " << ds.timestamp() << endl;
//		_ss << "[" << otg_enum << "]" << endl;
		_ss << "cnl" << endl;
		_ss << "[" << cnl_enum << "]" << endl;
//		if(otg_enum != cnl_enum){
//			util::log(_ss);
//			cout << "diff" << endl;
//		}
	}

	cout << "end run_canmicrosoft" << endl;
}

int canonical::update(int _ins){
	this->t_remove.begin();
	this->remove();
	this->t_remove.end();

	citem* cit = new citem(_ins, this->timestamp, this->win_size);

	this->t_insert.begin();
	this->insert(cit);
	this->t_insert.end();

	this->timestamp ++;
	return 0;
}
int canonical::construction(vector<Vtype>& ivec)
{
	for(int i = 0; i < ivec.size(); i ++)
	{
		citem* cit = new citem(ivec[i], this->timestamp, this->win_size);

		this->t_insert.begin();
		this->insert(cit);
		this->t_insert.end();

		this->timestamp ++;
	}

	return 0;
}
string canonical::to_str(){
	stringstream _ss;
	_ss << "ts=" << this->timestamp << endl;
	for(int i = 0; i < this->lis_len; i ++)
	{
		citem* it = this->hlist[i];
		_ss << "hlist" << i+1 << ":" << endl;
		while(it != NULL)
		{
			_ss << "\t" << it->val;
			it = it->next;
		}
		_ss << endl;

//		it = this->hlist[i];
//		_ss << "hlist" << i+1 << ":" << endl;
//		while(it != NULL)
//		{
//			_ss << "\t" << it->to_str() << "\n";
//			it = it->next;
//		}
//		_ss << endl;
	}
	return _ss.str();
}
int canonical::to_size(){
	int sz = 0;
//	for(int i = 0; i < this->lis_len; i ++)
//	{
//		citem* it = this->hlist[i];
//		while(it != NULL)
//		{
//			sz += it->to_size();
//			it = it->next;
//		}
//	}
	citem* tmp = new citem(0, 0, this->win_size);
	sz += this->win_size * (tmp->to_size());
	delete tmp;

	sz += sizeof(citem*) * this->win_size;
	sz += sizeof(citem*) * this->win_size;
	sz += sizeof(citem*) * this->win_size;
	sz += sizeof(int) * this->win_size;

	sz += sizeof(canonical);

	return sz;
}
string canonical::seq_str(){
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
string canonical::enum_str(){
	stringstream _ss;
	citem** S = new citem*[this->lis_len];
	citem* cit = this->hlist[this->lis_len-1];
	int lis_num = 0;
	while(cit != NULL){
		cit->enumlis(this->lis_len, 0, S, _ss, lis_num);
#ifdef ENABLE_MAX_LIS_NUM
		if(lis_num > util::MAX_LIS_NUM)
			break;
#endif

		cit = cit->next;
	}

	delete[] S;
	return _ss.str();
}

string canonical::minh_str(){
	stringstream _ss;
	citem** S = new citem*[this->lis_len];
	citem** Store = new citem*[this->lis_len];
	citem* cit = this->hlist[this->lis_len-1];
	Vtype _val = 1 << 30;
	int lis_num = 0;
	while(cit != NULL){
		cit->enumlis_constrained(this->lis_len, 0, S, _ss, Store, canonical::mingap, _val, lis_num);
		cit = cit->next;
	}

	delete[] S;
	delete[] Store;
	return _ss.str();
}
string canonical::maxh_str(){
	stringstream _ss;
	citem** S = new citem*[this->lis_len];
	citem** Store = new citem*[this->lis_len];
	citem* cit = this->hlist[this->lis_len-1];
	Vtype _val = -1;
	int lis_num = 0;
	while(cit != NULL){
		cit->enumlis_constrained(this->lis_len, 0, S, _ss, Store, canonical::maxgap, _val, lis_num);
		cit = cit->next;
	}

	delete[] S;
	delete[] Store;
	return _ss.str();
}
string canonical::minw_str(){
	stringstream _ss;
	citem** S = new citem*[this->lis_len];
	citem** Store = new citem*[this->lis_len];
	citem* cit = this->hlist[this->lis_len-1];
	Vtype _val = 2 << 30;
	int lis_num = 0;
	while(cit != NULL){
		cit->enumlis_constrained(this->lis_len, 0, S, _ss, Store, canonical::minwei, _val, lis_num);
		cit = cit->next;
	}

	delete[] S;
	delete[] Store;
	return _ss.str();
}
string canonical::maxw_str(){
	stringstream _ss;
	citem** S = new citem*[this->lis_len];
	citem** Store = new citem*[this->lis_len];
	citem* cit = this->hlist[this->lis_len-1];
	Vtype _val = -1;
	int lis_num = 0;
	while(cit != NULL){
		cit->enumlis_constrained(this->lis_len, 0, S, _ss, Store, canonical::maxwei, _val, lis_num);
		cit = cit->next;
	}

	delete[] S;
	delete[] Store;
	return _ss.str();
}
string canonical::minwid_str(){
	stringstream _ss;
	citem** S = new citem*[this->lis_len];
	citem** Store = new citem*[this->lis_len];
	citem* cit = this->hlist[this->lis_len-1];
	Vtype _val = 2 << 30;
	int lis_num = 0;
	while(cit != NULL){
		cit->enumlis_constrained(this->lis_len, 0, S, _ss, Store, canonical::minwid, _val, lis_num);
		cit = cit->next;
	}

	delete[] S;
	delete[] Store;
	return _ss.str();
}
string canonical::maxwid_str(){
	stringstream _ss;
	citem** S = new citem*[this->lis_len];
	citem** Store = new citem*[this->lis_len];
	citem* cit = this->hlist[this->lis_len-1];
	Vtype _val = -1;
	int lis_num = 0;
	while(cit != NULL){
		cit->enumlis_constrained(this->lis_len, 0, S, _ss, Store, canonical::maxwid, _val, lis_num);
		cit = cit->next;
	}

	delete[] S;
	delete[] Store;
	return _ss.str();
}

//private:
int canonical::insert(citem* it){
	int i = 0;
	while(hlist[i] != NULL && htail[i]->val <= it->val)
	{
		it->insert_pred(htail[i]);
		htail[i]->insert_succ(it);
		i ++;
	}
	/* horizontal */
	if(hlist[i] == NULL)
	{
		hlist[i] = it;
		htail[i] = it;
		this->lis_len ++;
	}
	else
	{
		htail[i]->next = it;
		it->prev = htail[i];
		htail[i] = it;
	}

	it->rlen = i+1;

	this->buf[this->buf_t] = it->val;
	this->buf_t = (this->buf_t + 1) % this->win_size;

	return 0;
}
/*
 * recalculate pmax
 * update inD for each
 * update pred
 * update succ
 * */
int canonical::remove(){
	if(this->timestamp < this->win_size)
	{
		return -1;
	}

	this->buf_h = (this->buf_h + 1) % this->win_size;
	citem* _del = this->hlist[0];
	this->pmax_update();
//	if(this->timestamp >= 14)
//	{
//		cout << "hello" << endl;
//		cout << "hello" << endl;
//	}
	this->horizontal();

	this->pred_update();

	delete _del;

	return 0;
}
int canonical::pmax_update(){

	for(int i = 0; i < this->win_size; i ++){
		this->pmax[i] = NULL;
	}

	for(int i = 0; i < this->lis_len; i ++)
	{
		citem* it = this->hlist[i];
		while(it != NULL){
			it->inD = false;
			it = it->next;
		}
	}
	this->hlist[0]->inD = true;
	this->pmax[0] = this->hlist[0];
//	cout << "pmax" << 0 << "=" << this->pmax[0]->val << endl;
	for(int i = 1; i < this->lis_len; i ++)
	{
		if(this->pmax[i-1]->timestamp == this->htail[i-1]->timestamp)
		{
			this->pmax[i] = this->htail[i];
//			cout << "pmax" << i << "=" << this->htail[i]->val << endl;
		}
		else
		{
			citem* tmp_p = this->pmax[i-1]->next;// Won't be NULL
			if(this->hlist[i]->timestamp > tmp_p->timestamp){
				this->pmax[i] = NULL;
//				cout << "pmax" << i << " = inull" << endl;
				break;
			}
			else
			{
				citem* p = this->hlist[i];
				while(p->timestamp < tmp_p->timestamp){
					this->pmax[i] = p;
					p = p->next;
					if(p == NULL) break;
				}
//				cout << "pmax" << i << "=" << this->pmax[i]->val << endl;
			}
		}

		citem* it = this->hlist[i];
		while(it != NULL)
		{
			if(it->timestamp <= this->pmax[i]->timestamp)
			{
				it->inD = true;
				it = it->next;
			}
			else
			{
				break;
			}
		}
	}/* end for */


	return 0;
}
int canonical::horizontal(){
	citem** tmp_left = new citem*[this->win_size];
	citem** tmp_right = new citem*[this->win_size];
	for(int i = 0; i < this->win_size; i ++)
	{
		tmp_left[i] = tmp_right[i] = NULL;
	}

//	cout << "time:" << this->timestamp+1 << endl;
	for(int i = 0; i < this->lis_len; i ++)
	{
		if(this->pmax[i] != NULL)
		{
			tmp_left[i] = this->hlist[i];
			tmp_right[i] = this->pmax[i]->next;
//			cout << "hpmax=" << this->pmax[i]->val << "\t";
		}
		else
		{
			tmp_left[i] = NULL;
			tmp_right[i] = this->hlist[i];
//			cout << "hpmax=NULL" << "\t";
		}
//
//		int l = -1, r = -1;
//		if(tmp_left[i] != NULL){
//			l = tmp_left[i]->val;
//		}
//		if(tmp_right[i] != NULL){
//			r = tmp_right[i]->val;
//		}
//		cout << "(" << l << ", " << r << ")" << endl;
	}

	for(int i = 0; i < this->lis_len-1; i ++)
	{
		citem* newleft = tmp_left[i+1];
		citem* newright = tmp_right[i];
		if(newleft == NULL){
			this->hlist[i] = newright;
			this->hlist[i]->prev = NULL;
		}
		else
		if(newright == NULL)
		{
			this->hlist[i] = this->hlist[i+1];
		}
		else
		{
			this->hlist[i] = this->hlist[i+1];
			this->pmax[i+1]->next = newright;
			newright->prev = this->pmax[i+1];
		}
	}
	/* for last right part of the last hlist */
	if(tmp_right[this->lis_len-1] != NULL)
	{
		this->hlist[this->lis_len-1] = tmp_right[this->lis_len-1];
		this->hlist[this->lis_len-1]->prev = NULL;
	}
	else
	{
		this->hlist[this->lis_len-1] = NULL;
		this->lis_len --;
	}

	for(int i = 0; i < this->win_size; i ++)
	{
		if(i < this->lis_len)
		{
			citem* it = this->hlist[i];
			while(it != NULL)
			{
				it ->rlen = i+1;
				if(it->next == NULL)
				{
					this->htail[i] = it;
				}
				it = it->next;
			}
		}
		else
		{
			this->hlist[i] = NULL;
		}
	}

	delete[] tmp_left;
	delete[] tmp_right;

	return 0;
}
int canonical::pred_update(){
	citem** tmp_tail = new citem*[this->win_size];
	citem** tmp_index = new citem*[this->win_size];
	for(int i = 0; i < this->win_size; i ++)
	{
		tmp_index[i] = this->hlist[i];
		tmp_tail[i] = NULL;
	}

	int _i = 0;
	while(_i < this->win_size-1)
	{
		citem * _next = NULL;
		int _i_record;
		for(int i = 0; i < this->lis_len; i ++)
		{
			if(tmp_index[i] == NULL)
			{
				continue;
			}

			if(_next == NULL)
			{
				_next = tmp_index[i];
				_i_record = i;
				continue;
			}
			else
			{
				if(_next->timestamp > tmp_index[i]->timestamp)
				{
					_next = tmp_index[i];
					_i_record = i;
				}
			}
		}
//		if(_next == NULL)
//		{
//			cout << "hello" << endl;
//			cout << "hello" << endl;
//		}

		_next->clear_pred_succ();
		tmp_index[_i_record] = tmp_index[_i_record]->next;

		int r_len = _next->rlen;
		tmp_tail[r_len-1] = _next;
		for(int i = 0; i < r_len-1; i ++){
			tmp_tail[i]->insert_succ(_next);
			_next->insert_pred(tmp_tail[i]);
		}

		_i ++;
	}

	delete[] tmp_tail;
	delete[] tmp_index;

	return 0;
}
int canonical::succ_update(citem* _del){
	for(int i = 0; i < this->lis_len; i ++)
	{
		citem* it = this->hlist[0];
		while(it != NULL){


			it = it->next;
		}
	}
	return 0;
}

long long int canonical::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}
long long int canonical::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

void canonical::log_running(){
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
string canonical::sum_running(){
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

