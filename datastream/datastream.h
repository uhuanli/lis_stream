/*
 * datastream.h
 *
 *  Created on: 2015-9-14
 *      Author: liyouhuan
 */

#ifndef DATASTREAM_H_
#define DATASTREAM_H_
#include<iostream>
#include<fstream>
#include<stdlib.h>
#include<vector>
using namespace std;
class datastream{
public:
	datastream(string _data_set);
	~datastream();
	//
	bool hasnext();
	double next();
	int timestamp();
	void to_vec(vector<double>& _data);
private:
	ifstream fin;
	int ts;
};


#endif /* DATASTREAM_H_ */
