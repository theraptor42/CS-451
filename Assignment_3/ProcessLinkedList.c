//
// Created by caspi on 11/25/2018.
//

#include "ProcessLinkedList.h"


int addPNodeToPLinkedList(ProcessLinkedList *list, ProcessNode *newNode)
{
    if (newNode == NULL)
        return -1;
    //simple empty list check
    if (list->head == NULL)
    {
        list->head = newNode;
        list->numberOfElements = 1;
        return 0;
    }

    ProcessNode *prev = NULL;
    ProcessNode *current = list->head;

    int counter;
    for(counter = 0; counter< list->numberOfElements; counter++)
    {
        // if the compare is less than zero, than the new node has a lower priority than current
        if (comparePNodePriority(newNode, current) < 0)
        {
            //insert it into the linked list here
            if (prev == NULL)
            {
                //Current is old the head
                newNode->next = current;
                list->head = newNode;
            }
            else
            {
                //insert it between the prev and current
                newNode->next = current;
                prev->next = newNode;
            }
            //we're done here
            list->numberOfElements++;
            return counter;
        }
        //go to the next node
        prev = current;
        current = current->next;
    }

    //the new node is going to the tail of the list
    //current is null
    //Prev cannot be a null element here, that is handled by the node head being null
    prev->next = newNode;
    list->numberOfElements++;
    return list->numberOfElements-1;

}
ProcessNode* removeHeadFromPLinkedList(ProcessLinkedList *list)
{
    //If the list is empty
    if (list->head == NULL)
        return NULL;

    //pop the head off the list
    ProcessNode *returnNode = list->head;
    //set the next node as head
    list->head = returnNode->next;
    //return the old head
    return returnNode;

}

int comparePNodePriority(ProcessNode *nodeA, ProcessNode *nodeB)
{
    if (nodeA == NULL || nodeB == NULL || nodeA->process == NULL || nodeB->process == NULL)
        return NULL_COMPARE_ERROR;

    //returns negative if A has a 'better' (lower) priority
    //positive if B has a lower priority
    //0 if they have the same priority

    return (nodeA->process->priority - nodeB->process->priority);
}