#include "hashset.h"
#include <assert.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

void HashSetNew(hashset *h, int elemSize, int numBuckets,
		HashSetHashFunction hashfn, HashSetCompareFunction comparefn, HashSetFreeFunction freefn){
    assert(elemSize > 0 && numBuckets > 0 && hashfn && comparefn);
    h->numBuckets = numBuckets;
    h->buckets = (vector *)malloc(numBuckets * sizeof(vector));
    assert(h->buckets);
    for(int i = 0; i < numBuckets; i++)
        VectorNew(h->buckets + i, elemSize, freefn, 0);
    h->elemSize = elemSize;
    h->hashfn = hashfn;
    h->comparefn = comparefn;
    h->freefn = freefn;
}

void HashSetDispose(hashset *h){
    for(int i = 0; i < h->numBuckets; i++)
        VectorDispose(h->buckets + i);
    free(h->buckets);
}

int HashSetCount(const hashset *h){
    int count = 0;
    for(int i = 0; i < h->numBuckets; i++)
        count += VectorLength(h->buckets + i);
    return count;
}

void HashSetMap(hashset *h, HashSetMapFunction mapfn, void *auxData){
    assert(mapfn);
    for(int i = 0; i < h->numBuckets; i++){
        VectorMap(h->buckets + i, mapfn, auxData);
    }
}

void HashSetEnter(hashset *h, const void *elemAddr){
    assert(elemAddr);
    int bucket_index = h->hashfn(elemAddr, h->numBuckets);
    assert(bucket_index >= 0 && bucket_index < h->numBuckets);
    vector *bucket = h->buckets + bucket_index;
    int position = VectorLowerBound(bucket, elemAddr, h->comparefn);
    if(position < VectorLength(bucket) &&
            h->comparefn(elemAddr, VectorNth(bucket, position)) == 0)
        VectorReplace(bucket, elemAddr, position);
    else
        VectorInsert(bucket, elemAddr, position);
}

void *HashSetLookup(const hashset *h, const void *elemAddr){
    assert(elemAddr);
    int bucket_index = h->hashfn(elemAddr, h->numBuckets);
    // fprintf(stdout, "bucket_index = %d\n", bucket_index);
    // fflush(stdout);
    assert(bucket_index >= 0 && bucket_index < h->numBuckets);
    int position = VectorSearch(h->buckets + bucket_index, elemAddr, h->comparefn, 0, 1);
    // fprintf(stdout, "position = %d\n", position);
    // fflush(stdout);
    if(position == -1)
        return NULL;
    else
        return VectorNth(h->buckets + bucket_index, position);
}
