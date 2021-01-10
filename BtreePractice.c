#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>


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
void TreeCreate(BTREE*);
void Insert_nonfull(BTREENODE*, int);
// Insert()
// Delete()
void Split_Child(BTREENODE*, int);

int main() {
    BTREE tree;
    TreeCreate(&tree);
    Insert_nonfull(tree.root, 10);
    Insert_nonfull(tree.root, 20);
    Insert_nonfull(tree.root, 30);
    Split_Child(tree.root, 1);
    for ( int i = 0; i < tree.root->KeyCount; ++i) {
        printf("keys: %d\n", tree.root->keys[i]);
        printf("keysaddress: %d\n", &(tree.root->keys[i]));
    }
    printf("child: %d\n", tree.root->childs);
    printf("child: %d\n", tree.root->childs[1]);
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
void TreeCreate(BTREE* tree) { // BTREE�� root* �� �ּҸ� ���� ����ü�̴�.
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
            KeyIndex -= 1;
        }

        // key���� ���� �ڸ��� ã�Ұ�, ������ �ش� �ڸ��� ������� ������
        // ������ ã�� key[idx]�� ���ο� key���� �Ҵ��Ѵ�.
        node->keys[KeyIndex] = KeyValue;
        // �׸��� ����� keycount�� 1 �÷��ش�.
        node->KeyCount += 1;
    }
}


// node�� key ������ 2T-1�� ���, node�� �и��Ѵ�. 
void Split_Child(BTREENODE* node, int ChildIndex) {
    // right_node�� �и��ϸ鼭 ������� ���ο� node�̱� ������ �޸𸮸� ���� �Ҵ��Ѵ�.
    BTREENODE* right_node = Allocate();

    // 
    BTREENODE* left_node = node->childs[ChildIndex];

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
    for ( int i = node->KeyCount; i > ChildIndex ; --i ) {
        node->childs[i+1] = node->childs[i];
        node->keys[i] = node->keys[i-1];
    }
    node->childs[ChildIndex+1] = right_node;
    node->keys[ChildIndex] = left_node->keys[T-1];
    node->KeyCount++;
}

