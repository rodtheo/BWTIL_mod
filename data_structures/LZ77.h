/*
 *  This file is part of BWTIL.
 *  Copyright (c) by
 *  Nicola Prezza <nicolapr@gmail.com>
 *
 *   BWTIL is free software: you can redistribute it and/or modify
 *   it under the terms of the GNU General Public License as published by
 *   the Free Software Foundation, either version 3 of the License, or
 *   (at your option) any later version.

 *   BWTIL is distributed in the hope that it will be useful,
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *   GNU General Public License for more details (<http://www.gnu.org/licenses/>).
 */

//============================================================================
// Name        : LZ77.h
// Author      : Nicola Prezza
// Version     : 1.0
// Description : In this version, just count the number of LZ77 phrases of a text.

/*
 * Counts number of phrases of the LZ77 parse. Uses a dynamic compressed BWT.
 * zero-order compressed space, n log n time.
 *
 */
//============================================================================

#ifndef LZ77_H_
#define LZ77_H_

#include "../common/common.h"
#include "DynamicBWT.h"
#include "FileReader.h"


namespace bwtil {

template <typename bitvector_type>
class LZ77 {

public:

	LZ77(){}

	/*
	 * given the path of a ASCII-coded file, builds the LZ77 parse. Warning: the text file must contain < 256 distinct characters (one is reserved for the BWT terminator)
	 */
	LZ77(string path, bool verbose=false){

		symbol_to_int = vector<uint>(256);
		sigma=0;

		if(verbose) cout << "Scanning input file to compute character frequencies ... " << endl<<endl;

		FileReader fr(path);
		set<symbol> alphabet;

		while(not fr.eof())
			alphabet.insert(fr.get());

		fr.rewind();

		for(set<symbol>::iterator it=alphabet.begin();it!=alphabet.end();++it){

			symbol_to_int[*it] = sigma;
			sigma++;

		}

		assert(sigma<256);

		vector<ulint> freqs(sigma,0);

		while(not fr.eof())
			freqs[ symbol_to_int[fr.get()] ]++;

		fr.rewind();

		//start parsing

		number_of_phrases=0;
		DynamicBWT<bitvector_type> dbwt(freqs);

		pair<ulint, ulint> interval(0,dbwt.size());

		if(verbose) cout << "Parsing input file ... " << endl<<endl;

		ulint i=0;//characters read
		ulint n=fr.size();//file length
		int last_perc=-1;

		while(not fr.eof()){

			//read next symbol from file
			symbol s = fr.get();

			//obtain interval of the symbol
			interval = dbwt.BS( interval, symbol_to_int[s] );

			if(interval.second<=interval.first){

				//no matches: extend with s, increment phrases, begin new phrase

				dbwt.extend( symbol_to_int[s] );

				number_of_phrases++;

				interval = pair<ulint, ulint>(0,dbwt.size());

			}else{

				//we have at least one match: extend with s

				dbwt.extend( symbol_to_int[s] );
				interval.second++;//increment upper bound because we inserted a new prefix that falls in this interval


			}

			i++;

			if(verbose){

				int perc = (i*100)/n;

				if(perc>last_perc){
					cout << perc << "% done." << endl;
					last_perc=perc;
				}

			}

		}

		fr.close();

	}

	ulint getNumberOfPhrases(){return number_of_phrases;}

private:



	//remapping of symbols:
	vector<uint> symbol_to_int;

	uint sigma;//alphabet size

	ulint number_of_phrases;

};

typedef LZ77<bitv> lz77_t;

} /* namespace data_structures */
#endif /* LZ77_H_ */
