/*
 * Query.cpp
 *
 *  Created on: 27 ��� 2013
 *      Author: mple
 */

#include "Query.h"
#include "../Includes/core.h"
#include <iostream>
#include "string.h"
#include <tuple>
using namespace std;
namespace mple {


Query::~Query() {
	// TODO Auto-generated destructor stub
}


void Query::AddToken(const char * token){
  int i = 0, s;
  while(i< NTokens && (s = strcmp(tokens[i],token)) < 0){
    i++;
  }
  if (i< NTokens && s == 0){
	  return;
  }

  memmove(tokens[i]+MAX_WORD_LENGTH+1, tokens[i], (NTokens-i)*(MAX_WORD_LENGTH+1));
  strcpy(tokens[i], token);
  NTokens++;
}

void Query::printall(void){
  cout << "qid:" << qid << endl;
  /*cout << "type -- dist" << type_<<" "<< dist_<<endl;
  cout << "NTokens:" << (int)NTokens << endl;
  for (int i=0 ; i<NTokens; ++i)
    cout << "token" << i << ": "<<tokens[i] << endl;*/
}


unsigned int Query::hashvalue(){
	int t=0;
  unsigned int i=0;
    for(int x=0;x<NTokens;x+=2){
      i=0;
      while(tokens[x][i] != '\0'){
    	  t += (x+1)*(i+1)*(tokens[x][i]);
        i++;
        }
    }
    t = t +100000*((dist_+(3*type_)));//first digit 1XXXXX or 2XXXXX is the type

    return t;
}
} /* namespace mple */
