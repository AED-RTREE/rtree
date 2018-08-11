#ifndef MBR_H
#define MBR_H

#include <vector>

using namespace std;

class RTree;

class MBR
{
	friend class Rtree;

public:
	MBR* subMbr;//Los hijos del MBR
	MBR* coMbr;//Los MBR's dentro de una misma region
	vector<pair<int,int>> poligono;
	bool estado;//El estado me dice si un MBR es hoja o nodo
	int x1,x2,y1,y2;
    
public:
	MBR();
	MBR(vector<pair<int,int>> pol);
	void setXY(int x1, int x2, int y1,int y2);
	void setXY(MBR* other);
	int areaBetween(MBR* other);
	int count();
};

#endif
