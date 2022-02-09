#include "HashMap.h"

#define CHECK_ERROR(expression, error) if (!(expression)){\
return error;                                           \
}
#define LOAD_FACTOR(size, capacity) ((double)(size)/(double)(capacity))
#define HASH(hash_map, key) (((hash_map)->hash_func(key)) & ((hash_map)->capacity - 1))

// ------ Functions of my own ------

/**
 * The function overrides a pair with another pair with the same key
 * @param pHashMap (struct HashMap **) pointer to pointer to dynamically allocated hash map
 * @param cpyPair (struct Pair *) pointer to the pair which we want to run over another
 * pair with if required (if keys are the same)
 * @param key the key of the pair
 */
static void RunOverIfRequired(HashMap **hash_map, Pair *pair, size_t key){
    for (size_t i = 0; i < (*hash_map)->buckets[key]->size; i ++){
        Pair *p_pair = (Pair *)(VectorAt((*hash_map)->buckets[key], i));
        if (p_pair->key_cmp(p_pair->key, pair->key)){
            // if keys are identical, then we copy "pair" and put the copied pair instead
            (*hash_map)->buckets[key]->data[i] = (*hash_map)->pair_cpy(pair);
            PairFree(&p_pair);
        }
    }
}


/**
 * This function is called through HashMapInsert or HashMapErase
 * only if change in the capacity of the hash map is required.
 * @param pMap (struct HashMap **) pointer to pointer to dynamically allocated hash map
 * @param IncOrDec an integer
 * if IncOrDec ==  1, the function INCREASES the capacity and rehashes the pairs accordingly,
 * and returns 1 for success, 0 otherwise.
 * if IncOrDec == -1, the function DECREASES the capacity and rehashes the pairs accordingly,
 * and returns 1 for success, 0 otherwise.
 */
static void HashMapReHash(HashMap **hash_map, int IncOrDec){
    // If we wish to increase the capacity, it means we called
    // this function through HashMapInsert, therefore we increased the hash map's
    // size but we didnt insert the new pair yet, so the size of pair should be hash map's size minus 1.
    // If we wish to decrease the capacity, it means we called
    // this function through HashMapErase, and in there we decreased the size of the
    // hash map (already erased the pair), so the size in this case
    // would be the size of the hash map.
    size_t size_pairs;
    if (IncOrDec == 1){
        size_pairs = (*hash_map)->size - 1;
    }
    else{
        size_pairs = (*hash_map)->size;
    }

    // 1. Copy the pairs in the hash map to a pairs array
    Pair **pairs = calloc(1, sizeof(Pair *) * size_pairs);

    size_t ind = 0;
    for (size_t i = 0; i < (*hash_map)->capacity; i ++) {
        if ((*hash_map)->buckets[i]->size){
            for (size_t j = 0; j < (*hash_map)->buckets[i]->size; j ++){
                pairs[ind] = (Pair *)((*hash_map)->pair_cpy((*hash_map)->buckets[i]->data[j]));
                ind++;
            }
        }
        VectorClear((*hash_map)->buckets[i]);
    }


    // 2. Increase/Decrease the capacity of the new hash map according to IncOrDec value
    Vector **tmp;
    if (IncOrDec == 1){
        (*hash_map)->capacity *= HASH_MAP_GROWTH_FACTOR;
        tmp = realloc((*hash_map)->buckets,
                                    sizeof(Vector *) * (*hash_map)->capacity);
        (*hash_map)->buckets = tmp;

        // 3(a). Allocating memory for the new vectors added
        for (size_t x = (*hash_map)->capacity / HASH_MAP_GROWTH_FACTOR; x < (*hash_map)->capacity; x ++){
            (*hash_map)->buckets[x] = VectorAlloc((*hash_map)->pair_cpy,
                                                  (*hash_map)->pair_cmp,
                                                  (*hash_map)->pair_free);
            if (!(*hash_map)->buckets[x]){
                return;
            }
        }
    }

    else{
        if ((*hash_map)->capacity >= 2){
            // 3(b). Freeing the memory of the vectors that are not part of the hash map anymore
            for (size_t y = (*hash_map)->capacity / HASH_MAP_GROWTH_FACTOR; y < (*hash_map)->capacity; y ++){
                VectorFree(&((*hash_map)->buckets[y]));
            }

            (*hash_map)->capacity /= HASH_MAP_GROWTH_FACTOR;
            tmp = realloc
                    ((*hash_map)->buckets, sizeof(Vector *) * (*hash_map)->capacity);
            (*hash_map)->buckets = tmp;

        }
    }

    // 4. Hash the pairs to the new hash map with the new capacity
    size_t key;
    for (size_t k = 0; k < size_pairs; k ++){
        key = HASH((*hash_map), (pairs[k]->key));
        VectorPushBack((*hash_map)->buckets[key], pairs[k]);
    }

    // 5. free the pairs we allocated here
    //    (because they are being copied again in VectorPushBack)
    for (size_t k_i = 0; k_i < size_pairs; ++k_i) {
        PairFree(&pairs[k_i]);
    }
    free(pairs);
}

// ------ End of my own functions ------

// ------ Start of the header functions ------

HashMap *HashMapAlloc(
        HashFunc hash_func, HashMapPairCpy pair_cpy,
        HashMapPairCmp pair_cmp, HashMapPairFree pair_free)
{
    // Checking if the functions are valid
    CHECK_ERROR(hash_func && pair_cpy && pair_cmp && pair_free, NULL)

    // Checking if allocation of the hash map succeeded
    HashMap *hash = calloc(1, sizeof(HashMap));
    CHECK_ERROR(hash, NULL)

    // memory allocation for the hash map buckets
    hash->buckets = calloc(1, sizeof(Vector *) * HASH_MAP_INITIAL_CAP);
    CHECK_ERROR(hash->buckets, NULL)

    // allocate memory for vector in each bucket
    for (size_t i = 0; i < HASH_MAP_INITIAL_CAP; i ++){
        hash->buckets[i] = VectorAlloc(pair_cpy, pair_cmp, pair_free);
        CHECK_ERROR(hash->buckets, NULL)
    }

    hash->capacity = HASH_MAP_INITIAL_CAP;
    hash->hash_func = hash_func;
    hash->pair_cpy = pair_cpy;
    hash->pair_cmp = pair_cmp;
    hash->pair_free = pair_free;

    return hash;
}

void HashMapFree(HashMap **p_hash_map){
    if (p_hash_map && *p_hash_map){
        for (size_t i = 0; i < (*p_hash_map)->capacity; i ++){
            VectorFree(&((*p_hash_map)->buckets[i]));
        }
        free((*p_hash_map)->buckets);
        free(*p_hash_map);
        *p_hash_map = NULL;
    }
}

int HashMapInsert(HashMap *hash_map, Pair *pair){
    CHECK_ERROR(hash_map && pair, 0)
    // check if key already exists in the hash map
    // if so, run it over and insert the new pair instead in its place
    if (HashMapContainsKey(hash_map, pair->key)){
       RunOverIfRequired(&hash_map, pair, HASH(hash_map, pair->key));
       return 1;
    }

    hash_map->size++;

    // if load factor is higher then the maximum load factor after increasing size
    if (HashMapGetLoadFactor(hash_map) > HASH_MAP_MAX_LOAD_FACTOR){
        HashMapReHash(&hash_map, 1);
    }

    size_t key = HASH(hash_map, pair->key);
    VectorPushBack(hash_map->buckets[key], pair);


    return 1;
}

int HashMapContainsKey(HashMap *hash_map, KeyT key){
    // if hash map is NULL
    CHECK_ERROR(hash_map, 0)
    size_t i = HASH(hash_map, key);

    // if the hash(key)'th bucket has no values in it yet
    if (!hash_map->buckets[i]->size){
        return 0;
    }

    else{
        for (size_t j = 0; j < hash_map->buckets[i]->size; j++){
            Pair *pair = (Pair *)(hash_map->buckets[i]->data[j]);
            if (pair->key_cmp(pair->key, key)){
                return 1;
            }
        }
        return 0;
    }
}

int HashMapContainsValue(HashMap *hash_map, ValueT value){
    // if hash map is NULL
    CHECK_ERROR(hash_map, 0)

    for (size_t i = 0; i < hash_map->capacity; i ++){
        if (hash_map->buckets[i]->size){
            for (size_t j = 0; j < hash_map->buckets[i]->size; j ++){
                Pair *pair = (Pair *)(hash_map->buckets[i]->data[j]);
                if (pair->value_cmp(pair->value, value)){
                    return 1;
                }
            }
        }
    }

    return 0;
}

ValueT HashMapAt(HashMap *hash_map, KeyT key){
    CHECK_ERROR(hash_map, NULL)
    size_t i = HASH(hash_map, key);

    // if the hash(key)'th bucket has no values in it yet
    if (!hash_map->buckets[i]->size){
        return NULL;
    }

    else{
        for (size_t j = 0; j < hash_map->buckets[i]->size; j ++){
            Pair *pair = (Pair *)(hash_map->buckets[i]->data[j]);
            if (pair->key_cmp(pair->key, key)){
                return pair->value;
            }
        }
        return NULL;
    }
}

int HashMapErase(HashMap *hash_map, KeyT key){
    // if hash map is NULL
    CHECK_ERROR(hash_map, 0)

    size_t i = HASH(hash_map, key);

    // if the hash(key)'th bucket has no values in it yet
    if (!hash_map->buckets[i]->size){
        return 0;
    }

    else {
        int erased = 0;
        for (size_t j = 0; j < hash_map->buckets[i]->size; j++) {
            Pair *pair = (Pair *) (hash_map->buckets[i]->data[j]);
            if (pair->key_cmp(pair->key, key)) {
                erased = VectorErase(hash_map->buckets[i], j);
                break;
            }
        }

        if (!erased){
            return 0;
        }

        hash_map->size--;

        if (HashMapGetLoadFactor(hash_map) < HASH_MAP_MIN_LOAD_FACTOR) {
            HashMapReHash(&hash_map, -1);
        }
        return 1;
    }
}

double HashMapGetLoadFactor(HashMap *hash_map){
    CHECK_ERROR(hash_map, -1)
    return LOAD_FACTOR(hash_map->size, hash_map->capacity);
}

void HashMapClear(HashMap *hash_map){
    if (hash_map){
        int res = 0;
        int i =(int)(hash_map->capacity - 1);
        while (i >= 0){
            // for every non empty bucket (vector)
            if (hash_map->buckets[i]->size){
                for (size_t j = 0; j < hash_map->buckets[i]->size; j++){
                    Pair *pair = (Pair *)(hash_map->buckets[i]->data[j]);
                    res = HashMapErase(hash_map, pair->key);
                    if (!res){
                        return;
                    }
                }
            }
            i --;
        }
    }
}
