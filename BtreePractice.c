#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <time.h>

#define DEBUG 1
#define M 3
#define T 2
typedef struct BtreeNode  {
    int KeyCount; // key�� ����
    bool leaf; // leaf���� ����
    int keys[2*T-1]; // node�� ������ �ִ� key ����
    struct BtreeNode* childs[M]; // ���� ���� ����Ǿ��ִ� child�� �迭�̴�.
} BTREENODE;


// tree ����ü ����
typedef struct Btree {
    struct BtreeNode* root; 
} BTREE;
// Btree��� ����ü�� node�����͸� ������. (�̸��� root)
// ��Ʈ�� ��Ʈ ����� �����͸� �������μ� ��Ʈ�� ��ü�� ������ �� �ֵ����Ѵ�.


BTREENODE* Allocate();
void Tree_Create(BTREE*);
void Insert_nonfull(BTREENODE*, int);
void Insert(BTREE*, int);
void Insert_Of_N(BTREE*, int);
void Delete(BTREENODE*, int);
bool Search(BTREENODE*, int);
void Split_Child(BTREENODE*, int);
int Get_Rand_Int();
void Traverse(BTREENODE*, int);

int main() {
    srand((unsigned int)time(NULL)); 
    BTREE tree;
    Tree_Create(&tree);
    Insert_Of_N(&tree, 10);
    if(DEBUG) Traverse(tree.root, 0);

    if (Search(tree.root, 15)){
        printf("\nYES");
    } else {
        printf("\nNONE");
    }
    
    return 0;
}

// ��带 �����ϱ� ���� �޸𸮸� �Ҵ��Ѵ�.
BTREENODE* Allocate() {

    // BTREENODE�� ũ�⸸ŭ �Ҵ��Ѵ�. (malloc�� �޸𸮸� �Ҵ��ϰ�, �Ҵ��� �ּҰ��� ��ȯ�Ѵ�.)
    BTREENODE* new_node = (BTREENODE*)malloc(sizeof(BTREENODE));
    // BTREENODE*������ new_node ���� = BTREENODE*�� ������ ��ȯ�� malloc�� �ּҰ� 
    
    // new_node(BTREENODE*)�� ��ȯ
    return new_node;
}


// Ʈ���� �����.
void Tree_Create(BTREE* tree) { // BTREE�� root* �� �ּҸ� ���� ����ü�̴�.
    BTREENODE* new_node = Allocate(); // �� ��带 ����� ���� �����Ϳ� �ּҸ� �Ҵ��Ѵ�.
    new_node->KeyCount = 0; // ���θ���� key�� �ȵ�������Ƿ� keycount�� 0���� �Ѵ�.
    new_node->leaf = true; // ���������� leaf�Ӽ��� on�Ѵ�.
    tree->root = new_node; // new_node�� ��Ʈ��尡 �ǹǷ� tree�� root�� new_node�� �Ѵ�. (�Ѵ� �������̴�.)
}


// key ������ 2T-1���� ���� node�� ���� �����Ѵ�.
void Insert_nonfull(BTREENODE* node, int KeyValue) {
    // KeyIndex�� node�� key ������ �Ҵ��Ѵ�.
    // ��, key[]�� ���� ������ key�� idx�� �Ҵ��Ѵ�.
    int KeyIndex = node->KeyCount;

    // ���� node�� leaf node���
    if ( node->leaf) {

        // node�� key ������ 1 �̻��̰� (�� ��尡 �ƴϰ�),
        // ���� key���� key[idx]���� �۾��������� idx�� ���δ�.
        // idx�� ���ο� key���� ���� �ڸ��� �ȴ�.
        while ( KeyIndex >= 1 && KeyValue < node->keys[KeyIndex-1]) {
            // ���ο� key���� ���� �ڸ��� �������ش�. (��ĭ�� ���������� �̵��Ѵ�.)
            node->keys[KeyIndex] = node->keys[KeyIndex-1];
            KeyIndex--;
        }

        // key���� ���� �ڸ��� ã�Ұ�, ������ �ش� �ڸ��� ������� ������
        // ������ ã�� key[idx]�� ���ο� key���� �Ҵ��Ѵ�.
        node->keys[KeyIndex] = KeyValue;
        // �׸��� ����� keycount�� 1 �÷��ش�.
        node->KeyCount += 1;
    }
    else {
        while (KeyIndex >= 1 && KeyValue < node->keys[KeyIndex-1]) {
            KeyIndex--;
        }
        //! KeyIndex++;
        if (node->childs[KeyIndex]->KeyCount == 2*T - 1){
            Split_Child(node, KeyIndex);
            if (KeyValue > node->keys[KeyIndex]){
                KeyIndex++;
            }
        }
        Insert_nonfull(node->childs[KeyIndex], KeyValue);
    }
    
}


// node�� key ������ 2T-1�� ���, node�� �и��Ѵ�. 
void Split_Child(BTREENODE* parentNode, int ChildIndex) {
    // right_node�� �и��ϸ鼭 ������� ���ο� node�̱� ������ �޸𸮸� ���� �Ҵ��Ѵ�.
    BTREENODE* right_node = Allocate();

    // 
    BTREENODE* left_node = parentNode->childs[ChildIndex];

    // right_node�� leaf �Ӽ��� left_node�� leaf�� �޾ƿ´�.
    right_node->leaf = left_node->leaf;

    // 2T-1�� �и��ϸ鼭 ��� ���� �θ�� �ø���
    // ������  2T-2���� key���� left�� right�� ���������. 
    right_node->KeyCount = T-1;
    left_node->KeyCount = T-1;

    // �׷��� left_node[i]
    for ( int i = 0; i < T-1; ++i ) {
        right_node->keys[i] = left_node->keys[T+i];
    }
    for ( int i = 0; i < T ; ++i ) {
        right_node->childs[i] = left_node->childs[T+i];
    }
    for ( int i = parentNode->KeyCount; i > ChildIndex ; --i ) {
        parentNode->childs[i+1] = parentNode->childs[i];
        parentNode->keys[i] = parentNode->keys[i-1];
    }
    parentNode->childs[ChildIndex+1] = right_node;
    parentNode->keys[ChildIndex] = left_node->keys[T-1];
    parentNode->KeyCount++;
}


// key���� tree�� �����Ѵ�.
void Insert(BTREE* tree, int keyValue){
    BTREENODE* rootNode = tree->root;
    if (rootNode->KeyCount == 2*T - 1){
        BTREENODE* newRootNode = Allocate();
        tree->root = newRootNode;
        newRootNode->leaf = false;
        newRootNode->KeyCount = 0;
        newRootNode->childs[0] = rootNode;
        Split_Child(newRootNode, 0);
        Insert_nonfull(newRootNode, keyValue);
    }
    else {
        Insert_nonfull(rootNode, keyValue);
    }

}

void Insert_Of_N(BTREE* tree, int n) {
    printf("Inserted randNum :: ");
    for(int index = 0; index < n; ++index) {
        int item = Get_Rand_Int() % 101;
        Insert(tree, item);
        if(DEBUG) printf("%d ", item);
    }
}


bool Search(BTREENODE* node, int keyValue){
    int index = 0;
    while ( index < node->KeyCount && keyValue > node->keys[index]){
        index++;
    }
    if ( index <= node->KeyCount && keyValue == node->keys[index] ){
        return true;
    }
    else if ( node->leaf ){
        return false;
    }
    else {
        return Search(node->childs[index], keyValue);
    }
}

int Get_Rand_Int() {
    
    return rand();
}

void Traverse(BTREENODE * p, int level)
{
    printf("\n");
    int i;
    for(i = 0; i < p->KeyCount; i++)
    {
        if(p->leaf == false)
        {
            Traverse(p->childs[i], level + 1);
        }
        printf(" %d", p->keys[i]);
    }
    if(p->leaf == false)
    {
        Traverse(p->childs[i], level + 1);
    }
    printf("\n");
}