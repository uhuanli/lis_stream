/*
 * stopwords.cpp
 *
 *  Created on: 2015-3-26
 *      Author: liyouhuan
 */
#include "stopwords.h"

void stopwords::initial(){
	string words[] = {
			"sai",
			"know",
			"want",
			"todai",
			"forget",
			"think",
			"thing",
			"thank",
			"dai",
			"said",
			"come",
			"ask",
			"like",
			"try",
			"look",
			"person",
			"go",
			"talk",
			"night",
			"listen",
			"need",
			"read",
			"gotta",
			"got",
			"eat",
			"sleep",
			"shit",
			"fuck",
			"feel",
			"5so",
			"23lb",
			"end"
	};
	int i = 0;
	while(words[i] != "end"){
		stopword_set.insert(words[i]);
		i++;
	}
}

bool stopwords::is_stopword(string& _str){
	return stopword_set.find(_str) != stopword_set.end();
}

set<string> stopwords::stopword_set;



