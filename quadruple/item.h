/*
 * item.h
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#ifndef ITEM_H_
#define ITEM_H_

#include<iostream>
#include<string>
#include<stdlib.h>
#include<stdio.h>
#include<fstream>
#include<sstream>

#include "../util/util.h"
using namespace std;

//#define INLIS 1

class item{
public:
	item(double _a, int _ts);
	item();
	~item();

	void initial(double _a, int _ts);
	string to_str();
	int to_size();
	bool partial(item* a);
	bool before(item* a);
	bool after(item* a);
	item* child_lm();
	item* un;
	item* dn;
	item* ln;
	item* rn;
	item* rm_most;
	item* lm_most;
#ifdef INLIS
	bool inLIS;
#endif
	bool black;
	int timestamp;
	double val;
#ifdef COUNT_LISNUM
	double is_num;
	double count_is_num();
#endif

#ifdef RLEN
	int rlen;
#endif
};


#endif /* ITEM_H_ */
