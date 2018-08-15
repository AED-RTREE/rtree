//============================================================================
// Name        : Structs.h
// Author      : Daniel Palomino
// Version     : 1.0
// Copyright   : GNU General Public License v3.0
// Description : RTree Backend
// Created on  : 14 ago. 2018
//============================================================================

#ifndef STRUCTS_H
#define STRUCTS_H

#include <vector>
#include "Constants.h"

using namespace std;


struct Rect
{
	Rect() {}

	Rect(int a_minX, int a_minY, int a_maxX, int a_maxY)
	{
		m_min[0] = a_minX;
		m_min[1] = a_minY;

		m_max[0] = a_maxX;
		m_max[1] = a_maxY;
	}

	int m_min[2];                      ///< Min dimensions of bounding box 
	int m_max[2];                      ///< Max dimensions of bounding box 
};

struct  Node;

struct Branch
{
	Rect m_rect;                                  ///< Bounds
	Node* m_child;                                ///< Child node
	vector<pair<int, int>> m_data;                              ///< Data Id
};

struct Node
{
	bool IsInternalNode() { return (m_level > 0); } // Not a leaf, but a internal node
	bool IsLeaf() { return (m_level == 0); } // A leaf, contains data

	int m_count;                                  ///< Count
	int m_level;                                  ///< Leaf is zero, others positive
	Branch m_branch[MAXNODES];                    ///< Branch
};

/// A link list of nodes for reinsertion after a delete operation
struct ListNode
{
	ListNode* m_next;                             ///< Next in list
	Node* m_node;                                 ///< Node
};

/// Variables for finding a split partition
struct PartitionVars
{
	enum { NOT_TAKEN = -1 }; // indicates that position

	int m_partition[MAXNODES + 1];
	int m_total;
	int m_minFill;
	int m_count[2];
	Rect m_cover[2];
	float m_area[2];

	Branch m_branchBuf[MAXNODES + 1];
	int m_branchCount;
	Rect m_coverSplit;
	float m_coverSplitArea;
};

#endif