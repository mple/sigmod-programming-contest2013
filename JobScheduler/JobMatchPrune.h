/*
 * JobMatchPrune.h
 *
 *  Created on: Mar 27, 2013
 *      Author: johnchronis
 */

#ifndef JOBMATCHPRUNE_H_
#define JOBMATCHPRUNE_H_

#include "job.h"

namespace mple {

class JobMatchPruneQD3: public mple::Job {
public:
	//int start; int end; int length;
	int querylength,doclength,start,end;
	JobMatchPruneQD3(int q,int d,int s3,int e3){//,int s2,int e2,int s1,int e1,int s0,int e0){
		querylength = q;doclength=d;
		start=s3;end=e3;
	}
	int Run();
	virtual ~JobMatchPruneQD3(){}
	
	int getWeight() {
		return 100;
	}
};
class JobMatchPruneQD2: public mple::Job {
public:
	//int start; int end; int length;
	int querylength,doclength,start,end;
	JobMatchPruneQD2(int q,int d,int s2,int e2){//,int s2,int e2,int s1,int e1,int s0,int e0){
		querylength = q;doclength=d;
		start=s2;end=e2;

	}
	int Run();
	virtual ~JobMatchPruneQD2(){}
	
	int getWeight() {
		return 30;
	}
};
class JobMatchPruneQD1: public mple::Job {
public:
	//int start; int end; int length;
	int querylength,doclength,start,end;
	JobMatchPruneQD1(int q,int d,int s1,int e1){//,int s2,int e2,int s1,int e1,int s0,int e0){
		querylength = q;doclength=d;
		start=s1;end=e1;
	}
	int Run();
	virtual ~JobMatchPruneQD1(){}
	
	int getWeight() {
		return 1;
	}
};
class JobMatchPruneQD0: public mple::Job {
public:
	//int start; int end; int length;
	int querylength,doclength,start,end;
	JobMatchPruneQD0(int q,int d,int s0,int e0){//,int s2,int e2,int s1,int e1,int s0,int e0){
		querylength = q;doclength=d;
		start=s0;end=e0;

	}
	int Run();
	virtual ~JobMatchPruneQD0(){}

	int getWeight() {
		return 1;
	}
};
class JobMatchPrune1: public mple::Job {
public:
	//int start; int end; int length;
	int querylength,doclength,start,end;
	JobMatchPrune1(int d,int q,int s,int e){
		doclength=d;querylength = q;
		start=s;end=e;
	}
	int Run();
	virtual ~JobMatchPrune1(){}
};
} /* namespace mple */
#endif /* JOBMATCHPRUNE_H_ */
