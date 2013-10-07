/*
 * top_impl.h
 *
 *  Created on: Mar 12, 2013
 *      Author: sib
 */

#ifndef TOP_IMPL_H_
#define TOP_IMPL_H_

#include "Includes/core.h"
#include "Includes/defn.h"
#include "math.h"

#define THREADS 24
#define STUCKTHREADS 18
#define JOBS 12

#include <algorithm>
///////////////////////////////////////////////////////////////////////////////////////////////
// Computes Hamming distance between a null-terminated string "a" with length "na"
//  and a null-terminated string "b" with length "nb"
struct Document {
	DocID doc_id;
	unsigned int num_res;
	QueryID* query_ids;
};

inline int ana(int n,int k)
{
    int bla=1,bla2=1;
    for(int i=1;i<=k;i++)
        bla*=i;
    for(int i=n-k+1;i<=n;i++)
        bla2*=i;
    return bla2/bla;
}

// Keeps all currently available results that has not been returned yet

inline int HammingDistance(char* a, int na, char* b, int nb,int thr)
{
	int j, oo=100;

	int num_mismatches=0;
	for(j=0;j<na && num_mismatches<=thr;j++){
		num_mismatches+= (a[j]!=b[j]);
	}

	return ((num_mismatches<=thr)?num_mismatches:thr+5);
}
inline
void TranslateTypeDistance(bool size,char byte, MatchType * mtype, char * dist) {
	*dist=5;
	if (size) {
		char hamming_mask= (byte & 0b00001110) >>1;
		char editdist_mask=(byte & 0b01110000) >>4;
		if(hamming_mask!=0)
		{
			*mtype = MT_HAMMING_DIST;
			*dist=(hamming_mask>3)?3:((hamming_mask>1)?2:1);
			return;
		}
		else if(editdist_mask!=0)
		{
			*mtype = MT_EDIT_DIST;
			*dist=(editdist_mask>3)?3:((editdist_mask>1)?2:1);
			return;
		}
		else if ((byte & 0b00000001) == 0b00000001){
			*mtype = MT_EXACT_MATCH;
			*dist = 0;
			return;
		}
	}
			

	else {
		
		char editdist_mask=(byte & 0b01110000) >>4;
		if(editdist_mask!=0)
		{
			*mtype = MT_EDIT_DIST;
			*dist=(editdist_mask>3)?3:((editdist_mask>1)?2:1);
			return;
		}

	}
}

inline int test_max(int x, int y) {
	return ( x > y ) ? x : y;
	}

inline int test_min(int x, int y){
	return ( x > y ) ? y : x;
}



inline char EditHammingdiff3(char *s,char *t,int n,int m)
{
	int startpos;
	int i,j;
	int bla;
	int bla2;
	int bla3;
	int diag;
	int diag2;
	int diag3;
	for(i=0;i<n;++i)
	{
		if(s[i]!=t[i])
		{
			startpos=i;
			break;
		}
	}
	if(i==n)
		return (0b01000000);//prefix
	bla=startpos;
	for(diag=1;diag<=3;++diag)
	{
		i=bla;
		j=bla+diag;
		if(s[i]==t[j])
		{
			++i;
			++j;
			while(i<n)
			{
				if(s[i]!=t[j])
				{
					startpos=i;
					break;
				}
				++i;
				++j;
			}
			if(i==n)
				return (0b01000000);
			bla2=startpos;
			for(diag2=diag+1;diag2<=3;++diag2)
			{
				i=bla2;
				j=bla2+diag2;
				if(s[i]==t[j])
				{
					++i;
					++j;
					while(i<n)
					{
						if(s[i]!=t[j])
						{
							startpos=i;
							break;
						}
						++i;
						++j;
					}
					if(i==n)
						return (0b01000000);
					bla3=startpos;
					diag3=diag2+1;
					if(diag3==3)
					{
						i=bla3;
						j=bla3+diag3;
						while(s[i]==t[j] && i<n)
						{
							++i;
							++j;
						}
						if(i==n)
							return (0b01000000);
					}
				}
			}
		}
	}
	return 0;
}


inline char EditHammingdiff2(char *s,char *t,int n,int m)
{
	int startpos[2];
	int i;
	int c=0;
	int h1=0;
	int h2=0;
	int temp;
	for( i=0 ; i<n ; ++i )
	{
		temp = (s[i]!=t[i]);
		startpos[c] = i;
		c+=temp;
		if(c==2)
			break;
	}
	if(c==0)
		return (0b01100000);//prefix
	if(c==1)
	{
		startpos[1]=n-1;
		if(startpos[0]==n-1)
		{
			temp=1;
			temp-=((s[n-1]!=t[n-1])*(s[n-1]==t[n+1])) || ((s[n-1]!=t[n-1])*(s[n-1]==t[n]));
			return (0b01100000)<<temp;
		}
	}
	i=startpos[0];
	c=0;
	while(i<startpos[1])
	{
		if(s[i]!=t[i+2])
			++h2;
		else
			h2=test_min(h1,h2);
		if(s[i]!=t[i+1])
			h1=1;
		if(h1>0 && h2>0)
		{
			break;
		}
		i++;
	}
	h1=test_min(1,h1);
	h2=test_min(1,h2);
	if(h1>0 && h2>0)
	{
		c=1;
		i=startpos[1];
		if(i==n-1)
		{
			temp=test_min(h1,h2);
			temp+=(((s[n-1]!=t[n-1])*(s[n-1]!=t[n+1]))*(s[n-1]!=t[n]));
			return (0b01100000)<<temp;
		}
	}
	while(i<n)
	{
		if(s[i]!=t[i+2])
			++h2;
		else
			h2=test_min(h1,h2);
		if(s[i]!=t[i+1])
			++h1;
		if(h2>1 && h1>1)
			return 0;
		++i;
	}
	temp=(test_min(h1,h2));
	return (0b01100000)<<temp;
}



inline char EditHammingdiff1(char *s, char *t, int n,int m)
{
	int h=0;
	int hr=1;
	int hr2=2;
	int hl=1;
	int temp;
	int th,thr,thr2,thl;
	int diag=0;
	int diagl=0;
	int diagr=0;
	int diagr2=0;
	int i=0;
	diag=(s[i]==t[i]);
	diagr=(s[i]==t[i+1]);
	diagr2=(s[i]==t[i+2]);
	diagl=(s[i+1]==t[i]);
	thr=hr;
	thr2=hr2;
	thl=hl;
	h=(test_min(h,test_min(hr+1,hl+1)))+(!diag);
	hr=(test_min(h+(s[i]==t[i]),test_min(hr,test_min(hr2+1,hl+1+1))))+(!diagr);
	hr2=(test_min(h+1+(s[i]==t[i]),test_min(hr+(s[i]==t[i+1]),hr2)))+(!diagr2);
	hl=(test_min(h+1,hl))+(!diagl);
	temp=test_min(h+1,test_min(hr,test_min(hr2,hl+1)));
	for( i=1 ; i<n ; ++i )
	{
		diag=(s[i]==t[i]);
		diagr=(s[i]==t[i+1]);
		diagr2=(s[i]==t[i+2]);
		diagl=(s[i+1]==t[i]);
		th=h;
		thr=hr;
		thr2=hr2;
		thl=hl;
		h=(test_min(h,test_min(thr+(s[i-1]==t[i]),thl+(s[i]==t[i-1]))))+(!diag);
		hr=(test_min(h+(s[i]==t[i]),test_min(hr,test_min(thr2+(s[i-1]==t[i+1]),thl+1+(s[i]==t[i-1])))))+(!diagr);
		hr2=(test_min(h+1+(s[i]==t[i]),test_min(hr+(s[i]==t[i+1]),hr2)))+(!diagr2);
		hl=(test_min(h+(s[i]==t[i]),hl))+(!diagl);
		temp=test_min(h+1,test_min(hr,test_min(hr2,hl+1)));
		if(temp>3)
			return 0;
	}
	return 0b00111000<<temp;
}




inline char EditHamming(char * s, char * t, int n,char requests)
{
	int ed;
	char edrequests;
	int HamEx;
	int result;
	int temp;
	int temp1;
	int i=0,j=0;
	int c=0;
	int startpos[5];
	int val;
	int cur_thr;
	char res;
	int thr;
	ed=10;
	edrequests=( requests & 0b1110000 );
	thr=(edrequests>=64 ?3:(edrequests>=32 ? 2 :(edrequests>=16? 1:0)));
	HamEx = 0b1111;
	for( i=0 ; ((i<n) && HamEx ) ; ++i )
	{
		temp = (s[i]!=t[i]);
		HamEx = (HamEx<<temp)&0b1111;
		startpos[c] = i;
		c+=temp;
	}

	ed = c + n-i; //periptwsh to ena na einai sxedon prefix tou allou
	result = (HamEx<<3)|HamEx;
	result = result | ( (0b0111000<<ed) & 0b01111111) ;

	if( ( edrequests & result ) == edrequests )
		return ( requests & result );
	for(int p=0;p<2;++p)
	{
		i=startpos[p];
		j=startpos[p]+1;
		for(int q=0;q<2;++q)
		{
			cur_thr=test_min(ed-1,thr);
			c=2;
			val=p+1;
			startpos[2]=startpos[3]=n-1;
			while(i<n && j<n && val<cur_thr)
			{
				temp = (s[i]!=t[j]);
				val += temp ;
				startpos[c]=(q==0)?i:j;
				c+=temp;
				i++;
				j++;
			}
			startpos[3] = (p==1)?n-1:startpos[3];
			if(i==n || j==n)
			{
				temp1=ed;
				ed=test_min(ed,val+1-(s[n-1]==t[n-1])*temp);
				res = ((0b0111000<<ed) & 0b01111111) ;
				if(temp1 != ed)
					if( (edrequests & res) == edrequests)
						return (result|res)&requests;
			}
			val=p+2;
			for(i=startpos[2]+1;i<startpos[3]+1;++i)
			{
				temp = (s[i]!=t[i]);
				val += temp;
			}
			i=startpos[3]+1;
			val=test_min(val,p+3);

			while(i<n)
			{
				temp = (s[i]!=t[i]);
				val += temp;
				++i;
			}
			temp=ed;
			ed=test_min(ed,val);
			res = ((0b0111000<<ed) & 0b01111111) ;
			if( (edrequests & res) == edrequests)
				return (result|res)&requests;
			i=startpos[p]+1;
			j=startpos[p];
		}
	}
	result = result | ( (0b0111000<<ed) & 0b01111111);
	return result&requests;
}

inline char stavroshtein(char *s,char *t, int n, int m)
{
	char result=0;
	if(n > m) {
		// swap the two strings to consume less memory
		char* tmp = s;
		s = t;
		t = tmp;
		int tmplen=n;
		n = m;
		m = tmplen;
	}
	int diff=m-n;
	switch(diff){
		case 0:
//			cout<<"calling edithamming0"<<endl;
			result=EditHamming(s,t,n,127);
			break;
		case 1:
//			cout<<"calling edithamming1"<<endl;
			result=EditHammingdiff1(s,t,n,m);
		break;
		case 2:
//			cout<<"calling edithamming2"<<endl;
			result=EditHammingdiff2(s,t,n,m);
		break;
		case 3:
//			cout<<"calling edithamming3"<<endl;
			result=EditHammingdiff3(s,t,n,m);
		break;
	}
//	cout<<"result apo stavroshtein "<<int(result)<<endl;
	return (result&0b01111111);
}



////////////










#endif /* TOP_IMPL_H_ */
