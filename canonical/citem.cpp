/*
 * citem.cpp
 *
 *  Created on: 2015Äê9ÔÂ23ÈÕ
 *      Author: liyouhuan
 */
#include "citem.h"
#include "canonical.h"
citem::citem(Vtype _val, int _ts, int _winsz){
	this->initial(_val, _ts, _winsz);
}
citem::~citem(){
	delete[] this->pred;
	delete[] this->succ;
}

string citem::to_str(){
	stringstream _ss;
	_ss << "(v=" << this->val << ", ";
	_ss << "r=" << this->rlen << ", ";
	_ss << "d=" << this->inD;

	if(this->prev != NULL){
		_ss << ", p=" << this->prev->val;
	}
	if(this->next != NULL){
		_ss << ", n=" << this->next->val;
	}
	_ss << ")";
	return _ss.str();
}

int citem::to_size(){
	int sz = 0;

	sz += sizeof(citem*) * this->win_size;
	sz += sizeof(citem*) * this->win_size;

	sz += sizeof(citem);
	return sz;
}

int citem::insert_pred(citem* it){
	if(this->lpred > this->win_size){
		cout << "err insert_pred" << endl;
		exit(0);
	}

	this->pred[this->lpred] = it;
	this->lpred ++;

	return 0;
}

int citem::insert_succ(citem* it){
	if(this->lsucc > this->win_size){
		cout << "err insert_succ" << endl;
		exit(0);
	}

	this->succ[this->lsucc] = it;
	this->lsucc ++;

	return 0;
}

int citem::enumlis(int _lis_l, int _i, citem** _S, stringstream& _ss, int& lis_num){
	_S[_i] = this;
	if(_i == _lis_l -1)
	{
		/*
		for(int i = _lis_l-1; i >= 0; i --)
		{
			_ss << "\t" << _S[i]->val;
		}
		*/
		lis_num ++;
		_ss << endl;
		return 0;
	}

	citem* child = this->pred[this->rlen-2];
	while(child != NULL)
	{
		if(! child->partial(this)){
			break;
		}
		child->enumlis(_lis_l, _i+1, _S, _ss, lis_num);
#ifdef ENABLE_MAX_LIS_NUM
		if(lis_num > util::MAX_LIS_NUM)
		{
			break;
		}
#endif

		child = child->prev;
	}
	return 0;
}

int citem::enumlis_constrained(int _lis_l, int _i, citem** _S, stringstream& _ss, citem** _Sstore, int _constrained, Vtype& _value, int& lis_num){
	_S[_i] = this;
	if(_i == _lis_l -1)
	{
#ifdef ENABLE_MAX_LIS_NUM
		if(lis_num++ > util::MAX_LIS_NUM) return 0;
#endif

#ifdef ENUMSTR
		for(int i = _lis_l-1; i >= 0; i --)
		{
			_ss << "\t" << _S[i]->val;
		}
#endif
		if(_constrained == canonical::mingap)/* min height */
		{

			Vtype min_h = _S[0] - _S[_lis_l-1];
			if(_value > min_h)
			{
				for(int i = _lis_l-1; i >= 0; i --)
				{
					_Sstore[i]->val = _S[i]->val;
				}
				_value = min_h;
			}
			_ss << endl;
		}
		else
		if(_constrained == canonical::maxgap)/* max height */
		{

			Vtype max_h = _S[0] - _S[_lis_l-1];
			if(_value < max_h)
			{
				for(int i = _lis_l-1; i >= 0; i --)
				{
					_Sstore[i]->val = _S[i]->val;
				}
				_value = max_h;
			}
			_ss << endl;
		}
		else
		if(_constrained == canonical::minwei)/* min weight */
		{
			Vtype min_w = 0;
			for(int i = _lis_l-1; i >= 0; i --)
			{
				min_w += _S[i]->val;
			}
			if(_value > min_w)
			{
				for(int i = _lis_l-1; i >= 0; i --)
				{
					_Sstore[i]->val = _S[i]->val;
				}
				_value = min_w;
			}
			_ss << endl;
		}
		else
		if(_constrained == canonical::maxwei)/* max weight */
		{
			Vtype max_w = 0;
			for(int i = _lis_l-1; i >= 0; i --)
			{
				max_w += _S[i]->val;
			}
			if(_value < max_w)
			{
				for(int i = _lis_l-1; i >= 0; i --)
				{
					_Sstore[i]->val = _S[i]->val;
				}
				_value = max_w;
			}
			_ss << endl;
		}
		else
		if(_constrained == canonical::minwid)/* min width */
		{
			Vtype min_wid = 0;
			min_wid = _S[_lis_l-1]->timestamp - _S[0]->timestamp;
			if(_value > min_wid)
			{
				for(int i = _lis_l-1; i >= 0; i --)
				{
					_Sstore[i]->val = _S[i]->val;
				}
				_value = min_wid;
			}
			_ss << endl;
		}
		else
		if(_constrained == canonical::maxwid)/* max width */
		{
			Vtype max_wid = 0;
			max_wid = _S[_lis_l-1]->timestamp - _S[0]->timestamp;
			if(_value < max_wid)
			{
				for(int i = _lis_l-1; i >= 0; i --)
				{
					_Sstore[i]->val = _S[i]->val;
				}
				_value = max_wid;
			}
			_ss << endl;
		}
		return 0;
	}

	citem* child = this->pred[this->rlen-2];
	while(child != NULL)
	{
		if(! child->partial(this)){
			break;
		}
		child->enumlis(_lis_l, _i+1, _S, _ss, lis_num);
#ifdef ENABLE_MAX_LIS_NUM
		if(lis_num > util::MAX_LIS_NUM) break;
#endif

		child = child->prev;
	}
	return 0;
}

bool citem::partial(citem* it){
	return this->val <= it->val && this->timestamp < it->timestamp;
}

bool citem::pred_del(int _i){
	if(_i >= this->lpred){
		cout << "err pred_del" << endl;
		system("pause");
		return false;
	}

	while(_i < this->lpred-1){
		this->pred[_i] = this->pred[_i+1];
	}

	this->pred[this->lpred-1] = NULL;
	this->lpred --;

	return true;
}

bool citem::be_removed(){
	for(int i = 0; i < this->lpred; i ++){
		this->pred[i]->pred_del(0);
	}
	return true;
}

void citem::clear_pred_succ(){
	this->lpred = 0;
	this->lsucc = 0;
	for(int i = 0; i < this->win_size; i ++)
	{
		this->pred[i] = this->succ[i] = NULL;
	}
}

void citem::initial(Vtype _val, int _ts, int _winsz){
	this->win_size = _winsz;
	this->val = _val;
	this->rlen = -1;
	this->timestamp = _ts;
	this->inD = false;
	this->next = NULL;
	this->prev = NULL;

	this->pred = new citem*[this->win_size];
	this->succ = new citem*[this->win_size];
	this->lpred = 0;
	this->lsucc = 0;
	for(int i = 0; i < this->win_size; i ++){
		this->pred[i] = NULL;
		this->succ[i] = NULL;
	}
}

