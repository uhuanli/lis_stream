/*
 * mh_item.h
 *
 *  Created on: 2015��9��22��
 *      Author: liyouhuan
 */

#ifndef MINHEIGHT_MH_ITEM_H_
#define MINHEIGHT_MH_ITEM_H_
#include<iostream>
#include<sstream>
#include<fstream>
using namespace std;
class mh_item{
public:
	mh_item();
	mh_item(int _val, mh_item* _pred);
	mh_item(int _v, int _t);
	string to_str();
	int to_size();
	int val;
	mh_item* pred;
	mh_item* next;
	mh_item* p_most;
	mh_item* un;
	mh_item* dn;
	mh_item* rn;
	int timestamp;
};



#endif /* MINHEIGHT_MH_ITEM_H_ */
