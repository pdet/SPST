#include "spst.h"
#include "structs.h"
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <stdio.h>

inline SPSTTree RR_Rotate(SPSTTree k2)
{
    SPSTTree k1 = k2->lchild;
    k2->lchild = k1->rchild;
    k1->rchild = k2;
    return k1;
}

inline SPSTTree LL_Rotate(SPSTTree k2)
{
    SPSTTree k1 = k2->rchild;
    k2->rchild = k1->lchild;
    k1->lchild = k2;
    return k1;
}

SPSTTree IntervalSplay(int ElementL,int ElementR, SPSTTree root){
    int midValue;
    root = Splay(ElementL,root);
    root = Splay(ElementR,root);
    midValue = (ElementL + ElementR) / 2; //Down-bound
    root = Splay(midValue,root);
    return root;

}

SPSTTree Splay(int Element, SPSTTree root)
{
    if(!root)
        return NULL;
    SPSTNode header;
    /* header.rchild points to L tree; header.lchild points to R Tree */
    header.lchild = header.rchild = NULL;
    SPSTTree LeftTreeMax = &header;
    SPSTTree RightTreeMin = &header;

    /* loop until root->lchild == NULL || root->rchild == NULL; then break!
       (or when find the Element, break too.)
     The zig/zag mode would only happen when cannot find Element and will reach
     null on one side after RR or LL Rotation.
     */
    while(1)
    {
        if(Element < root->Element)
        {
            if(!root->lchild)
                break;
            if(Element < root->lchild->Element)
            {
                root = RR_Rotate(root); /* only zig-zig mode need to rotate once,
										   because zig-zag mode is handled as zig
										   mode, which doesn't require rotate,
										   just linking it to R Tree */
                if(!root->lchild)
                    break;
            }
            /* Link to R Tree */
            RightTreeMin->lchild = root;
            RightTreeMin = RightTreeMin->lchild;
            root = root->lchild;
            RightTreeMin->lchild = NULL;
        }
        else if(Element > root->Element)
        {
            if(!root->rchild)
                break;
            if(Element > root->rchild->Element)
            {
                root = LL_Rotate(root);/* only zag-zag mode need to rotate once,
										  because zag-zig mode is handled as zag
										  mode, which doesn't require rotate,
										  just linking it to L Tree */
                if(!root->rchild)
                    break;
            }
            /* Link to L Tree */
            LeftTreeMax->rchild = root;
            LeftTreeMax = LeftTreeMax->rchild;
            root = root->rchild;
            LeftTreeMax->rchild = NULL;
        }
        else
            break;
    }
    /* assemble L Tree, Middle Tree and R tree together */
    LeftTreeMax->rchild = root->lchild;
    RightTreeMin->lchild = root->rchild;
    root->lchild = header.rchild;
    root->rchild = header.lchild;

    return root;
}

SPSTTree New_Node(int64_t offset,ElementType Element)
{
    SPSTTree p_node = new SPSTNode;
    if(!p_node)
    {
        fprintf(stderr, "Out of memory!\n");
        exit(1);
    }
    p_node->Element = Element;
    p_node->offset = offset;

    p_node->lchild = p_node->rchild = NULL;
    return p_node;
}


SPSTTree Insert(int64_t offset,ElementType Element, SPSTTree root)
{
    static SPSTTree p_node = NULL;
    if(!p_node)
        p_node = New_Node( offset,Element);
    else {
        p_node->offset = offset;
        p_node->Element = Element;

    }// could take advantage of the node remains because of there was duplicate Element before.
    if(!root)
    {
        root = p_node;
        p_node = NULL;
        return root;
    }
    root = Splay(Element, root);
    /* This is BST that, all Elements <= root->Element is in root->lchild, all Elements >
       root->Element is in root->rchild. (This BST doesn't allow duplicate Elements.) */
    if(Element < root->Element)
    {
        p_node->lchild = root->lchild;
        p_node->rchild = root;
        root->lchild = NULL;
        root = p_node;
    }
    else if(Element > root->Element)
    {
        p_node->rchild = root->rchild;
        p_node->lchild = root;
        root->rchild = NULL;
        root = p_node;
    }
    else
        return root;
    p_node = NULL;
    return root;
}

SPSTTree PruneLeaves(SPSTTree node){
    if (node == NULL)
        return NULL;
    if (node->lchild == NULL && node->rchild == NULL){
        free(node);
        return NULL;
    }
    node->lchild = PruneLeaves(node->lchild);
    node->rchild = PruneLeaves(node->rchild);
    return node;

}


SPSTTree Delete(ElementType Element, SPSTTree root)
{
    SPSTTree temp;
    if(!root)
        return NULL;
    root = Splay(Element, root);
    if(Element != root->Element) // No such node in splay tree
        return root;
    else
    {
        if(!root->lchild)
        {
            temp = root;
            root = root->rchild;
        }
        else
        {
            temp = root;
            /*Note: Since Element == root->Element, so after Splay(Element, root->lchild),
              the tree we get will have no right child tree. (Element > any Element in
              root->lchild)*/
            root = Splay(Element, root->lchild);
            root->rchild = temp->rchild;
        }
        free(temp);
        return root;
    }
}

SPSTTree Search(ElementType Element, SPSTTree root)
{
    return Splay(Element, root);
}

void InOrder(SPSTTree root)
{
    if(root)
    {
        InOrder(root->lchild);
        std::cout<< "Element: " <<root->Element;
        if(root->lchild)
            std::cout<< " | left child: "<< root->lchild->Element;
        if(root->rchild)
            std::cout << " | right child: " << root->rchild->Element;
        std::cout<< "\n";
        InOrder(root->rchild);
    }
}

void PostOrder(SPSTTree p, int indent=0)
{
    if(p != NULL) {
        std::cout<< p->Element << "\n ";
        if (indent) {
            std::cout << std::setw(indent) << ' ';
        }
        if(p->lchild)
            PostOrder(p->lchild, indent+4);
        if(p->rchild)
            PostOrder(p->rchild, indent+4);

    }
}

SPSTTree
FindLastPiece(SPSTTree T) {
    SPSTTree auxNode;
    while (T != NULL) {
        auxNode = T;
        T = T->rchild;
    }

    return auxNode;
}
PositionSPST
FindMax(SPSTTree T) {
    if (T != NULL)
        while (T->rchild != NULL)
            T = T->rchild;

    return T;
}
SPSTTree FindNeighborsLTFinal(ElementType X, SPSTTree T) {
    SPSTTree aux = NULL;
    SPSTTree original;
    original = T;
    while (T != NULL) {
        if (X < T->Element) {
            aux = T;
            T = T->lchild;
        } else if (X > T->Element) {
            T = T->rchild;
        } else {
            aux = T;
            if (T->lchild != NULL) {
                aux = FindMax(T->lchild);
            }
            break;
        }
    }
    if (!aux) {
        aux = FindMax(original);
    }
    return aux;
}

void _GetNodesInOrder(SPSTTree T, std::vector<SPSTTree> &vector) {
    if (T->lchild) {
        _GetNodesInOrder(T->lchild, vector);
    }
    vector.push_back(T);
    if (T->rchild) {
        _GetNodesInOrder(T->rchild, vector);
    }
}


std::vector<SPSTTree> GetNodesInOrder(SPSTTree T) {
    std::vector<SPSTTree> nodesOrder;
    _GetNodesInOrder(T, nodesOrder);
    return nodesOrder;
}

SPSTTree FindNeighborsGTFinal(ElementType X, SPSTTree T) {
    SPSTTree aux = NULL;
    while (T != NULL) {
        if (X < T->Element) {
            T = T->lchild;
        } else if (X > T->Element) {
            aux = T;
            T = T->rchild;
        } else {
            aux = T;
            break;
        }
    }

    return aux;
}

SPSTTree
FindFirstPiece(SPSTTree T) {
    SPSTTree auxNode;
    while (T != NULL) {
        auxNode = T;
        T = T->lchild;
    }

    return auxNode;
}

IntPair createOffsetPair(PositionSPST first, PositionSPST second, ElementType limit) {
    IntPair op = (IntPair) malloc(sizeof(struct int_pair));
    if (first && second) {
        if (first->offset == 0)
            op->first = first->offset;
        else
            op->first = first->offset + 1;
        op->second = second->offset;
    } else if (first) {
        if (first->offset == 0)
            op->first = first->offset;
        else
            op->first = first->offset + 1;
        op->second = limit;
    } else if (second) {
        op->first = 0;
        op->second = second->offset;
    } else {
        op->first = 0;
        op->second = limit;
    }
    return op;
}

IntPair
FindNeighborsLT(ElementType X, SPSTTree T, ElementType limit) {
    PositionSPST first = 0, second = 0;
    //if( T == NULL )
    //	return NULL;

    while (T != NULL) {
        if (X < T->Element) {
            second = T;
            T = T->lchild;
        } else if (X > T->Element) {
            first = T;
            T = T->rchild;
        } else {
            second = T;
            if (T->lchild != NULL) {
                first = FindMax(T->rchild);
            }
            break;
        }
    }

    return createOffsetPair(first, second, limit);
}

IntPair
FindNeighborsGTE(ElementType X, SPSTTree T, ElementType limit) {
    PositionSPST first = 0, second = 0;

    while (T != NULL) {
        if (X < T->Element) {
            second = T;
            T = T->lchild;
        } else if (X > T->Element) {
            first = T;
            T = T->rchild;
        } else {
            first = T;        // this is the only difference from FindNeighborsLT !
            break;
        }
    }

    return createOffsetPair(first, second, limit);
}

int main(int argc, char **argv) {
  SPSTTree T = NULL;
  //! do some insertions
  for (size_t i = 0; i < 100; i ++){
    T = Insert(i,rand()%1000,T);
  }

  //! perform some range queries
  T = IntervalSplay(200,500,T);
  T = IntervalSplay(300,700,T);
  T = IntervalSplay(100,900,T);
  auto aux = GetNodesInOrder(T);

  //! do some pruning
  T = PruneLeaves(T);
}