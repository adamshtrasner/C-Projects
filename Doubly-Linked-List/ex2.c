#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "ex2.h"


#define BUFF 1025
#define INPUT_INVALID_ERROR "Input is invalid or NULL\n"
#define MEMORY_ALLOCATION_ERROR "ERROR: Memory allocation failed\n"
#define INVALID_FILE_ERROR "ERROR: The given file is invalid\n"
#define DELIMITER ","
#define FILE_OPEN_MODE "r"
#define ADD_TO_START_MODE "s"
#define MULT(a,b) a*b
#define VAR 2

/**
 *
 * @param n number of elements for the data array
 * @param arr the array from which we parse the elements to the daya array
 * @return a new node with the data: {arr[0], ..., arr[n-1]} .
 */
Node *NewNode(size_t n, int *arr){
    int *data = malloc(sizeof(int)*n);
    if (!data){
        return NULL;
    }
    Node *node = malloc(sizeof(Node));
    if (!node){
        return NULL;
    }
    int i;
    for (i=0; i<n; i++){
        data[i] = arr[i];
    }
    node->data = data;
    node->len = n;
    node->next = node->prev = NULL;
    return node;
}


/**
 *
 * @param arr an array
 * @return the average of the numbers in the array
 */
double GetAverage(int const *arr, double const n){
    // CHECKED
    int i;
    double sum = 0.0;
    for (i = 0; i < n; i++){
        sum = (double)(sum + arr[i]);
    }
    return sum/n;
}


void AddToStartLinkedList(LinkedList *const list, Node *const node){

    if ((!list) || (!node)){
        fprintf(stderr, INPUT_INVALID_ERROR);
        return;
    }

    // if the list is empty
    if (!(list->head) && !(list->tail)){
        list->head = node;
        list->tail = node;
        return;
    }

    // adds a node as the head of the list

    Node *tmp_head = list->head;
    node->next = tmp_head;
    tmp_head->prev = node;

    list->head = node;
}


void AddToEndLinkedList(LinkedList *const list, Node *const node){

    if ((!list) || (!node)){
        fprintf(stderr, INPUT_INVALID_ERROR);
        return;
    }

    // if the list is empty
    if (!(list->head) && !(list->tail)){
        list->head = node;
        list->tail = node;
        return;
    }

    // adds a node as the tail of the list

    Node *tmp_tail = list->tail;
    node->prev = tmp_tail;
    tmp_tail->next = node;

    list->tail = node;
}


void RemoveNode(LinkedList *const list, Node *const node){

    if ((!list) || (!node)){
        fprintf(stderr, INPUT_INVALID_ERROR);
        return;
    }

    // check if the node is in the list.
    // if it is, remove it, and free its memory.

    // There are 4 cases (if the node we want to remove exists):
    // 1. The node we want to remove is the only node there is in the list,
    //    therefore its the tail and the head of the list altogether.
    // 2. The node we want to remove is the tail of the list.
    // 3. The node we want to remove is the head of the list.
    // 4. The node is "in the middle" of the list,
    //    that is its next and prev fields are not NULL.

    Node *curr = list->head;
    Node *prev = NULL, *next = NULL;
    while (curr){
        if (curr == node){

            // free the memory of the node's data
            free(curr->data);
            curr->data = NULL;

            // case 1
            if (list->head == curr && list->tail == curr){
                free(curr);
                list->head = list->tail = curr = NULL;
                break;
            }
            // case 2
            if (list->tail == curr){
                list->tail = curr->prev;
                free(curr);
                list->tail->next = curr = NULL;
                break;
            }

            // case 3
            if (list->head == curr){
                list->head = curr->next;
                free(curr);
                list->head->prev = curr = NULL;
                break;
            }

            // case 4
            prev = curr->prev;
            next = curr->next;
            prev->next = next;
            next->prev = prev;
            free(curr);
            curr = NULL;
            break;

        }

        else{

            curr = curr->next;
        }
    }
}


void FreeLinkedList(LinkedList *const list) {

    if (!list) {
        return;
    }
    Node *next;
    Node *tmp = list->head;
    while (tmp) {
        list->head = list->head->next;
        next = list->head;
        free(tmp->data);
        tmp->data = NULL;
        free(tmp);
        tmp = next;
    }
    free(list);
}



double *GetAverages(LinkedList *const list, size_t *const num_elements_in_returned_array){

    // Checking validation of the linked list
    if (!(list) || !(num_elements_in_returned_array)){
        fprintf(stderr, INPUT_INVALID_ERROR);
        return NULL;
    }
    // Allocating memory for the averages array
    double *averages = malloc(sizeof(double));
    if (!averages){
        fprintf(stderr, MEMORY_ALLOCATION_ERROR);
        return NULL;
    }

    double *res = NULL;                     // We'll store the result of
                                            // reallocating memory for averages
                                            // in res

    *(num_elements_in_returned_array) = 0;  // num_elements always points to
                                            // the same address. we set and change the
                                            // value of the variable in that address

    Node *curr = list->head;
    double avg = 0;
    size_t capacity = 1;
    while (curr){
        if (curr->len >= 1){
            (*num_elements_in_returned_array)++;
            avg = GetAverage(curr->data, (double)(curr->len));
            if ((*num_elements_in_returned_array) > capacity){
                capacity *=2;
                res = realloc(averages, capacity * sizeof(double));
                if (!res){
                    free(averages);
                    averages = NULL;
                    fprintf(stderr, MEMORY_ALLOCATION_ERROR);
                    return NULL;
                }
                else{
                    averages = res;
                }
            }

            averages[(int)(*num_elements_in_returned_array) - 1] = avg;

        }

        curr = curr->next;
    }

    // We need to free the memory that is not used, if exists,
    // that is - if capacity is greater then num_elements
    if (capacity > *(num_elements_in_returned_array)){
        res = realloc(averages, *(num_elements_in_returned_array) * sizeof(double));
        if (!res){
            free(averages);
            averages = NULL;
            fprintf(stderr, MEMORY_ALLOCATION_ERROR);
            return NULL;
        }
        else{
            averages = res;
        }
    }
    return averages;
}


LinkedList *ParseLinkedList(const char *const filename){

    FILE *list_file = fopen(filename, FILE_OPEN_MODE);

    // handle invalid path or file
    if (!list_file){
        fprintf(stderr, INVALID_FILE_ERROR);
        return NULL;
    }

    // defining a node
    Node *node;

    // handle allocation failure for linked list
    LinkedList *list = malloc(sizeof(struct LinkedList));
    if (!list){
        fprintf(stderr, MEMORY_ALLOCATION_ERROR);
        return NULL;
    }

    list->tail = NULL;
    list->head = NULL;

    // handle allocation failure for data array
    int *data = malloc(sizeof(int));
    if (!data){
        fprintf(stderr, MEMORY_ALLOCATION_ERROR);
        return NULL;
    }

    int *res_data = NULL;  // result after reallocating memory for data
    size_t n = 1, capacity_data = 1; // length of the data array

    // defining line and token
    char line[BUFF] = {0}, *token = NULL, *mode = NULL; // mode is the first token
                                                        // in each line, that is "s"
                                                        // or "e"
    while (fgets(line, BUFF, list_file) != NULL){
        token = strtok(line, DELIMITER);
        mode = token;
        while ((token = strtok(NULL, DELIMITER))) {
            if (n > capacity_data) {
                capacity_data *= 2;
                res_data = realloc(data, capacity_data * sizeof(int));
                if (!res_data) {
                    free(data);
                    data = NULL;
                    fprintf(stderr, MEMORY_ALLOCATION_ERROR);
                    return NULL;
                }
                data = res_data;
            }

            data[n-1] = (int) (strtol(token, NULL, 10));
            n++;

        }

        // setting the fields of the node
        node = NewNode(n - 1, data);


        // deciding whether to add node to the start of the list
        // or the end of it, according to mode
        if (!strcmp(mode, ADD_TO_START_MODE)) {
            AddToStartLinkedList(list, node);
        } else {
            AddToEndLinkedList(list, node);
        }

        // reallocating memory for data to be of length 1 again
        n = capacity_data = 1;
        res_data = realloc(data, sizeof(int));
        if (!res_data) {
            free(data);
            data = NULL;
            fprintf(stderr, MEMORY_ALLOCATION_ERROR);
            return NULL;
        }
        data = res_data;

    }
    free(data);
    // handle closure of file
    fclose(list_file);
    return list;
}

int main(){
#ifdef VAR
    printf("%d", VAR + 5);
#else
    printf("%d", VAR + 9);
#endif
    return 0;

}