#ifdef MAIN
#include <stdio.h>
#include "HashMap.h"
#include "PairCharInt.h"
#include "Hash.h"


void *ElemCpy(const void *elem) {
    int *a = malloc(sizeof(int));
    *a = *((int *) elem);
    return a;
}
int ElemCmp(const void *elem_1, const void *elem_2) {
    return *((const int *) elem_1) == *((const int *) elem_2);
}
void ElemFree(void **elem) {
    free(*elem);
}

void PrintIntVector(Vector *vec){
    if (!vec->size){
        printf("empty!\n");
    }
    else{
        for (size_t i = 0; i < vec->size; i++){
            Pair *pair = (Pair *)(vec->data[i]);
            if (i == vec->size - 1){
                printf("%d |\n", *(int *)(pair->value));
            }
            else{
                printf("|%d ", *(int *)(pair->value));
            }
        }
    }

}
/////

int main() {


    // Create Pairs.
    Pair *pairs[14];
    for (int k_i = 0; k_i < 14; ++k_i) {
        char key = (char) (((k_i) % 75) + 48);
        int value = k_i;
        pairs[k_i] = PairAlloc(&key, &value, CharKeyCpy, IntValueCpy,
                               CharKeyCmp,
                               IntValueCmp, CharKeyFree, IntValueFree);
    }

    // Create hash-map and inserts elements into it.
    // Uses the Char-Int pairs you received.
    HashMap *hash_map = HashMapAlloc(HashChar, PairCharIntCpy,
                                     PairCharIntCmp, PairCharIntFree);
    for (int kI = 0; kI < 12; ++kI) {
        HashMapInsert(hash_map, pairs[kI]);
    }

    for (int k = 0; k < hash_map->capacity; k ++){
        PrintIntVector(hash_map->buckets[k]);
    }

    HashMapClear(hash_map);
    printf("\n");

    for (int k = 0; k < hash_map->capacity; k ++){
        PrintIntVector(hash_map->buckets[k]);
    }

    printf("%lu", hash_map->size);


    // Free the pairs.
    for (int k_i = 0; k_i < 8; ++k_i) {
        PairFree(&pairs[k_i]);
    }
    // Free the hash-map.
    HashMapFree(&hash_map);


    return 0;
}
#endif