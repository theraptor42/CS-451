//
// Created by caspi on 11/25/2018.
//

#ifndef ASSIGNMENT_3_PROCESSLINKEDLIST_H
#define ASSIGNMENT_3_PROCESSLINKEDLIST_H

#include "MyProcess.h"

struct PNode
{
    PROCESS *process;
    struct PNode *next;
};
typedef struct PNode ProcessNode;

struct PLinkedList
{
    ProcessNode *head;
    int numberOfElements;
};
typedef struct PLinkedList ProcessLinkedList;

//defines
#define NULL_COMPARE_ERROR          0
//Global Variables

//Function Prototypes
int addPNodeToPLinkedList(ProcessLinkedList *list, ProcessNode *newNode);
ProcessNode* removeHeadFromPLinkedList(ProcessLinkedList *list);

int comparePNodePriority(ProcessNode *nodeA, ProcessNode *nodeB);





#endif //ASSIGNMENT_3_PROCESSLINKEDLIST_H
