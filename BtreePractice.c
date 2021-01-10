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
void Insert(BTREE*, int);
void Delete(BTREENODE*, int);
bool Search(BTREENODE*, int);
void Split_Child(BTREENODE*, int);
int Get_Ran_Int();

int main() {
    BTREE tree;
    TreeCreate(&tree);
    Insert(&tree, 10);
    Insert(&tree, 20);
    Insert(&tree, 30);
    Insert(&tree, 40);
    Insert(&tree, 25);
    Insert(&tree, 15);
    Insert(&tree, 50);
    // Split_Child(tree.root, 0);
    // for ( int i = 0; i < tree.root->KeyCount; ++i) {
    //     printf("keys: %d\n", tree.root->keys[i]);
    //     printf("keysaddress: %d\n", &(tree.root->keys[i]));
    // }
    // printf("root keycount: %d\n", tree.root->KeyCount);
    // printf("root key 0: %d\n", tree.root->keys[0]);
    // printf("root key 1: %d\n", tree.root->keys[1]);
    // printf("root key 2: %d\n", tree.root->keys[2]);
    // printf("child key 0-0: %d\n", tree.root->childs[0]->keys[0]);
    // printf("child key 1-0: %d\n", tree.root->childs[1]->keys[0]);
    // printf("child key 0-1: %d\n", tree.root->childs[0]->keys[1]);
    // printf("child key 1-1: %d\n", tree.root->childs[1]->keys[1]);
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
    printf("rootNode keycount : %d\n", rootNode->KeyCount);
    if (rootNode->KeyCount == 2*T - 1){
        printf("0");
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