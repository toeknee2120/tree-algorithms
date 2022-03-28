#include "tree.h"
#include "string.h"

/**********  Helper functions for removing from an AVL tree **********/
TNode* removeNextInorder( TNode** pRoot );

/**********  Helper functions for balancing an AVL tree **********/
void updateHeights(TNode* root);
void rebalanceTree(Tree* t, TNode* x);
void rightRotate(Tree* t, TNode* root);
void leftRotate(Tree* t, TNode* root);

TNode* getTallerSubTree(TNode* root);
bool isSameSignBalance(TNode* x, TNode* z);
int subTreeHeight(TNode* root);

/* printNames
 * input: none
 * output: none
 *
 * Prints names of the students who worked on this solution
 */
void printNames( )
{
    /* TODO : Fill in you and your partner's names (or N/A if you worked individually) */
    printf("\nThis solution was completed by:\n");
    printf("<student name #1>\n");
    printf("<student name #2 (if no partner write \"N/A\")>\n\n");
}

/* createTree
 * input: none
 * output: a pointer to a Tree (this is malloc-ed so must be freed eventually!)
 *
 * Creates a new empty Tree and returns a pointer to it.
 */
Tree *createTree( )
{
    Tree* t = (Tree*)malloc( sizeof(Tree) );
    t->root = NULL;

    return t;
}

Tree *createTreeFromTNode( TNode* root )
{
    Tree* t = (Tree*)malloc( sizeof(Tree) );
    t->root = root;

    return t;
}

/* attachChildNodes
 * input: three pointers to a TNodes
 * output: none
 *
 * Sets root's left and right children to the specified nodes
 */
void attachChildNodes( TNode* root, TNode* left, TNode* right ){
    root->pLeft = left;
    root->pRight = right;
    if( left!=NULL )
        root->pLeft->pParent = root;
    if( right!=NULL )
        root->pRight->pParent = root;
    updateHeights(root);
}

/* createTNode
 * input: none
 * output: TNode*
 *
 * Malloc and returns a new empty TNode
 */
TNode* createTNode( ){
    TNode* newNode = (TNode*)malloc( sizeof(TNode) );
    newNode->height = 1;
    newNode->pParent = NULL;
    attachChildNodes( newNode, NULL, NULL );
    return newNode;
}

/* freeTree and freeTreeContents
 * input: a pointer to a Tree
 * output: none
 *
 * frees the given Tree and all of Data elements
 */
void freeTree( Tree *t )
{
    freeTreeContents(t->root, t->type);
    free(t);
}

void freeTreeContents( TNode *root, treeType type )
{
    if(root==NULL)
        return;

    freeTreeContents(root->pLeft, type);
    freeTreeContents(root->pRight, type);
    if(type==AVL && root->data!=NULL)
        freeData(root->data);
    if(type==HUFFMAN && root->str!=NULL)
        free(root->str);

    free(root);
}


/**********  Functions for searching an AVL tree **********/

/* searchTree and searchTreeRec
 * input: a pointer to a Tree, a Data* tData
 * output: a pointer to the TNode that contains tData or, if no such node exists, NULL
 *
 * Finds and returns a pointer to the TNode that contains tData or, if no such node exists, it returns a NULL
 */
TNode* searchTree( Tree *t, Data* tData )
{
    return searchTreeRec( t->root, tData );
}

TNode* searchTreeRec( TNode *root, Data* tData )
{
    if( root==NULL || compareData( tData, root->data ) == 0 )
        return root;
    else if( compareData( tData, root->data ) < 0 )
        return searchTreeRec( root->pLeft, tData );
    else /* compareData( tData, root->data ) > 0 */
        return searchTreeRec( root->pRight, tData );
}


/**********  Functions for inserting/removing from an AVL tree **********/

/* insertNode
 * input: a pointer to a TNode, a Data*
 * output: none
 *
 * Stores the passed Data* in the given tree, does not rebalance tree
 */
TNode* insertNode( TNode *root, TNode* newNode )
{
    if( root==NULL )
        return newNode;
    if( compareData( newNode->data, root->data ) == 0 ){
        free( newNode );
        return root;
    }
    else if( compareData( newNode->data, root->data ) < 0 ){
        root->pLeft = insertNode( root->pLeft, newNode );
        if( root->pLeft!=NULL )
          root->pLeft->pParent = root;
        return root;
    }
    else{ /* compareData( newNode->data, root->data ) > 0 */
        root->pRight = insertNode( root->pRight, newNode );
        if( root->pRight!=NULL )
          root->pRight->pParent = root;
        return root;
    }
}

/* insertTree
 * input: a pointer to a Tree, a Data*
 * output: none
 *
 * Stores the passed Data* into the Tree following BST order, Does not rebalance tree
 */
void insertTree( Tree *t, Data* tData )
{
    TNode* newNode = createTNode( );
    newNode->data = tData;
    t->root = insertNode( t->root, newNode );
    updateHeights(newNode);
}

/* insertTreeBalanced
 * input: a pointer to a Tree, a Data*
 * output: none
 *
 * Stores the passed Data* into the Tree following BST order and rebalances the tree
 */
void insertTreeBalanced( Tree *t, Data* tData )
{
    TNode* newNode = createTNode( );
    newNode->data = tData;
    t->root = insertNode( t->root, newNode );
    updateHeights(newNode);
    rebalanceTree( t, newNode );
}

/* removeTree
 * input: a pointer to a Tree
 * output: a Data*
 *
 * Remove and returns the Data* with the specified key or NULL if its not in the tree
 */
Data* removeTree( Tree *t, char* key )
{
    Data temp;
    Data* ret;
    TNode *del, *update;
    TNode **parentDelPtr;

    temp.key = key;
    del = searchTree( t, &temp );

    if( del == NULL )
        return NULL;
        
    ret = del->data;

    /* Get previous node's pointer to del */
    if( del->pParent==NULL )
        parentDelPtr = &t->root;    /* del is the root */
    else
        parentDelPtr = del->pParent->pLeft==del ? &del->pParent->pLeft : &del->pParent->pRight; /* check if del if left or right child of its parent */

    /* del has no left child */
    if( del->pLeft==NULL ){
        *parentDelPtr = del->pRight;
        if( del->pRight!=NULL )
            del->pRight->pParent = del->pParent;
        update = del->pParent;
        free( del );
    }

    /* del has no right child */
    else if( del->pRight==NULL ){
        *parentDelPtr = del->pLeft;
        if( del->pLeft!=NULL )
            del->pLeft->pParent = del->pParent;
        update = del->pParent;
        free( del );
    }

    /* del has two children */
    else{
        TNode *next = removeNextInorder( &del->pRight );
        update = next->pParent;
        del->data = next->data;
        free(next);
    }

    /* Update the heights and rebalance around the node update */
    updateHeights(update);
    rebalanceTree(t, update);
    return ret;
}

TNode* removeNextInorder( TNode** pRoot ){
    TNode* temp = *pRoot;

    if( temp->pLeft==NULL ){
        *pRoot = temp->pRight;
        if( temp->pRight!=NULL )
            temp->pRight->pParent = temp->pParent;
    }
    else
        temp = removeNextInorder( &temp->pLeft );

    return temp;
}

int subTreeHeight(TNode* root){
    if( root==NULL )
        return 0;
    return root->height;
}

/* updateHeights
 * input: a pointer to a TNode
 * output: none
 *
 * Recursively recomputes the height of the current node and then recurses on the nodes parent
 */
void updateHeights(TNode* root){
    if( root!=NULL ){
        root->height = subTreeHeight(root->pLeft)>subTreeHeight(root->pRight) ? subTreeHeight(root->pLeft) : subTreeHeight(root->pRight);
        root->height = root->height + 1;
        updateHeights( root->pParent );
    }
}

/* rebalanceTree
 * input: a pointer to a tree and a pointer to TNode
 * output: none
 *
 * Should attempt to rebalance the tree starting at x and going up through the root (i.e., until it reaches null).
 * After this function runs, every node should be balanced (i.e. -2 < balance < 2).
 *
 * For additional help in testing set the parameters PRINT_AVL_TREE and PRINT_AVL_ERRORS to true in the driver
 */
void rebalanceTree(Tree* t, TNode* x){
    int balance;
    TNode* z;

    while ( x != NULL ){
        balance = getBalance(x);

        // 1 if the balance of x is <= -2 or >= 2
        if (balance <= -2 || balance >= 2){
            // (i) Set z equal to the child of x with the greater height
            z = getTallerSubTree(x);
            
            // (ii) if the balance of x and the balance of z have DIFFERENT signs
            if (!isSameSignBalance(x, z)){
                
                // (A) if the sign of the balance of z is +, right rotate on z
                if (getBalance(z) > 0){ //TODO: does "positive" mean >= 0?
                    if (z->pLeft != NULL)
                        rightRotate(t,z);
                }
                else{
                    // (B) otherwiase, the balance is -, so rotate left on z
                    if (z->pRight != NULL)
                        leftRotate(t,z);
                }
            }
            else{
                // (iii) if the balance of x >=2, right rotate on x
                if (getBalance(x) >= 2){
                    if (x->pLeft != NULL)
                        rightRotate(t,x);
                }
                else{
                    // (iv) otherwise the balance is <= -2 so left rotate on x
                    if (x->pRight != NULL)
                        leftRotate(t,x);
                }

            }
        }

        // 2 set x equal to the parent of x
        else{
            x = x->pParent;
        }

    }
}

/* rightRotate and leftRotate
 * input: a pointer to the Tree, a pointer to a TNode
 * output: none
 *
 * Performs specified rotation around a given node and updates the root of the tree if needed
 */
void rightRotate(Tree* t, TNode* oldRoot){
    TNode *newRoot;

    if( oldRoot==NULL ){
        printf("ERROR - Attempting to do a rightRotate on a NULL Root.");
        return;
    }

    newRoot = oldRoot->pLeft;

    if( newRoot==NULL ){
        printf("ERROR - Attempting to do a rightRotate on a node with no left child (i.e. left child is NULL).");
        return;
    }

    /* check whether oldRoot is the root of the entire tree */
    if( oldRoot->pParent!=NULL ){
        if( oldRoot->pParent->pLeft==oldRoot )
            oldRoot->pParent->pLeft = oldRoot->pLeft;
        else
            oldRoot->pParent->pRight = oldRoot->pLeft;
    }
    else
        t->root = newRoot;
    newRoot->pParent = oldRoot->pParent;

    oldRoot->pLeft = newRoot->pRight;
    if( newRoot->pRight!=NULL )
        newRoot->pRight->pParent = oldRoot;

    oldRoot->pParent = newRoot;
    newRoot->pRight = oldRoot;

    updateHeights( oldRoot );
}

void leftRotate(Tree* t, TNode* oldRoot){
    TNode *newRoot;

    if( oldRoot==NULL ){
        printf("ERROR - Attempting to do a leftRotate on a NULL Root.");
        return;
    }

    newRoot = oldRoot->pRight;

    if( newRoot==NULL ){
        printf("ERROR - Attempting to do a leftRotate on a node with no right child (i.e. right child is NULL).");
        return;
    }

    /* check whether oldRoot is the root of the entire tree */
    if( oldRoot->pParent!=NULL ){
        if( oldRoot->pParent->pRight==oldRoot )
            oldRoot->pParent->pRight = oldRoot->pRight;
        else
            oldRoot->pParent->pLeft = oldRoot->pRight;
    }
    else
        t->root = newRoot;
    newRoot->pParent = oldRoot->pParent;

    oldRoot->pRight = newRoot->pLeft;
    if( newRoot->pLeft!=NULL )
        newRoot->pLeft->pParent = oldRoot;

    oldRoot->pParent = newRoot;
    newRoot->pLeft = oldRoot;

    updateHeights( oldRoot );
}

/* getBalance
 * input: a pointer to a TNode
 * output: none
 *
 * Finds the balance of the given node
 */
int getBalance(TNode* root){
    if( root==NULL )
        return 0;
    return subTreeHeight(root->pLeft) - subTreeHeight(root->pRight);
}

/**********  Functions for getting Huffman Encoding **********/

/* printHuffmanEncoding
 * input: a pointer to TNode and a char
 * output: none
 *
 * The variable root points to the root of a Huffman tree that has already been built for you.
 * This function prints the Huffman encoding for the char c (going left prints '0' and right prints '1').
 * Each node in the tree has a member variable called str which is a list of the chars whose encoding is provided at or below this node in the subtree.
 *
 * It may help to use the String.h function strchr. (check out https://www.tutorialspoint.com/c_standard_library/c_function_strchr.htm for usage details)
 *
 */
void printHuffmanEncoding( TNode* root, char c ){

    if ( root != NULL){

        //check myself
        if ( strcmp(root->str, &c) == 0 ){
            return;
        }


        //check the left side
        if ( (root->pLeft != NULL) && (strchr(root->pLeft->str, c) != NULL) )
        {
            printf("0");
            printHuffmanEncoding(root->pLeft, c);
        }
        
        //check the right side
        if ( (root->pLeft != NULL) && (strchr(root->pRight->str, c) != NULL) )
        {
            printf("1");
            printHuffmanEncoding(root->pRight, c);
        }

    }


 

}

/**********  Functions for Segment Tree **********/

/* constructSegmentTree
 * input: an array of doubles, an int low, an int high
 * output: the root of a tree
 *
 * Recursively builds a balanced tree containing all of the data in array points from index low to index high.
 */
TNode* constructSegmentTree( double* points, int low, int high ){
    TNode* root = createTNode( );
    root->cnt = 0;
    root->low = points[low];
    root->high = points[high];

    /* Recursively split the array around the mid point of the high and low indices */
    int mid = (high - low)/2 + low;
    if( low!=high ) /* more nodes left in the sub-array */
        attachChildNodes( root, constructSegmentTree( points, low, mid ), constructSegmentTree( points, mid+1, high ) );

    return root;
}

/* insertSegment
 * input: the root of a tree, a double segmentStart, and a double segmentEnd
 * output: none
 *
 * Recursively inserts given line segment from segmentStart to segmentEnd into the tree
 */
void insertSegment( TNode* root, double segmentStart, double segmentEnd ){
    //TODO
    // (1) go down the segment tree starting at the root
    
    
    //(2) If the root is NULL, return.
    if ( root == NULL){
        return;
    }
    else if ( segmentEnd < root->low || segmentStart > root->high ){
    // (3) Else if the given segment is completely to the left or right of the current node’s range,
    // return.
            return;
    }
    else if( segmentStart <= root->low && segmentEnd >= root->high ) {
    // (4) Else if the given segment completely covers the range represented by this node: increase
    // cnt by 1 and return.
        root->cnt += 1;
        return;

    }else{
    // (5) Else: Recursively call insertSegment on the left and right children of this node.
        insertSegment(root->pLeft, segmentStart, segmentEnd);
        insertSegment(root->pRight, segmentStart, segmentEnd);
    }


}

/* lineStabQuery
 * input: the root of a tree, a double queryPoint
 * output: none
 *
 * Recursively count the number of line segments which intersect the queryPoint.
 */
int lineStabQuery( TNode* root, double queryPoint ){
    //TODO
    // (1) Go down the segment tree starting at the root
    int left_sum;
    int right_sum;

    if ( root == NULL)
        // (2) If the root is NULL, return 0.
        return 0;
    else if ( queryPoint < root->low || queryPoint > root->high )
        // (3) Else if the queryPoint is completely to the left or right of the current node’s range:
        // return 0.
        return 0;
    else{
        // (4) Else: Recursively call lineStabQuery on the left and right children of this node. Return
        // the sum of their return values and current node’s cnt.
        left_sum = lineStabQuery(root->pLeft,queryPoint);
        right_sum = lineStabQuery(root->pRight,queryPoint);

        return ( left_sum + right_sum + root->cnt);
    }


    return -1;
}



/**********  Functions for debugging an AVL tree **********/

/* printTree
 * input: a pointer to a Tree
 * output: none
 *
 * Prints the contents of the tree below the root node
 */
void printTree( TNode* root ){
    int i;
    if(root!=NULL){
        printTree(root->pLeft);
        for( i=1; i<root->height; i++){
            printf("\t");
        }
        printf("%s\n",root->data->key);
        printTree(root->pRight);

    }
}

/* checkAVLTree
 * input: a pointer to a Tree
 * output: none
 *
 * Prints error messages if there are any problems with the AVL tree
 */
void checkAVLTree(TNode* root){
    if( root!=NULL ){
        if( getBalance(root)>1 ||  getBalance(root)<-1 )
            printf("ERROR - Node %s had balance %d\n",root->data->key,getBalance(root) );
        if( root->pLeft!=NULL && root->pLeft->pParent!=root )
            printf("ERROR - Invalid edge at %s-%s\n",root->data->key,root->pLeft->data->key );
        if( root->pRight!=NULL && root->pRight->pParent!=root )
            printf("ERROR - Invalid edge at %s-%s\n",root->data->key,root->pRight->data->key );

        checkAVLTree(root->pLeft);
        checkAVLTree(root->pRight);
    }
}

/**********  Functions for printing a tree **********/

/* printTreeByType
 * input: a pointer to a Tree
 * output: none
 *
 * Prints the contents of the tree below the root node based on the type of data stored in the tree
 */

void printTreeByType( Tree* t, TNode* root, int depth ){
    int i;
    char c = '+';
    if( root!=NULL ){
        printTreeByType( t, root->pRight, depth+1 );
        for( i=0; i<depth; i++){
            printf("\t");
        }
        if(root->height==1)
            c = '-';

        if( depth == 0 )
            printf("R");
        else if( root->pParent->pLeft == root )
            printf("\\");
        else
            printf("/");

        if( t->type == AVL ){ 
            /* For better readability sets the index 7 to a '+' when printing non-leaf nodes */
            root->data->key[7] = c;
            printf("%s\n",root->data->key);
            root->data->key[7] = '-';
        }
        else if( t->type == HUFFMAN )
            printf("-------%c--str = %s, priority = %d\n", c, root->str, root->priority);
        else if( t->type == SEGMENT )
            printf("-------%c--(low, high) = ( %.2lf, %.2lf ), cnt = %d\n", c, root->low, root->high, root->cnt);
        else
            printf("-------%c-- INVALID TREE TYPE", c);
        printTreeByType( t, root->pLeft, depth+1 );

    }
}



/*
    Returns the taller subtree
*/
TNode* getTallerSubTree(TNode* root){
    if ( subTreeHeight(root->pLeft) > subTreeHeight(root->pRight))
        return root->pLeft;
    else
        return root->pRight;
}

bool isSameSignBalance(TNode* x, TNode* z){
    if ( 
        (getBalance(x) >= 0 && getBalance(z) >= 0) ||
        (getBalance(x) <= 0 && getBalance(z) <= 0)
    )
        return true;
    else
        return false;
}