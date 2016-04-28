/*
 * runtime.cpp
 *
 *  Created on: 2015Äê9ÔÂ24ÈÕ
 *      Author: liyouhuan
 */
#include "runtime.h"

runtime::runtime(){
 this->initial();
}
void runtime::initial(){
	sum = 0;
	dup_num = 0;
	cur = 0;
}
void runtime::begin(){
	this->cur = util::get_time_cur();
	this->dup_num ++;
}
void runtime::end(){
	long long int tmp = util::get_time_cur();
	this->cur = util::cal_time(tmp, this->cur);
	this->sum += this->cur;
	this->dup_num ++;
	if(this->dup_num % 2 != 0){
		cout << "num err != 0 " << this->dup_num << endl;
	}
}
double runtime::getsum(){
	if(this->dup_num % 2 != 0){
		cout << "err sum " << this->dup_num << endl;
		system("pause");
		return -1;
	}

	return this->sum;
}
double runtime::getcur(){
	if(this->dup_num % 2 != 0){
		cout << "err cur " << this->dup_num << endl;
		system("pause");
		return -1;
	}

	return this->cur;
}
double runtime::getavg()
{
	return this->getsum() / (this->getnum());
}
int  runtime::getnum(){
	if(this->dup_num % 2 != 0){
		cout << "err num " << this->dup_num << endl;
		system("pause");
		return -1;
	}

	return this->dup_num / 2;
}


