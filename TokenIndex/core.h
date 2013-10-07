/*
 * core.h version 1.0
 * Copyright (c) 2013 KAUST - InfoCloud Group (All Rights Reserved)
 * Authors: Amin Allam, Fuad Jamour
 *
 * Permission is hereby granted, free of charge, to any person
 * obtaining a copy of this software and associated documentation
 * files (the "Software"), to deal in the Software without
 * restriction, including without limitation the rights to use,
 * copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the
 * Software is furnished to do so, subject to the following
 * conditions:
 *
 * The above copyright notice and this permission notice shall be
 * included in all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
 * EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES
 * OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
 * NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
 * HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY,
 * WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
 * FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR
 * OTHER DEALINGS IN THE SOFTWARE.
 *
 * Current version: 1.0 (initial release -- Feb 1, 2013)
 */

#ifndef __SIGMOD_CORE_H_
#define __SIGMOD_CORE_H_

#ifdef __cplusplus
extern "C" {
#endif

///////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************

/// Maximum document length in characters.
#define MAX_DOC_LENGTH (1<<22)

/// Maximum word length in characters.
#define MAX_WORD_LENGTH 31

/// Minimum word length in characters.
#define MIN_WORD_LENGTH 4

/// Maximum number of words in a query.
#define MAX_QUERY_WORDS 5

/// Maximum query length in characters.
#define MAX_QUERY_LENGTH ((MAX_WORD_LENGTH+1)*MAX_QUERY_WORDS)


/// Query ID type.
typedef unsigned int QueryID;

/// Document ID type.
typedef unsigned int DocID;


/// Matching types:
enum MatchType {
    /**
    * Two words match if they are exactly the same.
    */
    MT_EXACT_MATCH,
    /**
    * Two words match if they have the same number of characters, and the
    * number of mismatching characters in the same position is not more than
    * a specific threshold.
    */
    MT_HAMMING_DIST,
    /**
    * Two words match if one of them can can be transformed into the other word
    * by inserting, deleting, and/or replacing a number of characters. The number
    * of such operations must not exceed a specific threshold.
    */
    MT_EDIT_DIST
};

/// Error codes:			
enum ErrorCode {
    /**
    * Must be returned by each core function unless specified otherwise.
    */
    EC_SUCCESS,
    /**
    * Must be returned only if there is no available result to be returned
    * by GetNextAvailRes(). That is, all results have already been returned
    * via previous calls to GetNextAvailRes().
    */
    EC_NO_AVAIL_RES,
    /**
    * Used only for debugging purposes, and must not be returned in the
    * final submission.
    */
    EC_FAIL
};

///////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************

/**
* Called only once at the beginning of the whole test.
* Performs any required initializations. 
*/
ErrorCode InitializeIndex();

/**
* Called only once at the end of the whole test.
* Can be used for releasing all memory used to index active queries.
* The time spent in this function will not affect the score of the
* submission.
*/
ErrorCode DestroyIndex();

/**
* Add a query (associated with matching type) to the active query set.
* 
* @param[in] query_id
*   The integral ID of the query. This function will not be called twice
*   with the same query ID.
*
* @param[in] query_str
*   A null-terminated string representing the query. It consists of
*   a space separated set of words. The length of any word will be at
*	least MIN_WORD_LENGTH characters and at most MAX_WORD_LENGTH
*	characters. The number of words in a query will not exceed
*	MAX_QUERY_WORDS words.
*   "query_str" contains at least one non-space character.
*   "query_str" contains only lower case letters from 'a' to 'z'
*   and space characters.
* 
* @param[in] match_type
*   The type of mechanism used to consider a query as a
*   match to any document, as specified in MatchType enumeration.
*
* @param[in] match_dist
*   The hamming or edit distance (according to "match_type")
*   threshold used as explained in MatchType enumeration.
*   This parameter must be equal 0 for exact matching. The possible
*   values of this parameter are 0,1,2,3.
*   A query matches a document if and only if: for each word in
*	the query, there exist a word in the document that matches it
*	under the "match_type" and "match_dist" constraints. Note that
*	the "match_dist" constraint is applied independently for each
*	word in the query.
*
* @return ErrorCode
*   - \ref EC_SUCCESS
*          if the query was registered successfully
*/
ErrorCode StartQuery(QueryID        query_id,
                     const char*    query_str,
                     MatchType      match_type,
                     unsigned int   match_dist);

/**
* Remove a query from the active query set.
*
* @param[in] query_id
*   The integral ID of the query. This function will not be called twice
*   with the same query ID.
*
* @return ErrorCode
*   - \ref EC_SUCCESS
*         if the query was unregistered successfully
*/
ErrorCode EndQuery(QueryID query_id);

/**
* Push a document to the server.
*
* @param[in] doc_id
*   The integral ID of the document. This function will not be called twice
*   with the same document ID.
*
* @param[in] doc_str
*   A null-terminated string representing the document. It consists of
*   a space separated set of words. The length of any word
*   will be at least MIN_WORD_LENGTH characters and at most
*	MAX_WORD_LENGTH characters. The length of any document will not
*   exceed MAX_DOC_LENGTH characters.
*   "doc_str" contains at least one non-space character.
*   "doc_str" contains only lower case letters from 'a' to 'z'
*   and space characters.
*
*   @return ErrorCode
*   - \ref EC_SUCCESS
*          if the document was added successfully
*/
ErrorCode MatchDocument(DocID         doc_id,
                        const char*   doc_str);

/**
* Return the next available active queries subset that matches any previously
* submitted document, sorted by query IDs. The returned result must depend
* on the state of the active queries at the time of calling MatchDocument().
*
* @param[out] *p_doc_id
*   A document ID that has not been returned before. You can choose to return
*   the results of any document that has not been returned before.
*
* @param[out] *p_num_res
*   The number of active queries that matched the document *p_doc_id.
*
* @param[out] *p_query_ids
*   An array of the IDs of the *p_num_res matching queries,
*   ordered by the ID values. This array must be allocated by this
*   core library using malloc(). This array must not be freed by the
*   core library, since it will be freed by the testing benchmark.
*   If *p_num_res=0, this array must not be allocated, as it will not
*   be freed by the testing benchmark in that case.
*   Allocating this array using "new" is not acceptable.
*   In case of *p_num_res is not zero, The size of this array must be 
*   equal to "(*p_num_res)*sizeof(QueryID)" bytes.
*
* @return ErrorCode
*   - \ref EC_NO_AVAIL_RES
*          if all documents have already been returned by previous calls to this function
*   - \ref EC_SUCCESS
*          results returned successfully
*/
ErrorCode GetNextAvailRes(DocID*         p_doc_id,
                          unsigned int*  p_num_res,
                          QueryID**      p_query_ids);

///////////////////////////////////////////////////////////////////////////////////////////////
//*********************************************************************************************

#ifdef __cplusplus
}
#endif

#endif // __SIGMOD_CORE_H_
