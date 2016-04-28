/*
 * item.cpp
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */
#include "../quadruple/item.h"

item::item(int _a, int _ts){
	this->initial(_a, _ts);
}
item::item(){
	this->initial(-1, 0);
}
item::~item(){

}
void item::initial(int _a, int _ts){
	this->un = NULL;
	this->dn = NULL;
	this->ln = NULL;
	this->rn = NULL;
	this->rm_most = NULL;
	this->lm_most = NULL;
	this->inLIS = false;
	this->timestamp = _ts;
	this->val = _a;
	this->rlen = 0;
}

string item::to_str(){
	stringstream _ss;
	_ss << this->val << "  [";
	if(this->un != NULL){
		_ss << this->un->val << ", ";
	}else{
		_ss << "-1, ";
	}

	if(this->dn != NULL){
		_ss << this->dn->val << ", ";
	}else{
		_ss << "-1, ";
	}

	if(this->ln != NULL){
		_ss << this->ln->val << ", ";
	}else{
		_ss << "-1, ";
	}

	if(this->rn != NULL){
		_ss << this->rn->val << ", ";
	}else{
		_ss << "-1, ";
	}

	_ss << "r=" << this->rlen << "]";

	return _ss.str();
}
int item::to_size(){
	int sz = 0;

	sz += sizeof(item);


	return sz;
}
bool item::partial(item* a){
	return this->before(a) && this->val <= a->val;
}
bool item::before(item* a){
	return this->timestamp < a->timestamp;
}
bool item::after(item* a){
	return this->timestamp > a->timestamp;
}
item* item::child_lm(){
	if(this->un == NULL){
		cout << "err called" << endl;
		cout << "val=" << this->val << endl;
		cout << "rlen=" << this->rlen << endl;
		system("pause");
		return NULL;
	}
	item* it = this->un;
	while(it ->ln != NULL)
	{
		if((it->ln)->partial(this)){
			it = it->ln;
		}else{
			break;
		}
	}
	return it;
}
