/*
 * lisw.cpp
 *
 *  Created on: 2016Äê7ÔÂ26ÈÕ
 *      Author: liyouhuan
 */
#include "lisw.h"
#include "../util/util.h"
#include "../datastream/datastream.h"
#include "../quadruple/quadruple.h"

lisw::lisw(int _winsz){
	this->win_size = _winsz;
	this->timestamp = 0;
	this->principle_row = new lw_item*[_winsz];
	for(int i = 0; i < _winsz; i ++){
		this->principle_row[i] = NULL;
	}
	this->lis_len = 0;

	this->item_pool = new lw_item*[_winsz];
	for(int i = 0; i < _winsz; i ++){
		this->item_pool[i] = NULL;
	}
	pool_head = 0;
	pool_tail = 0;

	this->buf = new Vtype[this->win_size];
	for(int i = 0; i < this->win_size; i ++){
		this->buf[i] = 0;
	}

	this->buf_h = 0;
	this->buf_t = 0;
}
lisw::~lisw(){
	for(int i = 0; i < this->win_size; i ++)
	{
		delete this->principle_row[i];
	}

	delete[] this->principle_row;
	delete[] this->item_pool;
}

void lisw::run(string _data_f, int _method){
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
		_ss << "[lisw]";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		label_type << "lisw\t" << "LISlength";
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
		Vtype ai = ds.next();
		this->update(ai);
	}

	while(ds.hasnext())
	{
		this->t_total.begin();
		this->t_update.begin();
		Vtype ai = ds.next();
		this->update(ai);

		this->t_update.end();
		this->t_compute.begin();
		util::log(this->lis_str().c_str(), "");
		this->t_compute.end();

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
}
void lisw::run_lwdebug(){

}
void lisw::run_lwstream(){

}
void lisw::run_lwmicrosoft(){
	cout << "begin run_lwmicrosoft" << endl;
	int winsize = 10;
	string file = "microsoft_stock.dat";
	datastream ds(file);
	qnlist otg(winsize);
	while(ds.hasnext())
	{
		int ai = ds.next();
		otg.update(ai);
		this->update(ai);

		string otg_lis = "";
		{
			stringstream _tmp_ss;
			otg.minweight_str(_tmp_ss);
			otg_lis = _tmp_ss.str();
		}

		int lw_len = this->LISlength();
		int otg_len = otg.LISlength();
		string lw_lis = this->lis_str();
		stringstream _ss;
		_ss << "\n\n++++++++++++++" << endl;
		_ss << otg.seq_str() << endl;
		_ss << "Time at " << ds.timestamp() << endl;
		_ss << "otg=[" << otg_lis << "]" << endl;
		_ss << "lw=[" << lw_lis << "]" << endl;
		if(lw_len != otg_len){
			cout << "diff" << endl;
		}

		if(lw_lis != otg_lis)
		{
			cout << "diff lis" << endl;
			util::log(_ss);
			util::log(this->to_str().c_str(), "\npool\n");
			util::log(this->pool_str().c_str(), "\n\nlis\n\n");
			util::log(this->lis_str().c_str(), "\n");
		}
	}

	cout << "end run_dpmicrosoft" << endl;
}

int lisw::LISlength() const{
	return this->lis_len;
}
int lisw::to_size() const{
	int sz = sizeof(lisw);
	lw_item* tmp = new lw_item(-1,-1,-1,-1,this->win_size);
	sz += this->win_size*(tmp->to_size());
	sz += this->win_size*(tmp->to_size());
	sz += this->win_size*(sizeof(int*));
	sz += this->win_size*(sizeof(int*));
	sz += this->win_size*(sizeof(int*));
	return sz;
}
int lisw::update(Vtype _ins){
	if(this->timestamp >= this->win_size)
	{
		this->buf_h ++;
	}
	this->buf[this->buf_t] = _ins;
	this->buf_t ++;
	this->buf_h %= this->win_size;
	this->buf_t %= this->win_size;

	this->remove();
	int ins_pos = this->insert(_ins);
	this->build_parents(ins_pos);
	{
		stringstream _ss;
		_ss << "\tparent for " << _ins << ": "
				<< this->principle_row[ins_pos]->parent_str() << endl;
		util::log(_ss);
	}


	this->timestamp ++;

	return 0;
}
int lisw::construction(vector<Vtype>& ivec){
	for(int i = 0; i < ivec.size(); i ++)
	{
		this->insert(ivec[i]);
	}
	return 0;
}

string lisw::to_str() const{
	stringstream _ss;
	_ss << "lisLen=" << this->lis_len << endl;
	for(int i = 0; i < this->lis_len; i ++)
	{
		_ss << i << ":" << this->principle_row[i]->to_str() << endl;
	}
	return _ss.str();
}
string lisw::lis_str() const{
//	cout << "\n\n+++++IN lis_str at " << this->timestamp << endl;

	stringstream _ss;
//	cout << "cur_pos=" << (this->lis_len-1) << endl;
	lw_item* cur = this->principle_row[this->lis_len-1];
	lw_item** tmp_lis = new lw_item*[this->lis_len];
	int i = this->lis_len;
	lw_item* prev = cur;
	int pred;
	while(i > 0)
	{
		if(cur == NULL)
		{
			cout << "is NULL !!!!!" << endl;
			cout << "t=" << this->timestamp << endl;
			cout << "i=" << i << endl;
			cout << "lis=" << this->lis_len << endl;
			cout << "str: " << _ss.str() << endl;
			cout << "prev: " << prev->parent_str() << endl;
			cout << "predecessor=" << pred << endl;
			system("pause");
			exit(0);
		}

		tmp_lis[i-1] = cur;

		if(this->timestamp <= this->win_size)
		{
			pred = this->win_size-1;
		}
		else
		if(this->timestamp > this->win_size)
		{
			int min_t = this->timestamp-this->win_size;
			if(cur->timestamp < this->win_size)
			{
				pred = this->win_size-1-(min_t);
			}
			else
			{
				pred = this->win_size-(this->timestamp-cur->timestamp);
			}
		}

		prev = cur;
		cur = cur->parents[pred];
		i --;
	}
//	_ss << "]";
	for(i = 0; i < this->lis_len; i ++)
	{
		if(i == 0){
			_ss << tmp_lis[i]->val;
		}else{
			_ss << "\t" << tmp_lis[i]->val;
		}
	}
	delete[] tmp_lis;
//	cout << "+++++OUT lis_str at " << this->timestamp << endl;
	return _ss.str();
}
string lisw::seq_str(){
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
string lisw::pool_str() const
{
	stringstream _ss;

	_ss << "pool_head=" << this->pool_head << endl;
	_ss << "pool_tail=" << this->pool_tail << endl;

	for(int i = 0; i < this->win_size; i ++)
	{
		if(this->get_item(i) == NULL) continue;
		_ss << i << ":" << this->get_item(i)->to_str() << endl;
	}
	return _ss.str();
}

/* private */
int lisw::insert(Vtype _new_v){
	{
		stringstream _ss;
		_ss << "\n\n+++++IN insert at " << this->timestamp << endl;
		util::log(_ss);
	}

	int ins_pos = this->find_ins_pos(_new_v);
	vector<int> dump_pos;
	dump_pos.clear();
	int cur_d, cur_pos;
	if(ins_pos == this->lis_len)
	{
		this->lis_len ++;
		int new_d = this->win_size;
		if(new_d > this->timestamp){
			new_d = this->timestamp + 1;
		}

//		cout << "insert_pos=" << ins_pos << endl;
		this->principle_row[ins_pos]
		 = new lw_item(_new_v, this->timestamp, new_d, this->lis_len, this->win_size);

//		cout << "lis_len=" << this->lis_len << endl;

//		cout << "val=" << this->principle_row[ins_pos]->val << endl;

		this->addpool(this->principle_row[ins_pos]);
	}
	else
	if(ins_pos < this->lis_len)
	{
		cur_d = this->principle_row[ins_pos]->drop_out_d;
		int decrease_d = cur_d;
		cur_pos = ins_pos;
		dump_pos.push_back(ins_pos);
		for(int i = ins_pos+1; i < this->lis_len; i ++)
		{
			if(this->principle_row[i]->drop_out_d > cur_d)
			{
				cur_d = this->principle_row[i]->drop_out_d;
				dump_pos.push_back(i);
			}
		}
		for(int i = dump_pos.size()-1; i > 0; i --)
		{
			this->principle_row[dump_pos[i]]->drop_out_d
			 	 = this->principle_row[dump_pos[i-1]]->drop_out_d;
			this->principle_row[dump_pos[i]]->drop_out_alpha
			 	 = this->principle_row[dump_pos[i-1]]->drop_out_alpha;
		}
		// adjust drop_out_d larger than cur_d except the new inserted one
		for(int i = 0; i < this->lis_len; i ++)
		{
			if(this->principle_row[i]->drop_out_d > cur_d && i != ins_pos)
			{
				this->principle_row[i]->drop_out_alpha --;
			}
		}

		int new_d = this->win_size;
		if(new_d > this->timestamp){
			new_d = this->timestamp + 1;
		}

//		delete this->principle_row[dump_pos[0]];
//		cout << "insert_pos=" << ins_pos << endl;

		this->principle_row[dump_pos[0]]
		 = new lw_item(_new_v, this->timestamp, new_d, this->lis_len, this->win_size);

//		cout << "lis_len=" << this->lis_len << endl;

		this->addpool(this->principle_row[dump_pos[0]]);
	}

	{
		stringstream _ss;
		_ss << "+++++OUT insert at " << this->timestamp << endl;
		util::log(_ss);
	}

	return ins_pos;
}

bool lisw::build_parents(int _ins_pos)
{
	lw_item* new_item = this->principle_row[_ins_pos];
	if(_ins_pos == 0)
	{
		for(int i = 0; i < this->win_size; i ++)
		{
			new_item->parents[i] = NULL;
		}
	}
	else
	{
		int cur_column = _ins_pos-1;
		int prev_d = 0;
		int parent_pos = this->win_size-1;
		for(int i = _ins_pos-1; i >= 0; i --)
		{
			lw_item* cur_item = this->principle_row[i];
			if(cur_item->drop_out_d < prev_d)
				continue;
			int num_pos = cur_item->drop_out_d - prev_d;
			while(num_pos > 0)
			{
				new_item->parents[parent_pos] = cur_item;
				parent_pos --;
				num_pos --;
			}

			prev_d = cur_item->drop_out_d;
		}

		while(parent_pos >=0)
		{
			new_item->parents[parent_pos] = NULL;
			parent_pos --;
		}

	}
	return false;
}
/*
 * decrease each in d by 1
 * delete expiry 0
 * if delete happen, delete ele 0 in alpha
 * decrease each in alpha by 1
 */
bool lisw::remove(){
	if(this->timestamp < this->win_size)
	{
		return false;
	}

	int origin_lis_len = this->lis_len;
	this->decline_d();
	if(origin_lis_len != this->lis_len)
	{
		this->decline_alpha();
	}

	this->removepool();

	return true;
}

int lisw::decline_d(){
	int i_zero = -1;
	for(int i = 0; i < this->lis_len; i ++)
	{
		this->principle_row[i]->drop_out_d --;
		if(this->principle_row[i]->drop_out_d == 0)
		{
			i_zero = i;
		}
	}
	if(i_zero == -1) return false;

//	delete this->principle_row[i_zero];
	for(int i = i_zero; i < this->lis_len-1; i ++)
	{
		this->principle_row[i] = this->principle_row[i+1];
	}
	this->principle_row[this->lis_len-1] = NULL;
	this->lis_len --;
}

bool lisw::decline_alpha(){
	for(int i = 0; i < this->lis_len; i ++)
	{
		this->principle_row[i]->drop_out_alpha --;
	}
	return true;
}

int lisw::find_ins_pos(Vtype _val){
	for(int i = 0; i < this->lis_len; i ++)
	{
		if(this->principle_row[i]->val > _val) return i;
	}

	return this->lis_len;
}

int lisw::construct(){

	return 0;
}

Vtype lisw::get_buf(int _i){
	return this->buf[(this->buf_h+_i) % this->win_size];
}

lw_item* lisw::get_item(int _i) const
{
	return this->item_pool[(this->pool_head+_i) % this->win_size];
}

bool lisw::addpool(lw_item* _lw_i)
{
	this->item_pool[this->pool_tail] = _lw_i;
	this->pool_tail ++;
	this->pool_tail %= this->win_size;

	return true;
}
bool lisw::removepool()
{
	if(this->timestamp < this->win_size)
	{
		return false;
	}

	if(this->timestamp >= 16){
//		cout << this->pool_str() << endl;
//		cout.flush();
	}

	delete this->item_pool[this->pool_head];

	this->item_pool[this->pool_head] = NULL;
	this->pool_head ++;
	this->pool_head %= this->win_size;

	return true;
}

void lisw::log_running(){
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

string lisw::sum_running(){
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

long long int lisw::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

long long int lisw::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

