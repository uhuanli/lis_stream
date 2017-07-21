/*
 * stopwords.h
 *
 *  Created on: 2015-3-26
 *      Author: liyouhuan
 */

#ifndef STOPWORDS_H_
#define STOPWORDS_H_
#include "util.h"
class stopwords{
public:
	static void initial();
	static bool is_stopword(string& _str);
private:
	static set<string> stopword_set;
};


#endif /* STOPWORDS_H_ */
