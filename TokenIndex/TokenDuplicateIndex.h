#ifndef __TokenDuplicateIndex__
#define __TokenDuplicateIndex__
#include <string>
#include <unordered_map>
#include "TokenStructs_Vector.h"
#include "../MapStuff/UnorderMap.h"
#include <google/dense_hash_map>
#include <google/sparsehash/libc_allocator_with_realloc.h>
namespace mple
{
	class DuplicateIndex
	{
		
	private:
		//std::unordered_map<const char * , TokenId, mur, eqstr> index_;
		google::dense_hash_map<const char * , TokenId, mur, eqstr> index_;

	public:
		DuplicateIndex();
		~DuplicateIndex();
		ErrorCode AddPair(char* str,TokenId &tid);
		//ErrorCode RemovePair(char* str);
		ErrorCode Exists(char* str,bool *exist);
		ErrorCode  LookUpTokenExactManager(char*,TokenId * id);
	};
}

#endif
