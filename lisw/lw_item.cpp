/*
 * lw_item.cpp
 *
 *  Created on: 2016Äê7ÔÂ26ÈÕ
 *      Author: liyouhuan
 */
#include "lw_item.h"

//lw_item::lw_item()
//{
//	this->val = -1;
//	this->drop_out_d = -1;
//	this->drop_out_alpha = -1;
//}
lw_item::lw_item(Vtype _val, int _ts, int _d, int _alpha, int _win)
{
	this->val = _val;
	this->timestamp = _ts;
	this->drop_out_d = _d;
	this->drop_out_alpha = _alpha;
	this->win_size = _win;
	this->parents = new lw_item*[this->win_size];
	for(int i = 0; i < this->win_size; i ++)
	{
		this->parents[i] = NULL;
	}
}
lw_item::~lw_item()
{
	delete[] this->parents;
}

string lw_item::to_str()
{
	stringstream _ss;
	_ss << "[";
	_ss << "t= " << this->timestamp;
	_ss << ", v=" << this->val;
	_ss << ", d=" << this->drop_out_d;
	_ss << ", a=" << this->drop_out_alpha;
	_ss << "]";

	return _ss.str();
}

string lw_item::parent_str()
{
	stringstream _ss;
	_ss << "[";
	for(int i = this->win_size-1; i >= 0; i --)
	{
		if(this->parents[i] == NULL) continue;

		_ss << i << "=" << this->parents[i]->val << ", ";
	}
	_ss << "]";
	return _ss.str();
}

int lw_item::to_size()
{
	int sz = sizeof(lw_item);
	sz += sizeof(lw_item) * this->win_size;
	return sz;
}
