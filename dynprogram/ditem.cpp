/*
 * ditem.cpp
 *
 *  Created on: 2016Äê2ÔÂ16ÈÕ
 *      Author: liyouhuan
 */
#include "ditem.h"
#include "../util/util.h"
// public
//ditem::ditem()
//{
//	this->val = -1;
//	this->pre_first = NULL;
//	this->pre_num = 0;
//	this->rlen = 0;
//	this->timestamp = -1;
//	this->winsz = -1;
//}
ditem::ditem(int _val, int _ts, int _winsz){
	this->val = _val;
	this->timestamp = _ts;
	this->winsz = _winsz;
	this->rlen = 0;
	this->pred = new ditem*[_winsz];
	this->pre_num = 0;
}
ditem::~ditem()
{
	delete[] this->pred;
}

string ditem::to_str(){
	stringstream _ss;
	_ss << "val=" << this->val << ", ";
	_ss << "rlen=" << this->rlen << ", ";
	_ss << "ts=" << this->timestamp << ", ";
	_ss << "prenum=" << this->pre_num << "(";
	for(int i = 0; i < this->pre_num; i ++)
	{
		_ss << this->pred[i]->val;
		if(i != this->pre_num-1)
		{
			_ss << ", ";
		}
	}
	_ss << ")";
	return _ss.str();
}
int ditem::to_size(){
	int _sz = 0;
	_sz += sizeof(int) * (4 + 1);
	_sz += sizeof(ditem*) * this->winsz;
	return _sz;
}

int ditem::prenum(){
	return this->pre_num;
}

void ditem::enumlis(int _lis_l, int _i, ditem** _S, stringstream& _ss){
	_S[_i] = this;
	if(_i == _lis_l -1)
	{
		if(util::testdp) // inverse to check
		{
			stringstream _tmp_ss;
			for(int i = _lis_l-1; i >= 0; i --)
			{
				_tmp_ss << "\t" << _S[i]->val;
			}
			_tmp_ss << endl << _ss.str();
			_ss.str("");
			_ss.clear();
			_ss << _tmp_ss.str();
		}
		else
		{
			for(int i = _lis_l-1; i >= 0; i --)
			{
				_ss << "\t" << _S[i]->val;
			}
			_ss << endl;
		}
		return ;
	}

	for(int i = 0; i < this->pre_num; i ++)
	{
		ditem* dchild = this->get_pre(i);
		dchild->enumlis(_lis_l, _i+1, _S, _ss);
	}

	return ;
}
ditem* ditem::get_pre(int _i){
	return this->pred[_i];
}
void ditem::add_pre(ditem* _pre){
	this->pred[this->pre_num] = _pre;
	this->pre_num ++;
}



