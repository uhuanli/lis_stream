/*
 * item.cpp
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */
#include "../quadruple/item.h"

item::item(double _a, int _ts){
	this->initial(_a, _ts);
}
item::item(){
	this->initial(-1, 0);
}
item::~item(){

}
void item::initial(double _a, int _ts){
	this->un = NULL;
	this->dn = NULL;
	this->ln = NULL;
	this->rn = NULL;
	this->rm_most = NULL;
	this->lm_most = NULL;
#ifdef INLIS
	this->inLIS = false;
#endif
	this->black = false;
	this->timestamp = _ts;
	this->val = _a;
#ifdef RLEN
	this->rlen = 0;
#endif

#ifdef COUNT_LISNUM
	this->is_num = 0;
#endif
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
#ifdef RLEN
	_ss << "r=" << this->rlen << ", ";
#endif
	//_ss << "black:" << this->black << "]";
	_ss << "time:" << this->timestamp << "]";

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
#ifdef RLEN
		cout << "rlen=" << this->rlen << endl;
#endif
		system("pause");
		return NULL;
	}
	item* it = this->un;
	while(it->ln != NULL)
	{
		if((it->ln)->partial(this)){
			it = it->ln;
		}else{
			break;
		}
	}
	return it;
}
#ifdef COUNT_LISNUM
double item::count_is_num()
{
	this->is_num = 0;
	item* it = this->un;
	while(it != NULL)
	{
		if((it)->partial(this)){
			this->is_num += it->is_num;
			it = it->ln;
		}else{
			break;
		}
	}

	if(this->is_num == 0) this->is_num = 1;

	return this->is_num;
}
#endif
