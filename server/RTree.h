//============================================================================
// Name        : RTree.h
// Author      : Daniel Palomino
// Version     : 0.1
// Copyright   : Your copyright notice
// Description : RTree Backend
// Created on  : 08 ago. 2018
//============================================================================

#ifndef RTREE_H
#define RTREE_H

#include <vector>

using namespace std;

class RTree 
{
private:

public:
	RTree();
	bool split();
	bool insert(vector<pair<int,int>> polygon);//inserta polygon en el rtree
	bool mbr(vector<vector<pair<int,int>>>& mbrs);//devuelve todos los MBRs del rtree
	bool range(vector<pair<int, int>> polygon, vector<vector<pair<int, int>>>& objects);//retorna los objects dentro de polygon
	bool nearest(int k, vector<pair<int, int>> point, vector<vector<pair<int, int>>>& objects);//retorn los k objects más cercanos a point
	bool deleteAll();//Elimina todos los objetos del rtree
};

#endif