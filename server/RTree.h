//============================================================================
// Name        : RTree.h
// Author      : Daniel Palomino / Vittorino Mandujano
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : RTree Backend
// Created on  : 08 ago. 2018
//============================================================================

#ifndef RTREE_H
#define RTREE_H

#include <vector>
#include <iostream>
#include <cstdlib>
#include "MBR.h"

using namespace std;

class RTree
{
private:
	MBR * head;
	int M, m; //para saber cuantos hijos debe tener cada nodo   
public:
	RTree(int ord);
	bool insert(vector<pair<int, int>> polygon);//inserta polygon en el rtree
	bool getMBRs(vector<vector<pair<int, int>>>& mbrs);//devuelve todos los MBRs del rtree
	bool range(vector<pair<int, int>> polygon, vector<vector<pair<int, int>>>& objects);//retorna los objects dentro de polygon
	bool nearest(int k, vector<pair<int, int>> point, vector<vector<pair<int, int>>>& objects);//retorn los k objects más cercanos a point
	bool split();
	bool deleteAll();//Elimina todos los objetos del rtree
};

#endif