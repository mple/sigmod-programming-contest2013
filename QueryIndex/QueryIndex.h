/*
 * QueryIndex.h
 *
 *  Created on: 27 ��� 2013
 *      Author: mple
 */

#ifndef QUERYINDEX_H_
#define QUERYINDEX_H_
#include "../Includes/core.h"
#include "../Includes/defn.h"
#include <pthread.h>

#include "Query.h"
#include <stack>
#include <tuple>
#include <unordered_map>
#include <vector>
#include <map>
#include <utility>

//#include "../TokenIndex/TokenIndexBySize.h"
#include "../TokenIndex/TokenManager_Vector.h"

#include "../TokenIndex/TokenDuplicateIndex.h"

namespace mple {

typedef std::unordered_multimap<unsigned int, Qmid> UQMmap;

class QueryInfo;
class QueryIndex {
public:
  QueryIndex(TokenManager * tokenmanager);
  virtual ~QueryIndex();
  bool AddQuery(Query *Q/*input*/, Qmid * qmid/*output*/); //true if new query added, false if query exists and no need to add tokens
  ErrorCode RemoveQuery(Qid qid);
  QueryInfo* Lookup(Qmid qmid);
  void printall(void);
  std::vector<QueryInfo*>* GetVault(){
	  return &vault_;
  }
private:
  TokenManager * tokenmanager_;
  std::vector<QueryInfo *> vault_;
  int calchash(Query* q);
  Qmid HashLookup(Query * q);//checks if a query's hasvalue exists
  std::stack<Qmid> emptycells_;
  google::dense_hash_map<Qid, Qmid> qidindex;

  UQMmap qmidindex;
};

} /* namespace mple */
#endif /* QUERYINDEX_H_ */
