#include "MBR.h"

MBR::MBR()//el MBR que cubre otros MBR
{
	estado = true;//es nodo padre
	subMbr = NULL;
	coMbr = NULL;
	x1 = 0; x2 = 0; y1 = 0; y2 = 0;
}

MBR::MBR(vector<pair<int, int>> pol)//el MBR que cubre al poligono
{
	estado = false;//no es nodo hoja
	subMbr = NULL;
	coMbr = NULL;
	poligono = pol;
	x1 = poligono[0].first;
	x2 = poligono[0].first;
	y1 = poligono[0].second;
	y2 = poligono[0].second;
	if (poligono.size() == 1) {//Si "poligono" es un punto
		x1 -= 5; x2 += 5; y1 -= 5; y2 += 5;
	}
	else {//si el poligono tiene mas puntos
		for (unsigned int i = 1; i<poligono.size(); i++) {
			if (poligono[i].first<x1) {
				x1 = poligono[i].first;
			}
			if (x2<poligono[i].first) {
				x2 = poligono[i].first;
			}
			if (poligono[i].second<y1) {
				y1 = poligono[i].second;
			}
			if (y2<poligono[i].second) {
				y2 = poligono[i].second;
			}
		}
	}
}

void MBR::setXY(int xx1, int xx2, int yy1, int yy2) {
	x1 = xx1; x2 = xx2; y1 = yy1; y2 = yy2;
}

void MBR::setXY(MBR* other)
{
	x1 = other->x1;
	x2 = other->x2;
	y1 = other->y1;
	y2 = other->y2;
}

int MBR::areaBetween(MBR* other)
{
	int l1, l2;
	if (x2<(other->x1) || (other->x2)<x1) {
		return 0;
	}
	else if ((other->x1)<x1 && (other->x2)<x2) {
		l1 = (other->x2) - x1;
	}
	else if (x1<(other->x1) && x2<(other->x2)) {
		l1 = x2 - (other->x1);
	}
	else {
		l1 = (other->x2) - (other->x1);
	}

	if (y2<(other->y1) || (other->y2)<y1) {
		return 0;
	}
	else if ((other->y1)<y1 && (other->y2)<y2) {
		l2 = (other->y2) - y1;
	}
	else if (y1<(other->y1) && y2<(other->y2)) {
		l2 = y2 - (other->y1);
	}
	else {
		l2 = (other->y2) - (other->y1);
	}

	return l1 * l2;
}

int MBR::count()
{
	MBR* temp = subMbr;
	int i = 0;
	while (temp != NULL) {
		i++;
		temp = temp->subMbr;
	}
	return i;
}
