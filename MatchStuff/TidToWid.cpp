/*
 * TidToWid.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: vagos
 */

#include "TidToWid.h"
#include <iostream>

namespace mple {

WidToTid::WidToTid() {
	// TODO Auto-generated constructor stub

}

WidToTid::~WidToTid() {

	// TODO Auto-generated destructor stub
	//std::cout << vault_.size() << std::endl;
}

void WidToTid::Add(TokenId wid, char matchtype){
	//vault_[wid]=matchtype;
	wordsat tmp;
	tmp.tid = wid;
	tmp.matchtype = matchtype;

	vault_.push_back(tmp);
}

} /* namespace mple */
