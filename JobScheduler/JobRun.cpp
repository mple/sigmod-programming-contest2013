/*
 * JobRun.cpp
 *
 *  Created on: Mar 30, 2013
 *      Author: johnchronis
 */

#include "JobRun.h"
extern mple::bitarea * results;
extern mple::QueryIndex * indexx;
extern std::vector<DocID> globaldocs;
namespace mple {
void SplitGenHashes();
void SplitGenerateRes();
JobRun::JobRun() {
	// TODO Auto-generated constructor stub

}

JobRun::~JobRun() {
	// TODO Auto-generated destructor stub
}

int JobRun::Run() {

	results = new mple::bitarea(indexx->GetVault()->size(), indexx->GetVault(),
			&globaldocs);
	mple::SplitGenHashes() ;
	mple::SplitGenerateRes();
	return 0;
}

} /* namespace mple */
