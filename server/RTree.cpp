//============================================================================
// Name        : RTree.cpp
// Author      : Daniel Palomino / Vittorino Mandujano
// Version     : 1.0
// Copyright   : GNU General Public License v3.0
// Description : RTree Backend
// Created on  : 08 ago. 2018
//============================================================================

#include "RTree.h"

RTree::RTree()
{
	m_root = AllocNode();
	m_root->m_level = 0;
	m_unitSphereVolume = (float)3.141593f;
}


RTree::RTree(const RTree& other) : RTree()
{
	CopyRec(m_root, other.m_root);
}


RTree::~RTree()
{
	Reset(); 
}


void RTree::Insert(const int a_min[2], const int a_max[2], const vector<pair<int, int>>& a_dataId)
{

	Branch branch;
	branch.m_data = a_dataId;
	branch.m_child = NULL;

	for (int axis = 0; axis<2; ++axis)
	{
		branch.m_rect.m_min[axis] = a_min[axis];
		branch.m_rect.m_max[axis] = a_max[axis];
	}

	InsertRect(branch, &m_root, 0);
}


void RTree::Remove(const int a_min[2], const int a_max[2], const vector<pair<int, int>>& a_dataId)
{


	Rect rect;

	for (int axis = 0; axis<2; ++axis)
	{
		rect.m_min[axis] = a_min[axis];
		rect.m_max[axis] = a_max[axis];
	}

	RemoveRect(&rect, a_dataId, &m_root);
}


int RTree::Search(const pair<int,int> a_min, const pair<int, int> a_max, vector<vector<pair<int, int>>>& objs) const
{

	objs.clear();
	Rect rect;

	for (int axis = 0; axis<2; ++axis)
	{
		rect.m_min[0] = a_min.first;
		rect.m_min[1] = a_min.second;
		rect.m_max[0] = a_max.first;
		rect.m_max[1] = a_max.second;
	}


	int foundCount = 0;
	Search(m_root, &rect, foundCount, objs);

	return foundCount;
}


int RTree::Count()
{
	int count = 0;
	CountRec(m_root, count);

	return count;
}



void RTree::CountRec(Node* a_node, int& a_count)
{
	if (a_node->IsInternalNode())
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			CountRec(a_node->m_branch[index].m_child, a_count);
		}
	}
	else
	{
		a_count += a_node->m_count;
	}
}


void RTree::CopyRec(Node* current, Node* other)
{
	current->m_level = other->m_level;
	current->m_count = other->m_count;

	if (current->IsInternalNode())
	{
		for (int index = 0; index < current->m_count; ++index)
		{
			Branch* currentBranch = &current->m_branch[index];
			Branch* otherBranch = &other->m_branch[index];

			std::copy(otherBranch->m_rect.m_min,
				otherBranch->m_rect.m_min + 2,
				currentBranch->m_rect.m_min);

			std::copy(otherBranch->m_rect.m_max,
				otherBranch->m_rect.m_max + 2,
				currentBranch->m_rect.m_max);

			currentBranch->m_child = AllocNode();
			CopyRec(currentBranch->m_child, otherBranch->m_child);
		}
	}
	else // A leaf node
	{
		for (int index = 0; index < current->m_count; ++index)
		{
			Branch* currentBranch = &current->m_branch[index];
			Branch* otherBranch = &other->m_branch[index];

			std::copy(otherBranch->m_rect.m_min,
				otherBranch->m_rect.m_min + 2,
				currentBranch->m_rect.m_min);

			std::copy(otherBranch->m_rect.m_max,
				otherBranch->m_rect.m_max + 2,
				currentBranch->m_rect.m_max);

			currentBranch->m_data = otherBranch->m_data;
		}
	}
}


void RTree::RemoveAll()
{
	Reset();

	m_root = AllocNode();
	m_root->m_level = 0;
}


void RTree::Reset()
{
	RemoveAllRec(m_root);
}


void RTree::RemoveAllRec(Node* a_node)
{
	if (a_node->IsInternalNode()) 
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			RemoveAllRec(a_node->m_branch[index].m_child);
		}
	}
	FreeNode(a_node);
}


Node* RTree::AllocNode()
{
	Node* newNode;
	newNode = new Node;
	InitNode(newNode);
	return newNode;
}


void RTree::FreeNode(Node* a_node)
{
	delete a_node;
}


ListNode* RTree::AllocListNode()
{
	return new ListNode;
}


void RTree::FreeListNode(ListNode* a_listNode)
{
	delete a_listNode;
}


void RTree::InitNode(Node* a_node)
{
	a_node->m_count = 0;
	a_node->m_level = -1;
}


void RTree::InitRect(Rect* a_rect)
{
	for (int index = 0; index < 2; ++index)
	{
		a_rect->m_min[index] = (int)0;
		a_rect->m_max[index] = (int)0;
	}
}

bool RTree::InsertRectRec(const Branch& a_branch, Node* a_node, Node** a_newNode, int a_level)
{
	if (a_node->m_level > a_level)
	{
		Node* otherNode;

		int index = PickBranch(&a_branch.m_rect, a_node);


		bool childWasSplit = InsertRectRec(a_branch, a_node->m_branch[index].m_child, &otherNode, a_level);

		if (!childWasSplit)
		{

			a_node->m_branch[index].m_rect = CombineRect(&a_branch.m_rect, &(a_node->m_branch[index].m_rect));
			return false;
		}
		else
		{

			a_node->m_branch[index].m_rect = NodeCover(a_node->m_branch[index].m_child);
			Branch branch;
			branch.m_child = otherNode;
			branch.m_rect = NodeCover(otherNode);

			return AddBranch(&branch, a_node, a_newNode);
		}
	}
	else if (a_node->m_level == a_level)
	{
		return AddBranch(&a_branch, a_node, a_newNode);
	}
	else
	{
		return false;
	}
}

bool RTree::InsertRect(const Branch& a_branch, Node** a_root, int a_level)
{
	Node* newNode;

	if (InsertRectRec(a_branch, *a_root, &newNode, a_level)) 
	{

		Node* newRoot = AllocNode();
		newRoot->m_level = (*a_root)->m_level + 1;

		Branch branch;

		branch.m_rect = NodeCover(*a_root);
		branch.m_child = *a_root;
		AddBranch(&branch, newRoot, NULL);

		branch.m_rect = NodeCover(newNode);
		branch.m_child = newNode;
		AddBranch(&branch, newRoot, NULL);

		*a_root = newRoot;

		return true;
	}

	return false;
}


Rect RTree::NodeCover(Node* a_node)
{

	Rect rect = a_node->m_branch[0].m_rect;
	for (int index = 1; index < a_node->m_count; ++index)
	{
		rect = CombineRect(&rect, &(a_node->m_branch[index].m_rect));
	}

	return rect;
}


bool RTree::AddBranch(const Branch* a_branch, Node* a_node, Node** a_newNode)
{
	if (a_node->m_count < MAXNODES)  // Split won't be necessary
	{
		a_node->m_branch[a_node->m_count] = *a_branch;
		++a_node->m_count;

		return false;
	}
	else
	{
		SplitNode(a_node, a_branch, a_newNode);
		return true;
	}
}

void RTree::DisconnectBranch(Node* a_node, int a_index)
{
	a_node->m_branch[a_index] = a_node->m_branch[a_node->m_count - 1];

	--a_node->m_count;
}

int RTree::PickBranch(const Rect* a_rect, Node* a_node)
{

	bool firstTime = true;
	float increase;
	float bestIncr = (float)-1;
	float area;
	float bestArea;
	int best = 0;
	Rect tempRect;

	for (int index = 0; index < a_node->m_count; ++index)
	{
		Rect* curRect = &a_node->m_branch[index].m_rect;
		area = CalcRectVolume(curRect);
		tempRect = CombineRect(a_rect, curRect);
		increase = CalcRectVolume(&tempRect) - area;
		if ((increase < bestIncr) || firstTime)
		{
			best = index;
			bestArea = area;
			bestIncr = increase;
			firstTime = false;
		}
		else if ((increase == bestIncr) && (area < bestArea))
		{
			best = index;
			bestArea = area;
			bestIncr = increase;
		}
	}
	return best;
}

Rect RTree::CombineRect(const Rect* a_rectA, const Rect* a_rectB)
{

	Rect newRect;

	for (int index = 0; index < 2; ++index)
	{
		newRect.m_min[index] = Min(a_rectA->m_min[index], a_rectB->m_min[index]);
		newRect.m_max[index] = Max(a_rectA->m_max[index], a_rectB->m_max[index]);
	}

	return newRect;
}

void RTree::SplitNode(Node* a_node, const Branch* a_branch, Node** a_newNode)
{

	PartitionVars localVars;
	PartitionVars* parVars = &localVars;

	GetBranches(a_node, a_branch, parVars);

	ChoosePartition(parVars, MINNODES);

	*a_newNode = AllocNode();
	(*a_newNode)->m_level = a_node->m_level;

	a_node->m_count = 0;
	LoadNodes(a_node, *a_newNode, parVars);

}


float RTree::CalcRectVolume(Rect* a_rect)
{

	float sumOfSquares = (float)0;
	float radius;

	for (int index = 0; index < 2; ++index)
	{
		float halfExtent = ((float)a_rect->m_max[index] - (float)a_rect->m_min[index]) * 0.5f;
		sumOfSquares += halfExtent * halfExtent;
	}

	radius = (float)sqrt(sumOfSquares);


	return (radius * radius * m_unitSphereVolume);
}

void RTree::GetBranches(Node* a_node, const Branch* a_branch, PartitionVars* a_parVars)
{

	for (int index = 0; index < MAXNODES; ++index)
	{
		a_parVars->m_branchBuf[index] = a_node->m_branch[index];
	}
	a_parVars->m_branchBuf[MAXNODES] = *a_branch;
	a_parVars->m_branchCount = MAXNODES + 1;

	a_parVars->m_coverSplit = a_parVars->m_branchBuf[0].m_rect;
	for (int index = 1; index < MAXNODES + 1; ++index)
	{
		a_parVars->m_coverSplit = CombineRect(&a_parVars->m_coverSplit, &a_parVars->m_branchBuf[index].m_rect);
	}
	a_parVars->m_coverSplitArea = CalcRectVolume(&a_parVars->m_coverSplit);
}




void RTree::ChoosePartition(PartitionVars* a_parVars, int a_minFill)
{

	float biggestDiff;
	int group, chosen = 0, betterGroup = 0;

	InitParVars(a_parVars, a_parVars->m_branchCount, a_minFill);
	PickSeeds(a_parVars);

	while (((a_parVars->m_count[0] + a_parVars->m_count[1]) < a_parVars->m_total)
		&& (a_parVars->m_count[0] < (a_parVars->m_total - a_parVars->m_minFill))
		&& (a_parVars->m_count[1] < (a_parVars->m_total - a_parVars->m_minFill)))
	{
		biggestDiff = (float)-1;
		for (int index = 0; index<a_parVars->m_total; ++index)
		{
			if (PartitionVars::NOT_TAKEN == a_parVars->m_partition[index])
			{
				Rect* curRect = &a_parVars->m_branchBuf[index].m_rect;
				Rect rect0 = CombineRect(curRect, &a_parVars->m_cover[0]);
				Rect rect1 = CombineRect(curRect, &a_parVars->m_cover[1]);
				float growth0 = CalcRectVolume(&rect0) - a_parVars->m_area[0];
				float growth1 = CalcRectVolume(&rect1) - a_parVars->m_area[1];
				float diff = growth1 - growth0;
				if (diff >= 0)
				{
					group = 0;
				}
				else
				{
					group = 1;
					diff = -diff;
				}

				if (diff > biggestDiff)
				{
					biggestDiff = diff;
					chosen = index;
					betterGroup = group;
				}
				else if ((diff == biggestDiff) && (a_parVars->m_count[group] < a_parVars->m_count[betterGroup]))
				{
					chosen = index;
					betterGroup = group;
				}
			}
		}
		Classify(chosen, betterGroup, a_parVars);
	}

	if ((a_parVars->m_count[0] + a_parVars->m_count[1]) < a_parVars->m_total)
	{
		if (a_parVars->m_count[0] >= a_parVars->m_total - a_parVars->m_minFill)
		{
			group = 1;
		}
		else
		{
			group = 0;
		}
		for (int index = 0; index<a_parVars->m_total; ++index)
		{
			if (PartitionVars::NOT_TAKEN == a_parVars->m_partition[index])
			{
				Classify(index, group, a_parVars);
			}
		}
	}
}


void RTree::LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars)
{


	for (int index = 0; index < a_parVars->m_total; ++index)
	{

		int targetNodeIndex = a_parVars->m_partition[index];
		Node* targetNodes[] = { a_nodeA, a_nodeB };

		// It is assured that AddBranch here will not cause a node split. 
		bool nodeWasSplit = AddBranch(&a_parVars->m_branchBuf[index], targetNodes[targetNodeIndex], NULL);
	}
}

void RTree::InitParVars(PartitionVars* a_parVars, int a_maxRects, int a_minFill)
{
	a_parVars->m_count[0] = a_parVars->m_count[1] = 0;
	a_parVars->m_area[0] = a_parVars->m_area[1] = (float)0;
	a_parVars->m_total = a_maxRects;
	a_parVars->m_minFill = a_minFill;
	for (int index = 0; index < a_maxRects; ++index)
	{
		a_parVars->m_partition[index] = PartitionVars::NOT_TAKEN;
	}
}



void RTree::PickSeeds(PartitionVars* a_parVars)
{
	int seed0 = 0, seed1 = 0;
	float worst, waste;
	float area[MAXNODES + 1];

	for (int index = 0; index<a_parVars->m_total; ++index)
	{
		area[index] = CalcRectVolume(&a_parVars->m_branchBuf[index].m_rect);
	}

	worst = -a_parVars->m_coverSplitArea - 1;
	for (int indexA = 0; indexA < a_parVars->m_total - 1; ++indexA)
	{
		for (int indexB = indexA + 1; indexB < a_parVars->m_total; ++indexB)
		{
			Rect oneRect = CombineRect(&a_parVars->m_branchBuf[indexA].m_rect, &a_parVars->m_branchBuf[indexB].m_rect);
			waste = CalcRectVolume(&oneRect) - area[indexA] - area[indexB];
			if (waste > worst)
			{
				worst = waste;
				seed0 = indexA;
				seed1 = indexB;
			}
		}
	}

	Classify(seed0, 0, a_parVars);
	Classify(seed1, 1, a_parVars);
}


void RTree::Classify(int a_index, int a_group, PartitionVars* a_parVars)
{
	a_parVars->m_partition[a_index] = a_group;

	if (a_parVars->m_count[a_group] == 0)
	{
		a_parVars->m_cover[a_group] = a_parVars->m_branchBuf[a_index].m_rect;
	}
	else
	{
		a_parVars->m_cover[a_group] = CombineRect(&a_parVars->m_branchBuf[a_index].m_rect, &a_parVars->m_cover[a_group]);
	}

	a_parVars->m_area[a_group] = CalcRectVolume(&a_parVars->m_cover[a_group]);

	++a_parVars->m_count[a_group];
}

bool RTree::RemoveRect(Rect* a_rect, const vector<pair<int, int>>& a_id, Node** a_root)
{
	ListNode* reInsertList = NULL;

	if (!RemoveRectRec(a_rect, a_id, *a_root, &reInsertList))
	{
		while (reInsertList)
		{
			Node* tempNode = reInsertList->m_node;

			for (int index = 0; index < tempNode->m_count; ++index)
			{
				InsertRect(tempNode->m_branch[index],
					a_root,
					tempNode->m_level);
			}

			ListNode* remLNode = reInsertList;
			reInsertList = reInsertList->m_next;

			FreeNode(remLNode->m_node);
			FreeListNode(remLNode);
		}

		if ((*a_root)->m_count == 1 && (*a_root)->IsInternalNode())
		{
			Node* tempNode = (*a_root)->m_branch[0].m_child;

			FreeNode(*a_root);
			*a_root = tempNode;
		}
		return false;
	}
	else
	{
		return true;
	}
}

bool RTree::RemoveRectRec(Rect* a_rect, const vector<pair<int, int>>& a_id, Node* a_node, ListNode** a_listNode)
{

	if (a_node->IsInternalNode())
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			if (Overlap(a_rect, &(a_node->m_branch[index].m_rect)))
			{
				if (!RemoveRectRec(a_rect, a_id, a_node->m_branch[index].m_child, a_listNode))
				{
					if (a_node->m_branch[index].m_child->m_count >= MINNODES)
					{
						
						a_node->m_branch[index].m_rect = NodeCover(a_node->m_branch[index].m_child);
					}
					else
					{
						
						ReInsert(a_node->m_branch[index].m_child, a_listNode);
						DisconnectBranch(a_node, index); 
					}
					return false;
				}
			}
		}
		return true;
	}
	else 
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			if (a_node->m_branch[index].m_data == a_id)
			{
				DisconnectBranch(a_node, index); 
				return false;
			}
		}
		return true;
	}
}


bool RTree::Overlap(Rect* a_rectA, Rect* a_rectB) const
{

	for (int index = 0; index < 2; ++index)
	{
		if (a_rectA->m_min[index] > a_rectB->m_max[index] ||
			a_rectB->m_min[index] > a_rectA->m_max[index])
		{
			return false;
		}
	}
	return true;
}

void RTree::ReInsert(Node* a_node, ListNode** a_listNode)
{
	ListNode* newListNode;

	newListNode = AllocListNode();
	newListNode->m_node = a_node;
	newListNode->m_next = *a_listNode;
	*a_listNode = newListNode;
}


bool RTree::Search(Node* a_node, Rect* a_rect, int& a_foundCount, vector<vector<pair<int, int>>>& objs) const
{
	if (a_node->IsInternalNode())
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			if (Overlap(a_rect, &a_node->m_branch[index].m_rect))
			{
				if (!Search(a_node->m_branch[index].m_child, a_rect, a_foundCount, objs))
				{
					return false;
				}
			}
		}
	}
	else
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			if (Overlap(a_rect, &a_node->m_branch[index].m_rect))
			{
				vector<pair<int, int>>& id = a_node->m_branch[index].m_data;
				++a_foundCount;
				objs.push_back(id);
			}
		}
	}

	return true;
}


bool RTree::getMBRs(vector<vector<vector<pair<int, int>>>>& mbrs_n)
{
	vector<Branch> v, w;
	mbrs_n.clear();
	vector<vector<pair<int, int>>> mbrs;

	int n = (m_root->m_level);

	for (int i = 0; i<(m_root->m_count); i++) {
		v.push_back(m_root->m_branch[i]);
	}

	for (unsigned int i = 0; i<v.size(); i++) {
		vector<pair<int, int>> q;
		pair<int, int> p;

		p.first = v[i].m_rect.m_min[0];
		p.second = v[i].m_rect.m_min[1];
		q.push_back(p);
		p.first = v[i].m_rect.m_max[0];
		p.second = v[i].m_rect.m_max[1];
		q.push_back(p);
		mbrs.push_back(q);
	}
	mbrs_n.push_back(mbrs);
	mbrs.clear();

	while (n--) {
		for (unsigned int i = 0; i<v.size(); i++) {
			for (int j = 0; j<(v[i].m_child->m_count); j++) {
				w.push_back(v[i].m_child->m_branch[j]);
			}
		}

		v = w;
		for (unsigned int i = 0; i<v.size(); i++) {
			vector<pair<int, int>> q;
			pair<int, int> p;

			p.first = v[i].m_rect.m_min[0];
			p.second = v[i].m_rect.m_min[1];
			q.push_back(p);
			p.first = v[i].m_rect.m_max[0];
			p.second = v[i].m_rect.m_max[1];
			q.push_back(p);
			mbrs.push_back(q);
		}

		mbrs_n.push_back(mbrs);
		w.clear();
		mbrs.clear();
	}
	return true;
}


bool RTree::nearest(int k, vector<pair<int, int>> points, vector<vector<pair<int, int>>>& objs)
{
	Node* cp_root = AllocNode();
	pair<int, int> point = points[0];
	objs.clear();

	CopyRec(cp_root, m_root);

	Node* nodo;
	int x, y;

	for (int i = 0; i<k; i++) {
		nodo = cp_root;
		Branch mbranch;

		if (cp_root->m_count == 0) {
			return true;
		}

		for (int ii = 0; ii <= cp_root->m_level; ii++) {

			int dist = numeric_limits<int>::max();
			for (int jj = 0; jj<nodo->m_count; jj++) {
				if (point.first < nodo->m_branch[jj].m_rect.m_min[0]) { x = nodo->m_branch[jj].m_rect.m_min[0]; }
				else if (nodo->m_branch[jj].m_rect.m_max[0] < point.first) { x = nodo->m_branch[jj].m_rect.m_max[0]; }
				else { x = point.first; }

				if (point.second < nodo->m_branch[jj].m_rect.m_min[1]) { y = nodo->m_branch[jj].m_rect.m_min[1]; }
				else if (nodo->m_branch[jj].m_rect.m_max[1] < point.second) { y = nodo->m_branch[jj].m_rect.m_max[1]; }
				else { y = point.second; }

				int tdist = (x - point.first)*(x - point.first) + (y - point.second)*(y - point.second);

				if (tdist < dist) {
					dist = tdist;
					mbranch = nodo->m_branch[jj];
				}
			}

			if (nodo->IsInternalNode()) {
				nodo = mbranch.m_child;
			}
		}
		objs.push_back(mbranch.m_data);
		RemoveRect(&mbranch.m_rect, mbranch.m_data, &cp_root);
	}
	return true;
}

Rect RTree::MBR(vector<pair<int, int>> pol)
{
	int x1 = pol[0].first;
	int x2 = pol[0].first;
	int y1 = pol[0].second;
	int y2 = pol[0].second;
	if (pol.size() == 1) {
		x1 -= 5; x2 += 5; y1 -= 5; y2 += 5;
	}
	else {
		for (unsigned int i = 1; i < pol.size(); i++) {
			if (pol[i].first < x1) {
				x1 = pol[i].first;
			}
			if (x2 < pol[i].first) {
				x2 = pol[i].first;
			}
			if (pol[i].second < y1) {
				y1 = pol[i].second;
			}
			if (y2 < pol[i].second) {
				y2 = pol[i].second;
			}
		}
	}

	return Rect(x1, y1, x2, y2);
}