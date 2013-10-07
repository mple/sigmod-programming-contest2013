/*
 * UnorderMap.h
 *
 *  Created on: Mar 11, 2013
 *      Author: sib
 */

#ifndef UNORDERMAP_H_
#define UNORDERMAP_H_
#include <unordered_map>
#include "MurmurHash3.h"
#include "string"
#include "cstring"

struct mur {
	inline size_t operator()(const char* val) const
	{
		unsigned long h = 0;
		MurmurHash3_x86_32( val,strlen(val) , 0, &h );
		//MurmurHash3_x64_128( val,strlen(val) , 0, &h );
		return size_t(h);
	  }
	};

struct eqstr {
	  inline bool operator()(const char *s1, const char *s2) const {
	    return strcmp(s1, s2) == 0;
	  }
	};


#endif /* UNORDERMAP_H_ */

//	std::unordered_map<const char * , bool, my_hash, eqstr>  docdupli_;
