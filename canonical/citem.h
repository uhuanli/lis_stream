/*
 * citem.h
 *
 *  Created on: 2015Äê9ÔÂ23ÈÕ
 *      Author: liyouhuan
 */

#ifndef CANONICAL_CITEM_H_
#define CANONICAL_CITEM_H_
#include "../util/util.h"

class citem{
public:
	citem(int _val, int _ts, int _winsz);
	~citem();

	string to_str();
	int to_size();
	int insert_pred(citem* it);
	int insert_succ(citem* it);
	int enumlis(int _lis_l, int _i, citem** _S, stringstream& _ss);
	int enumlis_constrained(int _lis_l, int _i, citem** _S, stringstream& _ss, citem** _Sstore, int _constrained, int& _value);
	bool partial(citem* it);
	bool pred_del(int _i);
	bool be_removed();
//	bool removable();
//	bool succtail_del();
//	citem* get_succtail();
	void clear_pred_succ();

	int val;
	int rlen;
	int timestamp;
	bool inD;
	citem* next;
	citem* prev;

private:
	void initial(int _val, int _ts, int _winsz);

	int win_size;
	citem** pred;
	int lpred;
	citem** succ;
	int lsucc;
};



#endif /* CANONICAL_CITEM_H_ */
