/*
 * ritem.h
 *
 *  Created on: 2016Äê8ÔÂ9ÈÕ
 *      Author: liyouhuan
 */

#ifndef RITEM_H_
#define RITEM_H_
#include "../util/util.h"

class ritem{
public:
	ritem(double _v, int _t, int _winsz);
	~ritem();

	int to_size();
	string to_str();

	int win_size;
	int timestamp;
	double val;
	int rank;
	ritem* backtrack;
};



#endif /* RITEM_H_ */
