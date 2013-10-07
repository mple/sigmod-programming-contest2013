/*
 * bitarea.cpp
 *
 *  Created on: Mar 3, 2013
 *      Author: mple
 */

#include "bitarea.h"
#include "../TokenIndex/TokenStructs_Vector.h"
#include <queue>
#include <unistd.h>

namespace mple {
void bitarea::SetStauros(std::vector<DocIDInfo>* doclist,
		std::vector<TokenQmidInfo>** pointer, char result) {
	if(doclist==NULL) return;
	int i = 0;
	for (i = 0; i < 7; i++) {
		if ((result & (1 << i)) != 0)
			this->Set(doclist, pointer[i], i);
	}
}

void bitarea::Set(std::vector<DocIDInfo>* doclist,
		std::vector<TokenQmidInfo>* qlist,  int pos) {
	if (qlist == NULL)
		return;
	char temp = (1 << pos);
	if ((127 & temp) == 0)
		return;
	for (unsigned int i=0 ; i<qlist->size() ; ++i){
		this->Set(doclist, (*qlist)[i].qmid_, (*qlist)[i].position_);
	}
}


ErrorCode bitarea::results(std::vector<QueryInfo *> * Qi,        //merge sort
		unsigned int * p_num_res, QueryID ** p_query_ids, Qmid did) {
	std::vector<QueryID> results;
	static int ca = 0;
	ca++;
	int xx;
	std::vector<int> pos;
	int lim;
	for (Qmid qmid = 1; qmid <= size_; ++qmid) {
		if (CheckQmid(did-1, qmid)) {
			QidsVaultIterator it = (*Qi)[qmid - 1]->GiveIterator();
			if (!it.End()) {
				pos.push_back(results.size());
				while (!it.End()) {
					results.push_back(*it);
					it.Next();
				}
			}

		}
	}

	*p_num_res = results.size();
	if (*p_num_res == 0) {
		return EC_SUCCESS;
	}
	*p_query_ids = (QueryID *) malloc((*p_num_res) * sizeof(QueryID));
	QueryID * source, *dest, *tmp;
	source = &results.front();
	dest = *p_query_ids;
	int vag, ii, j, x, k = 2, last, john, ind1, ind2, eind1, eind2;
	x = pos.size();

	vag = 1;
	if (pos.size() % 2 == 1) {
		vag = -1;
	}
	xx = x;
	if (x % 2 == 1 && x != 1) {
		xx = x + 1;
	}

	xx *= 2;
	while (xx > k){
		last = 0;
		john = pos.size();
		if (vag == -1) {
			john--;
		}
		vag *= -1;
		for (j = 0; j < john; j += k) {
			if ((size_t)j + (k / 2) > pos.size() - 1) {
				break;
			}

			ind1 = pos[j]; //-1 0, 1 2
			eind1 = pos[j + ((k / 2) > 1 ? (k / 2) : 1)] - 1;

			ind2 = pos[j + ((k / 2) > 1 ? (k / 2) : 1)];
			eind2 = j + k;

			if ((unsigned int) eind2 > pos.size() - 1) {
				eind2 = (*p_num_res) - 1;

			} else {
				eind2 = pos[j + k] - 1;
			}

			lim = eind2 - ind2 + eind1 - ind1;

			for (ii = 0; ii < lim + 1; ++ii) {

				if (source[ind1] < source[ind2]) {
					dest[last++] = source[ind1++];
				} else {
					dest[last++] = source[ind2++];
				}
				if (ind1 > eind1) {
					memcpy(&dest[last], &source[ind2],
							(eind2 - ind2 + 1) * sizeof(QueryID));
					last += (eind2 - ind2 + 1);
					break;
				} else if (ind2 > eind2) {
					memcpy(&dest[last], &source[ind1],
							(eind1 - ind1 + 1) * sizeof(QueryID));
					last += (eind1 - ind1 + 1);
					break;
				}
			}
		}
		if ((unsigned int) last < (*p_num_res)) {
			memcpy(dest + last, source + last,
					((*p_num_res) - last) * sizeof(QueryID));
		}
		tmp = source;
		source = dest;
		dest = tmp;
		k *= 2;
	}
	if (source != *p_query_ids) {
		memcpy(*p_query_ids, source, (*p_num_res) * sizeof(QueryID));
	}

	return EC_SUCCESS;

}

} /* namespace mple */
