/*
 * QidsVault.h
 *
 *  Created on: 27 ��� 2013
 *      Author: mple
 */

#ifndef QIDSVAULT_H_
#define QIDSVAULT_H_
#include "../Includes/core.h"
#include "../TokenIndex/TokenStructs_Vector.h"
#include <pthread.h>
#include <vector>
#include <iostream>

namespace mple {


class QidsVaultIterator {
	//DISALLOW_COPY_AND_ASSIGN(QidsVaultIterator);
protected:
		std::vector<Qid>* v_;
		unsigned int it_;
	public:
		void Next();
		QidsVaultIterator(int it,std::vector<Qid>* v){it_ = it;v_ = v;}
		Qid operator*(){
			return ((*v_)[it_]);
		}
		bool End(){
			return it_ == (*v_).size();
		}
		int size(){
					return (*v_).size();
				}
		//QidsVaultIterator& operator=(ListNode* ln ){
		//	it_ = ln;
		//	return *this;
		//}
};

class QidsVault {//keeps qids
private:
	//List vault_; //FIX private
	std::vector<Qid> vault_;
	DISALLOW_COPY_AND_ASSIGN(QidsVault);
public:
	virtual ~QidsVault();
	QidsVault();
	bool Empty(void){
	  return vault_.size() == 0;
	}
	ErrorCode Add(Qid q);
	ErrorCode Remove(Qid q);
	QidsVaultIterator GiveIterator();//returns a listnode* to the start of the list


	void printall(void){
		for(unsigned int i=0;i<vault_.size();i++){
			std::cout<<"Qid "<<vault_[i];
		}
		std::cout<<std::endl;
	}



};


} /* namespace mple */
#endif /* QIDSVAULT_H_ */
