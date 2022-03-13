#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "pointers.h"
#include "interpret.h"

value fileOpenBinary(value fileName, value mode)
{
    FILE *newFilePointer = NULL;
    if (strcmp(mode.Gen.String, "r") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "rb");
    }
    else if (strcmp(mode.Gen.String, "w") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "wb");
    }
    else if (strcmp(mode.Gen.String, "a") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "ab");
    }
    else if (strcmp(mode.Gen.String, "rw") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "w+b");
    }
    else if (strcmp(mode.Gen.String, "ra") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "a+b");
    }
    else {
        printf("Unrecognized file open mode.");
        exit(-1);
    }
    if (!newFilePointer) {
        printf("fopenb: Error: File not found.\n");
        exit(-1);
    }
    value returnVal = initValue();
    returnVal.type = SYS_FILE;
    returnVal.Gen.filestream = newFilePointer;
    return returnVal;
}

value fileOpenText(value fileName, value mode)
{
    FILE *newFilePointer = NULL;
    if (strcmp(mode.Gen.String, "r") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "r");
    }
    else if (strcmp(mode.Gen.String, "w") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "w");
    }
    else if (strcmp(mode.Gen.String, "a") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "a");
    }
    else if (strcmp(mode.Gen.String, "rw") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "w+");
    }
    else if (strcmp(mode.Gen.String, "ra") == 0) {
        newFilePointer = fopen(fileName.Gen.String, "a+");
    }
    else {
        printf("Unrecognized file open mode.");
        exit(-1);
    }
    if (!newFilePointer) {
        printf("fopent: Error: File not found.\n");
        exit(-1);
    }
    value returnVal = initValue();
    returnVal.type = SYS_FILE;
    returnVal.Gen.filestream = newFilePointer;
    return returnVal;
}

void writeToFile(value file, value material)
{
    int fileWrite = fputs(material.Gen.String, file.Gen.filestream);
    if (fileWrite < 0) {
        printf("write: Error: Failed to write to file.\n");
        exit(-1);
    }
}

void writeToFileInt(value file, value material)
{
    char *thisInt = (char *) calloc(1, sizeof(char));
    *thisInt = material.Gen.Integer;
    int fileWrite = fputs(thisInt, file.Gen.filestream);
    if (fileWrite < 0) {
        printf("write: Error: Failed to write to file.\n");
        exit(-1);
    }
}

value readFromFile(value file, value n)
{
    char *buffer = (char *) calloc(n.Gen.Integer + 1, sizeof(char));
    fgets(buffer, n.Gen.Integer, file.Gen.filestream);
    value returnVal = initValue();
    returnVal.type = STRING;
    returnVal.Gen.String = (char *) calloc(strlen(buffer) + 1, sizeof(char));
    memcpy(returnVal.Gen.String, buffer, strlen(buffer));
    return returnVal;
}

value readFromFileDelim(value file, value delimiter)
{
    char *buffer = NULL;
    size_t len = 0;
    ssize_t bytes_read = getdelim(&buffer, &len, delimiter.Gen.Integer, file.Gen.filestream);
    printf("readFromFile read %zd bytes.\n", bytes_read);
    value returnVal = initValue();
    returnVal.type = STRING;
    returnVal.Gen.String = (char *) calloc(strlen(buffer) + 1, sizeof(char));
    memcpy(returnVal.Gen.String, buffer, strlen(buffer));
    return returnVal;
}
