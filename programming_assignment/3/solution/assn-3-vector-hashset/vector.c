#include "vector.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>

void VectorNew(vector *v, int elemSize, VectorFreeFunction freeFn, int initialAllocation){
    assert(elemSize > 0);
    assert(initialAllocation >= 0);
    v->elemSize = elemSize;
    v->logLength = 0;
    v->allocLength = initialAllocation == 0 ? 4 : initialAllocation;
    v->freeFn = freeFn;
    v->elems = malloc(v->allocLength * elemSize);
    assert(v->elems);
}

void VectorDispose(vector *v){
    if(v->freeFn){
        for(int i = 0; i < v->logLength; i++)
            v->freeFn((char *)(v->elems) + i * v->elemSize);
    }
    free(v->elems);
}

void VectorGrowIfNeeded(vector *v){
    // fprintf(stdout, "allocLength = %d, logLength = %d\n", v->allocLength, v->logLength);
    if(v->allocLength == v->logLength){
        v->allocLength *= 2;
        v->elems = realloc(v->elems, v->allocLength * v->elemSize);
        assert(v->elems);
    }
}

int VectorLength(const vector *v){
    return v->logLength;
}

void *VectorNth(const vector *v, int position){
    assert(position >= 0 && position < v->logLength);
    return (void *)((char *)(v->elems) + position * v->elemSize);
}

void VectorReplace(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position < v->logLength);
    if(v->freeFn)
        v->freeFn((char *)(v->elems) + position * v->elemSize);
    memcpy((char *)(v->elems) + position * v->elemSize, elemAddr, v->elemSize);
}

void VectorInsert(vector *v, const void *elemAddr, int position){
    assert(position >= 0 && position <= v->logLength);
    VectorGrowIfNeeded(v);
    memmove((char *)(v->elems) + (position + 1) * v->elemSize,
            (char *)(v->elems) + position * v->elemSize,
            (v->logLength - position) * v->elemSize);
    memcpy((char *)(v->elems) + position * v->elemSize, elemAddr, v->elemSize);
    v->logLength++;
}

void VectorAppend(vector *v, const void *elemAddr){
    VectorGrowIfNeeded(v);
    memcpy((char *)(v->elems) + v->logLength * v->elemSize, elemAddr, v->elemSize);
    v->logLength++;
}

void VectorDelete(vector *v, int position){
    assert(position >= 0 && position < v->logLength);
    if(v->freeFn)
        v->freeFn((char *)(v->elems) + position * v->elemSize);
    memmove((char *)(v->elems) + position * v->elemSize,
            (char *)(v->elems) + (position + 1) * v->elemSize,
            (v->logLength - 1 - position) * v->elemSize);
    v->logLength--;
}

void VectorSort(vector *v, VectorCompareFunction compare){
    assert(compare);
    qsort(v->elems, v->logLength, v->elemSize, compare);
}

void VectorMap(vector *v, VectorMapFunction mapFn, void *auxData){
    assert(mapFn);
    for(int i = 0; i < v->logLength; i++){
        mapFn((char *)(v->elems) + i * v->elemSize, auxData);
    }
}

int VectorSearch(const vector *v, const void *key, VectorCompareFunction searchFn, int startIndex,
        bool isSorted){
    assert(startIndex >= 0 && startIndex <= v->logLength);
    assert(key);
    if(isSorted){
        void *match = bsearch(key, (char *)(v->elems) + startIndex * v->elemSize,
                v->logLength - startIndex, v->elemSize, searchFn);
        if(!match)
            return -1;
        return ((char *)match - (char *)(v->elems)) / v->elemSize;
    }
    for(int i = 0; i < v->logLength; i++){
        if(searchFn(key, (char *)(v->elems) + i * v->elemSize) == 0)
            return i;
    }
    return -1;
}

int VectorLowerBound(vector *v, const void *elemAddr, VectorCompareFunction comparefn){
    int i = 0;
    int j = v->logLength;
    int mid;
    while(i < j){
        mid = (i + j) / 2;
        if(comparefn((char *)v->elems + mid * v->elemSize, elemAddr) < 0)
            i = mid + 1;
        else
            j = mid;
    }
    return j;
}
