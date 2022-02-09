#include "Vector.h"

#define CHECK_ERROR(expression, error) if (!(expression)){\
return error;                                           \
}
#define LOAD_FACTOR(size, capacity) ((double)(size)/(double)(capacity))

// ------ Functions of my own ------


/**
 * The function organizes the vector. The function is called ONLY
 * after deletion of a data member from the vector.
 * By organizing, I mean - if ind is the index of the last data member,
 * then we only free the data member and no organization required.
 * otherwise, we must re-point each data member
 * from index k s.t (ind) <= k <= ((*pVector)->size) to (k+1)
 * ( we point each data member from index >= ind to its next data member)
 * @param pVector (struct Vector **) pointer to pointer to vector
 * @param ind index of the data from which we want to start organizing
 */
static void OrganizeVector(Vector **pVector, size_t ind) {
    Vector *vec = *pVector;
    void *p = vec->data[ind];
    if (ind != vec->size){
        vec->data[ind] = vec->data[ind + 1];
        ind++;
        while (ind != vec->size){
            vec->data[ind] = vec->data[ind + 1];
            ind ++;
        }
    }
    vec->data[vec->size] = NULL;
    vec->elem_free_func(&p);
}


// ------ End of my own functions ------

// ------ Start of the header functions ------

Vector *VectorAlloc(VectorElemCpy elem_copy_func, VectorElemCmp elem_cmp_func,
                    VectorElemFree elem_free_func)
{
    CHECK_ERROR(elem_copy_func && elem_cmp_func && elem_free_func, NULL)
    Vector *vec = calloc(1, sizeof(Vector));
    CHECK_ERROR(vec, NULL)
    vec->data = calloc(1, sizeof(vec->data) * VECTOR_INITIAL_CAP);
    CHECK_ERROR(vec->data, NULL)
    vec->capacity = VECTOR_INITIAL_CAP;
    vec->elem_copy_func = elem_copy_func;
    vec->elem_cmp_func = elem_cmp_func;
    vec->elem_free_func = elem_free_func;
    return vec;
}


void VectorFree(Vector **p_vector)
{
    if (p_vector && *p_vector){
        // free the *data array
        if ((*p_vector)->size){
            for(size_t i = 0; i < (*p_vector)->size; i ++){
                (*p_vector)->elem_free_func(&((*p_vector)->data[i]));
            }
        }
        free((*p_vector)->data);
        free(*p_vector);
        *p_vector = NULL;
    }
}


void *VectorAt(Vector *vector, size_t ind)
{
    CHECK_ERROR(vector, NULL)
    CHECK_ERROR((vector->size) && (ind < vector->size), NULL)
    return vector->data[ind];
}


int VectorFind(Vector *vector, void *value)
{
    CHECK_ERROR(vector && value, -1)
    CHECK_ERROR(vector->size, -1)

    for (size_t i = 0; i < vector->size; i ++){
        if (vector->elem_cmp_func(vector->data[i], value)){
            return (int)i;
        }
    }

    return -1;

}


int VectorPushBack(Vector *vector, void *value)
{
    CHECK_ERROR(vector && value, 0)
    vector->size++;
    if (VectorGetLoadFactor(vector) > VECTOR_MAX_LOAD_FACTOR){
        vector->capacity *= VECTOR_GROWTH_FACTOR;
        vector->data = realloc(vector->data, sizeof(vector->data) * vector->capacity);
    }
    void *to_add = vector->elem_copy_func(value);
    vector->data[vector->size-1] = to_add;
    return 1;
}


double VectorGetLoadFactor(Vector *vector)
{
    CHECK_ERROR(vector, -1)
    return LOAD_FACTOR(vector->size, vector->capacity);
}


int VectorErase(Vector *vector, size_t ind)
{
    CHECK_ERROR(vector, 0)
    CHECK_ERROR(ind < vector->size, 0)
    vector->size--;
    if (VectorGetLoadFactor(vector) < VECTOR_MIN_LOAD_FACTOR){
        // if capacity is 1, we dont keep shrinking the size of the vector
        // (cause capacity is always >= 1)
        if (vector->capacity >= 2){
            vector->capacity /= VECTOR_GROWTH_FACTOR;
            vector->data = realloc(vector->data, sizeof(vector->data) * (vector->capacity));
        }
    }
    // Organize the vector
    OrganizeVector(&vector, ind);
    return 1;
}


void VectorClear(Vector *vector)
{
    if (vector){
        int i =(int)(vector->size - 1);
        int res = VectorErase(vector, i);
        while (i >= 0 && res == 1){
            i--;
            res = VectorErase(vector, i);
        }
    }
}