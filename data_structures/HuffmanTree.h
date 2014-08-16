/*
 * HuffmanTree.h
 *
 *  Created on: Jun 15, 2014
 *      Author: nicola
 */

#ifndef HUFFMANTREE_H_
#define HUFFMANTREE_H_

#include "../common/common.h"

namespace bwtil {

class HuffmanTree {
public:

	//a node of the tree
	class Node{

	public:

		Node(symbol label, ulint freq){
			this->freq=freq;
			this->label=label;
			leaf=true;
		}

		Node(symbol nr, Node l, Node r){

			node_number=nr;
			freq = l.freq+r.freq;
			leaf=false;
			leaf_right = r.leaf;
			leaf_left = l.leaf;

			if(leaf_left)
				left = l.label;
			else
				left = l.node_number;

			if(leaf_right)
				right = r.label;
			else
				right = r.node_number;

		}

		bool operator<(const Node & n) const { return freq<n.freq; };

		symbol node_number;
		ulint freq;
		symbol label;//label if leaf
		bool leaf;//this node is leaf
		bool leaf_left;
		bool leaf_right;

		symbol left;//pointers to children
		symbol right;

	};

	HuffmanTree(){};

	//freq = array containing absolute number of occurrencies of each symbol {0,...,freq.size()-1}
	HuffmanTree(vector<ulint> freq){

		sigma_0 = freq.size();

		sigma = 0;//number of symbols with frequency > 0

		frequencies = freq;//copy freq

		ulint tot=0;

		for(uint i=0;i<sigma_0;i++){

			frequencies.at(i) = freq.at(i);

			tot+=frequencies.at(i);

			if(freq.at(i)>0)
				sigma++;

		}

		if(tot==0){
			cout << "Error (HuffmanTree constructor) : Empty Huffman tree.\n";
			exit(0);
		}

		codes = vector<vector<bool> >(sigma_0);//code associated to each symbol (empty vector if symbol has freq=0)

		//build Huffman tree using objects, then copy it in a more compact format inside the above vectors

		multiset<Node> nodes;
		vector<Node> nodes_vec;//here nodes (only internal nodes) are stored but not deleted.

		//create leafs
		for(uint i=0;i<sigma_0;i++)
			if(frequencies.at(i)>0)
				nodes.insert(Node(i,frequencies.at(i)));

		//Huffman's algorithm

		while(nodes.size()>1){//repeat until all trees are merged

			Node min1 = *nodes.begin();//extract the 2 smallest nodes
			nodes.erase(nodes.begin());//erase the 2 smallest nodes

			Node min2 = *nodes.begin();
			nodes.erase(nodes.begin());

			nodes.insert(Node(nodes_vec.size(),min1,min2));
			nodes_vec.push_back(Node(nodes_vec.size(),min1,min2));

		}

		//save the tree in arrays
		storeTree(*nodes.begin(),nodes_vec);

		//free memory
		nodes.clear();
		nodes_vec.clear();

	}

	void debug(){

		cout << "Coding: \n";

		for(uint i=0;i<sigma_0;i++){

			cout << i << " -> ";

			for(uint j=0;j<codes[i].size();j++)
				cout << codes[i].at(j);

			cout << endl;

		}

		cout << "\nDecoding: \n";

		for(uint i=0;i<sigma_0;i++){

			for(uint j=0;j<codes[i].size();j++)
				cout << codes[i].at(j);

			/*cout << " -> ";

			if(codes[i].size()>0)
				cout << (uint)decode(codes[i]);*/

			cout << endl;

		}

		cout << "\nentropy is " << entropy() << " bits per symbol" << endl;


	}

	double entropy(){//get entropy

		double entropy = 0;
		double tot=0;

		for(uint i=0;i<sigma_0;i++)
			tot+=frequencies.at(i);

		if(tot==0){
			cout << "Error (HuffmanTree entropy): Empty Huffman tree.\n";
			exit(0);
		}

		for(uint i=0;i<sigma_0;i++){

			double l = codes[i].size();
			double f = (double)frequencies.at(i)/tot;

			entropy += l*f;

		}

		return entropy;

	}

	ulint numberOfOccurrencies(symbol s){return frequencies.at(s);};//number of occurrencies of the symbol s

	vector<vector<bool> > getCodes(){return codes;}

	vector<bool> code(symbol s){return codes.at(s);};//from symbol -> to its binary Huffman code (compression)

private:

	void storeTree(Node n,vector<Node> nodes_vec){

			//root_node = n.node_number;
			storeTree(vector<bool>(),n,nodes_vec);

		}

	void storeTree(vector<bool> code, Node n,vector<Node> nodes_vec){

		vector<bool> codel = code;
		vector<bool> coder = code;

		codel.push_back(0);
		coder.push_back(1);

		//symbol node_nr = n.node_number;

		if(n.leaf_left){//leaf

			codes[n.left] = codel;
			//left_leafs.at(node_nr) = true;
			//left[node_nr] = n.left;

		}else{

			//left_leafs.at(node_nr) = false;
			//left[node_nr] = n.left;

			storeTree(codel,nodes_vec.at(n.left), nodes_vec);

		}

		if(n.leaf_right){//leaf

			codes[n.right] = coder;
			//right_leafs.at(node_nr) = true;
			//right[node_nr] = n.right;

		}else{

			//right_leafs.at(node_nr) = false;
			//right[node_nr] = n.right;

			storeTree(coder,nodes_vec.at(n.right), nodes_vec);

		}

	}

	//symbol decode(vector<bool> c, uint pos, symbol node);

	uint8 sigma_0;//nr of symbols
	uint8 sigma;//nr of symbols with frequency > 0

	//the Huffman tree:
	//symbol root_node;
	//symbol * left;//sigma-1 left pointers
	//symbol * right;//sigma-1 right pointers
	//vector<bool> left_leafs;//for each internal node i, memorizes if the symbol in left is pointer to internal nodes (0) or leaf label (1)
	//vector<bool> right_leafs;//for each internal node i, memorizes if the symbol in left is pointer to internal nodes (0) or leaf label (1)

	vector<ulint> frequencies;//table of sigma_0 entries. Symbol -> number of occurrencies.

	vector<vector<bool> > codes;

};

} /* namespace compressed_bwt_construction */
#endif /* HUFFMANTREE_H_ */
