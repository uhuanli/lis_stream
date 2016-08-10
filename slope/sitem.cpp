/*
 * sitem.cpp
 *
 *  Created on: 2016Äê8ÔÂ7ÈÕ
 *      Author: liyouhuan
 */
#include "sitem.h"

sitem::sitem(int _v, int _t, int _winsize)
{
	this->val = _v;
	this->timestamp = _t;
	this->win_size = _winsize;
	this->slope_backtrack = -1.0;
	this->backtrack = NULL;
	this->next = NULL;
	this->prev = NULL;

}
sitem::~sitem(){

}
int sitem::to_size(){
	return sizeof(sitem);
}

