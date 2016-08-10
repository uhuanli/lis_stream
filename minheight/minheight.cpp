/*
 * minheight.cpp
 *
 *  Created on: 2015Äê9ÔÂ22ÈÕ
 *      Author: liyouhuan
 */
#include "minheight.h"
#include "../util/util.h"
#include "../datastream/datastream.h"
#include "../quadruple/quadruple.h"
//public:
minheight::minheight(int _winsz){
	this->win_size = _winsz;
	this->timestamp = 0;
	this->htail = new mh_item*[_winsz];
	this->lis_len = 0;
	this->buf = new int[_winsz];
	this->buf_h = 0;
	this->buf_t = 0;
	this->pool_used = 0;
	this->item_pool = new mh_item*[_winsz];
	for(int i = 0; i < this->win_size; i ++){
		this->item_pool[i] = NULL;
	}
}
minheight::~minheight(){
	delete[] this->htail;
	delete[] this->item_pool;
	delete[] this->buf;
}
void minheight::run(string _data_f, const bool _update_vertical){
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
		if(_update_vertical){
			_ss << "[minH_v]";
		}else{
			_ss << "[minH]";
		}
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		if(_update_vertical){
			label_type << "minH_v\t" << "minheight_v";
//			cout << "is v" << endl;
//			system("pause");
		}else{
			label_type << "minH\t" << "minheight";
		}
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
		if(_update_vertical)
		{
			this->update_vertical(ai);
		}
		else
		{
			this->update(ai);
		}
	}

	while(ds.hasnext())
	{
		this->t_total.begin();
		this->t_update.begin();
		int ai = ds.next();
		if(_update_vertical)
		{
			this->update_vertical(ai);
		}
		else
		{
			this->update(ai);
		}

		this->t_update.end();

		this->t_compute.begin();
		util::log(this->minheight_str().c_str(), "");
		this->t_compute.end();

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
}
void minheight::run_mhdebug(){
	cout << "begin run_mhdebug" << endl;

	util::initial();
	string debug = "debug.dat";
	datastream ds(debug);
//	orthogonal otg(winsize);
	while(ds.hasnext())
	{
		int ai = ds.next();
		{
			stringstream _ss;
			_ss << "ai = " << ai << endl;
			util::log(_ss);
		}
		this->update(ai);
//		otg.update(ai);
		cout << "\n\n++++++++++++++" << endl;
		cout << this->seq_str() << endl;
		cout << "mh_str:\n" << this->to_str() << endl;
		cout << "mh: " << this->minheight_str() << endl;
//		cout << "ot: " << otg.mingap_str() << endl;
	}

	cout << "end run_mhdebug" << endl;
}
void minheight::run_mhstream(){

}
void minheight::run_mhmicrosoft(int _winsz){
	cout << "begin run_mhmicrosoft" << endl;

	string microsoft = "microsoft_stock.dat";
	datastream ds(microsoft);
//	minheight mh(winsize);
	orthogonal otg(_winsz);
	while(ds.hasnext())
	{
		double d_next = ds.next();
		int ai = ((int)(d_next));
		{
			stringstream _ss;
			_ss << "ai = " << ai << "\t" << d_next << endl;
			util::log(_ss);
		}
		this->update_vertical(ai);
		otg.update(ai);
		stringstream _ss;
		string mh_lis = this->minheight_str();
		string otg_lis;
		{
			stringstream _tmp_ss;
			otg.mingap_one_str(_tmp_ss);
			otg_lis = _tmp_ss.str();
		}
		_ss << "\n\n++++++++++++++" << endl;
		_ss << this->seq_str() << endl;
		_ss << "mh_str:\n" << this->to_str() << endl;
		_ss << "mh: [" << mh_lis << "]" << endl;
		_ss << "ot: [" << otg_lis << "]" << endl;

		if(mh_lis != otg_lis){
			cout << "different at " << this->timestamp << endl;
			_ss << "different at " << this->timestamp << endl;
			util::log(_ss);
		}
	}

	cout << "end run_mhmicrosoft" << endl;
}
string minheight::to_str(){
	stringstream _ss;
	_ss << " time: " << this->timestamp << endl;
	for(int i = 0; i < this->lis_len; i ++){
		mh_item* it = this->htail[i];
		while(it != NULL)
		{
			_ss << "\t" << it->to_str();
			it = it->next;
		}
		_ss << endl;
	}
	_ss << endl;
	return _ss.str();
}

int minheight::to_size(){
	int sz = 0;
	sz += sizeof(minheight);

	sz += sizeof(mh_item*) * this->win_size;
	sz += sizeof(mh_item*) * this->win_size;
	sz += sizeof(int) * this->win_size;

	mh_item* mit = new mh_item();
	sz += mit->to_size() * this->win_size;
	delete mit;

	return sz;
}
string minheight::seq_str(){
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
string minheight::minheight_str(){
	stringstream _ss;
	if(this->lis_len == 1){
		_ss << this->htail[0]->val;
		return _ss.str();
	}

	{/* compute p_most */
		mh_item* it = this->htail[0];
		while(it!= NULL){
			it->p_most = it;
			it->pred = NULL;
			it = it->next;
		}

		for(int i = 1; i < this->lis_len; i ++)
		{
			it = this->htail[i];
			while(it != NULL)
			{
				mh_item* tmp_x = it->un;
				while(tmp_x != NULL)
				{
					if(tmp_x->val <= it->val){
						it->pred = tmp_x;
						it->p_most = tmp_x->p_most;
					}
					tmp_x = tmp_x->next;
				}
				it = it->next;
			}

		}
	}

	mh_item* it = this->htail[this->lis_len-1];

	mh_item* min_h_it = it;
	int _min_h = it->val - it->p_most->val;
	it = it->next;
	while(it != NULL){
		if(it->val - it->p_most->val <= _min_h)
		{
			_min_h = it->val - it->p_most->val;
			min_h_it = it;
		}
		it = it->next;
	}

	it = min_h_it;

	mh_item** S = new mh_item*[this->lis_len];
	int _tmp_i = 0;
	while(it != NULL){
		S[_tmp_i] = it;
		it = it->pred;
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
int minheight::update(int _ins){
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
int minheight::update_vertical(int _ins)
{
	this->remove();
	this->insert(_ins);

	this->timestamp ++;

	return 0;
}
int minheight::construction(vector<int>& ivec){
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
int minheight::construct(){
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		mh_item* it = new mh_item();
		this->new_item(it);
		it->val = this->get_buf(i);
		int itail = this->find_ins_pos(it->val);

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
			mh_item* tmp = this->htail[itail-1];
			while(tmp != NULL)
			{
				if(tmp->val <= it->val){
					 it->pred = tmp;
					 it->p_most = tmp->p_most;
				}
				tmp = tmp->next;
			}
		}
		else
		{
			it->p_most = it;
		}
	}

	return 0;
}

int minheight::insert(int _ins)
{
	mh_item* ins = new mh_item(_ins, this->timestamp);
	int itail = this->find_ins_pos(ins->val);
	if(itail == this->lis_len){
		this->htail[itail] = ins;
		this->lis_len ++;
	}else{
		this->htail[itail]->rn = ins;
		ins->next = this->htail[itail];
		this->htail[itail] = ins;
	}
	/* un */
	if (itail >= 1)
	{
		ins->un = this->htail[itail - 1];
	}

	/* dn */
	if(itail < this->lis_len-1)
	{
		ins->dn = this->htail[itail+1];
	}

	this->buf[this->buf_t] = _ins;
	this->buf_t ++;
	this->buf_h %= this->win_size;
	this->buf_t %= this->win_size;

	return 0;
}
/*next & rn*/
int minheight::h_adjust()
{
	mh_item** hlist = new mh_item*[this->win_size];
	{
		for(int i = 0; i < this->lis_len; i ++)
		{
			hlist[i] = this->htail[i];
			while(hlist[i]->next != NULL)
			{
				hlist[i] = hlist[i]->next;
			}
		}
		for(int i = this->lis_len; i < this->win_size; i ++)
		{
			hlist[i] = NULL;
		}
	}
	mh_item * tmp_head = hlist[0];

	int Li = 0;
	mh_item * _ax_tmp = tmp_head;
	/* Divide */
	mh_item** left_tail = new mh_item*[this->win_size];
	mh_item** right_head = new mh_item*[this->win_size];
	while(true)
	{
		if(_ax_tmp->rn == NULL)
		{
			for(int i = Li; i < this->lis_len; i ++)
			{
				left_tail[i] = this->htail[i];
				right_head[i] = NULL;
			}
			break;
		}
		else
		{
			left_tail[Li] = _ax_tmp;
			right_head[Li] = _ax_tmp->rn;
			mh_item* _ay_tmp = (_ax_tmp->rn)->dn;
			if(_ay_tmp == NULL)
			{
				for(int i = Li+1; i < this->lis_len; i ++)
				{
					left_tail[i] = NULL;
					right_head[i] = this->htail[i];
					while(right_head[i]->next != NULL)
						right_head[i] = right_head[i]->next;
				}
				break;
			}
			else
			{
				_ax_tmp = _ay_tmp;
				Li ++;
			}
		}
	}
	/* join */
	for(int i = 0; i < this->lis_len-1; i ++)
	{
		if(left_tail[i+1] == NULL)
		{
			hlist[i] = right_head[i];
			hlist[i]->next = NULL;
		}
		else
		if(right_head[i] == NULL)
		{
			hlist[i] = hlist[i+1];
		}
		else
		{
			hlist[i] = hlist[i+1];
			left_tail[i+1]->rn = right_head[i];
			right_head[i]->next = left_tail[i+1];
		}
	}
	/*  */
	if(right_head[this->lis_len-1] != NULL)
	{
		hlist[this->lis_len-1] = right_head[this->lis_len-1];
		hlist[this->lis_len-1]->next = NULL;
	}
	else
	{
		hlist[this->lis_len-1] = NULL;
		this->lis_len --;
	}

	/*update htail*/
	for(int i = 0; i < this->win_size; i ++)
	{
		if(hlist[i] == NULL){
			this->htail[i] = NULL;
			continue;
		}

		this->htail[i] = hlist[i];
		mh_item* tmp = this->htail[i];
		while(this->htail[i]->rn != NULL)
		{
			this->htail[i] = this->htail[i]->rn;
			tmp = this->htail[i];
		}
	}

	delete[] hlist;
	delete tmp_head;

	return 0;
}
int minheight::v_adjust()
{
	/* update the un of hlist[0] */
	mh_item* _it_tmp = this->htail[0];
	while(_it_tmp != NULL){
		_it_tmp->un = NULL;
		_it_tmp = _it_tmp->next;
	}

	/* up neighbor */
	for(int i = 1; i < this->lis_len; i ++)
	{
		mh_item* _ah_tmp = this->htail[i-1];
		while(_ah_tmp->next != NULL){
			_ah_tmp = _ah_tmp->next;
		}
		mh_item* _ax_tmp = this->htail[i];
		while(_ax_tmp->next != NULL){
			_ax_tmp = _ax_tmp->next;
		}

		while(_ah_tmp != NULL && _ax_tmp != NULL)
		{
			if(_ax_tmp->timestamp > _ah_tmp->timestamp)//if(_ax_tmp->after(_ah_tmp))
			{
				if(_ah_tmp->rn == NULL)
				{
					_ax_tmp->un = _ah_tmp;
					_ax_tmp = _ax_tmp->rn;
				}
				else
				if(_ax_tmp->timestamp > (_ah_tmp->rn)->timestamp)//if(_ax_tmp->after(_ah_tmp->rn))
				{
					_ah_tmp = _ah_tmp->rn;
				}
				else
				{
					_ax_tmp->un = _ah_tmp;
					_ax_tmp = _ax_tmp->rn;
				}
			}
			else
			{
				cout << "err _ah_tmp _ax_tmp at " << this->timestamp << endl;
				cout << endl;
				system("pause");
			}
		}
	}

	/* down neighbor */
	for(int i = 0; i < this->lis_len-1; i ++)
	{
		mh_item* _ah_tmp = this->htail[i+1];
		while(_ah_tmp->next != NULL){
			_ah_tmp = _ah_tmp->next;
		}
		mh_item* _ax_tmp = this->htail[i];
		while(_ax_tmp->next != NULL){
			_ax_tmp = _ax_tmp->next;
		}

		while(_ah_tmp != NULL && _ax_tmp != NULL)
		{
			if(_ax_tmp->timestamp > _ah_tmp->timestamp)//if(_ax_tmp->after(_ah_tmp))
			{
				if(_ah_tmp->rn == NULL)
				{
					_ax_tmp->dn = _ah_tmp;
					_ax_tmp = _ax_tmp->rn;
				}
				else
				if(_ax_tmp->timestamp > (_ah_tmp->rn)->timestamp)//if(_ax_tmp->after(_ah_tmp->rn))
				{
					_ah_tmp = _ah_tmp->rn;
				}
				else
				{
					_ax_tmp->dn = _ah_tmp;
					_ax_tmp = _ax_tmp->rn;
				}
			}
			else
			{
				_ax_tmp->dn = NULL;
				_ax_tmp = _ax_tmp->rn;
			}
		}
	}
	/* update dn of the last hlist */
	_it_tmp = this->htail[this->lis_len-1];
	while(_it_tmp != NULL){
		_it_tmp->dn = NULL;
		_it_tmp = _it_tmp->next;
	}

	return 0;
}
int minheight::remove()
{
	if(this->timestamp < this->win_size)
	{
		return -1;
	}
	{/* debug */
		this->buf_h = (this->buf_h + 1) % this->win_size;
	}

	this->h_adjust();
	this->v_adjust();

	{/* removepool*/

	}
	return 0;
}
int minheight::build_pmost()
{

	return 0;
}
int minheight::find_ins_pos(int _val){
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
//private:
void minheight::new_item(mh_item* _it){
	this->item_pool[this->pool_used] = _it;
	this->pool_used ++;
}
int minheight::get_buf(int _i){
	return this->buf[(this->buf_h+_i) % this->win_size];
}

long long int minheight::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}
long long int minheight::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

void minheight::log_running(){
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
string minheight::sum_running(){
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
