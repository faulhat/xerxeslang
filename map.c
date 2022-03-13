#include <stdlib.h>
#include <string.h>

#include "parse.h"
#include "map.h"

// These "init" functions set all values in a struct to zero
value initValue()
{
    value newVal;
    newVal.type = 0;
    newVal.Gen.LongInt = 0;
    newVal.arrayLength = 0;
    newVal.Array = NULL;
    return newVal;
}

function initFunction()
{
    function newFunc;
    newFunc.body = initExpr(FUNCTION);
    newFunc.argumentNames = NULL;
    newFunc.argumentNumber = 0;
    return newFunc;
}

variable initVariable()
{
    variable newVar;
    newVar.name = NULL;
    newVar.hash = 0;
    newVar.Var.Value = initValue();
    return newVar;
}

map initMap()
{
    map newMap;
    newMap.hashLength = MAP_INIT_HASHLEN;
    newMap.totalBuckets = 1 << MAP_INIT_HASHLEN;
    newMap.usedBuckets = 0;
    newMap.Buckets = (bucket *) malloc(newMap.totalBuckets * sizeof(bucket));
    for (int i = 0; i < newMap.totalBuckets; ++i) {
        (newMap.Buckets + i)->length = 0;
        (newMap.Buckets + i)->contents = NULL;
    }

    return newMap;
}

scopes initScopes()
{
    scopes newStack;
    newStack.depth = 0;
    newStack.stack = NULL;
    return newStack;
}

uint32_t hashName(char *name)
{
    // Produce a hash for name
    uint32_t hash = 0;
    for (int i = 0; i < strlen(name); i++) {
        hash *= 11; // Multiply hash by a constant
        hash += name[i]; // Add the next character's value
    }

    return hash;
}

bucket addToBucket(bucket *target, variable newContent)
{
    // Adds a variable to a bucket and returns the result.
    // New contents with the same name as old contents overwrites the old contents.
    bucket newBucket;
    for (int i = 0; i < target->length; ++i) {
        if (strcmp((target->contents + i)->name, newContent.name) == 0) {
            newBucket.contents = (variable *) malloc(target->length * sizeof(variable));
            memcpy(newBucket.contents, target->contents, target->length * sizeof(variable));
            *(newBucket.contents + i) = newContent;
            return newBucket;
        }
    }

    newBucket.length = target->length + 1;
    newBucket.contents = (variable *) malloc(newBucket.length * sizeof(variable));
    memcpy(newBucket.contents, target->contents, target->length * sizeof(variable));
    newBucket.contents[newBucket.length - 1] = newContent;

    return newBucket;
}

void reorganize(map *target, int newHashLength)
{
    // Reorganizes a hash table into one of a different hash length.
    map temp = initMap();
    uint32_t mask = 0;
    mask = ~mask >> (32 - newHashLength);
    for (int i = 0; i < target->totalBuckets; ++i) {
        bucket thisBucket = *(target->Buckets + i);
        for (int j = 0; j < thisBucket.length; ++j) {
            variable content = *(thisBucket.contents + j);
            *(temp.Buckets + (content.hash & mask)) = addToBucket(temp.Buckets + (content.hash & mask), content);
        }
    }

    *target = temp;
}

void newVariable(map *destination, variable item)
{
    // Adds a new variable to a hash table, expanding the table if necessary.
    if (destination->usedBuckets > 7 * destination->totalBuckets / 8) {
        reorganize(destination, destination->hashLength + 1);
    }

    uint32_t mask = 0;
    mask = ~mask >> (32 - destination->hashLength);
    item.hash = hashName(item.name);
    destination->Buckets[item.hash & mask] = addToBucket(destination->Buckets + (item.hash & mask), item);
}

scopes plusScope(scopes currentStack, map newScope)
{
    // Adds a new scope to the stack of scopes.
    scopes newStack;
    newStack.depth = currentStack.depth + 1;
    newStack.stack = (map *) malloc(newStack.depth * sizeof(map));
    *(newStack.stack) = newScope;
    for (int i = 1; i < newStack.depth; ++i) {
        memcpy(newStack.stack + i, currentStack.stack + i - 1, sizeof(map));
    }

    return newStack;
}

// Get a variable from a map.
// Return a null pointer if the key is not in the map.
variable *mapGet(map *fromMap, char *key)
{
    uint32_t mask = 0;
    mask = ~mask >> (32 - fromMap->hashLength);
    int keyHash = hashName(key) & mask;
    bucket keyHashBucket = fromMap->Buckets[keyHash];
    for (int j = 0; j < keyHashBucket.length; ++j) {
        if (strcmp((keyHashBucket.contents + j)->name, key) == 0) {
            return keyHashBucket.contents + j;
        }
    }

    return NULL;
}

variable *getVariable(scopes currentStack, char *key)
{
    // Gets a variable from a stack of maps.
    for (int i = 0; i < currentStack.depth; ++i) {
        map *thisScope = currentStack.stack + i;
        variable *var_ptr = mapGet(thisScope, key);
        if (var_ptr) {
            return var_ptr;
        }
    }

    printf("Name Error: %s not found.\n", key);
    exit(-1);
}