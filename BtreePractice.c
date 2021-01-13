#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
/*
    1) search == True >> swap 하고 delete
    2) search == False >> 반복[Find_ChildIndex(sub 트리를 결정해야 한다.) > search] 하고 swap 대상 leaf에서 찾고 그 키와 swap한다.
*/
#define T 2
typedef struct BtreeNode
{
    int KeyCount;                // key의 갯수
    bool leaf;                   // leaf인지 판정
    int keys[2 * T - 1];         // node가 가지고 있는 key 값들
    struct BtreeNode *childs[2*T]; // 현재 노드와 연결되어있는 child의 배열이다.
} BTREENODE;
// tree 구조체 선언
typedef struct Btree
{
    struct BtreeNode *root;
} BTREE;
// Btree라는 구조체는 node포인터를 가진다. (이름이 root)
// 비트리 루트 노드의 포인터를 가짐으로서 비트리 전체에 관여할 수 있도록한다.

//! ------------------ 함수 선언부 --------------------//
// MAIN FUNCTION
BTREENODE *Allocate();
void Tree_Create(BTREE *);
void Insert(BTREE *, int);
void Deletion(BTREE *, int);
bool Search(BTREENODE *, int);

//SUB FUNCTION
void Insert_nonfull(BTREENODE *, int);
void SearchForDel(BTREE *, BTREENODE *, int);
void Arrange_for_Delete(BTREE *, BTREENODE *, int);
void Split_Child(BTREENODE *, int);
BTREENODE *Merge_Nodes(BTREENODE *, int);

//UTIL
void Insert_Of_N(BTREE *, int);
int Find_ChildIndex(BTREENODE *, int);
int Find_Value(BTREENODE *, int);
void Final_Delete(BTREENODE *, int);

void Shift_to_Right(BTREENODE *, int);
void Shift_to_Left(BTREENODE *, int);
bool Swap_Keys_Right(BTREENODE *, int);
bool Swap_Keys_Left(BTREENODE *, int);

int Find_KeyPrime_Presuccecor(BTREENODE *, int);
int Find_KeyPrime_succecor(BTREENODE *, int);

int Get_Rand_Int();
BTREENODE *Find_Leaf(BTREENODE *, int);
void Leaf_Node_Pop(BTREENODE *);


// Print
void Print_Tree(BTREENODE *, int);
void Heap_Counting(char);

//! ------------------ GLOBAL var --------------------//
unsigned int HEAPCOUNT = 0;
//! --------------------------------------------------//

//! ------------------- MAIN 함수 --------------------//
int main()
{
    printf("================================= START ===============================\n\n");
    srand((unsigned int)time(NULL));
    BTREE tree;
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
    Deletion(&tree, 40);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 50);
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

// 노드를 생성하기 전에 메모리를 할당한다.
BTREENODE *Allocate()
{

    // BTREENODE의 크기만큼 할당한다. (malloc은 메모리를 할당하고, 할당한 주소값을 반환한다.)
    BTREENODE *new_node = (BTREENODE *)malloc(sizeof(BTREENODE));
    // BTREENODE*형태의 new_node 선언 = BTREENODE*의 형태의 반환된 malloc의 주소값

    Heap_Counting('+');
    // new_node(BTREENODE*)를 반환
    return new_node;
}

// 트리를 만든다.
void Tree_Create(BTREE *tree)
{                                     // BTREE는 root* 의 주소를 가진 구조체이다.
    BTREENODE *new_node = Allocate(); // 새 노드를 만들기 위해 포인터에 주소를 할당한다.
    new_node->KeyCount = 0;           // 새로만들면 key가 안들어있으므로 keycount를 0으로 한다.
    new_node->leaf = true;            // 마찬가지로 leaf속성을 on한다.
    tree->root = new_node;            // new_node가 루트노드가 되므로 tree의 root는 new_node로 한다. (둘다 포인터이다.)
}

// key 갯수가 2T-1보다 작은 node에 값을 삽입한다.
void Insert_nonfull(BTREENODE *node, int KeyValue)
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

// node의 key 갯수가 2T-1인 경우, node를 분리한다.
void Split_Child(BTREENODE *parentNode, int ChildIndex)
{
    // right_node는 분리하면서 만들어질 새로운 node이기 때문에 메모리를 새로 할당한다.
    BTREENODE *right_node = Allocate();

    //
    BTREENODE *left_node = parentNode->childs[ChildIndex];

    // right_node의 leaf 속성은 left_node의 leaf를 받아온다.
    right_node->leaf = left_node->leaf;

    // 2T-1를 분리하면서 가운데 값은 부모로 올리고
    // 나머지  2T-2개의 key들을 left와 right가 나누어가진다.
    right_node->KeyCount = T - 1;
    left_node->KeyCount = T - 1;

    // 그래서 left_node[i]
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

// key값을 tree에 삽입한다.
void Insert(BTREE *tree, int keyValue)
{
    BTREENODE *rootNode = tree->root;
    if (rootNode->KeyCount == 2 * T - 1)
    {
        BTREENODE *newRootNode = Allocate();
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
    printf("Insertions [%d] is completed\n\n", keyValue);
}

void Insert_Of_N(BTREE *tree, int n)
{
    printf("Inserted randNum :: ");
    for (int index = 0; index < n; ++index)
    {
        int item = Get_Rand_Int() % 101;
        Insert(tree, item);
    }
}

void Deletion(BTREE *tree, int keyValue)
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

bool Search(BTREENODE *node, int keyValue)
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

void SearchForDel(BTREE *tree, BTREENODE *node, int keyValue)
{
    // 해당 key가 현재 node에 있는가?
    int index = Find_Value(node, keyValue);
    if (index == -1)
    {
        SearchForDel(tree, node->childs[Find_ChildIndex(node, keyValue)], keyValue);
    }
    else
    {
        // 있으면 리프인가? 내부인가?
        if (node->leaf)
        {
            // 리프이면서 leaf의 keycount 가 T-1 이상인 경우 바로 삭제한다.

            //! 아래 사항 수정함.
            if (node->KeyCount > T - 1)
            {
                int index = Find_ChildIndex(node, keyValue);
                Final_Delete(node, index);
            }

            // leaf의 keycount가 T-1보다 적은 경우 삭제가능한 환경을 구축해야해서, DELETION을 만들어야한다.
            else
            {
                // 삭제 가능한 환경을 구축해야함.
                // ArrangeForDel 안에서 Final_Delete를 실행한다.
                Arrange_for_Delete(tree, tree->root, keyValue);
            }
        }

        //! key를 찾았는데, leaf노드가 아니고, 키 개수가 충분하지 않은 경우
        else if (!node->leaf && node->KeyCount < T)
        {
            Arrange_for_Delete(tree, tree->root, keyValue);
        }

        else
        {
            //! k`을 찾는다.
            int childIndex = Find_ChildIndex(node, keyValue);
            int keyPrime;
            if (node->childs[childIndex]->KeyCount >= T)
            {
                keyPrime = Find_KeyPrime_Presuccecor(node->childs[childIndex], keyValue);
                node->keys[index] = keyPrime;
                Arrange_for_Delete(tree, tree->root->childs[childIndex], keyValue);
            }
            else if(node->childs[childIndex+1]->KeyCount >= T)
            {
                keyPrime = Find_KeyPrime_succecor(node->childs[childIndex + 1], keyValue);
                node->keys[index] = keyPrime;
                Arrange_for_Delete(tree, tree->root->childs[childIndex + 1], keyValue);
            }

            // 자식 양쪽 다 t-1일 경우
            else
            {
                BTREENODE *child_node = Merge_Nodes(node, childIndex);
                if (node->KeyCount == 0)
                {
                    tree->root = node->childs[0];
                    free(node);
                    printf("root is changed\n");
                    Heap_Counting('-');
                }

                //? leaf 바로 위 node에서 leaf를 보는 경우가 없음 (B+Tree와 차이점)
                Arrange_for_Delete(tree, child_node, keyValue);
            }
        }
    }
}

// 후행 자식 노드의 가장 작은 값을 반환한다. (origin, )
int Find_KeyPrime_Presuccecor(BTREENODE *node, int keyValue)
{
    if (node->leaf)
    {
        int keyPrime = node->keys[node->KeyCount - 1];
        node->keys[node->KeyCount - 1] = keyValue;
        return keyPrime;
    }
    else
        Find_KeyPrime_Presuccecor(node->childs[node->KeyCount], keyValue);
}
int Find_KeyPrime_succecor(BTREENODE *node, int keyValue)
{
    if (node->leaf)
    {
        int keyPrime = node->keys[0];
        node->keys[0] = keyValue;
        return keyPrime;
    }
    else
        Find_KeyPrime_succecor(node->childs[0], keyValue);
}

int Get_Rand_Int()
{

    return rand();
}

int Find_Value(BTREENODE *node, int keyValue)
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

void Arrange_for_Delete(BTREE *tree, BTREENODE *node, int keyValue)
{

    int childIndex = Find_ChildIndex(node, keyValue);
    // 정렬이 되어 있는 상태. key가 해당 노드에 없을 때,
    // printf("node's leaf :: %d\n", node->leaf);
    // printf("node's first key :: %d\n", node->keys[0]);
    if (node->leaf)
    {
        Final_Delete(node, childIndex);
        return;
    }


 
    if (node->childs[childIndex]->KeyCount < T)
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
                BTREENODE *child_node = Merge_Nodes(node, childIndex);
                if (node->KeyCount == 0)
                {
                    tree->root = node->childs[0];
                    free(node);
                    printf("root is changed \n");
                    Heap_Counting('-');
                }
                Arrange_for_Delete(tree, child_node, keyValue);
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
                BTREENODE *child_node = Merge_Nodes(node, childIndex);
                if (node->KeyCount == 0)
                {
                    tree->root = node->childs[0];
                    free(node);
                    printf("root is changed \n");
                    Heap_Counting('-');
                }
                Arrange_for_Delete(tree, child_node, keyValue);
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
                BTREENODE *child_node = Merge_Nodes(node, childIndex);
                if (node->KeyCount == 0)
                {
                    tree->root = node->childs[0];
                    free(node);
                    printf("root is changed \n");
                    Heap_Counting('-');
                }
                Arrange_for_Delete(tree, child_node, keyValue);
            }
        }
    }
    else
    {
        printf("enough\n");
        Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
    }
}

// 삭제를 위한 검색 함수 (index는 삭제할 대상 key값의 위치이다.)
void Final_Delete(BTREENODE *node, int index)
{
    int keyValue = node->keys[index];
    for (int i = index; i < node->KeyCount - 1; ++i)
    {
        node->keys[i] = node->keys[i + 1];
    }
    node->KeyCount--;

    // ! ----------------------- 추후 확인 --------------------------//
    if (node->KeyCount == 0)
    {
        Leaf_Node_Pop(node);
    }
    printf("Delete [%d] is completed. :P\n", keyValue);

    return;
}

//! -------------------------------- 추후 확인 -------------------------//
int Find_ChildIndex(BTREENODE *node, int keyValue)
{

    int index = 0;
    for (index = 0; index < node->KeyCount; ++index)
    {
        if (keyValue <= node->keys[index])
        {
            return index;
        }
    }
    return index;
}

bool Swap_Keys_Left(BTREENODE *node, int childIndex)
{
    int tmp_keyindex = node->childs[childIndex - 1]->KeyCount;
    int tmp_key = node->childs[childIndex - 1]->keys[tmp_keyindex];
    node->childs[childIndex - 1]->keys[tmp_keyindex] = node->keys[childIndex];
    node->keys[childIndex] = tmp_key;
}
bool Swap_Keys_Right(BTREENODE *node, int childIndex)
{
    int tmp_key = node->childs[childIndex + 1]->keys[0];
    node->childs[childIndex + 1]->keys[0] = node->keys[childIndex];
    node->keys[childIndex] = tmp_key;
}

void Shift_to_Left(BTREENODE *node, int childIndex)
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
void Shift_to_Right(BTREENODE *node, int childIndex)
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

BTREENODE *Merge_Nodes(BTREENODE *node, int childIndex)
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

// Tree를 출력한다.
void Print_Tree(BTREENODE *node, int level)
{
    if(node->KeyCount == 0) {
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
            // Print_Tree(node->childs[i + 1], level + 1);
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

void Leaf_Node_Pop(BTREENODE *node)
{
    free(node);
    printf("leaf is poped\n");
    Heap_Counting('-');
    //printf("도비는 자유에요! \n");
}