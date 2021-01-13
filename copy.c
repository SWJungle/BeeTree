#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// 최대 자식노드 개수
#define T 2

//! Node 구조체 선언
typedef struct BNode
{
    int KeyCount;                // key의 갯수
    bool leaf;                   // leaf인지 판정
    bool root;                   // root인지 판정
    int keys[2 * T - 1];         // node가 가지고 있는 key 값들
    struct BNode *childs[2 * T]; // 현재 노드와 연결되어있는 child의 배열이다.

    struct BNode *parents;  // 부모노드 포인터
    struct BNode *prevNode; // leaf 연결노드 포인터
    struct BNode *nextNode; // leaf 연결노드 포인터
} BNODE;


//! tree 구조체 선언
typedef struct BPlusTree
{
    struct BNode *root;
} BPLUSTREE;

//! ------------------ 함수 선언부 --------------------//
// MAIN FUNCTION
BNODE *Allocate();
void Tree_Create(BPLUSTREE *);
void Insert(BPLUSTREE *, int);
void Deletion(BPLUSTREE *, int);
bool Search(BNODE *, int);

//SUB FUNCTION
void Insert_nonfull(BNODE *, int);
void SearchForDel(BPLUSTREE *, BNODE *, int);
void Arrange_for_Delete(BPLUSTREE *, BNODE *, int);
void Split_Child(BNODE *, int);
BNODE *Merge_Nodes(BNODE *, int);

//UTIL
void Insert_Of_N(BPLUSTREE *, int);
int Find_ChildIndex(BNODE *, int);
int Find_Value(BNODE *, int);
void Final_Delete(BNODE *, int);

void Shift_to_Right(BNODE *, int);
void Shift_to_Left(BNODE *, int);
bool Swap_Keys_Right(BNODE *, int);
bool Swap_Keys_Left(BNODE *, int);

int Get_Rand_Int();
BNODE *Find_Leaf(BNODE *, int);
void Leaf_Node_Pop(BNODE *);


// Print
void Print_Tree(BNODE *, int);
void Heap_Counting(char);

//! --------------------------------------------------//


//! ------------------ GLOBAL var --------------------//
unsigned int HEAPCOUNT = 0;
//! --------------------------------------------------//


//! ------------------- MAIN 함수 --------------------//
int main()
{
    printf("================================= START ===============================\n\n");
    srand((unsigned int)time(NULL));
    BPLUSTREE tree;
    Tree_Create(&tree);
    Print_Tree(tree.root, 0);
    //Insert_Of_N(&tree, 10);
    Insert(&tree, 10);
    Print_Tree(tree.root, 0);
    Insert(&tree, 20);
    Print_Tree(tree.root, 0);
    Insert(&tree, 30);
    Print_Tree(tree.root, 0);
    Insert(&tree, 40);
    Print_Tree(tree.root, 0);
    Insert(&tree, 50);
    Print_Tree(tree.root, 0);
    Insert(&tree, 60);
    Print_Tree(tree.root, 0);
    Insert(&tree, 80);
    Print_Tree(tree.root, 0);
    Insert(&tree, 90);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 20);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 10);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 30);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 50);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 40);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 60);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 80);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 90);
    Print_Tree(tree.root, 0);

    Insert(&tree, 30);
    Print_Tree(tree.root, 0);
    Insert(&tree, 20);
    Print_Tree(tree.root, 0);
    Insert(&tree, 40);
    Print_Tree(tree.root, 0);
    Insert(&tree, 100);
    Print_Tree(tree.root, 0);
    Insert(&tree, 80);
    Print_Tree(tree.root, 0);
    Insert(&tree, 50);
    Print_Tree(tree.root, 0);

    printf("\n\n================================= END ===============================\n\n");
    // Heap_Counting('*');
    return 0;
}

//! 새로운 노드 생성 함수
BNODE *Allocate()
{

    // BNODE 크기만큼 할당한다. (malloc은 메모리를 할당하고, 할당한 주소값을 반환한다.)
    BNODE *new_node = (BNODE *)malloc(sizeof(BNODE));
    // BNODE 형태의 new_node 선언 = BNODE 의 형태의 반환된 malloc의 주소값
    Heap_Counting('+');
    new_node->prevNode = NULL;
    new_node->nextNode = NULL;

    // new_node(BNODE)를 반환
    return new_node;
}

void Heap_Counting(char operand)
{
    if (operand == '+')
    {
        HEAPCOUNT++;
        printf("HEAPCOUNT++\n");
    }
    else if (operand == '-')
    {
        HEAPCOUNT--;
        printf("HEAPCOUNT--\n");
    }
    printf("Assigned %d struct onto Heap.\n\n", HEAPCOUNT);
}

void Tree_Create(BPLUSTREE *tree)
{                                 // BPLUSTREE는 root* 의 주소를 가진 구조체이다.
    BNODE *new_node = Allocate(); // 새 노드를 만들기 위해 포인터에 주소를 할당한다.
    new_node->KeyCount = 0;       // 새로만들면 key가 안들어있으므로 keycount를 0으로 한다.
    new_node->leaf = true;        // 마찬가지로 leaf속성을 on한다.
    tree->root = new_node;        // new_node가 루트노드가 되므로 tree의 root는 new_node로 한다. (둘다 포인터이다.)
    tree->root->root = true;
}

void Insert(BPLUSTREE *tree, int keyValue)
{
    BNODE *rootNode = tree->root;
    if (rootNode->KeyCount == 2 * T - 1)
    {
        BNODE *newRootNode = Allocate();
        tree->root = newRootNode;
        newRootNode->leaf = false;
        newRootNode->KeyCount = 0;
        newRootNode->childs[0] = rootNode;
        Split_Child(newRootNode, 0);
        Insert_nonfull(newRootNode, keyValue);
    }
    else
    {
        Insert_nonfull(rootNode, keyValue);
    }
    printf("Insertions [%d] is completed\n", keyValue);
}

void Split_Child(BNODE *parentNode, int ChildIndex)
{
    // right_node는 분리하면서 만들어질 새로운 node이기 때문에 메모리를 새로 할당한다.
    BNODE *right_node = Allocate();
    BNODE *left_node = parentNode->childs[ChildIndex];

    // right_node의 leaf 속성은 left_node의 leaf를 받아온다.
    right_node->leaf = left_node->leaf;

    //!---- leaf Node를 분리하는 경우 ----//
    if (left_node->leaf)
    {
        // 오른쪽 자식에는 T개 만큼 옮긴다. (부모의 키를 포함해야 하므로)
        right_node->KeyCount = T;
        left_node->KeyCount = T - 1;

        // left_node의 키들을 right_node로 이동
        for (int i = 0; i < T; ++i)
        {
            right_node->keys[i] = left_node->keys[T + i - 1];
        }
        for (int i = parentNode->KeyCount; i > ChildIndex; --i)
        {
            parentNode->childs[i + 1] = parentNode->childs[i];
            parentNode->keys[i] = parentNode->keys[i - 1];
        }
        parentNode->childs[ChildIndex + 1] = right_node;
        //! B+트리는 부모 key = 오른쪽 자식의 첫번째 key 이다.
        parentNode->keys[ChildIndex] = right_node->keys[0];
        parentNode->KeyCount++;
    }

    else
    {
        // 2T-1를 분리하면서 가운데 값은 부모로 올리고
        // 나머지  2T-2개의 key들을 left와 right가 나누어가진다.
        right_node->KeyCount = T - 1;
        left_node->KeyCount = T - 1;

        // left의 키와 자식을 right로 옮긴다.
        for (int i = 0; i < T - 1; ++i)
        {
            right_node->keys[i] = left_node->keys[T + i];
        }
        for (int i = 0; i < T; ++i)
        {
            right_node->childs[i] = left_node->childs[T + i];
        }
        for (int i = parentNode->KeyCount; i > ChildIndex; --i)
        {
            parentNode->childs[i + 1] = parentNode->childs[i];
            parentNode->keys[i] = parentNode->keys[i - 1];
        }
        parentNode->childs[ChildIndex + 1] = right_node;
        parentNode->keys[ChildIndex] = left_node->keys[T - 1];
        parentNode->KeyCount++;
    }

    // 부모, 자식 재구성
    left_node->parents = parentNode;
    right_node->parents = parentNode;
    left_node->nextNode = right_node;
    right_node->prevNode = left_node;
}

void Insert_nonfull(BNODE *node, int KeyValue)
{
    // KeyIndex는 node의 key 갯수를 할당한다.
    // 즉, key[]의 가장 마지막 key의 idx를 할당한다.
    int KeyIndex = node->KeyCount;

    // 만약 node가 leaf node라면
    if (node->leaf)
    {

        // node의 key 갯수가 1 이상이고 (빈 노드가 아니고),
        // 넣을 key값이 key[idx]보다 작아질때까지 idx를 줄인다.
        // idx는 새로운 key값을 넣을 자리가 된다.
        while (KeyIndex >= 1 && KeyValue < node->keys[KeyIndex - 1])
        {
            // 새로운 key값을 넣을 자리를 마련해준다. (한칸씩 오른쪽으로 이동한다.)
            node->keys[KeyIndex] = node->keys[KeyIndex - 1];
            KeyIndex--;
        }

        // key값을 넣을 자리를 찾았고, 위에서 해당 자리도 비워놨기 때문에
        // 위에서 찾은 key[idx]에 새로운 key값을 할당한다.
        node->keys[KeyIndex] = KeyValue;
        // 그리고 노드의 keycount를 1 늘려준다.
        node->KeyCount += 1;
    }
    else
    {
        while (KeyIndex >= 1 && KeyValue < node->keys[KeyIndex - 1])
        {
            KeyIndex--;
        }
        //! KeyIndex++;
        if (node->childs[KeyIndex]->KeyCount == 2 * T - 1)
        {
            Split_Child(node, KeyIndex);
            if (KeyValue > node->keys[KeyIndex])
            {
                KeyIndex++;
            }
        }
        Insert_nonfull(node->childs[KeyIndex], KeyValue);
    }
}

void Print_Tree(BNODE *node, int level)
{
    if (node->KeyCount == 0)
    {
        printf("\n[EMPTY]\n");
    }
    // leaf가 node가 아니면 DFS 실행함
    if (!node->leaf)
    {
        for (int i = 0; i <= node->KeyCount; i++)
        {
            Print_Tree(node->childs[i], level + 1);
            if (i < node->KeyCount)
            {
                for (int j = 0; j < level; j++)
                {
                    printf("--------------------|");
                }

                printf("[%d]", node->keys[i]);
            }
            printf("\n");
        }
    }
    else
    {
        for (int i = 0; i < level; i++)
        {
            printf("--------------------|");
        }
        for (int i = 0; i < node->KeyCount; i++)
        {
            printf("[%d]", node->keys[i]);
        }
        printf("\n");
    }
    return;
}
//////////삭제////////////////////
void Insert_Of_N(BPLUSTREE *tree, int n)
{
    for (int index = 0; index < n; ++index)
    {
        int item = Get_Rand_Int() % 101;
        Insert(tree, item);
    }
}
int Get_Rand_Int()
{

    return rand();
}
void Deletion(BPLUSTREE *tree, int keyValue)
{
    if (!Search(tree->root, keyValue))
    {
        printf("The keyvalue[%d] is not in the tree\n", keyValue);
    }
    else
    {
        SearchForDel(tree, tree->root, keyValue);
    }
}

bool Search(BNODE *node, int keyValue)
{
    int index = 0;
    while (index < node->KeyCount && keyValue > node->keys[index])
    {
        index++;
    }
    if (index <= node->KeyCount && keyValue == node->keys[index])
    {
        return true;
    }
    else if (node->leaf)
    {
        return false;
    }
    else
    {
        return Search(node->childs[index], keyValue);
    }
}

void SearchForDel(BPLUSTREE *tree, BNODE *node, int keyValue)
{
    if ( node->leaf )
    {
        Final_Delete(node,keyValue);
    }
    else if ( node->childs[0]->leaf)
    {
        int childIndex = Find_ChildIndex(node,keyValue);
        Final_Delete(node->childs[childIndex],keyValue);
        if (childIndex == 0)
        {
            for (int i = childIndex; i < node->KeyCount - 1; ++i)
            {
                node->keys[i] = node->keys[i + 1];
            }
        }
        else
        {
            for (int i = childIndex - 1; i < node->KeyCount - 1; ++i)
            {
                node->keys[i] = node->keys[i + 1];
            }
        }

        for (int i = childIndex; i < node->KeyCount; ++i)
        {
            node->childs[i] = node->childs[i + 1];
        }
        node->KeyCount--;

        if (node->KeyCount == 0)
        {
            tree->root = node->childs[0];
            free(node);
            printf("root is changed \n");
            Heap_Counting('-');
        }
    }
    else
    {
        Arrange_for_Delete(tree, node, keyValue);
    }
}

int Find_ChildIndex(BNODE *node, int keyValue)
{

    int index = 0;
    for (index = 0; index < node->KeyCount; ++index)
    {
        if (keyValue < node->keys[index])
        {
            return index;
        }
    }
    return index;
}

void Arrange_for_Delete(BPLUSTREE *tree, BNODE *node, int keyValue)
{
    int childIndex = Find_ChildIndex(node, keyValue);

    if (node->childs[childIndex]->leaf)
    {

        Final_Delete(node->childs[childIndex], keyValue);
        if (childIndex == 0)
        {
            for (int i = childIndex; i < node->KeyCount - 1; ++i)
            {
                node->keys[i] = node->keys[i + 1];
            }
        }
        else
        {
            for (int i = childIndex - 1; i < node->KeyCount - 1; ++i)
            {
                node->keys[i] = node->keys[i + 1];
            }
        }

        for (int i = childIndex; i < node->KeyCount; ++i)
        {
            node->childs[i] = node->childs[i + 1];
        }
        node->KeyCount--;

        if (node->KeyCount == 0)
        {
            tree->root = node->childs[0];
            free(node);
            printf("root is changed \n");
            Heap_Counting('-');
        }
        return;
    }

    else if (node->childs[childIndex]->KeyCount < T)
    {

        //! 0 < childIndex < node->keycount - 1
        if (childIndex > 0 && childIndex < node->KeyCount - 1)
        {
            // 왼쪽 자식이 키가 많을 경우
            if (node->childs[childIndex - 1]->KeyCount >= T)
            {
                Swap_Keys_Left(node, childIndex);
                Shift_to_Right(node, childIndex);
                Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
            }
            // 오른쪽 자식이 키가 많을 경우
            else if (node->childs[childIndex + 1]->KeyCount >= T)
            {
                Swap_Keys_Right(node, childIndex);
                Shift_to_Left(node, childIndex);
                Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
            }

            // 둘 다 키가 충분하지 않을 경우
            else
            {
                BNODE *child_node = Merge_Nodes(node, childIndex);
                if (node->KeyCount == 0)
                {
                    tree->root = node->childs[0];
                    free(node);
                    printf("root is changed\n");
                    Heap_Counting('-');
                    Arrange_for_Delete(tree, tree->root, keyValue);
                }
                else
                {
                    Arrange_for_Delete(tree, child_node, keyValue);
                }
            }
        }

        //! childIndex == 0
        else if (childIndex == 0)
        {

            // 오른쪽 자식이 키가 많을 경우
            if (node->childs[childIndex + 1]->KeyCount >= T)
            {
                Swap_Keys_Right(node, childIndex);
                Shift_to_Left(node, childIndex);
                Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
            }
            else
            {
                BNODE *child_node = Merge_Nodes(node, childIndex);
                if (node->KeyCount == 0)
                {
                    tree->root = node->childs[0];
                    free(node);
                    printf("root is changed\n");
                    Heap_Counting('-');
                    Arrange_for_Delete(tree, tree->root, keyValue);
                }
                else
                {
                    Arrange_for_Delete(tree, child_node, keyValue);
                }
            }
        }

        //! childIndex == node->keycount
        else
        {
            // 오른쪽 자식이 키가 많을 경우
            if (node->childs[childIndex - 1]->KeyCount >= T)
            {
                Swap_Keys_Left(node, childIndex);
                Shift_to_Right(node, childIndex);
                Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
            }

            else
            {
                BNODE *child_node = Merge_Nodes(node, childIndex);
                if (node->KeyCount == 0)
                {
                    tree->root = node->childs[0];
                    free(node);
                    printf("root is changed\n");
                    Heap_Counting('-');
                    Arrange_for_Delete(tree, tree->root, keyValue);
                }
                else
                {
                    Arrange_for_Delete(tree, child_node, keyValue);
                }
            }
        }
    }
    else
    {
        printf("enough\n");
        Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
    }
}

void Final_Delete(BNODE *node, int keyValue)
{
    int index;
    for (int i = 0; i < node->KeyCount; ++i)
    {
        if (node->keys[i] == keyValue)
        {
            index = i;
            break;
        }
    }

    for (int i = index; i < node->KeyCount - 1; ++i)
    {
        node->keys[i] = node->keys[i + 1];
    }
    node->KeyCount--;

    if (node->KeyCount == 0)
    {
        Leaf_Node_Pop(node);
    }
    printf("Delete [%d] is completed. :P\n", keyValue);

    return;
}

void Leaf_Node_Pop(BNODE *node)
{
    if (node->prevNode != NULL)
    {
        BNODE *pointNext = node->nextNode;
        node->prevNode->nextNode = pointNext;
    }

    if (node->nextNode != NULL)
    {
        BNODE *pointPrev = node->prevNode;
        node->nextNode->prevNode = pointPrev;
    }

    free(node);
    printf("leaf is poped\n");
    Heap_Counting('-');
    //printf("도비는 자유에요! \n");
}

BNODE *Merge_Nodes(BNODE *node, int childIndex)
{ //! 여기는 무조건 양쪽 자식노드의 키가 T-1임을 알고 있다...

    //! childIndex가 node의 마지막 자식이 아닌 경우
    if (childIndex < node->KeyCount)
    {
        free(node->childs[childIndex + 1]);
        printf("Internal is poped \n");
        Heap_Counting('-');
        // 부모 노드의 childIndex key를 선행 자식 노드의 끝으로 이동
        node->childs[childIndex]->keys[T - 1] = node->keys[childIndex];
        // 부모 노드의 후행 자식 노드의 key들을 선행 자식 노드로 이동
        for (int i = 0; i < T - 1; ++i)
        {
            node->childs[childIndex]->keys[i + T] = node->childs[childIndex + 1]->keys[i];
        }
        // 부모 노드의 후행 자식 노드의 child들을 선행 자식 노드로 이동
        // childs leaf 가 아닐 때만 수행,
        for (int i = 0; i < T; ++i)
        {
            node->childs[childIndex]->childs[i + T] = node->childs[childIndex + 1]->childs[i];
        }
        // 부모 노드의 선행 자식 노드의 keyCount 갱신
        node->childs[childIndex]->KeyCount = 2 * T - 1;
        // 부모 노드의 key 갱신 및 child 갱신
        for (int i = childIndex; i < node->KeyCount - 1; ++i)
        {
            node->keys[i] = node->keys[i + 1];
        }
        for (int i = childIndex + 1; i < node->KeyCount; ++i)
        {
            node->childs[i] = node->childs[i + 1];
        }
        //! 부모 노드의 key count 갱신
        node->KeyCount--;
        return node->childs[childIndex];
    }
    //! childIndex가 node의 마지막 자식인 경우
    else
    {
        free(node->childs[childIndex]);
        printf("Internal is poped \n");
        Heap_Counting('-');
        // 부모 노드의 childIndex key를 선행 자식 노드의 처음으로 이동
        node->childs[childIndex - 1]->keys[T - 1] = node->keys[childIndex];
        // 부모 노드의 후행 자식 노드의 key들을 선행 자식 노드로 이동
        for (int i = 0; i < T - 1; ++i)
        {
            node->childs[childIndex - 1]->keys[i + T] = node->childs[childIndex]->keys[i];
        }
        // 부모 노드의 후행 자식 노드의 child들을 선행 자식 노드로 이동
        for (int i = 0; i < T; ++i)
        {
            node->childs[childIndex - 1]->childs[i + T] = node->childs[childIndex]->childs[i];
        }
        // 부모 노드의 선행 자식 노드의 keyCount 갱신
        node->childs[childIndex - 1]->KeyCount = 2 * T - 1;

        //! 부모노드의 key count 갱신해야함.
        //! 노드의 key count --;
        node->KeyCount--;
        return node->childs[childIndex - 1];
    }
}

bool Swap_Keys_Left(BNODE *node, int childIndex)
{
    int tmp_keyindex = node->childs[childIndex - 1]->KeyCount;
    int tmp_key = node->childs[childIndex - 1]->keys[tmp_keyindex];
    node->childs[childIndex - 1]->keys[tmp_keyindex] = node->keys[childIndex];
    node->keys[childIndex] = tmp_key;
}
bool Swap_Keys_Right(BNODE *node, int childIndex)
{
    int tmp_key = node->childs[childIndex + 1]->keys[0];
    node->childs[childIndex + 1]->keys[0] = node->keys[childIndex];
    node->keys[childIndex] = tmp_key;
}

void Shift_to_Left(BNODE *node, int childIndex)
{
    int target_position = node->childs[childIndex]->KeyCount;
    // 후행 자식 노드의 첫번째 key를 목표 자식 노드의 마지막으로 이동
    node->childs[childIndex]->keys[target_position] = node->childs[childIndex + 1]->keys[0];
    // 후행 자식 노드의 첫번째 포인터를 목표 자식 노드의 마지막으로 이동
    node->childs[childIndex]->childs[target_position + 1] = node->childs[childIndex + 1]->childs[0];

    // 후행 자식 노드의 key들을 왼쪽으로 한칸씩 이동
    int target_position_2 = node->childs[childIndex + 1]->KeyCount;
    int i = 0;
    while (i < target_position_2 - 1)
    {
        node->childs[childIndex + 1]->keys[i] = node->childs[childIndex + 1]->keys[i + 1];
        ++i;
    }
    // 후행 자식 노드의 자식들을 왼쪽으로 한칸씩 이동
    i = 0;
    while (i < target_position_2)
    {
        node->childs[childIndex + 1]->childs[i] = node->childs[childIndex + 1]->childs[i + 1];
        ++i;
    }
    // keyCount 조절
    node->childs[childIndex]->KeyCount++;
    node->childs[childIndex + 1]->KeyCount--;
}
void Shift_to_Right(BNODE *node, int childIndex)
{
    int target_position = node->childs[childIndex]->KeyCount;
    // 후행 자식 노드의 key들을 오른쪽으로 한칸씩 이동
    int i = target_position;
    while (i > 0)
    {
        node->childs[childIndex]->keys[i] = node->childs[childIndex]->keys[i - 1];
        --i;
    }
    // 후행 자식 노드의 자식들을 오른쪽으로 한칸씩 이동
    i = target_position + 1;
    while (i > 0)
    {
        node->childs[childIndex]->childs[i] = node->childs[childIndex]->childs[i - 1];
        --i;
    }
    int target_position_2 = node->childs[childIndex - 1]->KeyCount;
    // 선행 자식 노드의 마지막 key를 목표 자식 노드의 첫번째로 이동
    node->childs[childIndex]->keys[0] = node->childs[childIndex - 1]->keys[target_position_2 - 1];
    // 선행 자식 노드의 마지막 포인터를 목표 자식 노드의 첫번째로 이동
    node->childs[childIndex]->childs[0] = node->childs[childIndex - 1]->childs[target_position_2];
    // keyCount 조절
    node->childs[childIndex - 1]->KeyCount--;
    node->childs[childIndex]->KeyCount++;
}


int Find_Value(BNODE *node, int keyValue)
{
    for (int idx = 0; idx < node->KeyCount; ++idx)
    {
        if (node->keys[idx] == keyValue)
        {
            return idx;
        }
    }
    return -1;
}