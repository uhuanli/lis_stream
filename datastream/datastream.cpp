/*
 * datastream.cpp
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */
#include "datastream.h"

datastream::datastream(string _data_set){
//	cout << "init datastream" << endl;
	fin.open(_data_set.c_str(), ios::in);
	if(!fin){
		cerr << "can not open " << _data_set << endl;
		system("pause");
	}
	this->ts = 0;
//	cout << "init finish datastream" << endl;
}
datastream::~datastream(){
	fin.close();
}
bool datastream::hasnext(){
	return !fin.eof();
}
double datastream::next(){
	double _ret = 0;
	if(fin.eof()){
		cout << "err" << endl;
		return -1;
	}

	fin >> _ret;
	this->ts ++;
	return _ret;
}
int datastream::timestamp(){
	return this->ts;
}
void datastream::to_vec(vector<double>& _data)
{
	if(this->ts != 0)
	{
		cerr << "err ts != 0" << endl;
		exit(-1);
	}

	_data.clear();
	while(this->hasnext())
	{
		_data.push_back(this->next());
	}
	fin.close();
	return;
}

