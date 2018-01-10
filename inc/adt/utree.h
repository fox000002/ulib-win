#ifndef U_TREE_H
#define U_TREE_H

#include "ubinary_tree.h"

namespace huys
{

namespace ADT
{

enum _Color {
    _red,
    _black
};

template <class Entry>
struct RB_node
{
    Entry data;
    RB_node<Entry> *left;
    RB_node<Entry> *right;
    _Color color;



    RB_node(const Entry &new_entry)
    {
        color = _red;
        data = new_entry;
        left = right = NULL;
    }

    RB_node()
    {
        color = _red;
        left = right = NULL;
    }

    void set_color(_Color c) { color = c; }

    _Color get_color() const { return color; }
};

/*
 * red-black tree
 */
template <class TK, class TV, class TAlloc>
class URBTree
{
public:
    URBTree()
    {
    }

    ~URBTree()
    {
    }

};

/*
 * k-d Tree
 */
template <class TK, class TV, class TAlloc>
class UKDTree
{
public:
	UKDTree()
	{
	
	}
	
	~UKDTree()
	{
	
	}
};
 
 
}; // namespace ADT

}; // namespace huys

#endif // U_TREE_H
