/*
 * Query.h
 *
 *  Created on: 27 ��� 2013
 *      Author: mple
 */

#ifndef QUERY_H_
#define QUERY_H_
#include "../Includes/core.h"
#include <iostream>
#include "../Includes/defn.h"
#include <tuple>

namespace mple {



class Query {
	friend class QueryIndex;
	  friend class QueryInfo;
	    char NTokens;
	    char tokens[5][MAX_WORD_LENGTH+1];
	    MatchType type_;
	    char dist_;
	    Qid qid;
public:
	virtual ~Query();
	 Query(MatchType t,char dist, Qid q) : NTokens(0){
	      type_ = t;
	      dist_ = dist;
	      qid = q;
	    }
	 	 char GetNTokens(void){
	 		 return NTokens;
	 	 }
	    void AddToken(const char *);
	    void printall(void);
	    unsigned int hashvalue();
};

}/* namespace mple */
#endif /* QUERY_H_ */
