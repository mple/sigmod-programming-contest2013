#include "TokenDuplicateIndex.h"
#include "../MapStuff/UnorderMap.h"
namespace mple{
DuplicateIndex::DuplicateIndex(){
	//index_.rehash(100000);
	index_.set_empty_key("");
}
DuplicateIndex::~DuplicateIndex(){}
ErrorCode DuplicateIndex::AddPair(char* str,TokenId &tid)
{
	bool exists;
	Exists(str,&exists);
	if(exists)
		return ERROR;

	index_[str]=tid;
	return OK;
}
//ErrorCode DuplicateIndex::RemovePair(char* str)
//{
	/*if(index_.erase(str)==0)
		return ERROR;
	else
		return OK;
	*/
//	return (index_.erase(str)==0) ? ERROR : OK;
//}
ErrorCode DuplicateIndex::Exists(char* str,bool *result)
{
	google::dense_hash_map<const char * , TokenId, mur, eqstr>::const_iterator got = index_.find (str);
	/*if ( got == index_.end())
		* result= false;
	else
		* result= true;*/
	( got == index_.end())? *result =false : *result = true;
	return OK;
}
ErrorCode  DuplicateIndex::LookUpTokenExactManager(char* string,TokenId * id)
{
	google::dense_hash_map<const char * , TokenId, mur, eqstr>::iterator got = index_.find (string);
	if ( got == index_.end() )
		return NOTFOUND;
	else
	{
		*id=got->second;
		return OK;
	}
}
}
