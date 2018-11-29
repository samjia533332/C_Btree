//
//  B-tree.h
//  C_notes_Stack
//
//  Created by Zhenyu Jia on 2018/11/21.
//  Copyright © 2018年 Zhenyu Jia. All rights reserved.
//

#ifndef B_tree_h
#define B_tree_h

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>

#define BTREE_NODE_T   2
#define MAX_NUM_CHDR   (2*BTREE_NODE_T)
#define MAX_NUM_KEYS   (2*BTREE_NODE_T-1)
#define MIN_NUM_CHDR   (BTREE_NODE_T)
#define MIN_NUM_KEYS   (BTREE_NODE_T-1)
#define SIZE_KEYS      (MAX_NUM_CHDR+1)
#define SIZE_CHILDRENS (MAX_NUM_CHDR+1)

typedef struct Btree {
    unsigned int num;
    unsigned int keys[SIZE_KEYS];
    struct Btree *children[SIZE_CHILDRENS];
    bool isLeaf;
} BNode;

BNode* createBtree(void);

void   destroyBtree(BNode*);

void searchBtree(BNode* root, unsigned int key, BNode** ret, int* index);

void insertBtree(BNode** root, unsigned int key);

void deleteBtree(BNode**root, unsigned int key);

void printBtree(BNode* root, int margin);

void printBnode(BNode* root);

#endif /* B_tree_h */
