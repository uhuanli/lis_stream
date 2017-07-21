/*
 * mh_item.cpp
 *
 *  Created on: 2015Äê9ÔÂ22ÈÕ
 *      Author: liyouhuan
 */
#include "mh_item.h"

mh_item::mh_item(){
	this->val = -1;
	this->pred = NULL;
	this->next = NULL;
	this->p_most = NULL;
	this->un = NULL;
	this->dn = NULL;
	this->rn = NULL;
	this->timestamp = -1;
}
mh_item::mh_item(Vtype _val, mh_item* _pred){
	this->val = _val;
	this->pred = _pred;
	this->next = NULL;
	this->p_most = NULL;
	this->un = NULL;
	this->dn = NULL;
	this->rn = NULL;
	this->timestamp = -1;
}

mh_item::mh_item(Vtype _v, int _t)
{
	this->val = _v;
	this->timestamp = _t;
	this->next = NULL;
	this->p_most = NULL;
	this->un = NULL;
	this->dn = NULL;
	this->rn = NULL;
	this->pred = NULL;
}

string mh_item::to_str(){
	stringstream _ss;
	Vtype pre_val = (this->pred == NULL) ? (-1) : (this->pred->val);
	_ss << "("
		<< this->val << ", pre=" << pre_val
		<< ", "
		<< ")";
	return _ss.str();
}

int mh_item::to_size(){
	int sz = 0;

	sz += sizeof(mh_item);

	return sz;
}
