/*
* 
*
* This file is part of BRAT 
*
* BRAT is free software; you can redistribute it and/or
* modify it under the terms of the GNU General Public License
* as published by the Free Software Foundation; either version 2
* of the License, or (at your option) any later version.
*
* BRAT is distributed in the hope that it will be useful,
* but WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
* GNU General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program; if not, write to the Free Software
* Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#if !defined(_ObjectTree_h_)
#define _ObjectTree_h_

#include <map>
#include <string>
#include <vector>

#include "brathl.h"
#include "BratObject.h"
#include "new-gui/Common/tools/Trace.h"

namespace brathl
{


/** \addtogroup tools Tools
  @{ */

/** 
  ObjectTree template class


 \version 1.0
*/


//-------------------------------------------------------------
//------------------- CObjectTreeNode class --------------------
//-------------------------------------------------------------

class CObjectTree;
class CObjectTreeNode;
class CObjectTreeIterator;

typedef std::map<std::string, CObjectTreeNode*> mapTreeNode;


class CObjectTreeNode
{
public:

#if defined(PRE_CPP11)
    typedef std::vector<CObjectTreeNode*>::const_iterator const_iterator;
#else
    using const_iterator = std::vector<CObjectTreeNode*>::const_iterator;
#endif

	///holds iterator to current child processing
	std::vector<CObjectTreeNode*>::iterator m_current;
	////whom i'm belonging to
	CObjectTree* m_LinkedTree;
protected:
	///key to locate myself  
	std::string m_key;

	///points to node of parent
	CObjectTreeNode* m_parent;

	///holds pointers for children  
	std::vector<CObjectTreeNode*> m_children;

	///things stored in ObjectTreeNode
	CBratObject* m_data;
	//rootnode is of level 1		
	int m_level;	//rootnode is of level 1


public:
	CObjectTreeNode( CObjectTree* tr );

	CObjectTreeNode( CObjectTree* tr, const std::string& key, CBratObject* x );

	virtual ~CObjectTreeNode();


	const_iterator begin() const
	{
		return m_children.begin();
	}

	const_iterator end() const
	{
		return m_children.end();
	}


	///Deletes all the children of this node including all the down levels
	void DeleteAllChildren();

	///Deletes this node and all its children
	void Delete( const std::string& key );

	///Returns a reference to parent node in the ObjectTree
	CObjectTreeNode* GetParent() const { return m_parent; };

	///Returns a reference to std::vector to the children
	std::vector<CObjectTreeNode*>& GetChildren()   { return  m_children; };

	size_t ChildCount() const { return m_children.size(); };

	///Add a child node to this node
	void AddChild( CObjectTreeNode* child );

	/// Returns a pointer to the stored data 
	CBratObject* GetData( void ) const { return m_data; };

	void SetLevel( int32_t level ) { m_level = level; };
	int32_t GetLevel() { return m_level; };

	std::string GetKey() { return m_key; };

};

//-------------------------------------------------------------
//------------------- CObjectTreeIterator class --------------------
//-------------------------------------------------------------


class CObjectTreeIterator
{
  //typedef mapTreeNode::iterator _map_it;

public:
  mapTreeNode::iterator m_Node;

  CBratObject* operator*() { return m_Node->second->GetData(); };
  const CBratObject* operator*() const { return m_Node->second->GetData(); };

  CObjectTreeIterator& operator++()
    { 
      m_Node++;
      return *this; 
    };

  CObjectTreeIterator operator++(int)
    {
      CObjectTreeIterator __tmp = *this;
      m_Node++;
      return __tmp;
    };

  CObjectTreeIterator& operator--() 
    { 
      m_Node--;
      return *this; 
    };

  CObjectTreeIterator operator--(int)
    {
      CObjectTreeIterator __tmp = *this;
      m_Node--;
      return __tmp;
    };

   bool operator==(const CObjectTreeIterator& _y) const
      {
        return m_Node == _y.m_Node;
      };

  bool operator!=(const CObjectTreeIterator& _y) const 
    {
      return m_Node != _y.m_Node;
    };
};


//-------------------------------------------------------------
//------------------- CObjectTree class --------------------
//-------------------------------------------------------------

class CObjectTree
{
	friend class CObjectTreeNode;

protected:
	///provides a name of the ObjectTreeNode to direct access to it
	///a std::map for fast accessing ObjectTreeNode
	mapTreeNode m_nodemap;
	///a pointer to root node
	CObjectTreeNode* m_pTreeroot;

	//Sub-ObjectTree root for walking
	CObjectTreeNode* m_WalkPivot;

	//points to current node of walking
	CObjectTreeNode* m_WalkCurrent;

	//point to parent node of current node
	CObjectTreeNode* m_WalkParent;

public:

	typedef CObjectTreeIterator iterator;

	//typedef const iterator const_iterator;

	///instantiates an empty CObjectTree
	CObjectTree();

	///instantiates a CObjectTree with the root node
	CObjectTree( const std::string& nm, CBratObject* x );

	virtual ~CObjectTree();


	///set a ObjectTree with the root node
	virtual void SetRoot( const std::string& nm, CBratObject* x, bool goCurrent = false );

	///returns ObjectTree root node, or NULL for empty CObjectTree
	CObjectTreeNode* GetRoot() const { return m_pTreeroot; };

	CObjectTreeNode* GetWalkCurrent() const { return m_WalkCurrent; };
	CObjectTreeNode* GetWalkParent() const { return m_WalkParent; };

	virtual CObjectTreeIterator AddChild( CObjectTreeNode* parent, const std::string& nm, CBratObject* x, bool goCurrent = false );

	virtual CObjectTreeIterator AddChild( CObjectTreeIterator& parent, const std::string& nm, CBratObject* x, bool goCurrent = false );

	virtual CObjectTreeIterator AddChild( const std::string& nm, CBratObject* x, bool goCurrent = false );

	void DeleteAllChildren( CObjectTreeIterator & itr );
	void DeleteTree();

	size_t size( void ) { return m_nodemap.size(); };

	CBratObject* FindParentObject( const std::string& key );
	CObjectTreeNode* FindParentNode( const std::string& key );

	CBratObject* FindObject( const std::string& key );
	CObjectTreeNode* FindNode( const std::string& key );

	bool GoLevelUp( bool firstChild = true );
	//bool GoLevelDown( bool firstChild = true );
	void Go( CObjectTreeNode* child );
	void Go( CObjectTreeIterator child );
	void GoToParent( const std::string& key );


	///Set the sub-ObjectTree walking root and traverse to the first node
	//void SetPostOrderSubTreePivot( CObjectTreeIterator& it );

	///Set the sub-ObjectTree walking root and traverse to the first node
	//void SetPostOrderRootPivot();

	void SetWalkDownRootPivot();

	///it advances m_WalkCurrent in post-order, and returns true if a move is made
	///else it returns false
	//bool SubTreePostOrderWalk();

	//It advance the ObjectTree in top-down style with parent first
	bool SubTreeWalkDown();


	//void DelSubTree() {};

	CBratObject* SubTreeGetData( void ) { return m_WalkCurrent->GetData(); }
	int SubTreeGetLevel( void ) { return m_WalkCurrent->GetLevel(); }


	CObjectTreeIterator begin()
	{
		CObjectTreeIterator _tmp;
		_tmp.m_Node = m_nodemap.begin();
		return _tmp;
	}

	CObjectTreeIterator end()
	{
		CObjectTreeIterator _tmp;
		_tmp.m_Node = m_nodemap.end();
		return _tmp;
	}

	CObjectTreeIterator find( const std::string& key )
	{
		CObjectTreeIterator _tmp;
		_tmp.m_Node = m_nodemap.find( key );
		return _tmp;
	}

	bool IsKey( const std::string& key )
	{
		CObjectTreeIterator it = this->find( key );

		return ( it != this->end() );
	}

	/// Dump function
	virtual void Dump( std::ostream& fOut = std::cerr );
};



/** @} */

};


#endif // !defined(_ObjectTree_h_)
