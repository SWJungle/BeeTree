#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


#define M 3
#define T 2

typedef struct BtreeNode  {
    int KeyCount; // 
    bool leaf; // leaf���� ����
    int keys[2*T-1]; // node�� ������ �ִ� key ����
    struct BtreeNode* childs[M]; // 
} BTREENODE;


// tree ����ü ����
typedef struct Btree {
    struct BtreeNode* root;
} BTREE;

BTREENODE* Allocate();
void TreeCreate(BTREE*);
// Insert()
// Delete()
// Split_Child
// Insert_nonfull()

int main() {
    BTREE tree;
    TreeCreate(&tree);
    printf("%d", tree.root->KeyCount);
    return 0;
}

// �� ��忡 �޸𸮸� �Ҵ��Ѵ�.
BTREENODE* Allocate() {
    BTREENODE* new_node = (BTREENODE*)malloc(sizeof(BTREENODE));
    return new_node;
}

void TreeCreate(BTREE* tree) {
    BTREENODE* new_node = Allocate();
    new_node->KeyCount = 0;
    new_node->leaf = true;
    tree->root = new_node;
}






