/*
 * orthogonal.cpp
 *
 *  Created on: 2015-9-15
 *      Author: liyouhuan
 */
#include "orthogonal.h"
#include "../datastream/datastream.h"
orthogonal::orthogonal(int _n){
	this->hlist 	= new item*[_n];
	this->htail 	= new item*[_n];
	this->tmp_left  = new item*[_n];
	this->tmp_right = new item*[_n];
	this->S = new item*[_n];
	this->win_size = _n;
	for(int i = 0; i < this->win_size; i ++){
		this->hlist[i] 		= NULL;
		this->htail[i] 		= NULL;
		this->tmp_left[i]  	= NULL;
		this->tmp_right[i] 	= NULL;
		this->S[i] = NULL;
	}

	this->timestamp = 0;
	this->lis_len = 0;

	this->buf = new int[this->win_size];
	for(int i = 0; i < this->win_size; i ++){
		this->buf[i] = 0;
	}

	this->buf_h = 0;
	this->buf_t = 0;
}
orthogonal::~orthogonal(){
	delete[] this->hlist;
	delete[] this->htail;
	delete[] this->tmp_left;
	delete[] this->tmp_right;
	delete[] this->buf;
	delete[] this->S;
	this->run_log.close();
}
void orthogonal::run(int run_method, string _data_f){
//	cout << "begin orthogonal run" << endl;
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
		_ss << "[otg]";
		_ss << "_" << this->get_method(run_method) << "";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		label_type << "otg\t" << this->get_method(run_method);
		label_type << "\t" << _datatype << "\t" << this->win_size;
	}
	this->run_log.open(log_f.c_str(), ios::out);
	if(! this->run_log){
		cout << log_f << " can not be opened" << endl;
		system("pause");
	}

	util::init_space = util::get_space();

	int sliding = 0;
//	if(this->win_size == 3600){
//		sliding = 50;
//	}

	datastream ds(_data_f);
	while(ds.hasnext())
	{
		int ai = ds.next();

//		{
//			if(ds.timestamp() % 100 == 0)
//				cout << ds.timestamp() << endl;
//			if(ds.timestamp() >=3600)
//			{
//				cout << ds.timestamp() << endl;
//			}
//		}

		if(ds.timestamp() < sliding) continue;


		if(ds.timestamp() < this->win_size + sliding){
			this->update(ai);
			continue;
		}

		this->t_total.begin();
		this->t_update.begin();

		this->update(ai);
//		{
//			if(ds.timestamp() >= 3600){
//				cout <<"here" <<endl;
//			}
//		}
		this->t_update.end();

		this->t_compute.begin();
		util::log(this->compute_str(run_method).c_str(), "");
		this->t_compute.end();

//		{
//			if(ds.timestamp() >= 3600){
//				cout <<"here1" <<endl;
//			}
//		}

		this->t_total.end();

		this->log_running();

		if(ds.timestamp() > this->win_size + util::update_times + sliding){
			break;
		}

//		if(ds.timestamp() % 100 == 0) cout << "ts=" << ds.timestamp() << endl;
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;
//	cout << "end orthogonal run" << endl;
}

string orthogonal::compute_str(int run_method){
	stringstream _ss_comp;
	switch(run_method)
	{
		case 0:
		{
//			{
//				if(this->timestamp >= 3600){
//					cout << "here2 = " << this->timestamp << endl;
//				}
//			}
			_ss_comp << "--enum:" << endl;
			this->enum_str(_ss_comp);

			{
				if(this->timestamp > this->win_size && util::isconsole)
				{
					int sz = _ss_comp.str().size();
					cout << "ts=" << this->timestamp << "\t";
					cout << "size=" << sz << "\t";
					cout << "lisL=" << this->lis_len << "\t";
					cout << "numLis=" << sz/this->lis_len << "\t";
					cout << endl;
//					cout << _ss_comp.str() << endl;
//					system("pause");
				}
			}
//			{
//				if(this->timestamp >= 3600){
//					cout << "here3 = " << this->timestamp << endl;
//				}
//			}
			break;
		}
		case 1:
		{
			_ss_comp << "--mingap_one:" << endl;
			this->mingap_one_str(_ss_comp);
			break;
		}
		case 2:
		{
			_ss_comp << "--maxgap_one:" << endl;
			this->maxgap_one_str(_ss_comp);
			break;
		}
		case 3:
		{
			_ss_comp << "--maxweight:" << endl;
			this->maxweight_str(_ss_comp);
			break;
		}
		case 4:
		{
			_ss_comp << "--minweight:" << endl;
			this->minweight_str(_ss_comp);
			break;
		}
		case 5:
		{
			_ss_comp << "--maxfirst:" << endl;
			this->maxfirst_str(_ss_comp);
			break;
		}
		case 6:
		{
			_ss_comp << "--minfirst:" << endl;
			this->minfirst_str(_ss_comp);
			break;
		}
		case 7:
		{
			_ss_comp << "--maxlast:" << endl;
			this->maxlast_str(_ss_comp);
			break;
		}
		case 8:
		{
			_ss_comp << "--minlast:" << endl;
			this->minlast_str(_ss_comp);
			break;
		}
		case 9:
		{
			_ss_comp << "--maxgap:" << endl;
			this->maxgap_str(_ss_comp);
			break;
		}
		case 10:
		{
			_ss_comp << "--mingap:" << endl;
			this->mingap_str(_ss_comp);
			break;
		}
	}

	return _ss_comp.str();
}

string orthogonal::get_method(int _i){
	string _method[11] = {
			"enum",
			"mingap_one",
			"maxgap_one",
			"maxweight",
			"minweight",
			"maxfirst",
			"minfirst",
			"maxlast",
			"minlast",
			"maxgap",
			"mingap"
	};

	return _method[_i];
}

void orthogonal::run_debug(){
	cout << "begin run_debug" << endl;

	util::initial();
	string debug = "debug.dat";
	datastream ds(debug);
	while(ds.hasnext())
	{
		int ai = ds.next();
		this->update(ai);

		stringstream _ss;
		cout << "\n\n++++++++++++++" << endl;
		cout << this->seq_str() << endl;
		cout << this->to_str() << endl;
		cout << this->enum_str(_ss) << endl;
		cout << "--maxgap:" << endl << this->maxgap_str(_ss) << endl;
		cout << "--mingap:" << endl << this->mingap_str(_ss) << endl;
		cout << "--maxweight:" << endl << this->maxweight_str(_ss) << endl;
		cout << "--minweight:" << endl << this->minweight_str(_ss) << endl;
		cout << "--maxfirst:" << endl << this->maxfirst_str(_ss) << endl;
		cout << "--minfirst:" << endl << this->minfirst_str(_ss) << endl;
		cout << "--maxlast:" << endl << this->maxlast_str(_ss) << endl;
		cout << "--minlast:" << endl << this->minlast_str(_ss) << endl;
	}

	cout << "end run_debug" << endl;
}

void orthogonal::run_stream(){
	cout << "begin run_stream" << endl;
	util::initial();
	string test = "stream.dat";
	datastream ds(test);
	while(ds.hasnext())
	{
		int ai = ds.next();
		this->update(ai);

		cout << "\n\n++++++++++++++" << endl;
		cout << this->to_str() << endl;
		cout << this->all_str() << endl;
	}
	cout << "end run_stream" << endl;
}

void orthogonal::run_microsoft(){
	cout << "begin run_microsoft" << endl;
	util::initial();
	string microsoft_stock = "microsoft_stock.dat";
	datastream ds(microsoft_stock);
	while(ds.hasnext())
	{
		int ai = ((int)(ds.next()));
		this->update(ai);

//		util::log(this->computation_str().c_str(), "");
		stringstream _ss;
		this->enum_str(_ss);
		util::log(_ss.str().c_str(), "");
	}
	cout << "end run_microsoft" << endl;
}
int orthogonal::update(int _ins){
	item * item_ins = this->remove();

	item_ins->initial(_ins, this->timestamp);
	this->insert(item_ins);

	this->timestamp ++;
	return 0;
}
int orthogonal::construction(vector<int>& ivec){
	for(int i = 0; i < ivec.size(); i ++)
	{
		this->insert(new item(ivec[i], i));
	}
	return 0;
}
string orthogonal::to_str(){
	stringstream _ss;
	_ss << "time: " << this->timestamp << " winsize: " << this->win_size << " ";
	_ss << "lis_lengh: " << this->lis_len << endl;
	_ss << "in windows: " << this->seq_str() << endl;

	for(int i = 0; i < this->lis_len; i ++)
	{
		_ss << "hlist" << i+1 << ": ";
		item* _it = hlist[i];
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
int orthogonal::to_size(){
	int sz = 0;
	sz += sizeof(orthogonal);

	sz += sizeof(item*)*this->win_size;
	sz += sizeof(item*)*this->win_size;

	item* it = new item();
	sz += it->to_size() * this->win_size;
	delete it;

	return sz;
}
string orthogonal::all_str(){
	stringstream _ss;
	_ss << "\n\n++++++++++++++" << endl;
	_ss << this->seq_str() << endl;
	_ss << this->to_str() << endl;
	_ss << this->enum_str(_ss) << endl;
	_ss << "--maxgap:" << endl << this->maxgap_str(_ss) << endl;
	_ss << "--mingap:" << endl << this->mingap_str(_ss) << endl;
	_ss << "--maxweight:" << endl << this->maxweight_str(_ss) << endl;
	_ss << "--minweight:" << endl << this->minweight_str(_ss) << endl;
	_ss << "--maxfirst:" << endl << this->maxfirst_str(_ss) << endl;
	_ss << "--minfirst:" << endl << this->minfirst_str(_ss) << endl;
	_ss << "--maxlast:" << endl << this->maxlast_str(_ss) << endl;
	_ss << "--minlast:" << endl << this->minlast_str(_ss) << endl;
	return _ss.str();
}
string orthogonal::seq_str(){
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
string orthogonal::enum_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << "\t" << hlist[0]->val << endl;
		return "";
	}

	item* it = hlist[this->lis_len-1];

	while(it != NULL)
	{
		this->ending_str(_ss, it);
		it = it->rn;
	}
	return "";
}
string orthogonal::maxgap_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_rmmost();
	int max_gap = -1;
	item * it = this->hlist[this->lis_len-1];
	while(it != NULL){
		if(max_gap < it->val - it->rm_most->val){
			max_gap = it->val - it->rm_most->val;
		}
		it = it->rn;
	}
	_ss << max_gap << ":" << endl;

	it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->val - it->rm_most->val != max_gap){
			it = it->rn;
			continue;
		}
		int RM_ST = it->rm_most->val;
		this->S[0] = it;
		this->S[1] = it->un;
		int k = 2;
		while(k > 1)
		{
			if(this->S[k-1] == NULL){
				this->S[k-2] = this->S[k-2]->ln;
				k --;
				continue;
			}

			item* _ay_tmp = this->S[k-1]->rm_most;
			if(this->S[k-1]->partial(this->S[k-2]) && _ay_tmp->val == RM_ST)
			{
				if(k < this->lis_len)
				{
					this->S[k] = this->S[k-1]->un;
					k ++;
					continue;
				}
				else
				{
					_ss << "\t" << this->S[this->lis_len-1]->val;
					for(int i = this->lis_len-2; i >= 0; i --){
						_ss << "\t" << this->S[i]->val;
					}
					_ss << endl;
				}
			}

			this->S[k-2] = this->S[k-2]->ln;
			k --;
		}

		it = it->rn;
	}

	return "";
}
string orthogonal::mingap_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_lmmost();
	item * it = this->hlist[this->lis_len-1];
	int min_gap = it->val - (it->lm_most)->val;
	it = it->rn;
	while(it != NULL){
		if(min_gap > it->val - it->lm_most->val){
			min_gap = it->val - it->lm_most->val;
		}
		it = it->rn;
	}

	it = this->hlist[this->lis_len-1];

	while(it != NULL)
	{
		if(it->val - it->lm_most->val != min_gap){
			it = it->rn;
			continue;
		}

		int LM_ST = it->lm_most->val;
		this->S[0] = it;
		if(it ->rlen == 1){
			cout << "err rlen1" << endl;
		}
		this->S[1] = it->child_lm();
		int k = 2;
		while(k > 1)
		{
			if(this->S[k-1] == NULL){
				this->S[k-2] = this->S[k-2]->rn;
				k --;
				continue;
			}

			item* _ay_tmp = this->S[k-1]->lm_most;
			if(this->S[k-1]->partial(this->S[k-2]) && _ay_tmp->val == LM_ST)
			{
				if(k < this->lis_len)
				{
					if(this->S[k-1] ->rlen == 1){
						cout << "err rlen2" << endl;
						cout << "k=" << k << endl;
					}
					this->S[k] = this->S[k-1]->child_lm();
					k ++;
					continue;
				}
				else
				{
					_ss << "\t" << this->S[this->lis_len-1]->val;
					for(int i = this->lis_len-2; i >= 0; i --){
						_ss << "\t" << this->S[i]->val;
					}
					_ss << endl;
				}
			}

			this->S[k-2] = this->S[k-2]->rn;
			k --;
		}

		it = it->rn;
	}

	return "";
}
string orthogonal::maxfirst_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val << endl;
		return "";
	}

	this->update_inLIS();
	item* it = this->hlist[0];
	while(it != NULL){
		if(it->inLIS){
			this->starting_str(it, _ss);
			return "";
		}
		it = it->rn;
	}
	return "err max first_str";
}
string orthogonal::minfirst_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val << endl;
		return "";
	}

	this->update_inLIS();
	item* it = this->htail[0];
	while(it != NULL){
		if(it->inLIS){
			this->starting_str(it, _ss);
			return "";
		}
		it = it->ln;
	}
	return "err min first_str";
}
string orthogonal::maxlast_str(stringstream & _ss){
	this->ending_str(_ss, this->hlist[this->lis_len-1]);
	return "";
}
string orthogonal::minlast_str(stringstream & _ss){
	this->ending_str(_ss, this->htail[this->lis_len-1]);
	return "";
}
string orthogonal::maxgap_one_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_rmmost();
	int max_gap = -1;
	item * it = this->hlist[this->lis_len-1];
	while(it != NULL){
		if(max_gap < it->val - it->rm_most->val){
			max_gap = it->val - it->rm_most->val;
		}
		it = it->rn;
	}
	_ss << max_gap << ":" << endl;

	it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->val - it->rm_most->val != max_gap){
			it = it->rn;
			continue;
		}
		int RM_ST = it->rm_most->val;
		this->S[0] = it;
		this->S[1] = it->un;
		int k = 2;
		while(k > 1)
		{
			if(this->S[k-1] == NULL){
				this->S[k-2] = this->S[k-2]->ln;
				k --;
				continue;
			}

			item* _ay_tmp = this->S[k-1]->rm_most;
			if(this->S[k-1]->partial(this->S[k-2]) && _ay_tmp->val == RM_ST)
			{
				if(k < this->lis_len)
				{
					this->S[k] = this->S[k-1]->un;
					k ++;
					continue;
				}
				else
				{
					_ss << "\t" << this->S[this->lis_len-1]->val;
					for(int i = this->lis_len-2; i >= 0; i --){
						_ss << "\t" << this->S[i]->val;
					}
					_ss << endl;

					return "";
				}
			}

			this->S[k-2] = this->S[k-2]->ln;
			k --;
		}

		it = it->rn;
	}
	return "";
}
string orthogonal::mingap_one_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_lmmost();
	item * it = this->hlist[this->lis_len-1];
	int min_gap = it->val - (it->lm_most)->val;
	it = it->rn;
	while(it != NULL){
		if(min_gap > it->val - it->lm_most->val){
			min_gap = it->val - it->lm_most->val;
		}
		it = it->rn;
	}

	it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->val - it->lm_most->val != min_gap){
			it = it->rn;
			continue;
		}

		int LM_ST = it->lm_most->val;
		this->S[0] = it;
		if(it ->rlen == 1){
			cout << "err rlen1" << endl;
		}
		this->S[1] = it->child_lm();
		int k = 2;
		while(k > 1)
		{
			if(this->S[k-1] == NULL){
				this->S[k-2] = this->S[k-2]->rn;
				k --;
				continue;
			}

			item* _ay_tmp = this->S[k-1]->lm_most;
			if(this->S[k-1]->partial(this->S[k-2]) && _ay_tmp->val == LM_ST)
			{
				if(k < this->lis_len)
				{
					if(this->S[k-1] ->rlen == 1){
						cout << "err rlen2" << endl;
						cout << "k=" << k << endl;
					}
					this->S[k] = this->S[k-1]->child_lm();
					k ++;
					continue;
				}
				else
				{
					_ss << "\t" << this->S[this->lis_len-1]->val;
					for(int i = this->lis_len-2; i >= 0; i --){
						_ss << "\t" << this->S[i]->val;
					}
					_ss << endl;
					return "";
				}
			}

			this->S[k-2] = this->S[k-2]->rn;
			k --;
		}

		it = it->rn;
	}
	return "None";
}
string orthogonal::maxweight_str(stringstream & _ss){
	item* it = this->hlist[this->lis_len-1];
	this->S[0] = it;
	int i = 0;
	while(it->rlen > 1)
	{
		i ++;
		this->S[i] = it->child_lm();
		it = this->S[i];
	}
	for(int i = this->lis_len-1; i >= 0; i --){
		_ss << "\t" << this->S[i]->val;
	}
	_ss << endl;

	return "";
}
string orthogonal::minweight_str(stringstream & _ss){
	this->S[0] = this->htail[this->lis_len-1];
	int i = 1;
	while(i < this->lis_len)
	{
		this->S[i] = this->S[i-1]->un;
		i ++;
	}
	for(i = this->lis_len-1; i >=0; i --){
		_ss << "\t" << this->S[i]->val;
	}
	_ss << endl;

	return "";
}

/* private */
int orthogonal::insert(item* _ins){
	this->t_insert.begin();

	int i = this->find_ins_pos(_ins->val);
	/* horizontal */
	if(hlist[i] == NULL)
	{
		hlist[i] = _ins;
		htail[i] = _ins;
		this->lis_len ++;
	}
	else
	{
		htail[i]->rn = _ins;
		_ins->ln = htail[i];
		htail[i] = _ins;
	}
	/* vertical */
	if(i-1 >= 0)
	{
		_ins->un = htail[i-1];
	}
	if(i+1 < this->lis_len)
	{
		_ins->dn = htail[i+1];
	}

	_ins->rlen = i+1;

	{/* debug */
		this->buf[this->buf_t] = _ins->val;
		this->buf_t = (this->buf_t + 1) % this->win_size;
	}

	this->t_insert.end();
	return 0;
}
int orthogonal::find_ins_pos(int _val){
//	int i = 0;
	{/*
		while(hlist[i] != NULL && htail[i]->val <= _val)
		{
			i ++;
		}
		return i; */
	}
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
}
item* orthogonal::remove(){
	this->t_remove.begin();

	if(this->timestamp < this->win_size)
	{
		this->t_remove.end();
		return new item();
	}

	{/* debug */
		this->buf_h = (this->buf_h + 1) % this->win_size;
	}

	item* a_remove = hlist[0];
	this->h_adjust();
	this->v_adjust();

	this->t_remove.end();

	return a_remove;
}
int orthogonal::h_adjust(){
	_ax_tmp = hlist[0];
	int Li = 0;
	/* Divide */
	while(true)
	{
		if(_ax_tmp->rn == NULL)
		{
			for(int i = Li; i < this->lis_len; i ++)
			{
				this->tmp_left[i] = this->htail[i];/* tail of left part */
				this->tmp_right[i] = NULL;
			}
			break;
		}
		else
		{
			this->tmp_left[Li] = _ax_tmp;
			this->tmp_right[Li] = _ax_tmp->rn;
			_ay_tmp = (_ax_tmp->rn)->dn;
			if(_ay_tmp == NULL)
			{
				for(int i = Li+1; i < this->lis_len; i ++)
				{
					this->tmp_left[i] = NULL;
					this->tmp_right[i] = this->hlist[i]; /* head of right part */
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
		if(this->tmp_left[i+1] == NULL)
		{
			this->hlist[i] = this->tmp_right[i];
			this->hlist[i]->ln = NULL;
		}
		else
		if(this->tmp_right[i] == NULL)
		{
			this->hlist[i] = this->hlist[i+1];
		}
		else
		{
			this->hlist[i] = this->hlist[i+1];
			this->tmp_left[i+1]->rn = this->tmp_right[i];
			this->tmp_right[i]->ln = this->tmp_left[i+1];
		}
	}
	/* for last right part of the last hlist */
	if(this->tmp_right[this->lis_len-1] != NULL)
	{
		this->hlist[this->lis_len-1] = this->tmp_right[this->lis_len-1];
		this->hlist[this->lis_len-1]->ln = NULL;
	}
	else
	{
		this->hlist[this->lis_len-1] = NULL;
		this->lis_len --;
	}

	this->update_rlen_tail();

	return 0;
}
int orthogonal::v_adjust(){
	/* update the un of hlist[0] */
	item* _it_tmp = this->hlist[0];
	while(_it_tmp != NULL){
		_it_tmp->un = NULL;
		_it_tmp = _it_tmp->rn;
	}

	/* up neighbor */
	for(int i = 1; i < this->lis_len; i ++)
	{
		_ah_tmp = this->hlist[i-1];
		_ax_tmp = this->hlist[i];
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
				cout << "err _ah_tmp _ax_tmp" << endl;
				system("pause");
			}
		}
	}

	/* down neighbor */
	for(int i = 0; i < this->lis_len-1; i ++)
	{
		_ax_tmp = this->hlist[i];
		_ah_tmp = this->hlist[i+1];
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
	_it_tmp = this->hlist[this->lis_len-1];
	while(_it_tmp != NULL){
		_it_tmp->dn = NULL;
		_it_tmp = _it_tmp->rn;
	}

	return 0;
}
int orthogonal::update_rlen_tail(){
	int i;
	/* update rlen */
	for(i = 0; i < this->lis_len; i ++)
	{
		_it_tmp = this->hlist[i];
		while(true)
		{
			_it_tmp->rlen = i+1;
			if(_it_tmp->rn != NULL)
			{
				_it_tmp = _it_tmp->rn;
			}
			else
			{
				this->htail[i] = _it_tmp;
				break;
			}
		}
	}
	/*  */
//	for(; i < this->win_size; i ++)
//	{
//		this->hlist[i] = NULL;
//		this->htail[i] = NULL;
//	}


	return 0;
}

string orthogonal::starting_str(item* it_first, stringstream & _ss){
	if(this->lis_len == 1){
		_ss << it_first->val << endl;
		return _ss.str();
	}
	this->S[0] = it_first;
	this->S[1] = this->rn_of_dn(it_first);
//	cout << "starting: " << it_first->val << " " << this->S[1]->val << endl;
	int k = 2;
	while(k > 1)
	{
		if(this->S[k-1] == NULL){
			this->S[k-2] = this->S[k-2]->rn;
			k --;
			continue;
		}
		if(this->S[k-2]->partial(this->S[k-1]))
		{
			if(this->S[k-1]->inLIS == false)
			{
				this->S[k-1] = this->S[k-1]->rn;
				continue;
			}
			else
			if(k == this->lis_len)
			{
				for(int i = 0; i < this->lis_len; i ++)
				{
					_ss << "\t" << this->S[i]->val;
				}
				_ss << endl;
				this->S[k-1] = this->S[k-1]->rn;
			}
			else
			{
				this->S[k] = this->rn_of_dn(this->S[k-1]);
//				cout << "then " << this->S[k]->val << endl;
				k ++;
			}
		}
		else
		{
			this->S[k-2] = this->S[k-2]->rn;
			k --;
		}
	}

	return "";
}
item* orthogonal::rn_of_dn(item* it){
	if(it->rlen == this->lis_len){
		cout << "err rn of dn" << endl;
		return NULL;
	}

	item* ak = NULL;
	if(it->dn != NULL){
		ak = (it->dn)->rn;
	}
	if(ak == NULL){
		ak = this->hlist[it->rlen-1 + 1];
	}

	return ak;
}
string orthogonal::ending_str(item* it_last){
	stringstream _ss;
	if(this->lis_len == 1){
		_ss << it_last->val << endl;
		return _ss.str();
	}
	this->S[0] = it_last;
	this->S[1] = it_last->un;
	int k = 2;
	while(k > 1)
	{
		if(this->S[k-1] == NULL)
		{
			k --;
			this->S[k-1] = this->S[k-1]->ln;
		}
		else
		if(this->S[k-1]->partial(this->S[k-2]))
		{
			if(this->S[k-1]->rlen == 1)
			{
				for(int i = this->lis_len-1; i >= 0; i --)
				{
					_ss << "\t" << this->S[i]->val;
				}
				_ss << endl;
				this->S[k-1] = this->S[k-1]->ln;
			}
			else
			{
				this->S[k] =  this->S[k-1]->un;
				k ++;
			}
		}
		else
		{
			k --;
			this->S[k-1] = this->S[k-1]->ln;
		}
	}
	return _ss.str();
}

void orthogonal::ending_str(stringstream& _ss, item* it_last){
	if(this->lis_len == 1){
		_ss << it_last->val << endl;
		return ;
	}
	this->S[0] = it_last;
	this->S[1] = it_last->un;
	int k = 2;
	while(k > 1)
	{
		{
			if(this->timestamp >= 3600){
//				cout << "k = " << k << endl;
			}
		}
		if(this->S[k-1] == NULL)
		{
			k --;
			this->S[k-1] = this->S[k-1]->ln;
		}
		else
		if(this->S[k-1]->before(this->S[k-2]) &&
		   this->S[k-1]->val <= this->S[k-2]->val)
		{
			if(this->S[k-1]->rlen == 1)
			{
				for(int i = this->lis_len-1; i >= 0; i --)
				{
					_ss << "\t" << this->S[i]->val; // save time
				}
				_ss << endl; // save time
				this->S[k-1] = this->S[k-1]->ln;
			}
			else
			{
				this->S[k] =  this->S[k-1]->un;
				k ++;
			}
		}
		else
		{
			k --;
			this->S[k-1] = this->S[k-1]->ln;
		}
	}
	return ;
}

int orthogonal::update_rmmost(){
	item* it = this->hlist[0];
	while(it != NULL){
		it->rm_most = it;
		it = it->rn;
	}
	for(int i = 1; i < this->lis_len; i ++)
	{
		it = this->hlist[i];
		while(it != NULL)
		{
			it->rm_most = (it->un)->rm_most;
			it = it->rn;
		}
	}
	return 0;
}
int orthogonal::update_lmmost(){
	item* it = this->hlist[0];
	while(it != NULL){
		it->lm_most = it;
		it = it->rn;
	}
	for(int i = 1; i < this->lis_len; i ++)
	{
		it = this->hlist[i];
		while(it != NULL)
		{
			if(it->rlen == 1){
				cout << "err it_child" << endl;
				cout << "i=" << i << endl;
			}
			item* it_child = it->child_lm();
			it->lm_most = it_child->lm_most;
			it = it->rn;
		}
	}
	return 0;
}
int orthogonal::update_inLIS(){
	item* it = this->hlist[this->lis_len-1];
	while(it != NULL){
		it->inLIS = true;
//		cout << "(" << it->val << "," << it->inLIS << ")" << endl;
		it = it->rn;
	}

	item *a1, *a2;
	for(int i = this->lis_len-2; i >= 0; i --)
	{
		a1 = this->hlist[i];
		a2 = this->hlist[i+1];
		while(a1 != NULL)
		{
			if(a2 == NULL){
				a1->inLIS = false;
				a1 = a1->rn;
			}
			else
			if(a1->timestamp > a2->timestamp)
			{
				a2 = a2->rn;
			}
			else
			if(a1->val > a2->val)
			{
				a1->inLIS = false;
//				cout << "a1=(" << a1->val << "," << a1->inLIS << ")" << "\t";
//				cout << "a2=(" << a2->val << "," << a2->inLIS << ")" << endl;
				a1 = a1->rn;
			}
			else
			if(a2->inLIS == false)
			{
				a2 = a2->rn;
				continue;
			}
			else
			{
				a1->inLIS = true;
//				cout << "a1=(" << a1->val << "," << a1->inLIS << ")" << "\t";
//				cout << "a2=(" << a2->val << "," << a2->inLIS << ")" << endl;
				a1 = a1->rn;
			}
		}
	}

	return 0;
}

long long int orthogonal::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_Nus = 1000*1000;

	return one_second_Nus/ (sum/ts);
}
long long int orthogonal::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

void orthogonal::log_running(){
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

string orthogonal::sum_running(){
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
