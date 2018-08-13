//============================================================================
// Name        : RTree.cpp
// Author      : Daniel Palomino / Vittorino Mandujano
// Version     : 1.0
// Copyright   : Your copyright notice
// Description : RTree Backend
// Created on  : 08 ago. 2018
//============================================================================

#include "RTree.h"

RTree::RTree()
{
	ASSERT(MAXNODES > MINNODES);
	ASSERT(MINNODES > 0);

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
	Reset(); // Free, or reset node memory
}


void RTree::Insert(const int a_min[NUMDIMS], const int a_max[NUMDIMS], const vector<pair<int, int>>& a_dataId)
{
#ifdef _DEBUG
	for (int index = 0; index<NUMDIMS; ++index)
	{
		ASSERT(a_min[index] <= a_max[index]);
	}
#endif //_DEBUG

	Branch branch;
	branch.m_data = a_dataId;
	branch.m_child = NULL;

	for (int axis = 0; axis<NUMDIMS; ++axis)
	{
		branch.m_rect.m_min[axis] = a_min[axis];
		branch.m_rect.m_max[axis] = a_max[axis];
	}

	InsertRect(branch, &m_root, 0);
}


void RTree::Remove(const int a_min[NUMDIMS], const int a_max[NUMDIMS], const vector<pair<int, int>>& a_dataId)
{
#ifdef _DEBUG
	for (int index = 0; index<NUMDIMS; ++index)
	{
		ASSERT(a_min[index] <= a_max[index]);
	}
#endif //_DEBUG

	Rect rect;

	for (int axis = 0; axis<NUMDIMS; ++axis)
	{
		rect.m_min[axis] = a_min[axis];
		rect.m_max[axis] = a_max[axis];
	}

	RemoveRect(&rect, a_dataId, &m_root);
}


int RTree::Search(const pair<int,int> a_min, const pair<int, int> a_max, vector<vector<pair<int, int>>>& objs) const
{
#ifdef _DEBUG
	for (int index = 0; index<NUMDIMS; ++index)
	{
		ASSERT(a_min[index] <= a_max[index]);
	}
#endif //_DEBUG

	objs.clear();
	Rect rect;

	for (int axis = 0; axis<NUMDIMS; ++axis)
	{
		rect.m_min[0] = a_min.first;
		rect.m_min[1] = a_min.second;
		rect.m_max[0] = a_max.first;
		rect.m_max[1] = a_max.second;
	}

	// NOTE: May want to return search result another way, perhaps returning the number of found elements here.

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
	if (a_node->IsInternalNode())  // not a leaf node
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			CountRec(a_node->m_branch[index].m_child, a_count);
		}
	}
	else // A leaf node
	{
		a_count += a_node->m_count;
	}
}


void RTree::CopyRec(Node* current, Node* other)
{
	current->m_level = other->m_level;
	current->m_count = other->m_count;

	if (current->IsInternalNode())  // not a leaf node
	{
		for (int index = 0; index < current->m_count; ++index)
		{
			Branch* currentBranch = &current->m_branch[index];
			Branch* otherBranch = &other->m_branch[index];

			std::copy(otherBranch->m_rect.m_min,
				otherBranch->m_rect.m_min + NUMDIMS,
				currentBranch->m_rect.m_min);

			std::copy(otherBranch->m_rect.m_max,
				otherBranch->m_rect.m_max + NUMDIMS,
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
				otherBranch->m_rect.m_min + NUMDIMS,
				currentBranch->m_rect.m_min);

			std::copy(otherBranch->m_rect.m_max,
				otherBranch->m_rect.m_max + NUMDIMS,
				currentBranch->m_rect.m_max);

			currentBranch->m_data = otherBranch->m_data;
		}
	}
}


void RTree::RemoveAll()
{
	// Delete all existing nodes
	Reset();

	m_root = AllocNode();
	m_root->m_level = 0;
}


void RTree::Reset()
{
	// Delete all existing nodes
	RemoveAllRec(m_root);
}


void RTree::RemoveAllRec(Node* a_node)
{
	ASSERT(a_node);
	ASSERT(a_node->m_level >= 0);

	if (a_node->IsInternalNode()) // This is an internal node in the tree
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			RemoveAllRec(a_node->m_branch[index].m_child);
		}
	}
	FreeNode(a_node);
}


typename RTree::Node* RTree::AllocNode()
{
	Node* newNode;
	newNode = new Node;
	InitNode(newNode);
	return newNode;
}


void RTree::FreeNode(Node* a_node)
{
	ASSERT(a_node);

	delete a_node;
}


// Allocate space for a node in the list used in DeletRect to
// store Nodes that are too empty.
typename RTree::ListNode* RTree::AllocListNode()
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
	for (int index = 0; index < NUMDIMS; ++index)
	{
		a_rect->m_min[index] = (int)0;
		a_rect->m_max[index] = (int)0;
	}
}


// Inserts a new data rectangle into the index structure.
// Recursively descends tree, propagates splits back up.
// Returns 0 if node was not split.  Old node updated.
// If node was split, returns 1 and sets the pointer pointed to by
// new_node to point to the new node.  Old node updated to become one of two.
// The level argument specifies the number of steps up from the leaf
// level to insert; e.g. a data rectangle goes in at level = 0.

bool RTree::InsertRectRec(const Branch& a_branch, Node* a_node, Node** a_newNode, int a_level)
{
	ASSERT(a_node && a_newNode);
	ASSERT(a_level >= 0 && a_level <= a_node->m_level);

	// recurse until we reach the correct level for the new record. data records
	// will always be called with a_level == 0 (leaf)
	if (a_node->m_level > a_level)
	{
		// Still above level for insertion, go down tree recursively
		Node* otherNode;

		// find the optimal branch for this record
		int index = PickBranch(&a_branch.m_rect, a_node);

		// recursively insert this record into the picked branch
		bool childWasSplit = InsertRectRec(a_branch, a_node->m_branch[index].m_child, &otherNode, a_level);

		if (!childWasSplit)
		{
			// Child was not split. Merge the bounding box of the new record with the
			// existing bounding box
			a_node->m_branch[index].m_rect = CombineRect(&a_branch.m_rect, &(a_node->m_branch[index].m_rect));
			return false;
		}
		else
		{
			// Child was split. The old branches are now re-partitioned to two nodes
			// so we have to re-calculate the bounding boxes of each node
			a_node->m_branch[index].m_rect = NodeCover(a_node->m_branch[index].m_child);
			Branch branch;
			branch.m_child = otherNode;
			branch.m_rect = NodeCover(otherNode);

			// The old node is already a child of a_node. Now add the newly-created
			// node to a_node as well. a_node might be split because of that.
			return AddBranch(&branch, a_node, a_newNode);
		}
	}
	else if (a_node->m_level == a_level)
	{
		// We have reached level for insertion. Add rect, split if necessary
		return AddBranch(&a_branch, a_node, a_newNode);
	}
	else
	{
		// Should never occur
		ASSERT(0);
		return false;
	}
}


// Insert a data rectangle into an index structure.
// InsertRect provides for splitting the root;
// returns 1 if root was split, 0 if it was not.
// The level argument specifies the number of steps up from the leaf
// level to insert; e.g. a data rectangle goes in at level = 0.
// InsertRect2 does the recursion.
//
bool RTree::InsertRect(const Branch& a_branch, Node** a_root, int a_level)
{
	ASSERT(a_root);
	ASSERT(a_level >= 0 && a_level <= (*a_root)->m_level);
#ifdef _DEBUG
	for (int index = 0; index < NUMDIMS; ++index)
	{
		ASSERT(a_branch.m_rect.m_min[index] <= a_branch.m_rect.m_max[index]);
	}
#endif //_DEBUG  

	Node* newNode;

	if (InsertRectRec(a_branch, *a_root, &newNode, a_level))  // Root split
	{
		// Grow tree taller and new root
		Node* newRoot = AllocNode();
		newRoot->m_level = (*a_root)->m_level + 1;

		Branch branch;

		// add old root node as a child of the new root
		branch.m_rect = NodeCover(*a_root);
		branch.m_child = *a_root;
		AddBranch(&branch, newRoot, NULL);

		// add the split node as a child of the new root
		branch.m_rect = NodeCover(newNode);
		branch.m_child = newNode;
		AddBranch(&branch, newRoot, NULL);

		// set the new root as the root node
		*a_root = newRoot;

		return true;
	}

	return false;
}


// Find the smallest rectangle that includes all rectangles in branches of a node.
typename RTree::Rect RTree::NodeCover(Node* a_node)
{
	ASSERT(a_node);

	Rect rect = a_node->m_branch[0].m_rect;
	for (int index = 1; index < a_node->m_count; ++index)
	{
		rect = CombineRect(&rect, &(a_node->m_branch[index].m_rect));
	}

	return rect;
}


// Add a branch to a node.  Split the node if necessary.
// Returns 0 if node not split.  Old node updated.
// Returns 1 if node split, sets *new_node to address of new node.
// Old node updated, becomes one of two.
bool RTree::AddBranch(const Branch* a_branch, Node* a_node, Node** a_newNode)
{
	ASSERT(a_branch);
	ASSERT(a_node);

	if (a_node->m_count < MAXNODES)  // Split won't be necessary
	{
		a_node->m_branch[a_node->m_count] = *a_branch;
		++a_node->m_count;

		return false;
	}
	else
	{
		ASSERT(a_newNode);

		SplitNode(a_node, a_branch, a_newNode);
		return true;
	}
}


// Disconnect a dependent node.
// Caller must return (or stop using iteration index) after this as count has changed
void RTree::DisconnectBranch(Node* a_node, int a_index)
{
	ASSERT(a_node && (a_index >= 0) && (a_index < MAXNODES));
	ASSERT(a_node->m_count > 0);

	// Remove element by swapping with the last element to prevent gaps in array
	a_node->m_branch[a_index] = a_node->m_branch[a_node->m_count - 1];

	--a_node->m_count;
}


// Pick a branch.  Pick the one that will need the smallest increase
// in area to accomodate the new rectangle.  This will result in the
// least total area for the covering rectangles in the current node.
// In case of a tie, pick the one which was smaller before, to get
// the best resolution when searching.
int RTree::PickBranch(const Rect* a_rect, Node* a_node)
{
	ASSERT(a_rect && a_node);

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


// Combine two rectangles into larger one containing both
typename RTree::Rect RTree::CombineRect(const Rect* a_rectA, const Rect* a_rectB)
{
	ASSERT(a_rectA && a_rectB);

	Rect newRect;

	for (int index = 0; index < NUMDIMS; ++index)
	{
		newRect.m_min[index] = Min(a_rectA->m_min[index], a_rectB->m_min[index]);
		newRect.m_max[index] = Max(a_rectA->m_max[index], a_rectB->m_max[index]);
	}

	return newRect;
}



// Split a node.
// Divides the nodes branches and the extra one between two nodes.
// Old node is one of the new ones, and one really new one is created.
// Tries more than one method for choosing a partition, uses best result.
void RTree::SplitNode(Node* a_node, const Branch* a_branch, Node** a_newNode)
{
	ASSERT(a_node);
	ASSERT(a_branch);

	// Could just use local here, but member or external is faster since it is reused
	PartitionVars localVars;
	PartitionVars* parVars = &localVars;

	// Load all the branches into a buffer, initialize old node
	GetBranches(a_node, a_branch, parVars);

	// Find partition
	ChoosePartition(parVars, MINNODES);

	// Create a new node to hold (about) half of the branches
	*a_newNode = AllocNode();
	(*a_newNode)->m_level = a_node->m_level;

	// Put branches from buffer into 2 nodes according to the chosen partition
	a_node->m_count = 0;
	LoadNodes(a_node, *a_newNode, parVars);

	ASSERT((a_node->m_count + (*a_newNode)->m_count) == parVars->m_total);
}


// Calculate the n-dimensional volume of a rectangle
float RTree::RectVolume(Rect* a_rect)
{
	ASSERT(a_rect);

	float volume = (float)1;

	for (int index = 0; index<NUMDIMS; ++index)
	{
		volume *= a_rect->m_max[index] - a_rect->m_min[index];
	}

	ASSERT(volume >= (float)0);

	return volume;
}


// The exact volume of the bounding sphere for the given Rect

float RTree::RectSphericalVolume(Rect* a_rect)
{
	ASSERT(a_rect);

	float sumOfSquares = (float)0;
	float radius;

	for (int index = 0; index < NUMDIMS; ++index)
	{
		float halfExtent = ((float)a_rect->m_max[index] - (float)a_rect->m_min[index]) * 0.5f;
		sumOfSquares += halfExtent * halfExtent;
	}

	radius = (float)sqrt(sumOfSquares);

	// Pow maybe slow, so test for common dims like 2,3 and just use x*x, x*x*x.
	if (NUMDIMS == 3)
	{
		return (radius * radius * radius * m_unitSphereVolume);
	}
	else if (NUMDIMS == 2)
	{
		return (radius * radius * m_unitSphereVolume);
	}
	else
	{
		return (float)(pow(radius, NUMDIMS) * m_unitSphereVolume);
	}
}


// Use one of the methods to calculate retangle volume

float RTree::CalcRectVolume(Rect* a_rect)
{
#ifdef RTREE_USE_SPHERICAL_VOLUME
	return RectSphericalVolume(a_rect); // Slower but helps certain merge cases
#else // RTREE_USE_SPHERICAL_VOLUME
	return RectVolume(a_rect); // Faster but can cause poor merges
#endif // RTREE_USE_SPHERICAL_VOLUME  
}


// Load branch buffer with branches from full node plus the extra branch.

void RTree::GetBranches(Node* a_node, const Branch* a_branch, PartitionVars* a_parVars)
{
	ASSERT(a_node);
	ASSERT(a_branch);

	ASSERT(a_node->m_count == MAXNODES);

	// Load the branch buffer
	for (int index = 0; index < MAXNODES; ++index)
	{
		a_parVars->m_branchBuf[index] = a_node->m_branch[index];
	}
	a_parVars->m_branchBuf[MAXNODES] = *a_branch;
	a_parVars->m_branchCount = MAXNODES + 1;

	// Calculate rect containing all in the set
	a_parVars->m_coverSplit = a_parVars->m_branchBuf[0].m_rect;
	for (int index = 1; index < MAXNODES + 1; ++index)
	{
		a_parVars->m_coverSplit = CombineRect(&a_parVars->m_coverSplit, &a_parVars->m_branchBuf[index].m_rect);
	}
	a_parVars->m_coverSplitArea = CalcRectVolume(&a_parVars->m_coverSplit);
}


// Method #0 for choosing a partition:
// As the seeds for the two groups, pick the two rects that would waste the
// most area if covered by a single rectangle, i.e. evidently the worst pair
// to have in the same group.
// Of the remaining, one at a time is chosen to be put in one of the two groups.
// The one chosen is the one with the greatest difference in area expansion
// depending on which group - the rect most strongly attracted to one group
// and repelled from the other.
// If one group gets too full (more would force other group to violate min
// fill requirement) then other group gets the rest.
// These last are the ones that can go in either group most easily.

void RTree::ChoosePartition(PartitionVars* a_parVars, int a_minFill)
{
	ASSERT(a_parVars);

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

	// If one group too full, put remaining rects in the other
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

	ASSERT((a_parVars->m_count[0] + a_parVars->m_count[1]) == a_parVars->m_total);
	ASSERT((a_parVars->m_count[0] >= a_parVars->m_minFill) &&
		(a_parVars->m_count[1] >= a_parVars->m_minFill));
}


// Copy branches from the buffer into two nodes according to the partition.

void RTree::LoadNodes(Node* a_nodeA, Node* a_nodeB, PartitionVars* a_parVars)
{
	ASSERT(a_nodeA);
	ASSERT(a_nodeB);
	ASSERT(a_parVars);

	for (int index = 0; index < a_parVars->m_total; ++index)
	{
		ASSERT(a_parVars->m_partition[index] == 0 || a_parVars->m_partition[index] == 1);

		int targetNodeIndex = a_parVars->m_partition[index];
		Node* targetNodes[] = { a_nodeA, a_nodeB };

		// It is assured that AddBranch here will not cause a node split. 
		bool nodeWasSplit = AddBranch(&a_parVars->m_branchBuf[index], targetNodes[targetNodeIndex], NULL);
		ASSERT(!nodeWasSplit);
	}
}


// Initialize a PartitionVars structure.

void RTree::InitParVars(PartitionVars* a_parVars, int a_maxRects, int a_minFill)
{
	ASSERT(a_parVars);

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


// Put a branch in one of the groups.

void RTree::Classify(int a_index, int a_group, PartitionVars* a_parVars)
{
	ASSERT(a_parVars);
	ASSERT(PartitionVars::NOT_TAKEN == a_parVars->m_partition[a_index]);

	a_parVars->m_partition[a_index] = a_group;

	// Calculate combined rect
	if (a_parVars->m_count[a_group] == 0)
	{
		a_parVars->m_cover[a_group] = a_parVars->m_branchBuf[a_index].m_rect;
	}
	else
	{
		a_parVars->m_cover[a_group] = CombineRect(&a_parVars->m_branchBuf[a_index].m_rect, &a_parVars->m_cover[a_group]);
	}

	// Calculate volume of combined rect
	a_parVars->m_area[a_group] = CalcRectVolume(&a_parVars->m_cover[a_group]);

	++a_parVars->m_count[a_group];
}


// Delete a data rectangle from an index structure.
// Pass in a pointer to a Rect, the tid of the record, ptr to ptr to root node.
// Returns 1 if record not found, 0 if success.
// RemoveRect provides for eliminating the root.

bool RTree::RemoveRect(Rect* a_rect, const vector<pair<int, int>>& a_id, Node** a_root)
{
	ASSERT(a_rect && a_root);
	ASSERT(*a_root);

	ListNode* reInsertList = NULL;

	if (!RemoveRectRec(a_rect, a_id, *a_root, &reInsertList))
	{
		// Found and deleted a data item
		// Reinsert any branches from eliminated nodes
		while (reInsertList)
		{
			Node* tempNode = reInsertList->m_node;

			for (int index = 0; index < tempNode->m_count; ++index)
			{
				// TODO go over this code. should I use (tempNode->m_level - 1)?
				InsertRect(tempNode->m_branch[index],
					a_root,
					tempNode->m_level);
			}

			ListNode* remLNode = reInsertList;
			reInsertList = reInsertList->m_next;

			FreeNode(remLNode->m_node);
			FreeListNode(remLNode);
		}

		// Check for redundant root (not leaf, 1 child) and eliminate TODO replace
		// if with while? In case there is a whole branch of redundant roots...
		if ((*a_root)->m_count == 1 && (*a_root)->IsInternalNode())
		{
			Node* tempNode = (*a_root)->m_branch[0].m_child;

			ASSERT(tempNode);
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


// Delete a rectangle from non-root part of an index structure.
// Called by RemoveRect.  Descends tree recursively,
// merges branches on the way back up.
// Returns 1 if record not found, 0 if success.

bool RTree::RemoveRectRec(Rect* a_rect, const vector<pair<int, int>>& a_id, Node* a_node, ListNode** a_listNode)
{
	ASSERT(a_rect && a_node && a_listNode);
	ASSERT(a_node->m_level >= 0);

	if (a_node->IsInternalNode())  // not a leaf node
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			if (Overlap(a_rect, &(a_node->m_branch[index].m_rect)))
			{
				if (!RemoveRectRec(a_rect, a_id, a_node->m_branch[index].m_child, a_listNode))
				{
					if (a_node->m_branch[index].m_child->m_count >= MINNODES)
					{
						// child removed, just resize parent rect
						a_node->m_branch[index].m_rect = NodeCover(a_node->m_branch[index].m_child);
					}
					else
					{
						// child removed, not enough entries in node, eliminate node
						ReInsert(a_node->m_branch[index].m_child, a_listNode);
						DisconnectBranch(a_node, index); // Must return after this call as count has changed
					}
					return false;
				}
			}
		}
		return true;
	}
	else // A leaf node
	{
		for (int index = 0; index < a_node->m_count; ++index)
		{
			if (a_node->m_branch[index].m_data == a_id)
			{
				DisconnectBranch(a_node, index); // Must return after this call as count has changed
				return false;
			}
		}
		return true;
	}
}


// Decide whether two rectangles overlap.

bool RTree::Overlap(Rect* a_rectA, Rect* a_rectB) const
{
	ASSERT(a_rectA && a_rectB);

	for (int index = 0; index < NUMDIMS; ++index)
	{
		if (a_rectA->m_min[index] > a_rectB->m_max[index] ||
			a_rectB->m_min[index] > a_rectA->m_max[index])
		{
			return false;
		}
	}
	return true;
}


// Add a node to the reinsertion list.  All its branches will later
// be reinserted into the index structure.

void RTree::ReInsert(Node* a_node, ListNode** a_listNode)
{
	ListNode* newListNode;

	newListNode = AllocListNode();
	newListNode->m_node = a_node;
	newListNode->m_next = *a_listNode;
	*a_listNode = newListNode;
}


// Search in an index tree or subtree for all data retangles that overlap the argument rectangle.

bool RTree::Search(Node* a_node, Rect* a_rect, int& a_foundCount, vector<vector<pair<int, int>>>& objs) const
{
	ASSERT(a_node);
	ASSERT(a_node->m_level >= 0);
	ASSERT(a_rect);

	if (a_node->IsInternalNode())
	{
		// This is an internal node in the tree
		for (int index = 0; index < a_node->m_count; ++index)
		{
			if (Overlap(a_rect, &a_node->m_branch[index].m_rect))
			{
				if (!Search(a_node->m_branch[index].m_child, a_rect, a_foundCount, objs))
				{
					// The callback indicated to stop searching
					return false;
				}
			}
		}
	}
	else
	{
		// This is a leaf node
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

	return true; // Continue searching
}


bool RTree::getMBRs(vector<vector<pair<int, int>>>& mbrs)
{
	vector<Branch> v, w;
	mbrs.clear();

	int n = (m_root->m_level);

	for (int i = 0; i<(m_root->m_count); i++) {
		v.push_back(m_root->m_branch[i]);
	}

	for (int i = 0; i<v.size(); i++) {
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
	while (n--) {
		//theBranches(v, w);
		for (int i = 0; i<v.size(); i++) {
			for (int j = 0; j<(v[i].m_child->m_count); j++) {
				w.push_back(v[i].m_child->m_branch[j]);
			}
		}

		v = w;
		for (int i = 0; i<v.size(); i++) {
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

		w.clear();
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

		for (int ii = 0; ii <= cp_root->m_level; ii++) {

			float dist = numeric_limits<float>::max();
			for (int jj = 0; jj<nodo->m_count; jj++) {
				if (point.first<nodo->m_branch[jj].m_rect.m_min[0]) { x = nodo->m_branch[0].m_rect.m_min[0]; }
				else if (nodo->m_branch[jj].m_rect.m_max[0]<point.first) { x = nodo->m_branch[0].m_rect.m_max[0]; }
				else { x = point.first; }

				if (point.first<nodo->m_branch[jj].m_rect.m_min[1]) { y = nodo->m_branch[0].m_rect.m_min[1]; }
				else if (nodo->m_branch[jj].m_rect.m_max[1]<point.first) { y = nodo->m_branch[0].m_rect.m_max[1]; }
				else { y = point.second; }

				float tdist = (x - point.first)*(x - point.first) + (y - point.second)*(y - point.second);

				if (tdist < dist) {
					dist = tdist;
					mbranch = nodo->m_branch[jj];
				}
			}

			if (nodo->IsInternalNode()) {
				nodo = mbranch.m_child;
			}
		}
		//insertando el obj
		objs.push_back(mbranch.m_data);
		//Eliminando el mbr de la copia
		RemoveRect(&mbranch.m_rect, mbranch.m_data, &cp_root);// arreglar?! 
	}
	return true;
}