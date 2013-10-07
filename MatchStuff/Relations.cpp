/*
 * Relations.cpp
 *
 *  Created on: Mar 22, 2013
 *      Author: vagos
 */

#include "Relations.h"
#include <iostream>

using namespace std;
namespace mple {

Relations::Relations() {
	// TODO Auto-generated constructor stub
	Tids.set_empty_key(-1);
}

Relations::~Relations() {
	// TODO Auto-generated destructor stub

	for (google::dense_hash_map<int, WidToTid * >::iterator it = Tids.begin() ; it!=Tids.end() ; ++it){
		//cout << it->first << " " <<it->second->GetVector()->size() << endl;
		delete it->second;
	}

}
void Relations::Add(TokenId wid, TokenId tid, char type){
	int temp=wid.key();
	if (Tids.find(temp) == Tids.end()){
		Tids[temp] = new WidToTid;
	}
	Tids[temp]->Add(tid, type);
}

} /* namespace mple */
