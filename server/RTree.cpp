//============================================================================
// Name        : RTree.cpp
// Author      : Daniel Palomino / Vittorino Mandujano
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : RTree Backend
// Created on  : 08 ago. 2018
//============================================================================

#include "RTree.h"

RTree::RTree(int ord = 2)
{
	M = ord;
	m = (ord + 1) / 2;
	head = new MBR;
	head->coMbr = NULL;
	head->subMbr = NULL;
}

bool RTree::insert(vector<pair<int, int>> polygon)
{
	/*MBR* fig = new MBR(polygon);

	if (head->subMbr == NULL && head->coMbr == NULL) {//la primerafigura insertada
		head->subMbr = fig;
		head->setXY(fig);//Modifica el MBR's del recorrido            
	}
	else {
		Node* temp = head;
		Node* temp2;
		int area = 0;
		//vector<MBR*> v; v.push_back(temp); se añade a una pila
		while (temp->estado) {//busca el MBR que se ajuste a fig. y asignarlo
			temp2 = temp;
			area = temp->areaBetween(fig);
			while (temp->coMbr != NULL) {
				temp = temp->coMbr;
				if ((temp->areaBetween(fig))<area) {
					area = temp->areaBetween(fig);
					temp2 = temp;
				}
			}
			temp = temp2;
			//v.push_back(temp); se añade a una pila
			temp = temp->subMbr;//revisar,puede haber un error                        
		}
		temp = temp2;//insertar
		temp2 = temp->subMbr;
		temp->subMbr = fig;
		fig->coMbr = temp2;
		//===========================================
		//todavia usamos temp
		if (M<(temp->count())) {//verificar la cantidad para hacer el split
			split();//el split
			temp2 = head;// revisar!!!!                    
		}


		//Modificar MBR's del recorrido                             
	}*/

	return true;
}

bool RTree::split() {
	return true;
}

bool RTree::getMBRs(vector<vector<pair<int, int>>>& mbrs) {
	return true;
}

bool RTree::range(vector<pair<int, int>> polygon, vector<vector<pair<int, int>>>& objects) {
	return true;
}

bool RTree::nearest(int k, vector<pair<int, int>> point, vector<vector<pair<int, int>>>& objects) {
	return true;
}

bool RTree::deleteAll() {
	return true;
}