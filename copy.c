#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// �ִ� �ڽĳ�� ����
#define T 2

//! Node ����ü ����
typedef struct BNode
{
    int KeyCount;                // key�� ����
    bool leaf;                   // leaf���� ����
    bool root;                   // root���� ����
    int keys[2 * T - 1];         // node�� ������ �ִ� key ����
    struct BNode *childs[2 * T]; // ���� ���� ����Ǿ��ִ� child�� �迭�̴�.

    struct BNode *parents;  // �θ��� ������
    struct BNode *prevNode; // leaf ������ ������
    struct BNode *nextNode; // leaf ������ ������
} BNODE;


//! tree ����ü ����
typedef struct BPlusTree
{
    struct BNode *root;
} BPLUSTREE;

//! ------------------ �Լ� ����� --------------------//
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


// Console
void Console_Main(BPLUSTREE *, int);
void Console_Print();
int Console_Input();
void Clear_Tree(BPLUSTREE* );
void Node_Clear(BNODE*);






//! --------------------------------------------------//


//! ------------------ GLOBAL var --------------------//
unsigned int HEAPCOUNT = 0;
//! --------------------------------------------------//


//! ------------------- MAIN �Լ� --------------------//
int main()
{
srand((unsigned int)time(NULL));
    BPLUSTREE tree, prevTree;
    Tree_Create(&tree);
    
    int inputNumber;
    
    while (inputNumber != 7)
    {   
        system("cls");
        if(tree.root == NULL) Tree_Create(&tree);
        Console_Print();
        int inputNumber = Console_Input();
        Console_Main(&tree, inputNumber);
    }


    printf("\n\n================================= END ===============================\n\n");
    // Heap_Counting('*');
    return 0;
}

//! ���ο� ��� ���� �Լ�
BNODE *Allocate()
{

    // BNODE ũ�⸸ŭ �Ҵ��Ѵ�. (malloc�� �޸𸮸� �Ҵ��ϰ�, �Ҵ��� �ּҰ��� ��ȯ�Ѵ�.)
    BNODE *new_node = (BNODE *)malloc(sizeof(BNODE));
    // BNODE ������ new_node ���� = BNODE �� ������ ��ȯ�� malloc�� �ּҰ�
    Heap_Counting('+');
    new_node->prevNode = NULL;
    new_node->nextNode = NULL;

    // new_node(BNODE)�� ��ȯ
    return new_node;
}

void Heap_Counting(char operand)
{
    if (operand == '+')
    {
        HEAPCOUNT++;
    }
    else if (operand == '-')
    {
        HEAPCOUNT--;
    }
}

void Tree_Create(BPLUSTREE *tree)
{                                 // BPLUSTREE�� root* �� �ּҸ� ���� ����ü�̴�.
    BNODE *new_node = Allocate(); // �� ��带 ����� ���� �����Ϳ� �ּҸ� �Ҵ��Ѵ�.
    new_node->KeyCount = 0;       // ���θ���� key�� �ȵ�������Ƿ� keycount�� 0���� �Ѵ�.
    new_node->leaf = true;        // ���������� leaf�Ӽ��� on�Ѵ�.
    tree->root = new_node;        // new_node�� ��Ʈ��尡 �ǹǷ� tree�� root�� new_node�� �Ѵ�. (�Ѵ� �������̴�.)
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
    
}

void Split_Child(BNODE *parentNode, int ChildIndex)
{
    // right_node�� �и��ϸ鼭 ������� ���ο� node�̱� ������ �޸𸮸� ���� �Ҵ��Ѵ�.
    BNODE *right_node = Allocate();
    BNODE *left_node = parentNode->childs[ChildIndex];

    // right_node�� leaf �Ӽ��� left_node�� leaf�� �޾ƿ´�.
    right_node->leaf = left_node->leaf;

    //!---- leaf Node�� �и��ϴ� ��� ----//
    if (left_node->leaf)
    {
        // ������ �ڽĿ��� T�� ��ŭ �ű��. (�θ��� Ű�� �����ؾ� �ϹǷ�)
        right_node->KeyCount = T;
        left_node->KeyCount = T - 1;

        // left_node�� Ű���� right_node�� �̵�
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
        //! B+Ʈ���� �θ� key = ������ �ڽ��� ù��° key �̴�.
        parentNode->keys[ChildIndex] = right_node->keys[0];
        parentNode->KeyCount++;
    }

    else
    {
        // 2T-1�� �и��ϸ鼭 ��� ���� �θ�� �ø���
        // ������  2T-2���� key���� left�� right�� ���������.
        right_node->KeyCount = T - 1;
        left_node->KeyCount = T - 1;

        // left�� Ű�� �ڽ��� right�� �ű��.
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

    // �θ�, �ڽ� �籸��
    left_node->parents = parentNode;
    right_node->parents = parentNode;
    left_node->nextNode = right_node;
    right_node->prevNode = left_node;
}

void Insert_nonfull(BNODE *node, int KeyValue)
{
    // KeyIndex�� node�� key ������ �Ҵ��Ѵ�.
    // ��, key[]�� ���� ������ key�� idx�� �Ҵ��Ѵ�.
    int KeyIndex = node->KeyCount;

    // ���� node�� leaf node���
    if (node->leaf)
    {

        // node�� key ������ 1 �̻��̰� (�� ��尡 �ƴϰ�),
        // ���� key���� key[idx]���� �۾��������� idx�� ���δ�.
        // idx�� ���ο� key���� ���� �ڸ��� �ȴ�.
        while (KeyIndex >= 1 && KeyValue < node->keys[KeyIndex - 1])
        {
            // ���ο� key���� ���� �ڸ��� �������ش�. (��ĭ�� ���������� �̵��Ѵ�.)
            node->keys[KeyIndex] = node->keys[KeyIndex - 1];
            KeyIndex--;
        }

        // key���� ���� �ڸ��� ã�Ұ�, ������ �ش� �ڸ��� ������� ������
        // ������ ã�� key[idx]�� ���ο� key���� �Ҵ��Ѵ�.
        node->keys[KeyIndex] = KeyValue;
        // �׸��� ����� keycount�� 1 �÷��ش�.
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
    // leaf�� node�� �ƴϸ� DFS ������
    else
    {
        if (!node->leaf)
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
    }
    return;
}
//////////����////////////////////
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
            // ���� �ڽ��� Ű�� ���� ���
            if (node->childs[childIndex - 1]->KeyCount >= T)
            {
                Swap_Keys_Left(node, childIndex);
                Shift_to_Right(node, childIndex);
                Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
            }
            // ������ �ڽ��� Ű�� ���� ���
            else if (node->childs[childIndex + 1]->KeyCount >= T)
            {
                Swap_Keys_Right(node, childIndex);
                Shift_to_Left(node, childIndex);
                Arrange_for_Delete(tree, node->childs[childIndex], keyValue);
            }

            // �� �� Ű�� ������� ���� ���
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

            // ������ �ڽ��� Ű�� ���� ���
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
            // ������ �ڽ��� Ű�� ���� ���
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
    if (!node->root)
    {
        free(node);
        Heap_Counting('-');
    }
    else
    {
        return;
    }    
}

BNODE *Merge_Nodes(BNODE *node, int childIndex)
{ //! ����� ������ ���� �ڽĳ���� Ű�� T-1���� �˰� �ִ�...

    //! childIndex�� node�� ������ �ڽ��� �ƴ� ���
    if (childIndex < node->KeyCount)
    {
        free(node->childs[childIndex + 1]);
        Heap_Counting('-');
        // �θ� ����� childIndex key�� ���� �ڽ� ����� ������ �̵�
        node->childs[childIndex]->keys[T - 1] = node->keys[childIndex];
        // �θ� ����� ���� �ڽ� ����� key���� ���� �ڽ� ���� �̵�
        for (int i = 0; i < T - 1; ++i)
        {
            node->childs[childIndex]->keys[i + T] = node->childs[childIndex + 1]->keys[i];
        }
        // �θ� ����� ���� �ڽ� ����� child���� ���� �ڽ� ���� �̵�
        // childs leaf �� �ƴ� ���� ����,
        for (int i = 0; i < T; ++i)
        {
            node->childs[childIndex]->childs[i + T] = node->childs[childIndex + 1]->childs[i];
        }
        // �θ� ����� ���� �ڽ� ����� keyCount ����
        node->childs[childIndex]->KeyCount = 2 * T - 1;
        // �θ� ����� key ���� �� child ����
        for (int i = childIndex; i < node->KeyCount - 1; ++i)
        {
            node->keys[i] = node->keys[i + 1];
        }
        for (int i = childIndex + 1; i < node->KeyCount; ++i)
        {
            node->childs[i] = node->childs[i + 1];
        }
        //! �θ� ����� key count ����
        node->KeyCount--;
        return node->childs[childIndex];
    }
    //! childIndex�� node�� ������ �ڽ��� ���
    else
    {
        free(node->childs[childIndex]);
        Heap_Counting('-');
        // �θ� ����� childIndex key�� ���� �ڽ� ����� ó������ �̵�
        node->childs[childIndex - 1]->keys[T - 1] = node->keys[childIndex];
        // �θ� ����� ���� �ڽ� ����� key���� ���� �ڽ� ���� �̵�
        for (int i = 0; i < T - 1; ++i)
        {
            node->childs[childIndex - 1]->keys[i + T] = node->childs[childIndex]->keys[i];
        }
        // �θ� ����� ���� �ڽ� ����� child���� ���� �ڽ� ���� �̵�
        for (int i = 0; i < T; ++i)
        {
            node->childs[childIndex - 1]->childs[i + T] = node->childs[childIndex]->childs[i];
        }
        // �θ� ����� ���� �ڽ� ����� keyCount ����
        node->childs[childIndex - 1]->KeyCount = 2 * T - 1;

        //! �θ����� key count �����ؾ���.
        //! ����� key count --;
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
    // ���� �ڽ� ����� ù��° key�� ��ǥ �ڽ� ����� ���������� �̵�
    node->childs[childIndex]->keys[target_position] = node->childs[childIndex + 1]->keys[0];
    // ���� �ڽ� ����� ù��° �����͸� ��ǥ �ڽ� ����� ���������� �̵�
    node->childs[childIndex]->childs[target_position + 1] = node->childs[childIndex + 1]->childs[0];

    // ���� �ڽ� ����� key���� �������� ��ĭ�� �̵�
    int target_position_2 = node->childs[childIndex + 1]->KeyCount;
    int i = 0;
    while (i < target_position_2 - 1)
    {
        node->childs[childIndex + 1]->keys[i] = node->childs[childIndex + 1]->keys[i + 1];
        ++i;
    }
    // ���� �ڽ� ����� �ڽĵ��� �������� ��ĭ�� �̵�
    i = 0;
    while (i < target_position_2)
    {
        node->childs[childIndex + 1]->childs[i] = node->childs[childIndex + 1]->childs[i + 1];
        ++i;
    }
    // keyCount ����
    node->childs[childIndex]->KeyCount++;
    node->childs[childIndex + 1]->KeyCount--;
}
void Shift_to_Right(BNODE *node, int childIndex)
{
    int target_position = node->childs[childIndex]->KeyCount;
    // ���� �ڽ� ����� key���� ���������� ��ĭ�� �̵�
    int i = target_position;
    while (i > 0)
    {
        node->childs[childIndex]->keys[i] = node->childs[childIndex]->keys[i - 1];
        --i;
    }
    // ���� �ڽ� ����� �ڽĵ��� ���������� ��ĭ�� �̵�
    i = target_position + 1;
    while (i > 0)
    {
        node->childs[childIndex]->childs[i] = node->childs[childIndex]->childs[i - 1];
        --i;
    }
    int target_position_2 = node->childs[childIndex - 1]->KeyCount;
    // ���� �ڽ� ����� ������ key�� ��ǥ �ڽ� ����� ù��°�� �̵�
    node->childs[childIndex]->keys[0] = node->childs[childIndex - 1]->keys[target_position_2 - 1];
    // ���� �ڽ� ����� ������ �����͸� ��ǥ �ڽ� ����� ù��°�� �̵�
    node->childs[childIndex]->childs[0] = node->childs[childIndex - 1]->childs[target_position_2];
    // keyCount ����
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





void Console_Main(BPLUSTREE *tree, int inputNumber)
{
    // 0. reprint the menu
    if (inputNumber == 0)
    {
        Console_Print();
    }

    // 1. Print the Tree
    else if (inputNumber == 1)
    {

        Print_Tree(tree->root, 0);
        getchar();
        printf("\nPlease insert anykey to continue");
        getchar();
    }

    // 2. Insert a Number
    else if (inputNumber == 2)
    {
        printf("\n input a NUMBER to insert : ");
        int insertNumber;
        scanf("%d", &insertNumber);
        Insert(tree, insertNumber);
        getchar();
        printf("\n Inserting [%d] is completed. ", insertNumber);
        printf("\nPlease insert anykey to continue");
        getchar();
    }

    // 3. Delete a Number
    else if (inputNumber == 3)
    {
        printf("\n input a NUMBER to delete : ");
        int insertNumber;
        scanf("%d", &insertNumber);
        Deletion(tree, insertNumber);
        getchar();
        printf("\n Deleting [%d] is completed. ", insertNumber);
        printf("\nPlease insert anykey to continue");
        getchar();
    }

    // 4. Search a Number
    else if (inputNumber == 4)
    {
        printf("\n input a NUMBER to search : ");
        int searchNumber;
        scanf("%d", &searchNumber);
        getchar();
        if (Search(tree->root, searchNumber))
        {
            printf("\n [%d] is in the tree. ");
        }
        else
        {
            printf("\n [%d] is NOT in the tree. ");
        } 
        printf("\nPlease insert anykey to continue");
        getchar();
    }

    // 5. Clear the Tree
    else if (inputNumber == 5)
    {
        Clear_Tree(tree);
        getchar();
        printf("\n Clearing the tree is completed. ");
        printf("\nPlease insert anykey to continue");
        getchar();
        
    }
    

    else if (inputNumber == 6)
    {
        getchar();
        printf("\nUnder Construction");
        printf("\nPlease insert anykey to continue");
        getchar();
    }

    else if (inputNumber == 7)
    {
        getchar();
        printf("\nThanks for using our service! See U Next Time! ^^/");
        printf("\nDesigned by TrueSunDragon");
        printf("\nPlease insert anykey to continue");
        getchar();
        exit(0);
    }

    else
    {
        printf("Input wrong number. Please select a number in menu");
    }

    return;
}

    

void Console_Print()
{
    printf("\n������������������������������������������������������������������������������������������������������������������������������");
    printf("\n��             Welcome to the world of B-Tree!!                ��");
    printf("\n������������������������������������������������������������������������������������������������������������������������������");
    printf("\n��                   --- Select a menu ---                     ��");
    printf("\n��   1. Print                                                  ��");
    printf("\n��   2. Insert                                                 ��");
    printf("\n��   3. Delete                                                 ��");
    printf("\n��   4. Search                                                 ��");
    printf("\n��   5. Clear                                                  ��");
    printf("\n��   6. Help                                                   ��");
    printf("\n��   7. Quit                                                   ��");
    printf("\n������������������������������������������������������������������������������������������������������������������������������");
    printf("\n��                                   Designed by TrueSunDragon ��");
    printf("\n��                       copyright(c) 2021 All rights reserved ��");
    printf("\n������������������������������������������������������������������������������������������������������������������������������");

}

int Console_Input()
{
    printf("\n Please Select a menu ( 0 : reprint menu) : ");
    int inputNumber;
    scanf("%d", &inputNumber);
    return inputNumber;
}


void Clear_Tree(BPLUSTREE* tree)
{
    Node_Clear(tree->root);
    tree->root = NULL;
}
void Node_Clear(BNODE* node) {
    if(node->leaf) {
        free(node);
        return;
    }
    for(int i = 0; i<= node->KeyCount; ++i) {
        Node_Clear(node->childs[i]);
    }
}
