/*
 * TidToWid.h
 *
 *  Created on: Mar 22, 2013
 *      Author: vagos
 */

#ifndef TIDTOWID_H_
#define TIDTOWID_H_

#include <vector>
#include <unordered_map>
#include "../TokenIndex/TokenStructs_Vector.h"

namespace mple {

struct tidhash{
	inline int operator()(TokenId ref) const
	{
		return ref.id_*100+ ref.length_;
	}
};	


class WidToTid {
	
	public:
	std::vector<wordsat> vault_;
	WidToTid();
	virtual ~WidToTid();
	void Add(TokenId tid, char matchtype);
	std::vector<wordsat> * GetVector(){return &vault_;}
	
};

} /* namespace mple */
#endif /* TIDTOWID_H_ */
