//
//  main.c
//  C_Btree
//
//  Created by Zhenyu Jia on 2018/11/24.
//  Copyright © 2018年 Zhenyu Jia. All rights reserved.
//

#include <stdio.h>
#include "B_tree.h"

int main(int argc, const char * argv[]) {
    // insert code here...
    // Test 1
    BNode *test1    = createBtree();
    for (int i = 103; i < 300; i += 10) { insertBtree(&test1, i); }
    for (int i = 100; i < 110; i += 1) { insertBtree(&test1, i); }
    for (int i = 100; i < 140; i += 3) { insertBtree(&test1, i); }
    insertBtree(&test1, 144);
    insertBtree(&test1, 145);
//    printBtree(test1, 0);
    
    // Test2 test search
    BNode* ret;
    int pos  = 10;
    searchBtree(test1, 283, &ret, &pos);
//    printBnode(ret);
//    printf("Position %d, \n", pos);
    
    // Test3 test insert   passed
    BNode *test2   = createBtree();
    test2->num  =2 ;
    test2->isLeaf = false;
    test2->keys[0] = 7;
    test2->keys[1]  = 16;
    BNode *kid1    = createBtree();
    kid1->num      = 4;
    kid1->keys[0]  = 1;
    kid1->keys[1]  = 2;
    kid1->keys[2]  = 5;
    kid1->keys[3]  = 6;
    BNode* kid2    = createBtree();
    kid2->num      = 2;
    kid2->keys[0]  = 9;
    kid2->keys[1]  = 12;
    BNode* kid3    = createBtree();
    kid3->num      = 2;
    kid3->keys[0]  = 18;
    kid3->keys[1]  = 21;
    test2->children[0]   = kid1;
    test2->children[1]   = kid2;
    test2->children[2]   = kid3;
    printBtree(test2, 0);
    printf("\n Insert 20\n");
    insertBtree(&test2, 20);
    printBtree(test2, 0);
    
    printf("\n Insert 0\n");
    insertBtree(&test2, 0);
    printBtree(test2, 0);
    
    printf("\n Insert 22  and 23\n");
    insertBtree(&test2, 22);
    insertBtree(&test2, 23);
//    printBtree(test2, 0);
    
    printf("\n Insert 4 \n");
    insertBtree(&test2, 4);
//    printBtree(test2, 0);
    
    // test deletion
    deleteBtree(&test2, 4);
//    printBtree(test2, 0);
    
    deleteBtree(&test2, 0);
//    printBtree(test2, 0);
    
    deleteBtree(&test2, 1);
    printf("\nDetele 1 \n");
    
    
    printf("\nDetele 9 \n");
    deleteBtree(&test2, 9);
//    printBtree(test2, 0);
    
    deleteBtree(&test2, 12);
    deleteBtree(&test2, 2);
    
    printBtree(test2, 0);
    printf("\nDetele 5 \n");
    deleteBtree(&test2, 5);
    printBtree(test2, 0);
    return 0;
}
