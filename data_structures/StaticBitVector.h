//============================================================================
// Name        : StaticBitVector.h
// Author      : Nicola Prezza and Alberto Policriti
// Version     : 1.0
// Copyright   : GNU General Public License (http://www.gnu.org/copyleft/gpl.html)
// Description :

 /*
 *   This class implements a bitvector with constant time support for rank and access queries
 *   space occupancy = n + n/D^2 * log n + n/D * log D^2 (D=24 in the default setting)
 */

//============================================================================



#ifndef STATICBITVECTOR_H_
#define STATICBITVECTOR_H_

#include "WordVector.h"
#include "../common/common.h"

namespace bwtil {

class StaticBitVector {

private:

	string printWord(ulint x){

		string s;

		for(uint i=0;i<64;i++)
			s += (((x>>(63-i))&1)==0?'0':'1');

		return s;

	}

public:

	StaticBitVector(){};

	StaticBitVector(ulint n) {

		this->n = n;

		w = ceil(log2(n));

		if(w<1) w=1;

		D = 63;

		/*
		 * bits are grouped in words of D bits.
		 * rank structures:
		 * 	1) one pointer of log n bits every D^2 positions (rank_ptrs_1)
		 * 	2) one pointer of log D^2 bits every D positions (rank_ptrs_2)
		 *
		 * 	space occupancy = n + n/D^2 * log n + n/D * log D^2
		 *
		 */

		bitvector = WordVector( ceil((double)(n+1)/D) , D);// the bitvector
		rank_ptrs_1 = WordVector( ceil((double)(n+1)/(D*D)) , w);//log n bits every D^2 positions
		rank_ptrs_2 = WordVector( ceil((double)(n+1)/D) , 2*ceil(log2(D)));//2 log D bits every D positions

	}

	void computeRanks(){//compute rank structures basing on the content of the bitvector

		ulint nr_of_ones_global = 0;
		ulint nr_of_ones_local = 0;

		rank_ptrs_1.setWord(0,0);
		rank_ptrs_2.setWord(0,0);

		if(n==0)
			return;

		for(ulint i=0;i<n;i++){

			if((i+1)%(D*D)==0)
				nr_of_ones_local = 0;
			else
				nr_of_ones_local += bitAt(i);

			nr_of_ones_global += bitAt(i);

			if((i+1)%D==0){
				rank_ptrs_2.setWord((i+1)/D,nr_of_ones_local);

				if(rank_ptrs_2.wordAt((i+1)/D)>1000000000){

					cout << "\n*** ERR : read "<<rank_ptrs_2.wordAt((i+1)/D)<< " but write " << nr_of_ones_local << endl;//TODO debugging
					cout << "*** Position: " << (i+1)/D << endl;//TODO debugging
					cout << "*** max size: " << rank_ptrs_2.length() << endl;//TODO debugging

					cout << "This error shows up when compiling with clang++ and needs debugging. Please, switch to g++." << endl;
					exit(0);

				}

			}

			if((i+1)%(D*D)==0)
				rank_ptrs_1.setWord((i+1)/(D*D),nr_of_ones_global);

		}

	}

	ulint size(){

		return bitvector.size() + rank_ptrs_1.size() + rank_ptrs_2.size();

	}

	inline ulint rank1(ulint i){//number of 1's before position i (excluded) in the bitvector

		return rank_ptrs_1.wordAt(i/(D*D)) + rank_ptrs_2.wordAt(i/D) + rank(bitvector.wordAt(i/D),i%D);

	}

	inline ulint rank0(ulint i){return i-rank1(i);}//number of 0's before position i (excluded) in the bitvector

	inline uint bitAt(ulint i){//get value of the i-th bit

		return bitvector.bitAt(i);

	}

	inline void setBit(ulint i, uint b){//set bit in position i

		bitvector.setBit(i,b);

	}

	void test(){

		for(uint i=0;i<rank_ptrs_1.length();i++)
			cout << rank_ptrs_1.wordAt(i) << ", ";

		cout << endl;
		for(uint i=0;i<rank_ptrs_2.length();i++)
			cout << rank_ptrs_2.wordAt(i) << ", ";


	}

	void freeMemory(){

		bitvector.freeMemory();
		rank_ptrs_1.freeMemory();
		rank_ptrs_2.freeMemory();

	}

	void saveToFile(FILE *fp){

		fwrite(&n, sizeof(ulint), 1, fp);
		fwrite(&w, sizeof(uint), 1, fp);
		fwrite(&D, sizeof(uint), 1, fp);

		bitvector.saveToFile(fp);
		rank_ptrs_1.saveToFile(fp);
		rank_ptrs_2.saveToFile(fp);

	}

	void loadFromFile(FILE *fp){

		ulint numBytes;

		numBytes = fread(&n, sizeof(ulint), 1, fp);
		check_numBytes();
		numBytes = fread(&w, sizeof(uint), 1, fp);
		check_numBytes();
		numBytes = fread(&D, sizeof(uint), 1, fp);
		check_numBytes();

		bitvector = WordVector();
		rank_ptrs_1 = WordVector();
		rank_ptrs_2 = WordVector();

		bitvector.loadFromFile(fp);
		rank_ptrs_1.loadFromFile(fp);
		rank_ptrs_2.loadFromFile(fp);

	}

	ulint length(){return n;}

	ulint numberOf1(){return rank1(n);}
	ulint numberOf0(){return n-numberOf1();}

protected:

	ulint rank(ulint W, uint i){//number of 1's before position i (excluded) in the word W (of D bits)

		//warning: i must be <D

		ulint mask = (((ulint)1<<i)-1)<<(D-i);
		ulint masked = W & mask;

		return popcnt(masked);

	}

	ulint n;//length of the bitvector
	uint w;//size of the word = log2 n

	WordVector bitvector;//the bits are stored in a WordVector, so that they can be accessed in blocks of size D
	WordVector rank_ptrs_1;//rank pointers sampled every D^2 positions
	WordVector rank_ptrs_2;//rank pointers sampled every D positions

	uint D;//size of words

};

} /* namespace data_structures */
#endif /* BITVECTOR_H_ */
