#include "list.h"
#include "service.h"
#include <errno.h>
#include <stdlib.h>
#include <string.h>

int createList(List *list){
    Node *head;
    head = createNode();
    if (head == NULL){
        return ENOMEM;
    }

    list->head = head;
    initMutexSuccessAssertion(&list->mtx, NULL, "createList");
    return SUCCESS_CODE;
}

Node *createNode(){
    Node *res = (Node*)malloc(sizeof(Node));
    if (res == NULL){
        errno = ENOMEM;
        return NULL;
    }

    res->next = NULL;
    res->str[0] = '\0';
    return res;
}

int addElement(List *list, const char *str){
    if (list == NULL || str == NULL)
        return EINVAL;

    Node *new_el = createNode();
    if (new_el == NULL)
        return ENOMEM;

    strcpy(new_el->str, str);

    lockSuccessAssertion(&list->mtx, "addElement");

    Node *head = list->head;
    Node *prev = head->next;

    head->next = new_el;
    new_el->next = prev;

    unlockSuccessAssertion(&list->mtx, "addElement");
    return SUCCESS_CODE;
}

int showList(List *list){
    if (list == NULL){
        return EINVAL;
    }

    lockSuccessAssertion(&list->mtx, "showList");
    Node *node = list->head->next;
    while (node != NULL){
        printf("%s\n", node->str);
        node = node->next;
    }
    unlockSuccessAssertion(&list->mtx, "showList");
    
    return SUCCESS_CODE;
}

int destroyList(List *list){
    if (list == NULL){
        return EINVAL;
    }

    //free data
    Node *next;
    Node *head = list->head;
    while (head != NULL){
        next = head->next;
        free(head);
        head = next;
    }

    //destroy mtx
    int err = pthread_mutex_destroy(&list->mtx);
    assertSuccess("destroySuccess", err);
    
    return SUCCESS_CODE;
}

static void swap(Node **a, Node **b){
    if (a == NULL || b == NULL){
        errno = EINVAL;
        return;
    }

    Node *tmp = *a;
    *a = *b;
    *b = tmp;
}

int sortList(List *list){
    if (list == NULL){
        return EINVAL;
    }

    lockSuccessAssertion(&list->mtx, "sortList");
    
    char sorted = list->head->next == NULL ? TRUE : FALSE;
    while (sorted == FALSE){
        sorted = TRUE;

        Node *curr = list->head->next;
        Node *prev_curr = list->head;
        Node *follow = curr->next;
        while (follow != NULL){
            int cmp = strcmp(curr->str, follow->str);
            if (cmp > 0){
                sorted = FALSE;
                prev_curr->next = follow;
                curr->next = follow->next;
                follow->next = curr;

                swap(&curr, &follow);
            }

            follow = follow->next;
            curr = curr->next;
            prev_curr = prev_curr->next;
        }
    }

    unlockSuccessAssertion(&list->mtx, "sortList");

    return SUCCESS_CODE;
}
