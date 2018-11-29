//
//  B-tree.c
//  C_notes_Stack
//
//  Created by Zhenyu Jia on 2018/11/21.
//  Copyright © 2018年 Zhenyu Jia. All rights reserved.
//

#include "B_tree.h"

//typedef struct Btree {
//    unsigned int num;
//    unsigned int* keys;
//    struct Btree** children;
//    bool isLeaf;
//} BNode;
static void insertBtreeHelper(BNode**, BNode*, unsigned int, BNode**, BNode**, int*);

static void deleteBtreeHelper(BNode*, BNode*, BNode**, int, unsigned int);

static void _merge(BNode*, BNode*, int);

static void printMargin(int margeCount);

/* =============== CONSTRUCT AND DECONSTRUCT =========== */
BNode* createBtree(void) {
    BNode*  ret;
    
    if ((ret = (BNode*) malloc(sizeof(BNode))) == NULL) return NULL;
    
    ret -> num      = 0;
    ret -> isLeaf   = true;
    // initialize
    for (int i =0; i < SIZE_CHILDRENS; i++) ret->children[i] = NULL;
    for (int j =0; j < SIZE_KEYS; j++) ret->keys[j]     = -1;
    return ret;
}

void   destroyBtree(BNode* root) {
    for (int i = 0; i < SIZE_CHILDRENS; i++)
        if (root->children[i] != NULL) destroyBtree(root->children[i]);
    free(root);
}

/* =========== SEARCH ========== */
void searchBtree(BNode* root, unsigned int key, BNode** ret, int* index) {
    int i = 0;
    
    for (; i < root->num; i++) {
        if (key < root->keys[i]) break;
        if (key == root->keys[i]) {   // 相等找到
            *ret    = root;
            *index  = i;
            return ;
        }
    }
    if (root->isLeaf) {    // 没有找到
        *ret    = NULL;
        *index  = -1;
        return;
    }
    // 不是leaf 的话 需要再下一个间隔里面寻找
    BNode* p  = (root->children)[i];
    searchBtree(p, key, ret, index);
}

/* ========= INSERT ========== */
void insertBtree(BNode** root, unsigned int key) {
    BNode *pLeft, *pRight;
    pLeft = pRight = NULL;
    int  median    = -1;
    insertBtreeHelper(root, *root, key, &pLeft, &pRight, &median);
}

static void insertBtreeHelper(BNode** root, BNode* cur, unsigned int key,
                              BNode** left, BNode** right, int* median) {
    int ind = 0;
    for (; ind < cur->num; ind++) {
        if (key < cur->keys[ind]) break;
        if (key == cur->keys[ind]) return;   // no duplicate keys
    }
    
    if (cur->isLeaf) {
        // 先把key 插入数组 然后在 处理
        for (int j = cur->num -1; j >= ind; j--)
            cur->keys[j+1] = cur->keys[j];
        cur->keys[ind] = key;
        (cur->num)++;
        
        // 叶子结点 有空位 那么直接插入
        if (cur->num <= MAX_NUM_CHDR) return;  //  finish the situation where leaf has enough space

        // case leaf is full
        int medianVal     = cur->keys[MAX_NUM_CHDR / 2];
        // split
        BNode* rightP     = createBtree();
        if (rightP == NULL) {   // OOM
            printf("Split leaf failed due to OOM.\n");
            return;
        }
        rightP->num          = MAX_NUM_CHDR / 2;
        // copy t elements from left tree to right.
        for (int i = 0; i < MAX_NUM_CHDR/2; i++) rightP->keys[i]  = cur->keys[i+MAX_NUM_CHDR/2+1];
        for (int i = MAX_NUM_CHDR/2; i < SIZE_KEYS; i++) cur->keys[i]  = -1;
        cur->num             = MAX_NUM_CHDR / 2;
        if (cur == *root) {
            BNode* newHead;
            newHead  = (BNode*) malloc(sizeof(BNode));
            newHead->num            = 1;
            newHead->isLeaf         = false;
            newHead->keys[0]        = medianVal;
            newHead->children[0]    = cur;
            newHead->children[1]    = rightP;
            *root                   = newHead;
        } else {
            *median              = medianVal;
            *left                = cur;
            *right               = rightP;
        }
        return;
    } else {
        BNode* pChdr         = cur->children[ind];
        insertBtreeHelper(root, pChdr, key, left, right, median);
        
        if (*median>=0) {   // median is not NULL means lower level has a split
            for (int j = cur->num -1; j >= ind; j--) cur->keys[j+1]  = cur->keys[j];
            cur->keys[ind]            = *median;
            (cur->num)++;
            for (int j = MAX_NUM_CHDR-1; j >= ind; j--) cur->children[j+1]  = cur->children[j];
            cur->children[ind]        = *left;
            cur->children[ind+1]      = *right;
            
            if (cur->num <= MAX_NUM_KEYS) {  // 当前可以插入新的key
                *median   = -1;
                *left = *right      = NULL;
                return;
            } else {  // key 已经是 2t -1 了 不能再插入了 split at
                BNode* rightP       = createBtree();
                if (rightP == NULL) {
                    printf("Split leaf failed due to OOM.\n");
                    return;
                }
//                rightP->num       = MAX_NUM_CHDR/2-1;
                *median           = cur->keys[MAX_NUM_CHDR/2];   // 上传新的median
                
                for (int j = 0; j < MAX_NUM_CHDR/2; j++) rightP->keys[j]   = cur->keys[MAX_NUM_CHDR/2+1+j];
                if (!cur->isLeaf) for (int j = MAX_NUM_CHDR/2+1; j < MAX_NUM_CHDR+1; j++)
                    rightP->children[j-MAX_NUM_CHDR/2-1] = cur->children[j];
                cur->num          = MAX_NUM_CHDR/2;
                rightP->num       = MAX_NUM_CHDR/2-1;
                rightP->isLeaf    = false;
                *left             = cur;
                *right            = rightP;
                if (cur == *root) {   // 如果当前结点为 根
                    BNode *newRoot;
                    newRoot   = createBtree();
                    if (!newRoot) { // destroy
                        // todo
                        printf("Split at root failed! OOM\n");
                        return;
                    }
                    newRoot->num            = 1;
                    newRoot->isLeaf         = false;
                    newRoot->keys[0]        = *median;
                    newRoot->children[0]    = *left;
                    newRoot->children[1]    = *right;
                    *root                   = newRoot;
                }
                return;  // 处理完成 父亲满的时候
            }
        }
    }
}

/* ========= DELETION ========= */
void deleteBtree(BNode** root, unsigned int key) {
    deleteBtreeHelper(*root, NULL, root, -1, key);
}

static void deleteBtreeHelper(BNode* cur, BNode* parent, BNode** root, int pos, unsigned int key) {
    printf("\n Searching at node : ");
    printBnode(cur);
    int ind = 0, found = 0;
    
    for (; ind < cur->num; ind++) {
        if (key < *(cur->keys+ind)) break;
        if (key == *(cur->keys+ind)) {  // found
            found = 1;
            break;
        }
    }
    // case found
    if (found) {
        if (cur->isLeaf) {
            for (int j = ind; j < cur->num - 1; j++) cur->keys[j] = cur->keys[j+1];
            (cur->num)--;
            if (cur->num >= MIN_NUM_KEYS || parent == NULL) return;  // tell parent call that ok down here
            else  {
                _merge(cur, parent, pos);
                return;
            }
            // non-root leaf elements 1 short
        } else {
            // find max of left child
            BNode* child    = cur->children[ind];
            while (!child->isLeaf)
                child = child->children[child->num-1];
            cur->keys[ind]               = child->keys[child->num-1];
            key                          = cur->keys[ind];
        }
    }
    deleteBtreeHelper(cur->children[ind], cur, root, ind, key);
    //   root
    if (parent == NULL) {
        if (cur->num == 0) {
            *root = cur->children[0];
            free(cur);
        }
        return;   // root OK!
    }
    if (cur->num < MIN_NUM_KEYS) _merge(cur, parent, pos);
}

static void _merge(BNode* cur, BNode* parent, int pos) {
    // case a left or right has extra elements
    if (pos < parent->num && parent->children[pos+1]->num > MIN_NUM_KEYS) {  // 肯定有右兄妹
        BNode* rightSib        = parent->children[pos+1];
        cur->keys[cur->num]  = parent->keys[pos];
        (cur->num)++;
        parent->keys[pos]      = rightSib->keys[0];
        for (int j = 0; j < rightSib->num-1; j++) rightSib->keys[j] = rightSib->keys[j+1];
        if (!cur->isLeaf) {
            cur->children[cur->num]    = rightSib->children[0];   // 右边兄妹的第一个孩子过来
            for (int j = 0; j < rightSib->num-1; j++) rightSib->children[j]    = rightSib->children[j+1];
        }
        (rightSib->num)--;
    }
    else if (pos > 0 && parent->children[pos-1]->num > MIN_NUM_KEYS) { // 肯定有左孩子
        BNode* leftSib         = parent->children[pos-1];
        for (int j = MIN_NUM_KEYS-1; j >= 1; j--) cur->keys[j] = cur->keys[j-1];
        cur->keys[0]           = parent->keys[pos];
        (cur->num)++;
        parent->keys[pos]      = leftSib->keys[leftSib->num-1];
        if (!cur->isLeaf) {
            for (int j = cur->num-1; j > 0; j--) cur->children[j]    = cur->children[j-1];
            cur->children[0]  = leftSib->children[leftSib->num];
        }
        (leftSib->num)--;
    }
    // case non siblings have extra elements merge
    else if (pos > 0) {   // grab left
        BNode* leftSib         = parent->children[pos-1];   // put parent k to first of current tree
        leftSib->keys[leftSib->num]   = parent->keys[pos-1];
        for (int j = 0; j < cur->num; j++) leftSib->keys[leftSib->num+1+j]  = cur->keys[j];
        if (!cur->isLeaf)
            for (int j = 0; j < cur->num+1; j++) leftSib->children[leftSib->num+j+1] = cur->children[j];
        leftSib->num += 1+cur->num;
        free(cur);
        for (int i = pos-1; i < parent->num; i++) parent->keys[i]  = parent->keys[i+1];  // 父亲节点 需要左移动
        (parent->num)--;
    }
    else if (pos < parent->num) {
        BNode* rightSib        = parent->children[pos+1];
        cur->keys[cur->num]    = parent->keys[pos];
        // 合并 把右边的key 都拿过来
        for (int j = 0; j < rightSib->num; j++) cur->keys[cur->num+1+j]  = rightSib->keys[j];
        if (!cur->isLeaf)
            for (int j = 0; j < rightSib->num+1; j++) cur->children[cur->num+j+1] = rightSib->children[j];
        cur->num    += 1 + rightSib->num;
        free(rightSib);
        for (int i = pos; i < parent->num; i++) parent->keys[i]   = parent->keys[i+1];
        (parent->num)--;
    }
    for (int j = cur->num+1; j < SIZE_CHILDRENS; j++) cur->children[j]  = NULL;
    for (int j = parent->num+1; j < SIZE_CHILDRENS; j++) parent->children[j] = NULL;
}

/* ========= VISUALIAZATION  ======= */
void printBtree(BNode* root, int margin) {
    if (!root) return;
    printMargin(margin);
    printf("Num: %d is leaf: %d :", root->num, root->isLeaf);
    for (int i = 0; i < root->num; i++)  printf("%d, ", root->keys[i]);
    printf("\n");
    for (int i = 0; i < root->num+1; i++) printBtree(root->children[i], 4+margin);
}

void printBnode(BNode* root) {
    if (root == NULL) return;
    printf("Num of keys: %d", root->num);
    printf("\nIsLeaf %d\n", root->isLeaf);
    printf("Keys: ");
    for (int i = 0; i < SIZE_KEYS; i++)  printf("%d, ", root->keys[i]);
    printf("\nChildren: ");
    for (int i = 0; i < SIZE_CHILDRENS; i++) printf("%p, ", root->children[i]);
    printf("\nEND of BNode \n");
}

static void printMargin(int margeCount) {
    for (int i =0; i < margeCount; i++) printf(" ");
}
