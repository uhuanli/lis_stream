/*
 * vitem.h
 *
 *  Created on: 2015Äê9ÔÂ27ÈÕ
 *      Author: liyouhuan
 */

#ifndef VARIANT_VITEM_H_
#define VARIANT_VITEM_H_
#include "../util/util.h"

class vitem{
public:
	vitem(int _v, int _ts);
	~vitem();

	bool partial(vitem* vit);
	string to_str();
	int to_size();

	int val;
	vitem* ptr;
	vitem* dptr_most;
	vitem* rn;
	vitem* next;
	vitem* dn;
	vitem* un;
	int timestamp;
private:

};




#endif /* VARIANT_VITEM_H_ */
