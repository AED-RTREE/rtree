//============================================================================
// Name        : RTree.h
// Author      : Daniel Palomino / Vittorino Mandujano
// Version     : 1.0
// Copyright   : GNU General Public License v3.0
// Description : RTree Backend
// Created on  : 08 ago. 2018
//============================================================================

#ifndef RTREE_H
#define RTREE_H

#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <limits>
#include <iostream>

#include "Constants.h"
#include "Structs.h"

using namespace std;

#define ASSERT assert // RTree uses ASSERT( condition )
#define Min min
#define Max max

class RTree
{
public:

	RTree();
	RTree(const RTree& other);
	virtual ~RTree();

	void Insert(const int a_min[2], const int a_max[2], vector<pair<int, int>>& a_dataId);
	void Remove(const int a_min[2], const int a_max[2], const vector<pair<int, int>>& a_dataId);
	int Search(const pair<int, int> a_min, const pair<int, int> a_max, vector<vector<pair<int, int>>>& objs) const;
	void RemoveAll();

	/// Count the data elements in this container.  This is slow as no internal counter is maintained.
	int Count();

	/// get the MBRs;
	bool getMBRs(vector<vector<vector<pair<int, int>>>>& mbrs_n);
	bool nearest(int k, vector<pair<int, int>> points, vector<vector<pair<int, int>>>& v);
	Rect MBR(vector<pair<int, int>> pol);
	int MinDist(pair<int, int> point, Rect a_rect);	
	int MinMaxDist(pair<int, int> point, Rect a_rect);

	vector<vector<pair<int, int>>> mObjs;
	bool onSegment(pair<int, int> p, pair<int, int> q, pair<int, int> r);
	int orientation(pair<int, int> p, pair<int, int> q, pair<int, int> r);
	bool doIntersect(pair<int, int> p1, pair<int, int> q1, pair<int, int> p2, pair<int, int> q2);
	bool isInside(vector<pair<int, int>>& polygon, pair<int, int> p);
	float distanceFromLine(const pair<int, int>& p, const pair<int, int>& l1, const pair<int, int>& l2);
	float distanceFromPoly(const pair<int, int>& p, vector<pair<int, int>>& poly);

protected:

	Node* AllocNode();
	void FreeNode(Node* a_node);
	void InitNode(Node* a_node);
	void InitRect(Rect* a_rect);
	bool InsertRectRec(const Branch& a_branch, Node* a_node, Node** a_newNode, int a_level);
	bool InsertRect(const Branch& a_branch, Node** a_root, int a_level);
	Rect NodeCover(Node* a_node);
	bool AddBranch(const Branch* a_branch, Node* a_node, Node** a_newNode);
	void DisconnectBranch(Node* a_node, int a_index);
	int PickBranch(const Rect* a_rect, Node* a_node);
	Rect CombineRect(const Rect* a_rectA, const Rect* a_rectB);
	void SplitNode(Node* a_node, const Branch* a_branch, Node** a_newNode);
	float CalcRectVolume(Rect* a_rect);
	void GetBranches(Node* a_node, const Branch* a_branch, PartitionVars* a_parVars);
	void ChoosePartition(PartitionVars* a_parVars, int a_minFill);
	void LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars);
	void InitParVars(PartitionVars* a_parVars, int a_maxRects, int a_minFill);
	void PickSeeds(PartitionVars* a_parVars);
	void Classify(int a_index, int a_group, PartitionVars* a_parVars);
	bool RemoveRect(Rect* a_rect, const vector<pair<int, int>>& a_id, Node** a_root);
	bool RemoveRectRec(Rect* a_rect, const vector<pair<int, int>>& a_id, Node* a_node, ListNode** a_listNode);
	ListNode* AllocListNode();
	void FreeListNode(ListNode* a_listNode);
	bool Overlap(Rect* a_rectA, Rect* a_rectB) const;
	bool Overlap2(Rect* a_rectA, Rect* a_rectB) const;
	void ReInsert(Node* a_node, ListNode** a_listNode);
	bool Search(Node* a_node, Rect* a_rect, int& a_foundCount, vector<vector<pair<int, int>>>& objs) const;
	void RemoveAllRec(Node* a_node);
	void Reset();
	void CountRec(Node* a_node, int& a_count);
	
	void CopyRec(Node* current, Node* other);

	Node* m_root;                                    ///< Root of tree
	float m_unitSphereVolume;                 ///< Unit sphere constant for required number of dimensions
};

#endif