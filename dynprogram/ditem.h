/*
 * ditem.h
 *
 *  Created on: Feb 6, 2016
 *      Author: liyouhuan
 */

#ifndef DYNPROGRAM_DITEM_H_
#define DYNPROGRAM_DITEM_H_

#include<iostream>
#include<fstream>
#include<sstream>
#include<string.h>
using namespace std;

class ditem
{
public:
	ditem(int _val, int _ts, int _winsz);
	~ditem();

	string to_str();
	int to_size();
	int prenum();

	void enumlis(int _lis_l, int _i, ditem** _S, stringstream& _ss);
	ditem* get_pre(int _i);
	void add_pre(ditem* _pre);

	int val;
	int rlen;
	int winsz;
	int timestamp;


private:
	int pre_num;
	ditem** pred;
};


#endif /* DYNPROGRAM_DITEM_H_ */
