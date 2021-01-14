#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#define T 2
#define FULL 2 * T - 1

typedef struct DataAddress {
    int SHA;
}DATA;

typedef struct Node {
    struct Node * next;
    struct Node * prev;
    struct Node * childs[2 * T];
    DATA dPointer[2 * T - 1];
    int keys[2 * T - 1];
    int keyCount;
    bool isLeaf, isRoot;
}NODE;

typedef struct Tree {
    NODE * root;
}TREE;

void init(TREE **); // 트리를 초기화한다.
NODE *Allocate(); // 새로운 노드를 메모리에 할당한다.
bool search(NODE *, int); // 노드를 루트로 하는 서브트리에 key가 존재하는가
void insertion(TREE **, int, int); // 트리에 새로운 key를 추가한다.
void insertion_nonfull(NODE *, int, int); // 노드가 가득차지 않았을 때, key를 추가한다.
void split_node(NODE *, int); // 노드를 부모로 idx 자식 노드를 분리해 내용을 부모, idx + 1 자식 노드로 분리한다.
void deletion(TREE **, int); // 트리에 해당 key가 존재하면 삭제한다.
void deletion_enable(TREE **, NODE *, int);
void deletion_key_onto_Leaf(NODE *, int);
int find_subTree_index(NODE *, int);
void arrange_tree(TREE **, NODE *, int, int);
void shift_overwrite_node(TREE **, NODE *, int) ;
void print_for_exam(NODE*);
void swap_key_subtree_min(NODE *, int);
void swap_key_subtree_max(NODE *, int);
void shift_nextNode_firstItem_to_node(NODE *, int);
void shift_prevNode_lastItem_to_node(NODE *, int); 
void has_to_merge(TREE **, NODE *, int, int);
NODE *merge_node(NODE *, int); // 노드의 키를 병합하고 그 노드를 반환한다.
void traverse(NODE *); // 트리의 루트에서 모든 트리를 순회하여 출력

void main() {
    TREE *tree = (TREE*)malloc(sizeof(TREE));
    init(&tree);
    int keyValue, data;
    char *command = malloc(4 * sizeof(char));
    while(true) {
        system("cls");
        if(tree->root == NULL) init(&tree);
        print_for_exam(tree->root);
        printf("\ncommand _ tst(testcase), ins(insertion), del(deletion), sch(search)\nex)  [tst 1], [ins 10], [del 10], [sch 10]\n>> ");
        scanf("%s %d", command, &keyValue);
        if (!strcmp(command,"ins")) {
            data = keyValue * 1000;
            insertion(&tree, keyValue, data);
            print_for_exam(tree->root);
        }
        else if(!strcmp(command,"del")){
            deletion(&tree, keyValue);
            print_for_exam(tree->root);
        }
        else if(!strcmp(command,"sch")){
            if(search(tree->root, keyValue)){
                printf("keyValue [%d] exist.\n", keyValue);
                getchar(); getchar();
            }
        }
        else {
            printf("wrong command OR something wrong\n");
            return;
        }
    }
    return;
}

void init(TREE **tree) {
    NODE *new_node = Allocate();
    new_node->isLeaf = true;
    new_node->isRoot = true;
    (*tree)->root = new_node;
}
NODE *Allocate() {
    NODE *new_node = (NODE *)malloc(sizeof(NODE));
    new_node->next = NULL;
    new_node->isLeaf = false;
    new_node->isRoot = false;
    new_node->keyCount = 0;
    return new_node;
}
bool search(NODE *subTree_node, int key) {
    int keyIdx = 0;
    for(; keyIdx < subTree_node->keyCount; ++keyIdx) {
        if(subTree_node->keys[keyIdx] == key) {
            return true;
        }
        else if(subTree_node->keys[keyIdx] > key) {
            return search(subTree_node->childs[keyIdx], key);
        }
    }
    if(subTree_node->isLeaf) 
        return false;
    else 
        return search(subTree_node->childs[keyIdx], key);
}
void insertion(TREE **tree, int key, int data) {
    NODE *rootNode = (*tree)->root;
    if (rootNode->keyCount == 2 * T - 1) {
        NODE *newRootNode = Allocate();
        (*tree)->root = newRootNode;
        newRootNode->isLeaf = false;
        newRootNode->isRoot = true;
        newRootNode->childs[0] = rootNode;
        split_node(newRootNode, 0);
        insertion_nonfull(newRootNode, key, data);
    }
    else {
        insertion_nonfull(rootNode, key, data);
    }
}
void insertion_nonfull(NODE *node, int key, int data) {
    int keyIdx = node->keyCount;
    if (node->isLeaf) {
        for ( ; keyIdx >= 1 && key < node->keys[keyIdx - 1] ; --keyIdx) {
            node->keys[keyIdx] = node->keys[keyIdx - 1];
            node->dPointer[keyIdx].SHA = node->dPointer[keyIdx-1].SHA;
        }
        node->keys[keyIdx] = key;
        node->dPointer[keyIdx].SHA = data;
        node->keyCount++;
    }
    else {
        for ( ; keyIdx >= 1 && key < node->keys[keyIdx - 1] ; --keyIdx);
        if (node->childs[keyIdx]->keyCount == 2 * T - 1) {
            split_node(node, keyIdx);
            if (key > node->keys[keyIdx]) keyIdx++;
        }
        insertion_nonfull(node->childs[keyIdx], key, data);
    }
}
void split_node(NODE *parentNode, int chdIdx) {
    NODE *rightChildNode = Allocate();
    NODE *leftChildNode = parentNode->childs[chdIdx];

    rightChildNode->isLeaf = leftChildNode->isLeaf;
    rightChildNode->isRoot = leftChildNode->isRoot;

    if (leftChildNode->isLeaf) {
        rightChildNode->keyCount = T - 1;
        leftChildNode->keyCount = T;

        for (int i = 0; i < T - 1; ++i) {
            rightChildNode->keys[i] = leftChildNode->keys[T + i];
            rightChildNode->dPointer[i].SHA = leftChildNode->dPointer[T + i].SHA;
        }
        for (int i = parentNode->keyCount; i > chdIdx; --i) {
            parentNode->childs[i + 1] = parentNode->childs[i];
            parentNode->keys[i] = parentNode->keys[i - 1];
            parentNode->dPointer[i].SHA = parentNode->dPointer[i - 1].SHA;
        }
        parentNode->childs[chdIdx + 1] = rightChildNode;
        parentNode->keys[chdIdx] = leftChildNode->keys[T - 1];
        parentNode->dPointer[chdIdx].SHA = leftChildNode->dPointer[T - 1].SHA;
        parentNode->keyCount++;

        leftChildNode->next = rightChildNode;
        rightChildNode->prev = leftChildNode;
    }
    else {
        rightChildNode->keyCount = T - 1;
        leftChildNode->keyCount = T - 1;

        for(int i = 0 ; i < T - 1; ++i) {
            rightChildNode->keys[i] = leftChildNode->keys[T + i];
        }
        for(int i = 0; i < T; ++i) {
            rightChildNode->childs[i] = leftChildNode->childs[T + i];
        }
        for (int i = parentNode->keyCount; i > chdIdx; --i) {
            parentNode->childs[i + 1] = parentNode->childs[i];
            parentNode->keys[i] = parentNode->keys[i - 1];
            parentNode->dPointer[i].SHA = parentNode->dPointer[i - 1].SHA;
        }
        parentNode->childs[chdIdx + 1] = rightChildNode;
        parentNode->keys[chdIdx] = leftChildNode->keys[T - 1];
        parentNode->dPointer[chdIdx].SHA = leftChildNode->dPointer[T - 1].SHA;
        parentNode->keyCount++;
    }
}
void deletion(TREE **tree, int key ) {
    if(search((*tree)->root, key)) {
        deletion_enable(tree, (*tree)->root, key);    
    }
}
void deletion_enable(TREE **tree, NODE *node, int key){
    int chdIdx = find_subTree_index(node, key);
    if (node->isLeaf) {
        deletion_key_onto_Leaf(node, key);
    }
    else if (node->childs[0]->isLeaf) {
        if (node->childs[chdIdx]->keyCount == T - 1) {
            arrange_tree(tree, node, key, find_subTree_index(node, key));
        }
        else {
            deletion_key_onto_Leaf(node->childs[chdIdx], key);
            int childNode_lastItem_Idx = node->childs[chdIdx]->keyCount;
            if (node->keys[chdIdx] == node->childs[chdIdx]->keys[childNode_lastItem_Idx]) {
                node->keys[chdIdx] = node->childs[chdIdx]->keys[childNode_lastItem_Idx-1];
            }
        }
        // for(int i = chdIdx; i < node->keyCount -1; ++i) {
        //     node->keys[i] = node->keys[i + 1];
        //     node->dPointer[i].SHA = node->dPointer[i + 1].SHA;  
        // }
        // if (chdIdx != 0) {
        //     node->keys[chdIdx - 1] = node->keys[chdIdx];
        //     node->dPointer[chdIdx - 1].SHA = node->dPointer[chdIdx].SHA;
        // }
        // for (int i = chdIdx; i < node->keyCount; ++i) {
        //     node->childs[i] = node->childs[i + 1];
        // }
        // else if(node->keyCount == 0){ 
        //     (*tree)->root = node->childs[0];
        //     free(node);
        // }
        //node->keyCount--;
    }
    else {
        arrange_tree(tree, node, key, chdIdx);
    }
}
void deletion_key_onto_Leaf(NODE *node, int key){
    int keyIdx = 0;
    for(; keyIdx < node->keyCount; ++keyIdx) {
        if(node->keys[keyIdx] < key) continue;
        else if(keyIdx < node->keyCount - 1) {
            node->keys[keyIdx] = node->keys[keyIdx + 1];
            node->dPointer[keyIdx].SHA = node->dPointer[keyIdx + 1].SHA;
        }
    }
    node->keyCount--;
    if (node->keyCount == 0) {
        if(node->prev != NULL) {
            NODE *nextNode = node->next;
            node->prev->next = nextNode;
        }
        if(node->next != NULL) {
            NODE *prevNode = node->prev;
            node->next->prev = prevNode;
        }
        //if(!node->isRoot) free(node);
    }
}
int find_subTree_index(NODE *node, int key) {
    int subTreeIdx = 0;
    for(; subTreeIdx < node->keyCount; ++subTreeIdx) {
        if (key <= node->keys[subTreeIdx]) return subTreeIdx;
    }
    return subTreeIdx;
}
void arrange_tree(TREE **tree, NODE *node, int key, int subTreeIdx) {
    if(node->childs[subTreeIdx]->isLeaf) {
        deletion_key_onto_Leaf(node->childs[subTreeIdx], key);
        for (int i = subTreeIdx; i < node->keyCount; ++i) {
            node->childs[i] = node->childs[i + 1];
            if (subTreeIdx != 0 && subTreeIdx != node->keyCount - 1) {
                node->keys[subTreeIdx] = node->keys[subTreeIdx + 1];
                node->dPointer[subTreeIdx].SHA = node->dPointer[subTreeIdx + 1].SHA;
            }
        }
        node->keyCount--;

        if(node->keyCount == 0){ 
            (*tree)->root = node->childs[0];
            //free(node);
        }
        return;
    }
    else if (node->childs[subTreeIdx]->keyCount < T) {
        if (subTreeIdx > 0 && subTreeIdx < node->keyCount - 1) {
            if (node->childs[subTreeIdx - 1]->keyCount >= T) {
                swap_key_subtree_min(node, subTreeIdx);
                shift_prevNode_lastItem_to_node(node, subTreeIdx);
                arrange_tree(tree, node->childs[subTreeIdx], key, find_subTree_index(node->childs[subTreeIdx], key));
            }
            else if (node->childs[subTreeIdx + 1]->keyCount >= T) {
                swap_key_subtree_max(node, subTreeIdx);
                shift_nextNode_firstItem_to_node(node, subTreeIdx);
                arrange_tree(tree, node->childs[subTreeIdx], key, find_subTree_index(node->childs[subTreeIdx], key));
            }
            else {
                has_to_merge(tree, node, key, subTreeIdx);
            }
        }
        else if (subTreeIdx == 0) {
            if (node->childs[subTreeIdx + 1]->keyCount >= T) {
                swap_key_subtree_max(node, subTreeIdx);
                shift_nextNode_firstItem_to_node(node, subTreeIdx);
                arrange_tree(tree, node->childs[subTreeIdx], key, find_subTree_index(node->childs[subTreeIdx], key));
            }
            else {
                has_to_merge(tree, node, key, subTreeIdx);
            }
        }
        else if (subTreeIdx == node->keyCount) {
            if (node->childs[subTreeIdx - 1]->keyCount >= T) {
                swap_key_subtree_min(node, subTreeIdx);
                shift_prevNode_lastItem_to_node(node, subTreeIdx);
                arrange_tree(tree, node->childs[subTreeIdx], key, find_subTree_index(node->childs[subTreeIdx], key));
            }
            else {
                has_to_merge(tree, node, key, subTreeIdx);
            }
        }
    }
    else {
        arrange_tree(tree, node->childs[subTreeIdx], key, find_subTree_index(node->childs[subTreeIdx], key));
    }
}
void has_to_merge(TREE **tree, NODE *node, int key, int subTreeIdx) {
    NODE *merged_node = merge_node(node, subTreeIdx);
    if (node->keyCount == 0) {
        (*tree)->root = node->childs[0];
        //free(node);
        arrange_tree(tree, (*tree)->root, key, find_subTree_index((*tree)->root, key));
    }
    else {
        arrange_tree(tree, merged_node, key, find_subTree_index(merged_node,key));
    }    
}
void shift_overwrite_node(TREE **tree, NODE *node, int targetIdx) {
    if (targetIdx != 0) {
        node->keys[targetIdx - 1] = node->keys[targetIdx];
        node->dPointer[targetIdx - 1].SHA = node->dPointer[targetIdx].SHA;
    }
    for (int i = targetIdx; i < node->keyCount; ++i) {
        node->childs[i] = node->childs[i + 1];
    }
    node->keyCount--;

    if(node->keyCount == 0){ 
        (*tree)->root = node->childs[0];
        //free(node);
    }
}
void swap_key_subtree_min(NODE *node, int subTreeIdx) {
    int keyIdx = node->childs[subTreeIdx - 1]->keyCount;
    int key = node->childs[subTreeIdx - 1]->keys[keyIdx];
    int sha = node->childs[subTreeIdx - 1]->dPointer[keyIdx].SHA;
    node->childs[subTreeIdx - 1]->keys[keyIdx] = node->keys[subTreeIdx];
    node->childs[subTreeIdx - 1]->dPointer[keyIdx].SHA = node->dPointer[subTreeIdx].SHA;
    node->keys[subTreeIdx] = key;
    node->dPointer[subTreeIdx].SHA = sha;
}
void swap_key_subtree_max(NODE *node, int subTreeIdx) {
    int keyIdx = 0;
    int key = node->childs[subTreeIdx + 1]->keys[keyIdx];
    int sha = node->childs[subTreeIdx + 1]->dPointer[keyIdx].SHA;
    node->childs[subTreeIdx + 1]->keys[keyIdx] = node->keys[subTreeIdx];
    node->childs[subTreeIdx + 1]->dPointer[keyIdx].SHA = node->dPointer[subTreeIdx].SHA;
    node->keys[subTreeIdx] = key;
    node->dPointer[subTreeIdx].SHA = sha;
}
void shift_nextNode_firstItem_to_node(NODE *node, int targetIdx) {
    int lastIdx = node->childs[targetIdx]->keyCount;
    int firstIdx = 0;
    node->childs[targetIdx]->keys[lastIdx] = node->childs[targetIdx + 1]->keys[firstIdx];
    node->childs[targetIdx]->dPointer[lastIdx].SHA = node->childs[targetIdx + 1]->dPointer[firstIdx].SHA;
    node->childs[targetIdx]->childs[lastIdx + 1] = node->childs[targetIdx + 1]->childs[firstIdx];

    int nextNodeKeyCount = node->childs[targetIdx + 1]->keyCount;
    for (int i = 0; i < nextNodeKeyCount - 1; ++i) {
        node->childs[targetIdx + 1]->keys[i] = node->childs[targetIdx + 1]->keys[i + 1];
        node->childs[targetIdx + 1]->dPointer[i].SHA = node->childs[targetIdx + 1]->dPointer[i + 1].SHA;
    }
    for (int i = 0; i < nextNodeKeyCount; ++i) {
        node->childs[targetIdx + 1]->childs[i] = node->childs[targetIdx + 1]->childs[i + 1];
    }
    node->childs[targetIdx]->keyCount++;
    node->childs[targetIdx + 1]->keyCount--;
}
void shift_prevNode_lastItem_to_node(NODE *node, int targetIdx) {
    int lastIdx = node->childs[targetIdx]->keyCount;
    int firstIdx = 0;
    for (int i = lastIdx; i > 0; --i) {
        node->childs[targetIdx]->keys[i] = node->childs[targetIdx]->keys[i - 1];
        node->childs[targetIdx]->dPointer[i].SHA = node->childs[targetIdx]->dPointer[i - 1].SHA;
    }
    for (int i = lastIdx + 1; i > 0; --i) {
        node->childs[targetIdx]->childs[i] = node->childs[targetIdx]->childs[i - 1];
    }
    int prevNodeKeyCount = node->childs[targetIdx - 1]->keyCount;
    node->childs[targetIdx]->keys[firstIdx] = node->childs[targetIdx - 1]->keys[prevNodeKeyCount - 1];
    node->childs[targetIdx]->dPointer[firstIdx].SHA = node->childs[targetIdx - 1]->dPointer[prevNodeKeyCount - 1].SHA;
    node->childs[targetIdx]->childs[firstIdx] = node->childs[targetIdx - 1]->childs[prevNodeKeyCount];

    node->childs[targetIdx - 1]->keyCount--;
    node->childs[targetIdx]->keyCount++;
}
NODE *merge_node(NODE *node, int subTreeIdx) {
    if (subTreeIdx < node->keyCount) {
        //free(node->childs[subTreeIdx + 1]);
        node->childs[subTreeIdx]->keys[T - 1] = node->keys[subTreeIdx];
        for (int i = 0; i < T; ++i) {
            if(i != T-1) {
                node->childs[subTreeIdx]->keys[i + T] = node->childs[subTreeIdx + 1]->keys[i];
                node->childs[subTreeIdx]->dPointer[i + T].SHA = node->childs[subTreeIdx + 1]->dPointer[i].SHA;
            } 
            node->childs[subTreeIdx]->childs[i + T] = node->childs[subTreeIdx]->childs[i];
        }
        node->childs[subTreeIdx]->keyCount = 2 * T - 1;
        for (int i = subTreeIdx; i<node->keyCount; ++i) {
            if(i != node->keyCount - 1){
                node->keys[i] = node->keys[i + 1];
                node->dPointer[i].SHA = node->dPointer[i + 1].SHA;
            }
            node->childs[i] = node->childs[i + 1];
        }
        node->keyCount --;

        return node->childs[subTreeIdx];
    }
    else {
        //free(node->childs[subTreeIdx]);
    }   
    node->childs[subTreeIdx - 1]->keys[T - 1] = node->keys[subTreeIdx];
    for (int i = 0; i < T; ++i) {
        if(i != T-1) {
            node->childs[subTreeIdx - 1]->keys[i + T] = node->childs[subTreeIdx]->keys[i];
            node->childs[subTreeIdx - 1]->dPointer[i + T].SHA = node->childs[subTreeIdx]->dPointer[i].SHA;
        }
        node->childs[subTreeIdx - 1]->childs[i + T] = node->childs[subTreeIdx]->childs[i];
    }
    node->childs[subTreeIdx - 1]->keyCount = 2 * T - 1;
    node->keyCount--;
    return node->childs[subTreeIdx - 1];
}
void print_for_exam(NODE* cur) {
    if (cur->isRoot && cur->keyCount == 0) {
        printf("[EMPTY]\n");
    }
    if (cur->isLeaf) {
        for (int i = 0; i < cur->keyCount; i++) {
            printf("[%5d, %5d]\n", cur->keys[i], cur->dPointer[i].SHA);
        }
    }
    else {
        for (int i = 0; i < cur->keyCount; i++) {
            print_for_exam(cur->childs[i]);
            printf("[%5d]\n", cur->keys[i]);
        }
        print_for_exam(cur->childs[cur->keyCount]);
    }
}