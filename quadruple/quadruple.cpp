/*
 * qnlist.cpp
 *
 *  Created on: 2015-9-15
 *      Author: liyouhuan
 */
#include "../datastream/datastream.h"
#include "quadruple.h"
qnlist::qnlist(int _n){
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

#ifdef QN_SEQ
	this->buf = new Vtype[this->win_size];
	for(int i = 0; i < this->win_size; i ++){
		this->buf[i] = 0;
	}

	this->buf_h = 0;
	this->buf_t = 0;
#endif
}
qnlist::~qnlist(){
	delete[] this->hlist;
	delete[] this->htail;
	delete[] this->tmp_left;
	delete[] this->tmp_right;
#ifdef QN_SEQ
	delete[] this->buf;
#endif
	delete[] this->S;
	this->run_log.close();
}
void qnlist::run(int run_method, string _data_f){
#ifdef DEBUG_TRACK
	cout << "begin qnlist run" << endl;
#endif

	if(run_method > 500) {
		cout << "run_method is too large" << endl;
	}
	string _method[500] = {
			"enum",
			"maxgap_one",
			"mingap_one",
			"maxwid_one",
			"minwid_one",
			"maxwei_one",
			"minwei_one",
			"slis",
			"rlis",
			"slis_nocolor",
			"rlis_nocolor",
			"maxlast",
			"minlast",
			"maxgap",
			"mingap"
	};

#ifdef OPT_SAVE
	this->v_save = 0;
	this->v_total = 0;
	this->v_dn = 0;
	this->v_un = 0;
#endif

#ifdef NUM_LIS
	this->num_lis = 0;
#endif

	this->t_insert.initial();
	this->t_remove.initial();
	this->t_update.initial();
	this->t_compute.initial();
	this->t_total.initial();

	string log_f = util::exp_home + "runningtime/";
	string file_name;
	stringstream label_type;
	string _tag = "otg";
	if(util::run_mode == 1)
	{
		_tag += "_prev";
	}

	{
		int i1 = _data_f.rfind('/');
		int i2 = _data_f.rfind('.');
		string _datatype = _data_f.substr(i1+1, i2-i1-1);
		stringstream _ss;
		_ss << "["+ _tag +"]";
		_ss << "_" << _method[run_method] << "";
		_ss << "_" << _datatype << ".time";
		_ss << "_" << this->win_size << "";
		file_name = _ss.str();
		log_f += file_name;
		label_type << _tag << "\t" << _method[run_method];
		if(run_method == 9 || run_method == 7){
			label_type << "-" << this->rslope;
		}
		if(run_method == 10 || run_method == 8){
			label_type << "-" << this->rli << "," << this->rui;
			label_type << "," << this->rlv << "," << this->ruv;
		}
		label_type << "\t" << _datatype << "\t" << this->win_size;
	}

#ifdef LOG
	this->run_log.open(log_f.c_str(), ios::out);
	if(! this->run_log){
		cout << log_f << " can not be opened" << endl;
		system("pause");
	}
#endif

	util::init_space = util::get_space();

//	int sliding = 0;
//	if(this->win_size == 3600){
//		sliding = 50;
//	}

	datastream ds(_data_f);
	while(ds.hasnext() && ds.timestamp() < this->win_size)
	{
		Vtype ai = ds.next();
		this->update(ai);
	}

	while(ds.hasnext())
	{
		double ai = ds.next();

//		{
#ifdef DEADCYCLE
			if(ds.timestamp() % 1 == 0)
			{
				stringstream _ss;
				_ss << "ts=" << ds.timestamp() << endl;
				util::log(_ss);
			}
#endif
//		}
		if(ds.timestamp() < this->win_size)
		{
			this->update(ai);
#ifdef NUM_LIS
			if(num_lis != 0){
				cout << "err num lis" << endl;
				exit(-1);
			}
#endif
			continue;
		}
		this->t_total.begin();
		this->t_update.begin();

		this->update(ai);
//		{
//			if(ds.timestamp() >= 3600 || true){
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

		if(ds.timestamp() > this->win_size + util::update_times){
			break;
		}

//		if(ds.timestamp() % 100 == 0) cout << "ts=" << ds.timestamp() << endl;
#ifdef TRACK_CRASH
		if(ds.timestamp() > 45){
			break;
		}
#endif
	}

	cout << label_type.str() << "\t" << this->sum_running() << endl;

#ifdef DEBUG_TRACK
	cout << "end qnlist run" << endl;
#endif
}

string qnlist::compute_str(int run_method){
	stringstream _ss_comp;
#ifdef DEADCYCLE
	{
		stringstream _ss;
		_ss << "IN computer_str: run_method=" << run_method << endl;
		util::log(_ss);
	}
#endif
	switch(run_method)
	{
		case qnlist::ENUM:
		{
			_ss_comp << "--enum:" << endl;

			this->enum_str(_ss_comp);
			this->num_new = this->count_lis();
			this->num_lis += this->num_new;
			{

				if(this->timestamp > this->win_size && util::isconsole)
				{
#if (defined NUM_LIS)
					/*
					cout << this->num_lis/(this->timestamp-this->win_size+0.0) << "-";
					cout << "" << this->num_new << "-";
					cout << this->lis_len;
					cout << "\n";
					if(this->timestamp % 100 == 0)
						cout << endl << "ts="  <<this->timestamp << endl;
					*/
#endif
				}
			}
			break;
		}
		case qnlist::MAXGAP_ONE:
		{
			_ss_comp << "--maxgap_one:" << endl;
			this->maxgap_one_str(_ss_comp);
			break;
		}
		case qnlist::MINGAP_ONE:
		{
			_ss_comp << "--mingap_one:" << endl;
			this->mingap_one_str(_ss_comp);
			break;
		}
		case qnlist::MAXWID_ONE:
		{
			_ss_comp << "--maxwid_one:" << endl;
			this->maxwid_one_str(_ss_comp);
			break;
		}
		case qnlist::MINWID_ONE:
		{
			_ss_comp << "--minwid_one:" << endl;
			this->minwid_one_str(_ss_comp);
			break;
		}
		case qnlist::MAXWEI_ONE:
		{
			/*maxweight is exactly maxweight_one*/
			_ss_comp << "--maxwei_one:" << endl;
			this->maxweight_str(_ss_comp);
			break;
		}
		case qnlist::MINWEI_ONE:
		{
			_ss_comp << "--minwei_one:" << endl;
			this->minweight_str(_ss_comp);
			break;
		}
		case 7:
		{
			_ss_comp << "--slis:\t";
			this->slope_str(_ss_comp, this->rslope);
			break;
		}
		case 8:
		{
			_ss_comp << "--rlis:\t";
			this->range_str(_ss_comp, this->rli, this->rui, this->rlv, this->ruv);
			break;
		}
		case 9:
		{
			_ss_comp << "--slis_nocolor:" << endl;
			this->slope_nocolor_str(_ss_comp, this->rslope);
			break;
		}
		case 10:
		{
			_ss_comp << "--rlis_nocolor:" << endl;
			this->range_nocolor_str(_ss_comp, this->rli, this->rui, this->rlv, this->ruv);
			break;
		}
		case 11:
		{
			_ss_comp << "--maxlast:" << endl;
			this->maxlast_str(_ss_comp);
			break;
		}
		case 12:
		{
			_ss_comp << "--minlast:" << endl;
			this->minlast_str(_ss_comp);
			break;
		}
		case 13:
		{
			_ss_comp << "--maxgap:" << endl;
			this->maxgap_str(_ss_comp);
			break;
		}
		case 14:
		{
			_ss_comp << "--mingap:" << endl;
			this->mingap_str(_ss_comp);
			break;
		}
		case qnlist::LIS_COUNT:
		{
			this->num_new = this->count_lis();
			this->num_lis += this->num_new;
		}
	}

#ifdef DEADCYCLE
	{
		stringstream _ss;
		_ss << "OUT computer_str: run_method=" << run_method << endl;
		util::log(_ss);
	}
#endif

	return _ss_comp.str();
}

string qnlist::get_method(int _i){


	return "deprecated";
}

void qnlist::run_debug(){
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

void qnlist::run_stream(){
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

void qnlist::run_microsoft(){
	cout << "begin run_microsoft" << endl;
//	util::initial();
	string microsoft_stock = "microsoft_stock.dat";
	datastream ds(microsoft_stock);
	int max_width_one = 3;
	int min_width_one = 4;
	while( ds.hasnext() )
	{
		int ai = ((int)(ds.next()));
		this->update(ai);

		util::log(this->compute_str(min_width_one).c_str(), "");
		stringstream _ss;
		_ss << "\nEnum:"<<this->seq_str()<<"\n" << endl;
		this->enum_str(_ss);
		util::log(_ss.str().c_str(), "");
	}
	cout << "end run_microsoft" << endl;
}
void qnlist::run_sloperange()
{
	cout << "begin run_microsoft" << endl;
//	util::initial();
	string microsoft_stock = "microsoft_stock.dat";
	datastream ds(microsoft_stock);
	int islope = 7;
	int irange = 8;
	int Li = 40, Ui = 60;
	double Lv = 0, Uv = 150;
	double tmpslope = 1.5;
	this->set_range(Li, Ui, Lv, Uv);
	this->set_slope(tmpslope);
	while(ds.hasnext())
	{
		int ai = ((int)(ds.next()));
		this->update(ai);

		util::log(this->compute_str(islope).c_str(), "");
//		util::log(this->compute_str(irange).c_str(), "");
		stringstream _ss;
		_ss << "\nSeq:"<<this->seq_str()<<"\nEnum\n" << endl;
		this->enum_str(_ss);
		_ss << "\ntoStr\n" << this->to_str() << endl;
		util::log(_ss.str().c_str(), "");
	}
	cout << "end run_microsoft" << endl;
}

#ifdef DEMO
void qnlist::demo(string _dataset, int _winsz)
{
	util::initial();
	util::run_mode = 0;
	util::update_times = 7858 - _winsz - 100;
	this->win_size = _winsz;
	/* load_data */
	vector<double> data_vec;
	vector<double> align_datavec;
	vector<double> maxgap_vec;
	vector<double> moving_vec;
	vector<double> lislen_vec;
	datastream ds(_dataset);
	ds.to_vec(data_vec);
	for(int i = 0; i < (int)data_vec.size(); i ++)
	{
		double ai = data_vec[i];
		this->update(ai);

		if(i < _winsz)
		{
			continue;
		}

		this->compute_str(qnlist::MAXGAP_ONE);
		this->movingAVG(data_vec, _winsz, i);
		align_datavec.push_back(ai);
		maxgap_vec.push_back(this->max_gap);
		moving_vec.push_back(this->moving_avg);
		lislen_vec.push_back(this->lis_len);
	}

	//write_pair("movavg", data_vec, moving_vec, _winsz, _dataset);
	//write_pair("maxgap", data_vec, maxgap_vec, _winsz, _dataset);
	//write_pair("lislen", data_vec, lislen_vec, _winsz, _dataset);
	vector<vector<double> > allvec;
	allvec.push_back(align_datavec);
	allvec.push_back(moving_vec);
	allvec.push_back(maxgap_vec);
	allvec.push_back(lislen_vec);
	write_all(allvec, _winsz, _dataset);

}

double qnlist::movingAVG(vector<double> _data, int _winsz, int _i)
{
	double _sum = 0;
	for(int i = _i-_winsz+1; i <= _i; i ++)
	{
		if(i < 0) continue;
		_sum += _data[i];
	}
	this->moving_avg = _sum / (_winsz+0.0);

	return this->moving_avg;
}

void qnlist::write_all(vector<vector<double> >& allvec, int _winsz, string _dataset)
{
	string _file = "all";
	{
		stringstream _ss;
		_ss << _file << "_" << _dataset << "_" << _winsz;
		_file = _ss.str();
	}

	ofstream fout(_file.c_str(), ios::out);
	int sz = (int)allvec[0].size();

	for(int i = 0; i < 365*5; i ++)
	{
		fout << i  << "\t";
		for(int j = 0; j < (int)(allvec.size()); j ++)
		{
			fout << setprecision(3) << "\t" << fixed << allvec[j][i+sz-3650];
		}
		fout << endl;
	}
	fout.close();
}
void qnlist::write_pair
(string _file, vector<double>& _data, vector<double>& _value, int _winsz, string _dataset)
{
	{
		stringstream _ss;
		_ss << _file << "_" << _dataset << "_" << _winsz;
		_file = _ss.str();
	}

	ofstream fout(_file.c_str(), ios::out);
	for(int i = 0; i < (int) _value.size(); i ++)
	{
		fout << i  << "\t" << _data[i+_winsz] << "\t" << _value[i] << endl;
	}
	fout.close();
	return;
}

#endif

int qnlist::update(double _ins){
#ifdef DEBUG_TRACK
	cout << "IN update" << endl;
#endif
	this->_it_tmp = this->remove();

	this->_it_tmp->initial(_ins, this->timestamp);
	this->insert(this->_it_tmp);

	this->timestamp ++;
#ifdef DEBUG_TRACK
	cout << "OUT update" << endl;
#endif
	return 0;
}
int qnlist::construction(vector<Vtype>& ivec){
	for(int i = 0; i < (int)ivec.size(); i ++)
	{
		this->insert(new item(ivec[i], i));
	}
	return 0;
}
int qnlist::LISlength()
{
	return this->lis_len;
}
string qnlist::to_str(){
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
int qnlist::to_size(){
	int sz = 0;
	sz += sizeof(qnlist);

	sz += sizeof(item*)*this->win_size;
	sz += sizeof(item*)*this->win_size;

	item* it = new item();
	sz += it->to_size() * this->win_size;
	delete it;

	return sz;
}
string qnlist::all_str(){
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
string qnlist::seq_str(){

	stringstream _ss("");
#ifdef QN_SEQ
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
#endif
	return _ss.str();
}
string qnlist::enum_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << "\t" << hlist[0]->val << endl;
		return "";
	}

	item* it = hlist[this->lis_len-1];
#ifdef DEADCYCLE
	int count_start = 0;
#endif
	int count_lis_num = 0;
	while(it != NULL)
	{
#ifdef DEADCYCLE
		{
			stringstream _ss;
			count_start ++;
			_ss << count_start << " ending" << endl;
			util::log(_ss);
		}
#endif
		this->ending_str(_ss, it, count_lis_num);
		it = it->rn;
	}
	return "";
}
string qnlist::maxgap_str(stringstream & _ss){
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
string qnlist::mingap_str(stringstream & _ss){
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
		if(it ->un == NULL){
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
					if(this->S[k-1] ->un == NULL){
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
string qnlist::maxfirst_str(stringstream & _ss){
#ifdef INLIS
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
#endif
	return "err max first_str";
}
string qnlist::minfirst_str(stringstream & _ss){
#ifdef INLIS
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
#endif
	return "err min first_str";
}
string qnlist::maxlast_str(stringstream & _ss){
	int num_lis = 0;
	this->ending_str(_ss, this->hlist[this->lis_len-1], num_lis);
	return "";
}
string qnlist::minlast_str(stringstream & _ss){
	int num_lis = 0;
	this->ending_str(_ss, this->htail[this->lis_len-1], num_lis);
	return "";
}
string qnlist::maxgap_one_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_rmmost();
	double _max_gap = -1;
	item * it = this->hlist[this->lis_len-1];
	_max_gap = it->val - it->rm_most->val;
	it = it->rn;
	while(it != NULL){
		if(_max_gap < it->val - it->rm_most->val){
			_max_gap = it->val - it->rm_most->val;
		}
		it = it->rn;
	}

#ifdef DEMO
		this->max_gap = _max_gap;
		return "";
#endif

	_ss << _max_gap << ":" << endl;

	it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->val - it->rm_most->val != _max_gap){
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
string qnlist::mingap_one_str(stringstream & _ss){
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_lmmost();
	item * it = this->hlist[this->lis_len-1];
	double _min_gap = it->val - (it->lm_most)->val;
	it = it->rn;
	while(it != NULL){
		if(_min_gap > it->val - it->lm_most->val){
			_min_gap = it->val - it->lm_most->val;
		}
		it = it->rn;
	}

#ifdef DEMO
		this->min_gap = _min_gap;
		return "";
#endif

	it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->val - it->lm_most->val != _min_gap){
			it = it->rn;
			continue;
		}

		int LM_ST = it->lm_most->val;
		this->S[0] = it;
		if(it ->un == NULL){
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
					if(this->S[k-1] ->un == NULL){
						cout << "err rlen2" << endl;
						cout << "k=" << k << endl;
					}
					this->S[k] = this->S[k-1]->child_lm();
					k ++;
					continue;
				}
				else
				{
					_ss << this->S[this->lis_len-1]->val;
					for(int i = this->lis_len-2; i >= 0; i --){
						_ss << "\t" << this->S[i]->val;
					}
//					_ss << endl;
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


string qnlist::minwid_one_str(stringstream & _ss)
{
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_rmmost();
	int _min_wid = -1;
	item * it = this->hlist[this->lis_len-1];
	_min_wid = it->timestamp - it->rm_most->timestamp;
	it = it->rn;
	while(it != NULL){
		if(_min_wid > it->timestamp - it->rm_most->timestamp){
			_min_wid = it->timestamp - it->rm_most->timestamp;
		}
		it = it->rn;
	}

#ifdef DEMO
		this->min_wid = _min_wid;
		return "";
#endif

//	_ss << min_wid << ":" << endl;

	it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->timestamp - it->rm_most->timestamp != _min_wid){
			it = it->rn;
			continue;
		}
		int RM_ST = it->rm_most->timestamp;
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
			if(this->S[k-1]->partial(this->S[k-2]) && _ay_tmp->timestamp == RM_ST)
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
string qnlist::maxwid_one_str(stringstream & _ss)
{
	if(this->lis_len == 1){
		_ss << this->hlist[0]->val;
		return "";
	}

	this->update_lmmost();
	item * it = this->hlist[this->lis_len-1];
	double _max_wid = it->timestamp - (it->lm_most)->timestamp;
	it = it->rn;
	while(it != NULL){
		if(_max_wid < it->timestamp - it->lm_most->timestamp){
			_max_wid = it->timestamp - it->lm_most->timestamp;
		}
		it = it->rn;
	}

#ifdef DEMO
		this->max_wid = _max_wid;
		return "";
#endif

	it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->timestamp - it->lm_most->timestamp != _max_wid){
			it = it->rn;
			continue;
		}

		int LM_ST = it->lm_most->timestamp;
		this->S[0] = it;
		if(it ->un == NULL){
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
			if(this->S[k-1]->partial(this->S[k-2]) && _ay_tmp->timestamp == LM_ST)
			{
				if(k < this->lis_len)
				{
					if(this->S[k-1] ->un == NULL){
						cout << "err rlen2" << endl;
						cout << "k=" << k << endl;
					}
					this->S[k] = this->S[k-1]->child_lm();
					k ++;
					continue;
				}
				else
				{
					_ss << this->S[this->lis_len-1]->val;
					for(int i = this->lis_len-2; i >= 0; i --){
						_ss << "\t" << this->S[i]->val;
					}
//					_ss << endl;
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

void qnlist::range_color(int Li, int Ui, double Lv, double Uv)
{
	for(int i = 0; i < this->lis_len; i ++){
		item* it = this->hlist[i];
		while(it != NULL){
			it->black = false;
			it = it->rn;
		}
	}

	for(int i = 1; i < this->lis_len; i ++)
	{
		item* it_ak = this->hlist[i-1];
		item* it_ai = this->hlist[i];
		while(it_ai != NULL)
		{
			if(it_ak == NULL)
			{
				it_ai->black = true;
				it_ai = it_ai->rn;
				continue;
			}
			bool ok_vlow = (it_ai->val-it_ak->val)>=Lv;
			bool ok_vup = (it_ai->val-it_ak->val)<=Uv;
			bool ok_ilow = (it_ai->timestamp-it_ak->timestamp)>=Li;
			bool ok_iup = (it_ai->timestamp-it_ak->timestamp)<=Ui;
			if(!it_ak->black && ok_vlow && ok_iup)
			{
				if(!ok_vup || !ok_ilow){
					it_ai->black = true;
				}
				it_ai = it_ai->rn;
				continue;
			}
			it_ak = it_ak->rn;
		}
	}
}
void qnlist::slope_color(double m_slope)
{
	for(int i = 0; i < this->lis_len; i ++){
		item* it = this->hlist[i];
		while(it != NULL){
			it->black = false;
			it = it->rn;
		}
	}


	for(int i = 1; i < this->lis_len; i ++)
	{
		item* it_ak = this->hlist[i-1];
		item* it_ai = this->hlist[i];
		while(it_ai != NULL)
		{
			if(it_ak == NULL)
			{
				it_ai->black = true;
				it_ai = it_ai->rn;
				continue;
			}
			bool ok_before = (it_ak->timestamp<it_ai->timestamp);
			double i_slope = (it_ai->val-it_ak->val)/(it_ai->timestamp-it_ak->timestamp+0.0);
			bool ok_slope = (i_slope >= m_slope) && it_ak->black==false;
			if(ok_before && !ok_slope)
			{
				it_ak = it_ak->rn;
				continue;
			}
			if(!ok_before){
				it_ai->black = true;
			}
			it_ai = it_ai->rn;
		}
	}
}

string qnlist::range_str(stringstream & _ss, int Li, int Ui, double Lv, double Uv)
{
	this->range_color(Li, Ui, Lv, Uv);
	this->no_black_str(_ss);
	return "";
}
string qnlist::slope_str(stringstream & _ss, double m_slope)
{
	this->slope_color(m_slope);
	this->no_black_str(_ss);
	return "";
}

string qnlist::range_nocolor_str(stringstream & _ss, int Li, int Ui, double Lv, double Uv)
{
	item* it_last = this->hlist[this->lis_len-1];
	while(it_last != NULL)
	{
		if(this->lis_len == 1){
			_ss << it_last->val << endl;
			return _ss.str();
		}
		this->S[0] = it_last;
		this->S[1] = it_last->un;

		if(! this->range_satisfy(this->S[0], this->S[1], Li, Ui, Lv, Uv))
		{
			it_last = it_last->rn;
			continue;
		}

		int k = 2;
		while(k > 1)
		{
			if(this->S[k-1] == NULL)
			{
				k --;
				this->S[k-1] = this->S[k-1]->ln;
			}
			else
			if(this->S[k-1]->before(this->S[k-2]) &&
			   this->S[k-1]->val <= this->S[k-2]->val &&
			   this->range_satisfy(this->S[k-2], this->S[k-1], Li, Ui, Lv, Uv))
			{
				if(this->S[k-1]->un == NULL)
				{
					for(int i = this->lis_len-1; i >= 0; i --)
					{
						_ss << "\t" << this->S[i]->val; // save time
					}
					return _ss.str();
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

		it_last = it_last->rn;
	}
	return "";
}
string qnlist::slope_nocolor_str(stringstream & _ss, double m_slope)
{
	item* it_last = this->hlist[this->lis_len-1];
	while(it_last != NULL)
	{
		if(this->lis_len == 1){
			_ss << it_last->val << endl;
			return _ss.str();
		}
		this->S[0] = it_last;
		this->S[1] = it_last->un;

		if(! this->slope_satisfy(this->S[0], this->S[1], m_slope))
		{
			it_last = it_last->rn;
			continue;
		}

		int k = 2;
		while(k > 1)
		{
			if(this->S[k-1] == NULL)
			{
				k --;
				this->S[k-1] = this->S[k-1]->ln;
			}
			else
			if(this->S[k-1]->before(this->S[k-2]) &&
			   this->S[k-1]->val <= this->S[k-2]->val &&
			   this->slope_satisfy(this->S[k-2], this->S[k-1], m_slope))
			{
				if(this->S[k-1]->un == NULL)
				{
					for(int i = this->lis_len-1; i >= 0; i --)
					{
						_ss << "\t" << this->S[i]->val; // save time
					}

					return _ss.str();
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

		it_last = it_last->rn;
	}
	return "";
}

void qnlist::no_black_str(stringstream & _ss)
{
	item* it = this->hlist[this->lis_len-1];
	while(it != NULL)
	{
		if(it->black == false)
			break;
		it = it->rn;
	}
	if(it == NULL){
		_ss << "None";
		return;
	}

	int itmp = 0;
	item * it_un;
	while(itmp < this->lis_len)
	{
		S[itmp] = it;
		_ss << S[itmp]->val << "\t";
		itmp ++;
		it_un = it->un;
		while(it_un != NULL){
			if(it_un->black == false){
				it = it_un;
				break;
			}
			it_un = it_un->ln;
		}
		if(it_un == NULL && itmp != this->lis_len){
			cout << "error it_un"<< itmp << " len="
				<< this->lis_len << " at " << this->timestamp << endl;
			system("pause");
		}
	}
}

bool qnlist::range_satisfy
(item* it_ai, item* it_ak, int Li, int Ui, double Lv, double Uv)
{
	bool ok_vlow = (it_ai->val-it_ak->val)>=Lv;
	bool ok_vup = (it_ai->val-it_ak->val)<=Uv;
	bool ok_ilow = (it_ai->timestamp-it_ak->timestamp)>=Li;
	bool ok_iup = (it_ai->timestamp-it_ak->timestamp)<=Ui;
	return ok_vlow && ok_vup && ok_ilow && ok_iup;
}

bool qnlist::slope_satisfy
(item* it_ai, item* it_ak, double m_slope)
{
	double i_slope = (it_ai->val-it_ak->val)/(it_ai->timestamp-it_ak->timestamp+0.0);
	return (i_slope >= m_slope);
}

void qnlist::set_range(int Li, int Ui, double Lv, double Uv)
{
	this->rli = Li;
	this->rui = Ui;
	this->rlv = Lv;
	this->ruv = Uv;
}
void qnlist::set_slope(double _slope)
{
	this->rslope = _slope;
}

string qnlist::maxweight_str(stringstream & _ss){
	item* it = this->hlist[this->lis_len-1];
	this->S[0] = it;
	int i = 0;
	while(it->un != NULL)
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
string qnlist::minweight_str(stringstream & _ss){
	this->S[0] = this->htail[this->lis_len-1];
	int i = 1;
	while(i < this->lis_len)
	{
		this->S[i] = this->S[i-1]->un;
		i ++;
	}
	_ss << this->S[this->lis_len-1]->val;
	for(i = this->lis_len-2; i >=0; i --){
		_ss << "\t" << this->S[i]->val;
	}
//	_ss << endl;

	return "";
}

double qnlist::count_lis()
{
	double _count = 0;
	for(int i = 0; i < this->lis_len; i ++)
	{
		item* _it = hlist[i];
		while(_it != NULL)
		{
			double _tmp_c = _it->count_is_num();
			if(i == this->lis_len-1)
			{
				_count += _tmp_c;
			}

			_it = _it->rn;
		}
	}

	this->num_new = _count;
	return this->num_new;
}

int qnlist::get_timestamp()
{
	return this->timestamp;
}

/* private */
int qnlist::insert(item* _ins){
	this->t_insert.begin();

	gtmpi = this->find_ins_pos(_ins->val);
	/* horizontal */
	if(hlist[gtmpi] == NULL)
	{
		hlist[gtmpi] = _ins;
		htail[gtmpi] = _ins;
		this->lis_len ++;
	}
	else
	{
		htail[gtmpi]->rn = _ins;
		_ins->ln = htail[gtmpi];
		htail[gtmpi] = _ins;
	}
	/* vertical */
	if(gtmpi-1 >= 0)
	{
		_ins->un = htail[gtmpi-1];
	}
	if(gtmpi+1 < this->lis_len)
	{
		_ins->dn = htail[gtmpi+1];
	}
#ifdef RLEN
	_ins->rlen = gtmpi+1;
#endif

#ifdef QN_SEQ
	{/* debug */
		this->buf[this->buf_t] = _ins->val;
		this->buf_t = (this->buf_t + 1) % this->win_size;
	}
#endif

	this->t_insert.end();
	return 0;
}
/* here each hlist is strictly decreasing */
int qnlist::find_ins_pos(double _val){
//	int i = 0;
	{/*
		while(hlist[i] != NULL && htail[i]->val <= _val)
		{
			i ++;
		}
		return i; */
	}

	gtmpibegin = 0;
	gtmpiend = this->lis_len;

	while(gtmpibegin < gtmpiend)
	{
//		if(htail[tmpibegin]->val > _val) return tmpibegin;
//		if(htail[tmpiend-1]->val <= _val) return tmpiend;

		gtmpimid = (gtmpibegin + gtmpiend)/2;
		if(htail[gtmpimid]->val <= _val)
		{
			gtmpibegin = gtmpimid + 1;
		}
		else
		if(gtmpimid > 0 && htail[gtmpimid-1]->val > _val)
		{
			gtmpiend = gtmpimid - 1;
		}
		else
		{
			return gtmpimid;
		}
	}

	return gtmpibegin;
}
item* qnlist::remove(){
#ifdef DEADCYCLE
	{
		stringstream _ss;
		_ss << "IN remove" << endl;
		util::log(_ss);
	}
#endif

	this->t_remove.begin();

	if(this->timestamp < this->win_size)
	{
		this->t_remove.end();
		return new item();
	}



	item* a_remove = hlist[0];

#ifdef DN_OPT
	cout << "Before: " << endl;
	cout << this->to_str() << endl;
#endif


//#ifdef QN_PREV
	if(util::run_mode == 1)
	{
		this->h_adjust();
		this->v_adjust();
	}
//#else
	else
	{
		this->remove_adjust();
	}
//#endif


	this->t_remove.end();

#ifdef QN_SEQ
	{/* debug */
		this->buf_h = (this->buf_h + 1) % this->win_size;
	}
#endif

#ifdef DN_OPT
	cout << "After: " << endl;
	cout << this->to_str() << endl;
#endif

#ifdef DEADCYCLE
	{
		stringstream _ss;
		_ss << "OUT remove" << endl;
		util::log(_ss);
	}
#endif

	return a_remove;
}

/* similar codes with that of h/v_adjust() */
int qnlist::remove_adjust()
{
#ifdef DN_OPT
	cout << this->seq_str() << endl;
#endif
	this->remove_divide(this->tmp_left, this->tmp_right);

	this->remove_join(this->tmp_left, this->tmp_right);

//	this->update_rlen_tail();
#ifdef OPT_SAVE
	this->vopt_adjust(this->tmp_left, this->tmp_right);
#else
	this->v_adjust();
#endif

	return 0;
}

/* tail of leftpart & head of the _rightpart */
int qnlist::remove_divide(item** _left, item** _right)
{
	_ax_tmp = hlist[0];
	int Li = 0;
	while(true)
	{
		if(_ax_tmp->rn == NULL)
		{
			while(Li < this->lis_len)
			{
				_left[Li] = this->htail[Li];/* tail of left part */
				_right[Li] = NULL;
				Li ++;
			}
			break;
		}
		else
		{
			_left[Li] = _ax_tmp;
			_right[Li] = _ax_tmp->rn;
			_ay_tmp = (_ax_tmp->rn)->dn;
			if(_ay_tmp == NULL)
			{
				Li ++;
				while(Li < this->lis_len)
				{
					_left[Li] = NULL;
					_right[Li] = this->hlist[Li]; /* head of right part */
					Li ++;
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
	return 0;
}
/* tail of leftpart & head of the _rightpart */
int qnlist::remove_join(item** _left, item** _right)
{
	for(int i = 0; i < this->lis_len-1; i ++)
	{
		if(_left[i+1] == NULL)
		{
			this->hlist[i] = _right[i];
			this->hlist[i]->ln = NULL;
#ifndef QN_PREV
			/* htail[i] remains */
#endif
		}
		else
		if(_right[i] == NULL)
		{
			this->hlist[i] = this->hlist[i+1];
//#ifndef QN_PREV
			if(util::run_mode != 1)
			{
				this->htail[i] = this->htail[i+1];
			}
//#endif
		}
		else
		{
			this->hlist[i] = this->hlist[i+1];
			_left[i+1]->rn = _right[i];
			_right[i]->ln = _left[i+1];
#ifndef QN_PREV
			/* htail[i] remains */
#endif
		}
	}
	/* for last right part of the last hlist */
#ifdef OPT_SAVE
		this->pre_len = this->lis_len;
#endif
	if(_right[this->lis_len-1] != NULL)
	{
		this->hlist[this->lis_len-1] = _right[this->lis_len-1];
		this->hlist[this->lis_len-1]->ln = NULL;
	}
	else
	{
		this->hlist[this->lis_len-1] = NULL;
//#ifndef QN_PREV
		if(util::run_mode != 1)
		{
			this->htail[this->lis_len-1] = NULL;
		}
//#endif
		this->lis_len --;
	}

	return 0;
}

/* tail of leftpart & head of the _rightpart
 * saved items:
 * */
int qnlist::vopt_adjust(item** _left, item** _right)
{
	/* update the un of new_hlist[0] */
	_it_tmp = this->hlist[0];
	while(_it_tmp != _right[0]){//=======================m1
		_it_tmp->un = NULL;
		_it_tmp = _it_tmp->rn;
#ifdef OPT_SAVE
		this->v_total ++;
#endif
	}

#ifdef OPT_SAVE
	/* up neighbor */
	for(int i = 2; i<this->pre_len && _left[i]!=NULL && _right[i-2]!=NULL; i ++)
	{
		_ah_tmp = _left[i-1];
		_ax_tmp = _ah_tmp->dn;
		if(_ax_tmp == NULL){
			_ax_tmp = this->hlist[i-1];
		}else{
			_ax_tmp = _ax_tmp->rn;
		}
		while(_ax_tmp != _left[i]->rn)
		{
			this->v_save ++;
			this->v_total += 2;
			this->v_un ++;

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
		}//while
		//num_save --; // for the leftmost to be adjusted
	}//for

#else
	for(int i = 1; i < this->lis_len; i ++)
	{
		_ah_tmp = this->hlist[i-1];
		_ax_tmp = this->hlist[i];
		// could be removed:  '_ah_tmp != NULL'
		while(_ah_tmp != NULL && _ax_tmp != NULL)
		{
			if(_ax_tmp->timestamp > _ah_tmp->timestamp) //if(_ax_tmp->after(_ah_tmp))
			{
				if(_ah_tmp->rn == NULL)
				{
					_ax_tmp->un = _ah_tmp;
					_ax_tmp = _ax_tmp->rn;
				}
				else
				if(_ax_tmp->timestamp > (_ah_tmp->rn)->timestamp) //if(_ax_tmp->after(_ah_tmp->rn))
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

#endif //OPT_SAVE

#ifdef OPT_SAVE
	/* down neighbor */
	for(int i = 0; i<this->pre_len-1 && _right[i]!=NULL && _left[i+1]!=NULL; i ++)
	{
		if(_right[i+1] != NULL)
		{
			_ax_tmp = _right[i+1]->un;
			_ah_tmp = _right[i+1]->ln;
		}
		else
		{
			_ax_tmp = this->htail[i];
			_ah_tmp = this->htail[i+1];
		}

		while(_ax_tmp != _left[i+1])
		{
			this->v_total +=2;
			this->v_dn ++;

			if(_ax_tmp == NULL)
			{
				cout << "err _ax_tmp " << endl;
				system("pause");
			}
			if(_ah_tmp == NULL)
			{
				_ax_tmp->dn = NULL;
				_ax_tmp = _ax_tmp->ln;
			}
			else
			if(_ax_tmp->timestamp > _ah_tmp->timestamp)
			{
				_ax_tmp->dn = _ah_tmp;
				_ax_tmp = _ax_tmp->ln;
			}
			else
			{
				_ah_tmp = _ah_tmp->ln;
			}
		}

#ifdef DN_OPT		item* _litmp = this->hlist[i];
//		if(_left[i+1] != NULL)
//		{
//			cout << "left[" << i+1<< "]: " << _left[i+1]->to_str() << endl;
//		}
//		else
//		{
//			cout << "left[" << i+1<< "]: NULL" << endl;
//		}
//		if(_right[i] != NULL)
//		{
//			cout << "right[" << i << "]: " << _right[i]->to_str() << endl;
//		}
//		else
//		{
//			cout << "right[" << i << "]: NULL" << endl;
//		}
		while(_litmp != NULL)
		{
			cout << "begin" << endl;
			if(_litmp->dn == NULL)
			{
				if(_litmp->timestamp > this->hlist[i+1]->timestamp)
				{
					cout << "dn bug i=" << i << " " << _litmp->to_str() << endl;
				}
			}
			else
			if(_litmp->dn->timestamp > _litmp->timestamp)
			{
				cout << "dn bug2 i=" << i << " " << _litmp->to_str() << endl;
			}
			else
			if(_litmp->dn->rn != NULL)
			{
				if(_litmp->dn->rn->timestamp < _litmp->timestamp)
				{
					cout << "dn bug3 i=" << i << " " << _litmp->to_str() << endl;
				}
			}
			_litmp = _litmp->rn;
			cout << "end" << endl;
		}
#endif

	}
#else
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
#endif

	/* update dn of the last hlist */
	_it_tmp = this->hlist[this->lis_len-1];
	while(_it_tmp != NULL){
		_it_tmp->dn = NULL;
		_it_tmp = _it_tmp->rn;
	}



	return 0;
}

int qnlist::h_adjust(){

	this->remove_divide(this->tmp_left, this->tmp_right);

	this->remove_join(this->tmp_left, this->tmp_right);

	this->update_rlen_tail();

	return 0;
}

int qnlist::v_adjust(){
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
int qnlist::update_rlen_tail(){
	int i;
	/* update rlen */
	for(i = 0; i < this->lis_len; i ++)
	{
		_it_tmp = this->hlist[i];
#ifndef RLEN
		while(_it_tmp->rn != NULL) _it_tmp = _it_tmp->rn;
		this->htail[i] = _it_tmp;
#else
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
#endif
	}
	/*  */
//	for(; i < this->win_size; i ++)
//	{
//		this->hlist[i] = NULL;
//		this->htail[i] = NULL;
//	}


	return 0;
}

string qnlist::starting_str(item* it_first, stringstream & _ss){
#ifdef INLIS
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
#endif
	return "";
}
item* qnlist::rn_of_dn(item* it){

//	if(it->dn == this->lis_len){
//		cout << "err rn of dn" << endl;
//		return NULL;
//	}

	item* ak = NULL;
#ifdef INLIS
	if(it->dn != NULL){
		ak = (it->dn)->rn;
	}
	if(ak == NULL){
		ak = this->hlist[it->rlen-1 + 1];
	}
#endif
	return ak;
}
string qnlist::ending_str(item* it_last){
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
			if(this->S[k-1]->un == NULL)
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

void qnlist::ending_str(stringstream& _ss, item* it_last, int& _cur_num_lis){
	if(this->lis_len == 1){
		_ss << it_last->val << endl;
		return ;
	}
	this->S[0] = it_last;
	this->S[1] = it_last->un;
	int k = 2;
#ifdef DEADCYCLE
	util::log("start log str", "\n");
	util::log(this->to_str().c_str(), "\n");
	util::log("end log str", "\n");
#endif
	while(k > 1)
	{
//#ifdef DEADCYCLE
//		{
//			stringstream _ss;
//			for(int tmpk = 0; tmpk < k; tmpk ++)
//			{
//				_ss << tmpk << "\t";
//			}
//			_ss << k << endl;
//			util::log(_ss);
//		}
//#endif
		if(this->S[k-1] == NULL)
		{
			k --;
			this->S[k-1] = this->S[k-1]->ln;
		}
		else
		if(this->S[k-1]->before(this->S[k-2]) &&
		   this->S[k-1]->val <= this->S[k-2]->val)
		{
			if(this->S[k-1]->un == NULL)
			{
				_cur_num_lis ++;
#ifdef ENABLE_MAX_LIS_NUM
				if(_cur_num_lis > util::MAX_LIS_NUM)
					break;
#endif

#ifdef NUM_LIS
				this->num_lis ++;
#endif
				/*
				for(int i = this->lis_len-1; i >= 0; i --)
				{
					_ss << "\t" << this->S[i]->val; // save time
				}
				_ss << endl; // save time
				*/
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

int qnlist::update_rmmost(){
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
int qnlist::update_lmmost(){
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
			if(it->un == NULL){
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
int qnlist::update_inLIS(){
#ifdef INLIS
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
#endif
	return 0;
}

long long int qnlist::throughput_total(){
	double ts = this->timestamp;
	double sum = this->t_total.getsum();
	double one_second_Nus = 1000*1000;

	return one_second_Nus/ (sum/ts);
}
long long int qnlist::throughput_update(){
	double ts = this->timestamp;
	double sum = this->t_update.getsum();
	double one_second_us = 1000*1000;

	return one_second_us * ts /sum;
}

void qnlist::log_running(){
#ifdef LOG
	if(this->timestamp == 1 + this->win_size)
	{
		stringstream _ss;
		_ss << "ts";
		_ss << "	total_sum	total_cur";
		_ss << "	compu_sum	compu_cur";
		_ss << "	updat_sum	updat_cur";
		_ss << "	inser_sum	inser_cur";
		_ss << "	remov_sum	remov_cur";
		_ss << endl;
		this->run_log << _ss.str();
	}

#ifdef OPT_SAVE
	if(util::run_mode == 1)//qn_prev
	{
		this->v_save = -1;
		this->v_total = 1;
	}
#endif
	stringstream _ss;
	_ss << this->timestamp;
	_ss << "\t" << this->t_total.getsum() << "\t" << this->t_total.getcur();
	_ss << "\t" << this->t_compute.getsum() << "\t" << this->t_compute.getcur();
	_ss << "\t" << this->t_update.getsum() << "\t" << this->t_update.getcur();
	_ss << "\t" << this->v_total << "\t" << (this->v_save/(this->v_total+0.0));
	_ss << "\t" << this->num_lis << "\t" << (this->num_lis/(this->timestamp-this->win_size+0.0));
	_ss << endl;

	if(util::isconsole) {
		_ss << endl << "***************************" << endl;
		_ss << "vdn=" << this->v_dn << " *2=" << this->v_dn*2 << endl;
		_ss << "vun=" << this->v_un << " *2=" << this->v_un*2 << endl;
		_ss << "vdn+vun=" << this->v_dn+this->v_un << " *2=" << (this->v_dn+this->v_un)*2 << endl;
		_ss << "vsave=" << this->v_save << endl;
		_ss << "vtotal=" << this->v_total << endl;
		_ss << "num_lis=" << this->num_lis << " avg/" << util::update_times << " ="
				<< this->num_lis/(util::update_times+0.0) << endl;
		cout << _ss.str() << endl;
	}

	this->run_log << _ss.str();
	this->run_log.flush();
#endif
}

string qnlist::sum_running(){

	stringstream _ss;
	if(util::isconsole)
	{
		_ss << endl;
		_ss << "vdn=" << this->v_dn << " *2=" << this->v_dn*2 << endl;
		_ss << "vun=" << this->v_un << " *2=" << this->v_un*2 << endl;
		_ss << "vdn+vun=" << this->v_dn+this->v_un << " *2=" << (this->v_dn+this->v_un)*2 << endl;
		_ss << "vsave=" << this->v_save << endl;
		_ss << "vtotal=" << this->v_total << endl;
		_ss << "num_lis=" << this->num_lis << " avg/" << util::update_times << " ="
				<< this->num_lis/(util::update_times+0.0) << endl;
	}
	_ss << "" << this->t_total.getsum() << "\t";
	_ss << "" << this->v_save << "\t";
	_ss << "" << this->v_total << "\t";
#ifdef OPT_SAVE
	if(util::run_mode == 1)//qn_prev
	{
		this->v_save = 1;
		this->v_total = 1;
	}
	_ss << "" << (this->v_save/(this->v_total+0.0)) << "\t";
#endif
#ifdef NUM_LIS
	_ss << "" << (this->num_lis/(this->timestamp-this->win_size+0.0)) << "\t";
#endif
	_ss << "" << this->to_size() << "\t";
	_ss << "" << this->throughput_total() << "\t";
	_ss << "" << this->throughput_update() << "\t";
	_ss << "" << this->t_total.getavg() << "\t";
	_ss << "" << this->t_compute.getavg() << "";

	return _ss.str();
 }
