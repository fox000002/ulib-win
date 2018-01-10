#include <iostream>

#include "adt/ubinary_tree.h"

void printTree(int &data)
{
    std::cout << "*" << data << "*";
}

class TournamentTree
{
public:
    TournamentTree()
    {

    }

    ~TournamentTree()
    {

    }
private:

};


void print_offset(int offset)
{
    int i;
    for (i = 0; i < offset; ++i)
    {
        std::cout << " ";
    }
}

void btree_print_ascii(huys::ADT::Binary_node<int> * node)
{
    static int offset = 0;

    print_offset(offset);

    if (node == NULL)
    {
        std::cout << "-" << std::endl;
        return;
    }
    std::cout << node->data << std::endl;

    offset += 3;
    btree_print_ascii(node->left);
    btree_print_ascii(node->right);
    offset -= 3;
}


void bst_print_dot_null(int key, int nullcount)
{
    std::cout << "    null"<< nullcount << " [shape=point];"  << std::endl;
    std::cout << "    " << key << "-> null" << nullcount << ";" << std::endl;
}

void bst_print_dot_aux(huys::ADT::Binary_node<int>* node)
{
    static int nullcount = 0;

    if (node->left)
    {
        std::cout << "    " << node->data << "->" << node->left->data << ";" << std::endl;
        bst_print_dot_aux(node->left);
    }
    else
        bst_print_dot_null(node->data, nullcount++);

    if (node->right)
    {
        std::cout << "    " << node->data << "->" << node->right->data << ";" << std::endl;
        bst_print_dot_aux(node->right);
    }
    else
        bst_print_dot_null(node->data, nullcount++);
}

void bst_print_dot(huys::ADT::Binary_node<int>* tree)
{
    std::cout << "digraph BST {" << std::endl;
    std::cout << "    node [fontname=\"Arial\"];" << std::endl;

    if (!tree)
        std::cout << std::endl;
    else if (!tree->right && !tree->left)
        std::cout << "    " << tree->data << std::endl;
    else
        bst_print_dot_aux(tree);

    std::cout << "}" << std::endl;
}

int main()
{
    using std::cout;
    using std::endl;
    using std::cin;

    huys::ADT::Binary_tree<int> b_tree;

    cout << "Size of tree : "  << b_tree.size() << endl;
    cout << "Height of tree : "  << b_tree.height() << endl;

    b_tree.insert(11);

    cout << "-----------------------------------------------------" << endl;
    cout << "After insert:" << endl;
    cout << "Size of tree : "  << b_tree.size() << endl;
    cout << "Height of tree : "  << b_tree.height() << endl;

    b_tree.insert(10);
    b_tree.insert(12);

    cout << "-----------------------------------------------------" << endl;
    cout << "After insert:" << endl;
    cout << "Size of tree : "  << b_tree.size() << endl;
    cout << "Height of tree : "  << b_tree.height() << endl;

    cout << "Inorder : ";
    b_tree.inorder(printTree);
    cout << endl;

    cout << "Preorder : ";
    b_tree.preorder(printTree);
    cout << endl;

    cout << "Postorder : ";
    b_tree.postorder(printTree);
    cout << endl;

    cout << "Doubleorder : ";
    b_tree.doubleorder(printTree);
    cout << endl;

    cout << "levelbylevel : ";
    b_tree.levelbylevel(printTree);
    cout << endl;

    cout << "=====================================================" << endl;

    btree_print_ascii(b_tree.root());

    cout << "=====================================================" << endl;

    //
    b_tree.insert(2);
    b_tree.insert(15);

    cout << "-----------------------------------------------------" << endl;
    cout << "After insert:" << endl;
    cout << "Size of tree : "  << b_tree.size() << endl;
    cout << "Height of tree : "  << b_tree.height() << endl;

    cout << "Inorder : ";
    b_tree.inorder(printTree);
    cout << endl;

    cout << "Preorder : ";
    b_tree.preorder(printTree);
    cout << endl;

    cout << "Postorder : ";
    b_tree.postorder(printTree);
    cout << endl;

    cout << "Doubleorder : ";
    b_tree.doubleorder(printTree);
    cout << endl;

    cout << "levelbylevel : ";
    b_tree.levelbylevel(printTree);
    cout << endl;

    cout << "=====================================================" << endl;

    btree_print_ascii(b_tree.root());

    bst_print_dot(b_tree.root());

    cout << "=====================================================" << endl;

    b_tree.mirror();

    cout << "-----------------------------------------------------" << endl;
    cout << "After mirror:" << endl;

    cout << "Inorder : ";
    b_tree.inorder(printTree);
    cout << endl;
    cout << "Preorder : ";
    b_tree.preorder(printTree);
    cout << endl;
    cout << "Postorder : ";
    b_tree.postorder(printTree);
    cout << endl;

    //class TTClass;
    //typedef char type_must_be_complete[sizeof(TTClass)?1:-1];
    //(void)sizeof(type_must_be_complete);

    huys::ADT::Binary_tree<int> new_tree(b_tree);
    cout << "-----------------------------------------------------" << endl;
    cout << "Copy tree:" << endl;

    cout << "Inorder : ";
    new_tree.inorder(printTree);
    cout << endl;
    cout << "Preorder : ";
    new_tree.preorder(printTree);
    cout << endl;
    cout << "Postorder : ";
    new_tree.postorder(printTree);
    cout << endl;



    cout << "-----------------------------------------------------" << endl;

    huys::ADT::Search_tree<int> stree;

    stree.insert(3);
    stree.insert(2);
    stree.insert(4);
    stree.insert(1);
    stree.insert(5);

    cout << "Size of tree : "  << stree.size() << endl;
    cout << "Height of tree : "  << stree.height() << endl;

    cout << "Inorder : ";
    stree.inorder(printTree);
    cout << endl;

    stree.remove(3);

    cout << "-----------------------------------------------------" << endl;
    cout << "After remove:" << endl;
    cout << "Inorder : ";
    stree.inorder(printTree);
    cout << endl;

    cout << "Press any key to continue...";
    cin.ignore();

    return 0;
}
