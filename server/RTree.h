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

// NOTE This file compiles under MSVC 6 SP5 and MSVC .Net 2003 it may not work on other compilers without modification.

// NOTE These next few lines may be win32 specific, you may need to modify them to compile on other platform
#include <stdio.h>
#include <math.h>
#include <assert.h>
#include <stdlib.h>

#include <algorithm>
#include <functional>
#include <vector>
#include <limits>

#define ASSERT assert // RTree uses ASSERT( condition )
#ifndef Min
#define Min std::min
#endif //Min
#ifndef Max
#define Max std::max
#endif //Max

using namespace std;

#define RTREE_USE_SPHERICAL_VOLUME // Better split classification, may be slower on some systems



/// \class RTree
/// Implementation of RTree, a multidimensional bounding rectangle tree.
/// Example usage: For a 3-dimensional tree use RTree<Object*, float, 3> myTree;
///
/// This modified, templated C++ version by Greg Douglas at Auran (http://www.auran.com)
///
/// DATATYPE Referenced data, should be int, void*, obj* etc. no larger than sizeof<void*> and simple type
/// ELEMTYPE Type of element such as int or float
/// NUMDIMS Number of dimensions such as 2 or 3
/// ELEMTYPEREAL Type of element that allows fractional and large values such as float or double, for use in volume calcs
///
/// NOTES: Inserting and removing data requires the knowledge of its constant Minimal Bounding Rectangle.
///        This version uses new/delete for nodes, I recommend using a fixed size allocator for efficiency.
///        Instead of using a callback function for returned results, I recommend and efficient pre-sized, grow-only memory
///        array similar to MFC CArray or STL Vector for returning search query result.
///
/*template<class DATATYPE, class ELEMTYPE, int NUMDIMS,
class ELEMTYPEREAL = ELEMTYPE, int TMAXNODES = 8, int TMINNODES = TMAXNODES / 2>*/

#define NUMDIMS 2

class RTree
{
protected:

	struct Node;  // Fwd decl.  Used by other internal structs and iterator

public:

	// These constant must be declared after Branch and before Node struct
	// Stuck up here for MSVC 6 compiler.  NSVC .NET 2003 is much happier.
	enum
	{
		MAXNODES = 8,                         ///< Max elements in node
		MINNODES = MAXNODES / 2,                         ///< Min elements in node
	};

public:

	RTree();
	RTree(const RTree& other);
	virtual ~RTree();

	/// Insert entry
	/// \param a_min Min of bounding rect
	/// \param a_max Max of bounding rect
	/// \param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
	void Insert(const int a_min[NUMDIMS], const int a_max[NUMDIMS], const vector<pair<int, int>>& a_dataId);

	/// Remove entry
	/// \param a_min Min of bounding rect
	/// \param a_max Max of bounding rect
	/// \param a_dataId Positive Id of data.  Maybe zero, but negative numbers not allowed.
	void Remove(const int a_min[NUMDIMS], const int a_max[NUMDIMS], const vector<pair<int, int>>& a_dataId);

	/// Find all within search rectangle
	/// \param a_min Min of search bounding rect
	/// \param a_max Max of search bounding rect
	/// \param a_searchResult Search result array.  Caller should set grow size. Function will reset, not append to array.
	/// \param a_resultCallback Callback function to return result.  Callback should return 'true' to continue searching
	/// \param a_context User context to pass as parameter to a_resultCallback
	/// \return Returns the number of entries found
	int Search(const pair<int, int> a_min, const pair<int, int> a_max, vector<vector<pair<int, int>>>& objs) const;

	/// Remove all entries from tree
	void RemoveAll();

	/// Count the data elements in this container.  This is slow as no internal counter is maintained.
	int Count();

	/// get the MBRs;
	bool getMBRs(vector<vector<pair<int, int>>>& mbrs);
	bool nearest(int k, vector<pair<int, int>> points, vector<vector<pair<int, int>>>& v);

protected:

	/// Minimal bounding rectangle (n-dimensional)
	struct Rect
	{
		int m_min[NUMDIMS];                      ///< Min dimensions of bounding box 
		int m_max[NUMDIMS];                      ///< Max dimensions of bounding box 
	};

	/// May be data or may be another subtree
	/// The parents level determines this.
	/// If the parents level is 0, then this is data
	struct Branch
	{
		Rect m_rect;                                  ///< Bounds
		Node* m_child;                                ///< Child node
		vector<pair<int, int>> m_data;                              ///< Data Id
	};

	/// Node for each branch level
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
	float RectSphericalVolume(Rect* a_rect);
	float RectVolume(Rect* a_rect);
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