#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>
/*
    1) search == True >> swap �ϰ� delete
    2) search == False >> �ݺ�[Find_ChildIndex(sub Ʈ���� �����ؾ� �Ѵ�.) > search] �ϰ� swap ��� leaf���� ã�� �� Ű�� swap�Ѵ�.
*/
#define DEBUG 1
#define M 3
#define T 2
typedef struct BtreeNode
{
    int KeyCount;                // key�� ����
    bool leaf;                   // leaf���� ����
    int keys[2 * T - 1];         // node�� ������ �ִ� key ����
    struct BtreeNode *childs[M]; // ���� ���� ����Ǿ��ִ� child�� �迭�̴�.
} BTREENODE;

// tree ����ü ����
typedef struct Btree
{
    struct BtreeNode *root;
} BTREE;
// Btree��� ����ü�� node�����͸� ������. (�̸��� root)
// ��Ʈ�� ��Ʈ ����� �����͸� �������μ� ��Ʈ�� ��ü�� ������ �� �ֵ����Ѵ�.

BTREENODE *Allocate();
void Tree_Create(BTREE *);
void Insert_nonfull(BTREENODE *, int);
void Insert(BTREE *, int);
void Insert_Of_N(BTREE *, int);
void ArrangeForDel(BTREENODE *, int);
bool Search(BTREENODE *, int);
void SearchForDel(BTREE *, BTREENODE *, int);
void Split_Child(BTREENODE *, int);
int Get_Rand_Int();
int Find_ChildIndex(BTREENODE *, int);
bool Swap_Keys(BTREENODE *, int);
BTREENODE *Merge_Nodes(BTREENODE *, int);
int Find_KeyPrime(BTREENODE *, int);
void Print_Tree(BTREENODE *, int);
void Final_Delete(BTREENODE *, int);
void Deletion(BTREE *, int);
int Find_Value(BTREENODE *, int);
void Shift_Left(BTREENODE *, int);
void Shift_Right(BTREENODE *, int);


int main()
{
    srand((unsigned int)time(NULL));
    BTREE tree;
    Tree_Create(&tree);
    Insert(&tree, 10);
    Insert(&tree, 20);
    Insert(&tree, 30);
    Insert(&tree, 40);
    Insert(&tree, 50);
    Insert(&tree, 60);
    Insert(&tree, 70);
    Insert(&tree, 80);
    Insert(&tree, 90);
    Insert(&tree, 100);
    Print_Tree(tree.root, 0);
    Deletion(&tree, 40);
    
    return 0;
}

// ��带 �����ϱ� ���� �޸𸮸� �Ҵ��Ѵ�.
BTREENODE *Allocate()
{

    // BTREENODE�� ũ�⸸ŭ �Ҵ��Ѵ�. (malloc�� �޸𸮸� �Ҵ��ϰ�, �Ҵ��� �ּҰ��� ��ȯ�Ѵ�.)
    BTREENODE *new_node = (BTREENODE *)malloc(sizeof(BTREENODE));
    // BTREENODE*������ new_node ���� = BTREENODE*�� ������ ��ȯ�� malloc�� �ּҰ�

    // new_node(BTREENODE*)�� ��ȯ
    return new_node;
}

// Ʈ���� �����.
void Tree_Create(BTREE *tree)
{                                     // BTREE�� root* �� �ּҸ� ���� ����ü�̴�.
    BTREENODE *new_node = Allocate(); // �� ��带 ����� ���� �����Ϳ� �ּҸ� �Ҵ��Ѵ�.
    new_node->KeyCount = 0;           // ���θ���� key�� �ȵ�������Ƿ� keycount�� 0���� �Ѵ�.
    new_node->leaf = true;            // ���������� leaf�Ӽ��� on�Ѵ�.
    tree->root = new_node;            // new_node�� ��Ʈ��尡 �ǹǷ� tree�� root�� new_node�� �Ѵ�. (�Ѵ� �������̴�.)
}

// key ������ 2T-1���� ���� node�� ���� �����Ѵ�.
void Insert_nonfull(BTREENODE *node, int KeyValue)
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

// node�� key ������ 2T-1�� ���, node�� �и��Ѵ�.
void Split_Child(BTREENODE *parentNode, int ChildIndex)
{
    // right_node�� �и��ϸ鼭 ������� ���ο� node�̱� ������ �޸𸮸� ���� �Ҵ��Ѵ�.
    BTREENODE *right_node = Allocate();

    //
    BTREENODE *left_node = parentNode->childs[ChildIndex];

    // right_node�� leaf �Ӽ��� left_node�� leaf�� �޾ƿ´�.
    right_node->leaf = left_node->leaf;

    // 2T-1�� �и��ϸ鼭 ��� ���� �θ�� �ø���
    // ������  2T-2���� key���� left�� right�� ���������.
    right_node->KeyCount = T - 1;
    left_node->KeyCount = T - 1;

    // �׷��� left_node[i]
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

// key���� tree�� �����Ѵ�.
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
}

void Insert_Of_N(BTREE *tree, int n)
{
    printf("Inserted randNum :: ");
    for (int index = 0; index < n; ++index)
    {
        int item = Get_Rand_Int() % 101;
        Insert(tree, item);
        if (DEBUG)
            printf("%d ", item);
    }
}


void Deletion(BTREE * tree, int keyValue)
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
    // �ش� key�� ���� node�� �ִ°�?
    int index = Find_Value(node, keyValue);
    if ( index == -1)
    {
        SearchForDel(tree, node->childs[Find_ChildIndex(node, keyValue)], keyValue);
    }
    else
    {
        // ������ �����ΰ�? �����ΰ�?
        if (node->leaf)
        {
            // �����̸鼭 leaf�� keycount �� T-1 �̻��� ��� �ٷ� �����Ѵ�.
            if (node->KeyCount > T - 1)
            {
                Final_Delete(node, node->KeyCount);
            }

            // leaf�� keycount�� T-1���� ���� ��� ���������� ȯ���� �����ؾ��ؼ�, DELETION�� �������Ѵ�.
            else
            {
                // ���� ������ ȯ���� �����ؾ���.
                // ArrangeForDel �ȿ��� Final_Delete�� �����Ѵ�.
                ArrangeForDel(tree->root, keyValue);
            }
        }
        else
        {
            // k`�� ã�Ƽ� swap
            int childIndex = Find_ChildIndex(node, keyValue);
            int keyPrime = Find_KeyPrime(node->childs[childIndex], keyValue);
            node->keys[index] = keyPrime;
            ArrangeForDel(tree->root, keyValue);
        }
        return;
    }
}

// ���� �ڽ� ����� ���� ���� ���� ��ȯ�Ѵ�. (origin, )
int Find_KeyPrime(BTREENODE *node, int keyValue)
{
    if (node->leaf)
    {
        int keyPrime = node->keys[node->KeyCount - 1];
        node->keys[node->KeyCount - 1] = keyValue;
        return keyPrime;
    }
    else
        Find_KeyPrime(node->childs[node->KeyCount], keyValue);
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

void ArrangeForDel(BTREENODE *node, int keyValue)
{
    int childIndex = Find_ChildIndex(node, keyValue);

    // ������ �Ǿ� �ִ� ����. key�� �ش� ��忡 ���� ��,
    if (node->leaf)
    {
        Final_Delete(node, childIndex);
    }

    if (node->childs[childIndex]->KeyCount < T)
    {
        if (node->childs[childIndex - 1]->KeyCount < T && node->childs[childIndex + 1]->KeyCount < T)
        {
            BTREENODE *child_node = Merge_Nodes(node, childIndex);
            ArrangeForDel(child_node, keyValue);
        }
        else
        {
            // LEFT->RIGHT
            if (!Swap_Keys(node, childIndex))
            {
                Shift_Left(node, childIndex);
            }
            // RIGHT
            else
            {
                Shift_Right(node, childIndex);
            }
            ArrangeForDel(node->childs[childIndex], keyValue);
        }
    }
    else
    {
        ArrangeForDel(node->childs[childIndex], keyValue);
    }
}

// ������ ���� �˻� �Լ� (index�� ������ ��� key���� ��ġ�̴�.)
void Final_Delete(BTREENODE *node, int index)
{
    for (int i = index; i < node->KeyCount - 1; ++i)
    {
        node->keys[i] = node->keys[i + 1];
    }
    node->KeyCount--;
    printf("Delete [%d] is completed. :P", node->keys[index]);

    return;
}

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

bool Swap_Keys(BTREENODE *node, int childIndex)
{

    if (childIndex < node->KeyCount && node->childs[childIndex + 1]->KeyCount >= T)
    {
        int tmp_key = node->childs[childIndex + 1]->keys[0];
        node->childs[childIndex + 1]->keys[0] = node->keys[childIndex];
        node->keys[childIndex] = tmp_key;
        return false;
    }

    else
    {
        int tmp_keyindex = node->childs[childIndex - 1]->KeyCount;
        int tmp_key = node->childs[childIndex - 1]->keys[tmp_keyindex];
        node->childs[childIndex - 1]->keys[tmp_keyindex] = node->keys[childIndex];
        node->keys[childIndex] = tmp_key;
        return true;
    }
}

void Shift_Left(BTREENODE *node, int childIndex)
{
    int target_position = node->childs[childIndex]->KeyCount;
    // ���� �ڽ� ����� ù��° key�� ��ǥ �ڽ� ����� ���������� �̵�
    node->childs[childIndex]->keys[target_position] = node->childs[childIndex + 1]->keys[0];
    // ���� �ڽ� ����� ù��° �����͸� ��ǥ �ڽ� ����� ���������� �̵�
    node->childs[childIndex]->childs[target_position + 1] = node->childs[childIndex + 1]->childs[0];
    // ���� �ڽ� ����� key���� �������� ��ĭ�� �̵�
    int i = 0;
    while (i < target_position - 1)
    {
        node->childs[childIndex + 1]->keys[i] = node->childs[childIndex + 1]->keys[i + 1];
        ++i;
    }
    // ���� �ڽ� ����� �ڽĵ��� �������� ��ĭ�� �̵�
    i = 0;
    while (i < target_position)
    {
        node->childs[childIndex + 1]->childs[i] = node->childs[childIndex + 1]->childs[i + 1];
        ++i;
    }
    // keyCount ����
    node->childs[childIndex]->KeyCount++;
    node->childs[childIndex + 1]->KeyCount--;
}
void Shift_Right(BTREENODE *node, int childIndex)
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

BTREENODE *Merge_Nodes(BTREENODE *node, int childIndex)
{ //! ����� ������ ���� �ڽĳ���� Ű�� T-1���� �˰� �ִ�...

    //! childIndex�� node�� ������ �ڽ��� �ƴ� ���
    if (childIndex < node->KeyCount)
    {
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
        for (int i = childIndex; i < node->KeyCount; ++i)
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
        // �θ� ����� key ���� �� child ����
        for (int i = childIndex; i < node->KeyCount - 1; ++i)
        {
            node->keys[i] = node->keys[i + 1];
        }
        for (int i = childIndex; i < node->KeyCount; ++i)
        {
            node->childs[i] = node->childs[i + 1];
        }
        //! �θ����� key count �����ؾ���.
        //! ����� key count --;
        node->KeyCount--;
        return node->childs[childIndex - 1];
    }
}

// Tree�� ����Ѵ�.
void Print_Tree(BTREENODE *node, int level)
{
    // leaf�� node�� �ƴϸ� DFS ������
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