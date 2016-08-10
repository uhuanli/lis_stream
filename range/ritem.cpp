/*
 * ritem.cpp
 *
 *  Created on: 2016Äê8ÔÂ9ÈÕ
 *      Author: liyouhuan
 */

#include "ritem.h"
ritem::ritem(double _v, int _t, int _winsz){
	this->val = _v;
	this->timestamp = _t;
	this->win_size = _winsz;
	this->rank = -1;
	this->backtrack = NULL;
}
ritem::~ritem(){

}

int ritem::to_size(){
	return sizeof(ritem);
}

string ritem::to_str(){
	stringstream _ss;
	_ss << "t=" << this->timestamp << "\t";
	_ss << "v=" << this->val << "\t";
	_ss << "r=" << this->rank << "\t";
	if(this->backtrack != NULL){
		_ss << "b=" << this->backtrack->val;
	}else{
		_ss << "b=NULL";
	}
	return _ss.str();
}

