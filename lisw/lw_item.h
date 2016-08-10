/*
 * lw_item.h
 *
 *  Created on: 2016Äê7ÔÂ26ÈÕ
 *      Author: liyouhuan
 */

#ifndef LISW_LW_ITEM_H_
#define LISW_LW_ITEM_H_

#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
class lw_item{
public:
//	lw_item();
	lw_item(int _val, int _ts, int _d, int _alpha, int _win);
	~lw_item();

	string to_str();
	string parent_str();
	int to_size();

	lw_item** parents;
	int win_size;
	int val;
	int timestamp;

	int drop_out_d;
	int drop_out_alpha;
};



#endif /* LISW_LW_ITEM_H_ */
