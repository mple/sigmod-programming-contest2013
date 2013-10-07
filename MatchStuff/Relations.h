/*
 * Relations.h
 *
 *  Created on: Mar 22, 2013
 *      Author: vagos
 */

#ifndef RELATIONS_H_
#define RELATIONS_H_

#include <google/dense_hash_map>
#include <google/sparsehash/libc_allocator_with_realloc.h>

#include <vector>
#include "../TokenIndex/TokenStructs_Vector.h"
#include "TidToWid.h"
#include <iostream>

namespace mple {


class Relations {
	google::dense_hash_map<int, WidToTid * > Tids;
public:

	Relations();
	virtual ~Relations();
	void Add(TokenId wid, TokenId tid, char type);
	WidToTid * GetTids(int k){
		if (Tids.find(k) == Tids.end())
			return NULL;
		return Tids[k];
	}


};

} /* namespace mple */
#endif /* RELATIONS_H_ */
