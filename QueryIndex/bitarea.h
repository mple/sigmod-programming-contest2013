/*
 * bitarea.h
 *
 *  Created on: Mar 3, 2013
 *      Author: mple
 */

#ifndef BITAREA_H_
#define BITAREA_H_
#include "../TokenIndex/TokenStructs_Vector.h"
#include "QueryIndex.h"
#include "QueryInfo.h"

#include "../TokenIndex/TokenDuplicateIndex.h"
#include "../TokenIndex/TokenManager_Vector.h"
#include <vector>
#include <stdio.h>
#include <bitset>
#include "../Includes/defn.h"
#include "../Includes/core.h"
#include <bitset>
#include <stdio.h>
#include <stdlib.h>
#include <list>
#include <unordered_map>
#include <google/dense_hash_map>
#include <google/sparsehash/libc_allocator_with_realloc.h>

#include <string.h>

#include <cstring>

namespace mple {
struct qmidinfo{
	char token[5];
	char count;
};

class bitarea {
	qmidinfo **vault_;
	unsigned int size_;
	int ndocs_;
	std::vector<Qmid> ready_;

public:

	bitarea(unsigned int Nqmids, std::vector<QueryInfo*>* QI, std::vector<DocID> * docids) { //mporoume ta arxika 3 bits na einai kapou eotima ola mazi gia na ta pairnoume
		ndocs_ = docids->size();
		vault_ = new qmidinfo * [ndocs_];

		for (int i = 0; i<ndocs_ ; ++i){
			vault_[i] = new qmidinfo[Nqmids];
			size_ = Nqmids;
			memset((void *)vault_[i], 0,(sizeof(qmidinfo)*Nqmids));
		}
		for (int ii = 0; ii<ndocs_ ; ++ii){
			for (Qmid i = 0; i < Nqmids; i++) {
				vault_[ii][i].count = (*QI)[i]->GetNTokens();
			}
		}

	}

	void UnionWith(bitarea &res);
	
		bitarea(const bitarea &c){
			operator=(c);
		}
		virtual ~bitarea() {
			for (int i = 0; i<ndocs_ ; ++i){
				delete[] vault_[i];
			}
			delete[] vault_;

		}
		void SetStauros(std::vector<DocIDInfo>* doclist, std::vector<TokenQmidInfo>** pointer,char result);
		void Set(std::vector<DocIDInfo>* doclist, std::vector<TokenQmidInfo>* qlist, int pos);

	ErrorCode Set(std::vector<DocIDInfo>* doclist, Qmid qmid, int TokenPos) {
		qmid--;

		for (unsigned int i = 0; i< doclist->size(); ++i){
				int Docid = (*doclist)[i].did_-1;
				vault_[Docid][qmid].token[TokenPos] = 1;
			}


		return OK;
	}


	bool CheckQmid(Qmid doc ,Qmid qmid){
			qmid--;
			return 	vault_[doc][qmid].token[1] +
					vault_[doc][qmid].token[0]+
					vault_[doc][qmid].token[2]+
					vault_[doc][qmid].token[3]+
					vault_[doc][qmid].token[4] ==vault_[doc][qmid].count;

		}
	void printall(int i = 1){
			int a = i-1;
			cout << "Docid = " << i << endl;
			for (unsigned int i = 0 ; i < size_ ; ++i){
				cout << (int) vault_[a][i].count << " "
						<< (int)vault_[a][i].token[0] << " "
						<< (int)vault_[a][i].token[1] << " "
						<< (int)vault_[a][i].token[2] << " "
						<< (int)vault_[a][i].token[3] << " "
						<< (int)vault_[a][i].token[4] << " " << endl;
			}

	}
	ErrorCode results(std::vector<QueryInfo *> * Qi,unsigned int * p_num_res, QueryID ** p_query_ids,Qmid qmid1) ;
};

} /* namespace mple */
#endif /* BITAREA_H_ */
