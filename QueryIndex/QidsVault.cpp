/*
 * QidsVault.cpp
 *
 *  Created on: 27 ��� 2013
 *      Author: mple
 */

#include "QidsVault.h"

#include <pthread.h>
using namespace std;
#include <vector>
namespace mple {

ErrorCode QidsVault::Add(Qid q){
	vector<Qid>::iterator it  = vault_.end();
	while(it!=vault_.begin()){
		it--;
		if(*it<q){it++;break;}

	}
	vault_.insert(it,q);
	return EC_SUCCESS;
}
ErrorCode QidsVault::Remove(Qid q){
	vector<Qid>::iterator it  = vault_.begin();
	while(it!=vault_.end()){
			if(*it==q){vault_.erase(it);break;}
			it++;
		}

	return EC_SUCCESS;
}
QidsVault::QidsVault(){
}
QidsVault::~QidsVault(){
}

QidsVaultIterator QidsVault::GiveIterator(){
	QidsVaultIterator q(0,&vault_);
	return q;
}

void QidsVaultIterator::Next(){
	it_++;
}

} /* namespace mple */
