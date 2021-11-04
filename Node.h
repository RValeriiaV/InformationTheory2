#pragma once
#include "Array.h"
typedef unsigned long long int ulInt;
class Node
{
	ulInt letter;
	Node* branch0; 
	Node* branch1; 

public: 
	Node() {
		letter = 0; 
		branch0 = nullptr;
		branch1 = nullptr;
	}

	ulInt getLetter() {
		return letter;
	}
	void setLetter(ulInt letter) {
		this->letter = letter;
	}

	Node* getBranch0() {
		return branch0;
	}
	void setBranch0(Node* branch0) {
		this->branch0 = branch0;
	}

	Node* getBranch1() {
		return branch1;
	}
	void setBranch1(Node* branch1) {
		this->branch1 = branch1;
	}

	~Node() {
		delete branch0;
		delete branch1;
	}
};

class Tree {
	Node* branch0; 
	Node* branch1; 

public: 
	Tree(Array* arr) {
		unsigned int maxSizeOfCode = 0; //ищем максимальную длину кодовой цепочки
		for (ulInt i = 0; i < arr->getSizeArr(); i++)
			if (arr->getArray()[i]->getSizeOfCode() > maxSizeOfCode) maxSizeOfCode = arr->getArray()[i]->getSizeOfCode();
		branch0 = new Node();
		branch1 = new Node();
		Branches(arr, branch0, 1, 0, maxSizeOfCode);
		Branches(arr, branch1, 1, 1, maxSizeOfCode);
	}
	void Branches(Array* arr, Node* cur, unsigned int sizeOfCode, ulInt code, unsigned int maxSizeOfCode) {
		for (ulInt i = 0; i < arr->getSizeArr(); i++)
			if (arr->getArray()[i]->getCode() == code && arr->getArray()[i]->getSizeOfCode() == sizeOfCode) {
				cur->setLetter(i);
				return;
			}
		if (sizeOfCode == maxSizeOfCode) return; 
		Node* node0 = new Node();
		Node* node1 = new Node(); 
		cur->setBranch0(node0);
		cur->setBranch1(node1);
		Branches(arr, node0, sizeOfCode + 1, code * 2, maxSizeOfCode);
		Branches(arr, node1, sizeOfCode + 1, code * 2 + 1, maxSizeOfCode);
	}

	Node* getBranch0() {
		return branch0;
	}
	Node* getBranch1() {
		return branch1;
	}

	~Tree() {
		delete branch0; 
		delete branch1; 
	}
};

