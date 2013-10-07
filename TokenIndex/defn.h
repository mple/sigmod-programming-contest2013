#ifndef DEFN_H
#define	DEFN_H

// A macro to disallow the copy constructor and operator= functions
// This should be used in the private: declarations for a class
#define DISALLOW_COPY_AND_ASSIGN(TypeName) \
  TypeName(const TypeName&);               \
  void operator=(const TypeName&)

namespace mple{
enum Type{
	Exact,Edit1,Edit2,Edit3,Ham1,Ham2,Ham3
};

typedef unsigned int Qmid;
typedef unsigned int zorder;
typedef unsigned int Tid;
typedef unsigned int Qid;//na kratame ti einai to query (type)??

zorder ZorderCalc(char* token,int n);

}
#endif	// DEFN_H
