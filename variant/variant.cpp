/*
 * variant.cpp
 *
 *  Created on: 2015Äê9ÔÂ27ÈÕ
 *      Author: liyouhuan
 */
#include "variant.h"
#include "../datastream/datastream.h"
#include "../quadruple/quadruple.h"
//public:
variant::variant(int _n){
	this->hlist 	= new vitem*[_n];
	this->htail 	= new vitem*[_n];
	this->win_size = _n;
	for(int i = 0; i < this->win_size; i ++){
		this->hlist[i] 		= NULL;
		this->htail[i] 		= NULL;
	}

	this->timestamp = 0;
	this->lis_len = 0;

	this->buf = new Vtype[this->win_size];
	for(int i = 0; i < this->win_size; i ++){
		this->buf[i] = 0;
	}
	this->buf_h = 0;
	this->buf_t = 0;

	this->pool_used = 0;
	this->item_pool = new vitem*[_n];
	for(int i = 0; i < this->win_size; i ++){
		this->item_pool[i] = NULL;
	}
}
variant::~variant(){
	delete[] this->hlist;
	delete[] this->htail;
	delete[] this->buf;
	this->run_log.close();
}
void variant::run(int run_method, string _data_f, const bool _update_vertical){
	this->t_insert.initial();
	this->t_remove.initial();
	this->t_update.initial();
	this->t_compute.initial();
	this->t_total.initial();

	string _method[16] = {
			"maxgap_one",
			"mingap_one",
			"maxwid_one",
			"minwid_one",
			"maxwei_one",
			"minwei_one",
			"maxgap",
			"mingap"
	};

	string log_f = util::exp_home + "runningtime/";
	string file_name;
	stringstream label_type;
	{
		int i1 = _data_f.rfind('/');
		int i2 = _data_f.rfind('.');
		string _datatype = _data_f.substr(i1+1, i2-i1-1);
		stringstream _ss;
		if(_update_vertical){
			_ss << "[vrt_v]";
		}else{
			_ss << "[vrt]";
		}

		_ss << "_" << _method[run_method] << "";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		if(_update_vertical){
			label_type << "vrt_v\t" << _method[run_method];
//			cout << "is v" << endl;
//			system("pause");
		}else{
			label_type << "vrt\t" << _method[run_method];
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
		Vtype ai = ds.next();
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
		Vtype ai = ds.next();
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
		util::log(this->compute_str(run_method, _update_vertical).c_str(), "");
		this->t_compute.end();

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
}

void variant::runmicrosoft()
{
	cout << "begin run_mhmicrosoft" << endl;

	string microsoft = "microsoft_stock.dat";
	datastream ds(microsoft);
//	minheight mh(winsize);
	qnlist otg(10);
	while(ds.hasnext())
	{
		double d_next = ds.next();
		Vtype ai = ((int)(d_next));
		{
			stringstream _ss;
			_ss << "ai = " << ai << "\t" << d_next << endl;
			util::log(_ss);
		}
		this->update_vertical(ai);
		otg.update(ai);
		stringstream _ss;
		string vrt_lis = this->mingap_str(true);
		string otg_lis;
		{
			stringstream _tmp_ss;
			otg.mingap_one_str(_tmp_ss);
			otg_lis = _tmp_ss.str();
		}
		_ss << "\n\n++++++++++++++" << endl;
		_ss << this->seq_str() << endl;
		_ss << "vrt_str:\n" << this->to_str() << endl;
		_ss << "vrt: [" << vrt_lis << "]" << endl;
		_ss << "ot: [" << otg_lis << "]" << endl;

		if(vrt_lis != otg_lis){
			cout << "different at " << this->timestamp << endl;
			util::log(_ss);
		}
	}

	cout << "end run_mhmicrosoft" << endl;
}

int variant::update(Vtype _ins){
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

	return 0;
}
int variant::update_vertical(Vtype _ins)
{
	this->remove();
	this->insert(_ins);
	this->timestamp ++;

	return 0;
}
int variant::construction(vector<Vtype>& ivec)
{
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

	for(int i = 0; i < ivec.size(); i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = this->find_ins_pos(it->val);

		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			it->ptr = this->htail[itail-1];
			it->dptr_most = (it->ptr)->dptr_most;
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	return 0;
}
int variant::insert(Vtype _ins)
{
	vitem* ins = new vitem(_ins, this->timestamp);
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
int variant::h_adjust()
{
	vitem** hlist = new vitem*[this->win_size];
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
	vitem * tmp_head = hlist[0];

	int Li = 0;
	vitem * _ax_tmp = tmp_head;
	/* Divide */
	vitem** left_tail = new vitem*[this->win_size];
	vitem** right_head = new vitem*[this->win_size];
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
			vitem* _ay_tmp = (_ax_tmp->rn)->dn;
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
		vitem* tmp = this->htail[i];
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
int variant::v_adjust()
{
	/* update the un of hlist[0] */
	vitem* _it_tmp = this->htail[0];
	while(_it_tmp != NULL){
		_it_tmp->un = NULL;
		_it_tmp = _it_tmp->next;
	}

	/* up neighbor */
	for(int i = 1; i < this->lis_len; i ++)
	{
		vitem* _ah_tmp = this->htail[i-1];
		while(_ah_tmp->next != NULL){
			_ah_tmp = _ah_tmp->next;
		}
		vitem* _ax_tmp = this->htail[i];
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
		vitem* _ah_tmp = this->htail[i+1];
		while(_ah_tmp->next != NULL){
			_ah_tmp = _ah_tmp->next;
		}
		vitem* _ax_tmp = this->htail[i];
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
int variant::remove()
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
string variant::one_str(vitem* start)
{
	stringstream _ss;
	vitem** S = new vitem*[this->lis_len];
	S[0] = start;
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}

	_ss << S[this->lis_len-1]->val;
	for(int i = this->lis_len-2; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
//	_ss << endl;
	delete[] S;

	return _ss.str();
}
int variant::most_rm()
{
	vitem* it = this->htail[0];
	while(it != NULL){
		it->ptr = NULL;
		it->dptr_most = it;
		it = it->next;
	}
	for(int i = 1; i < this->lis_len; i ++)
	{
		it = this->htail[i];
		while(it != NULL)
		{
			it->ptr = it->un;
			it->dptr_most = (it->ptr)->dptr_most;

			it = it->next;
		}

	}
	return 0;
}
int variant::most_lm()
{
	vitem* it = this->htail[0];
	while(it != NULL){
		it->ptr = NULL;
		it->dptr_most = it;
		it = it->next;
	}
	for(int i = 1; i < this->lis_len; i ++)
	{
		it = this->htail[i];
		while(it != NULL)
		{
			vitem* tmp_x = it->un;
			while(tmp_x != NULL)
			{
				if(tmp_x->val <= it->val){
					it->ptr = tmp_x;
					it->dptr_most = tmp_x->dptr_most;
				}
				tmp_x = tmp_x->next;
			}
			it = it->next;
		}

	}
	return 0;
}
int variant::find_ins_pos(Vtype _val){
	//	int itail = 0;
	//	while(itail < this->lis_len)
	//	{
	//		if(this->htail[itail]->val <= _val)
	//		{
	//			itail ++;
	//		}
	//		else
	//		{
	//			break;
	//		}
	//	}

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
string variant::to_str(){
	stringstream _ss;
	_ss << "time: " << this->timestamp << " winsize: " << this->win_size << " ";
	_ss << "lis_lengh: " << this->lis_len << endl;
	_ss << "in windows: " << this->seq_str() << endl;

	for(int i = 0; i < this->lis_len; i ++)
	{
		_ss << "hlist" << i+1 << ": ";
		vitem* _it = hlist[i];
		while(_it != NULL)
		{
			_ss << _it->val << " ";
			_it = _it->rn;
		}
		_ss << endl;

		_it = hlist[i];
		while(_it != NULL)
		{
			_ss << "\t" << _it->to_str() << endl;
			_it = _it->rn;
		}
	}
	_ss << endl;

	return _ss.str();
}
int variant::to_size(){
	int sz = 0;

	sz += sizeof(variant);

	sz += sizeof(vitem*) * this->win_size;
	sz += sizeof(vitem*) * this->win_size;
	sz += sizeof(vitem*) * this->win_size;
	vitem* vit = new vitem(0, 0);
	sz += vit->to_size() * this->win_size;
	delete vit;

	return sz;
}
string variant::all_str(){
	stringstream _ss;

	return _ss.str();
}
string variant::seq_str(){
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

string variant::maxweight_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->maxweight_str_ori();
	}

	this->most_lm();
	vitem* it = this->htail[this->lis_len-1];
	while(it->next != NULL){
		it = it->next;
	}
	return this->one_str(it);
}
string variant::minweight_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->minweight_str_ori();
	}

	this->most_rm();
	vitem* it = this->htail[this->lis_len-1];

	return this->one_str(it);
}
string variant::maxgap_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->maxgap_str_ori();
	}

	this->most_rm();
	vitem* it = this->htail[this->lis_len-1];
	vitem* target_it = it;
	int maxgap = it->val - (it->dptr_most)->val;
	while(it->next != NULL){
		if(maxgap <= it->val - (it->dptr_most)->val)
		{
			maxgap = it->val - (it->dptr_most)->val;
			target_it = it;
		}
		it = it->next;
	}
	return this->one_str(target_it);
}
string variant::mingap_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->mingap_str_ori();
	}

	this->most_lm();
	vitem* it = this->htail[this->lis_len-1];
	vitem* target_it = it;
	int mingap = it->val - (it->dptr_most)->val;
	while(it->next != NULL){
		if(mingap >= it->val - (it->dptr_most)->val)
		{
			mingap = it->val - (it->dptr_most)->val;
			target_it = it;
		}
		it = it->next;
	}
	return this->one_str(target_it);
}
string variant::maxgap_one_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->maxgap_one_str_ori();
	}
	this->most_rm();
	vitem* it = this->htail[this->lis_len-1];
	vitem* target_it = it;
	Vtype maxgap = it->val - (it->dptr_most)->val;
	while(it->next != NULL){
		if(maxgap <= it->val - (it->dptr_most)->val)
		{
			maxgap = it->val - (it->dptr_most)->val;
			target_it = it;
		}
		it = it->next;
	}
	return this->one_str(target_it);
}
string variant::mingap_one_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->mingap_one_str_ori();
	}
	this->most_lm();
	vitem* it = this->htail[this->lis_len-1];
	vitem* target_it = it;
	Vtype mingap = it->val - (it->dptr_most)->val;
	while(it->next != NULL){
		if(mingap >= it->val - (it->dptr_most)->val)
		{
			mingap = it->val - (it->dptr_most)->val;
			target_it = it;
		}
		it = it->next;
	}
	return this->one_str(target_it);
}

string variant::maxwidth_one_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->maxwidth_one_str_ori();
	}
	this->most_lm();
	vitem* it = this->htail[this->lis_len-1];
	vitem* target_it = it;
	Vtype maxwid = it->timestamp - (it->dptr_most)->timestamp;
	while(it->next != NULL){
		if(maxwid < it->timestamp - (it->dptr_most)->timestamp)
		{
			maxwid = it->timestamp - (it->dptr_most)->timestamp;
			target_it = it;
		}
		it = it->next;
	}
	return this->one_str(target_it);
}
string variant::minwidth_one_str(bool _vertical)
{
	if(this->lis_len == 1){
		stringstream _ss;
		_ss << this->htail[0]->val;
		return _ss.str();
	}
	if(!_vertical){
		return this->minwidth_one_str_ori();
	}
	this->most_rm();
	vitem* it = this->htail[this->lis_len-1];
	vitem* target_it = it;
	int minwid = it->timestamp - (it->dptr_most)->timestamp;
	while(it->next != NULL){
		if(minwid > it->timestamp - (it->dptr_most)->timestamp)
		{
			minwid = it->timestamp - (it->dptr_most)->timestamp;
			target_it = it;
		}
		it = it->next;
	}
	return this->one_str(target_it);
}

string variant::maxweight_str_ori(){
	stringstream _ss;
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = 0;
		while(itail < this->lis_len)
		{
			if(this->htail[itail]->val <= it->val)
			{
				itail ++;
			}
			else
			{
				break;
			}
		}
		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			vitem* tmp = this->htail[itail-1];
			while(tmp != NULL)
			{
				if(tmp->val <= it->val){
					 it->ptr = tmp;
					 it->dptr_most = tmp->dptr_most;
				}else{
					break;
				}
				tmp = tmp->next;
			}
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	vitem** S = new vitem*[this->lis_len];
	S[0] = this->htail[this->lis_len-1];
	while(S[0]->next != NULL){
		S[0] = S[0]->next;
	}
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}
	for(int i = this->lis_len-1; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
	_ss << endl;
	delete[] S;

	return _ss.str();
}
string variant::minweight_str_ori(){

	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = 0;
		while(itail < this->lis_len)
		{
			if(this->htail[itail]->val <= it->val)
			{
				itail ++;
			}
			else
			{
				break;
			}
		}

		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			it->ptr = this->htail[itail-1];
			it->dptr_most = (it->ptr)->dptr_most;
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	vitem** S = new vitem*[this->lis_len];
	S[0] = this->htail[this->lis_len-1];
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}
	stringstream _ss;
	for(int i = this->lis_len-1; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
	_ss << endl;
	delete[] S;

	return _ss.str();
}
string variant::maxgap_str_ori(){
	/* re-compute */
	stringstream _ss;
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = 0;
		while(itail < this->lis_len)
		{
			if(this->htail[itail]->val <= it->val)
			{
				itail ++;
			}
			else
			{
				break;
			}
		}
		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			it->ptr = this->htail[itail-1];
			it->dptr_most = (it->ptr)->dptr_most;
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	vitem* vit = this->htail[this->lis_len-1];
	vitem* target_it = vit;
	Vtype maxGap = vit->val - vit->dptr_most->val;
	vit = vit->next;
	while(vit != NULL){
		if(maxGap < vit->val - vit->dptr_most->val){
			maxGap = vit->val - vit->dptr_most->val;
			target_it = vit;
		}
		vit = vit->next;
	}

	vitem** S = new vitem*[this->lis_len];
	S[0] = target_it;
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}
	for(int i = this->lis_len-1; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
	_ss << endl;
	delete[] S;

	return _ss.str();
}
string variant::mingap_str_ori(){
	return "err call";
}
string variant::maxgap_one_str_ori(){
	stringstream _ss;
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = 0;
		while(itail < this->lis_len)
		{
			if(this->htail[itail]->val <= it->val)
			{
				itail ++;
			}
			else
			{
				break;
			}
		}
		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			it->ptr = this->htail[itail-1];
			it->dptr_most = (it->ptr)->dptr_most;
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	vitem* vit = this->htail[this->lis_len-1];
	vitem* target_it = vit;
	Vtype maxGap = vit->val - vit->dptr_most->val;
	vit = vit->next;
	while(vit != NULL){
		if(maxGap < vit->val - vit->dptr_most->val)
		{
			maxGap = vit->val - vit->dptr_most->val;
			target_it = vit;
		}
		vit = vit->next;
	}

	vitem** S = new vitem*[this->lis_len];
	S[0] = target_it;
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}
	for(int i = this->lis_len-1; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
	_ss << endl;
	delete[] S;

	return _ss.str();
}
string variant::mingap_one_str_ori(){

	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = 0;
		while(itail < this->lis_len)
		{
			if(this->htail[itail]->val <= it->val)
			{
				itail ++;
			}
			else
			{
				break;
			}
		}
		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			vitem* tmp = this->htail[itail-1];
			while(tmp != NULL)
			{
				if(tmp->val <= it->val){
					 it->ptr = tmp;
					 it->dptr_most = tmp->dptr_most;
				}else{
					break;
				}
				tmp = tmp->next;
			}
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	vitem* vit = this->htail[this->lis_len-1];
	Vtype minGap = vit->val - (vit->dptr_most)->val;
	vitem* target_it = vit;
	vit = vit->next;
	while(vit != NULL)
	{
		if(minGap > vit->val - (vit->dptr_most)->val)
		{
			minGap = vit->val - (vit->dptr_most)->val;
			target_it = vit;
		}
		vit = vit->next;
	}

	vitem** S = new vitem*[this->lis_len];
	S[0] = target_it;
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}
	stringstream _ss;
	for(int i = this->lis_len-1; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
	_ss << endl;
	delete[] S;

	return _ss.str();
}

string variant::minwidth_one_str_ori()
{
	stringstream _ss;
	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = 0;
		while(itail < this->lis_len)
		{
			if(this->htail[itail]->val <= it->val)
			{
				itail ++;
			}
			else
			{
				break;
			}
		}
		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			it->ptr = this->htail[itail-1];
			it->dptr_most = (it->ptr)->dptr_most;
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	vitem* vit = this->htail[this->lis_len-1];
	vitem* target_it = vit;
	int minWid = vit->timestamp - vit->dptr_most->timestamp;
	vit = vit->next;
	while(vit != NULL){
		if(minWid > vit->timestamp - vit->dptr_most->timestamp)
		{
			minWid = vit->timestamp - vit->dptr_most->timestamp;
			target_it = vit;
		}
		vit = vit->next;
	}

	vitem** S = new vitem*[this->lis_len];
	S[0] = target_it;
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}
	for(int i = this->lis_len-1; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
	_ss << endl;
	delete[] S;

	return _ss.str();
}
string variant::maxwidth_one_str_ori(){

	int sz = this->win_size;
	if(sz > this->timestamp){
		sz = this->timestamp;
	}

	for(int i = 0; i < sz; i ++)
	{
		vitem* it = new vitem(this->get_buf(i), i);
		this->new_item(it);
		int itail = 0;
		while(itail < this->lis_len)
		{
			if(this->htail[itail]->val <= it->val)
			{
				itail ++;
			}
			else
			{
				break;
			}
		}
		if(itail == this->lis_len){
			this->htail[itail] = it;
			this->lis_len ++;
		}
		else
		{
			it->next = this->htail[itail];
			(it->next)->rn = it;
			this->htail[itail] = it;
		}

		if(itail >= 1)
		{
			vitem* tmp = this->htail[itail-1];
			while(tmp != NULL)
			{
				if(tmp->val <= it->val){
					 it->ptr = tmp;
					 it->dptr_most = tmp->dptr_most;
				}else{
					break;
				}
				tmp = tmp->next;
			}
		}
		else
		{
			it->dptr_most = it;
			it->ptr = NULL;
		}
	}

	vitem* vit = this->htail[this->lis_len-1];
	int maxWid = vit->timestamp - (vit->dptr_most)->timestamp;
	vitem* target_it = vit;
	vit = vit->next;
	while(vit != NULL)
	{
		if(maxWid < vit->timestamp - (vit->dptr_most)->timestamp)
		{
			maxWid = vit->timestamp - (vit->dptr_most)->timestamp;
			target_it = vit;
		}
		vit = vit->next;
	}

	vitem** S = new vitem*[this->lis_len];
	S[0] = target_it;
	int k = 1;
	while(k < this->lis_len){
		S[k] = S[k-1]->ptr;
		k ++;
	}
	stringstream _ss;
	for(int i = this->lis_len-1; i >= 0; i --)
	{
		_ss << "\t" << S[i]->val;
	}
	_ss << endl;
	delete[] S;

	return _ss.str();
}

//private:

string variant::compute_str(int run_method, const bool _vertical){
	stringstream _ss_comp;
	switch(run_method)
	{
		case 0:
		{
			_ss_comp << "--maxgap_one:" << endl << this->maxgap_one_str(_vertical) << endl;
			break;
		}
		case 1:
		{
			_ss_comp << "--mingap_one:" << endl << this->mingap_one_str(_vertical) << endl;
			break;
		}
		case 2:
		{
			_ss_comp << "--maxwid_one:" << endl << this->maxwidth_one_str(_vertical) << endl;
			break;
		}
		case 3:
		{
			_ss_comp << "--minwid_one:" << endl << this->minwidth_one_str(_vertical) << endl;
			break;
		}
		case 4:
		{
			_ss_comp << "--maxweight:" << endl << this->maxweight_str(_vertical) << endl;
			break;
		}
		case 5:
		{
			_ss_comp << "--minweight:" << endl << this->minweight_str(_vertical) << endl;
			break;
		}
		case 6:
		{
			_ss_comp << "--maxgap:" << endl << this->maxgap_str(_vertical) << endl;
			break;
		}
		case 7:
		{
			_ss_comp << "--mingap:" << endl << this->mingap_str(_vertical) << endl;
			break;
		}
	}

	return _ss_comp.str();
}
string variant::get_method(int _i){


	return "deprecated";
}
Vtype variant::get_buf(int _i){
	return this->buf[(this->buf_h+_i) % this->win_size];
}
void variant::new_item(vitem* _it){
	this->item_pool[this->pool_used] = _it;
	this->pool_used ++;
}

vitem* variant::getRM(int _i, vitem* _cur){
	return NULL;
}


long long int variant::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}
long long int variant::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

void variant::log_running(){
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
string variant::sum_running(){
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

