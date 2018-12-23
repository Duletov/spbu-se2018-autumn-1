#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "md5.h"

#define MAX_SIZE_ARR 50
#define MAX_WORD_LENGTH 100

void printErrors(char *errorMessage, uint32_t numberError)
{
    fprintf(stderr, "%s", errorMessage);
    exit(numberError);
}

void deletePunctuation(char *word)
{
    uint32_t currPosLetter;
    char buffer[MAX_WORD_LENGTH];
    uint32_t posWithoutPunct = 0;

    for (currPosLetter = 0 ; currPosLetter <= strlen(word)-1; currPosLetter++)
    {
        buffer[currPosLetter] = word[currPosLetter];

        if( ( (buffer[currPosLetter] >= 'a') && (buffer[currPosLetter] <= 'z') ) ||
            ( (buffer[currPosLetter] >= 'A') && (buffer[currPosLetter] <= 'Z') ) ||
            ( ( (buffer[currPosLetter] == '-') || (buffer[currPosLetter] == '\'') ) &&
            ( ( (currPosLetter != 0) && (currPosLetter != strlen(word)-1) ) &&
                strlen(buffer) != 1) ) )
        {
            word[posWithoutPunct] = tolower(buffer[currPosLetter]);
            posWithoutPunct++;
        }
    }

    word[posWithoutPunct] = '\0';
}

typedef struct HashList
{
    char* key;
    uint32_t numberReplies;
    struct HashList *ptrNext;
}List;

typedef struct
{
    List **lists;
    uint32_t *lenghtList;
    uint32_t sizeOfTable;

}HashTable;


HashTable newHashTable(uint32_t size)
{
    HashTable newTable;
    newTable.lists = (List **) calloc(size, sizeof(List*));
    if (newTable.lists == NULL)
    {
        printErrors("Cannot allocate memory for creating hash table", 4);
    }
    newTable.sizeOfTable = size;
    newTable.lenghtList = (uint32_t *) calloc(size, sizeof(uint32_t));
    if (newTable.lenghtList == NULL)
    {
        printErrors("Cannot allocate memory for counter of Length each of lists", 4);
    }
    return newTable;

}

uint32_t getHash(char *key)
{
    uint32_t hashRes;
    uint32_t *res = (uint32_t *)calloc(4, sizeof(uint32_t));
    if (res == NULL)
    {
        printErrors("Cannot allocate memory for buffer of Hash", 4);
    }
    md5((uint8_t *)key, strlen(key), (uint8_t *)res);
    hashRes = res[0];
    free(res);
    return hashRes;
}

List *findWord(HashTable *table, uint32_t index, char* word )
{
    List *currElemList;
    currElemList = table->lists[index];
    while (currElemList != NULL)
    {
        if (strcmp(currElemList->key , word) == 0)
        {
            return currElemList;
        }
        currElemList = currElemList->ptrNext;
    }
    return currElemList;
}


void addWord(char *word, HashTable *table, uint32_t numberOfReplies)
{
    uint32_t index = getHash(word) % table->sizeOfTable;
    List *elemList;
    elemList = findWord(table, index, word);
    if (elemList != NULL)
    {
        elemList->numberReplies += numberOfReplies;
    }
    else
    {
        List *newElemList = malloc(sizeof(List));
        if (newElemList == NULL)
        {
            printErrors("Cannot allocate memory for adding new element of List", 4);
        }

        newElemList->key = (char *) malloc(strlen(word)*sizeof(uint32_t));
        if (newElemList->key == NULL)
        {
            printErrors("Cannot allocate memory for adding new word to the lists", 4);
        }
        strcpy(newElemList->key , word);
        newElemList->numberReplies = numberOfReplies;

        if (table->lists[index] == NULL)
        {
            newElemList->ptrNext = NULL;
            table->lists[index] = newElemList;
            table->lenghtList[index] = numberOfReplies;
        }
        else
        {
            newElemList->ptrNext = table->lists[index]->ptrNext;
            table->lists[index]->ptrNext = newElemList;
            table->lenghtList[index]++;
        }
    }

}



uint32_t getValueWord(HashTable *table, char* word)
{
    uint32_t index = getHash(word) % table->sizeOfTable;
    List *elemList;
    elemList = findWord(table, index, word);
    if (elemList == NULL)
    {
        printf("There is no here current word\n");
        return 0;
    }

    return elemList->numberReplies;

}
void getStat(HashTable *table)
{
    uint32_t maxNumberReplies = 0;
    uint32_t avgLengthList = 0;
    uint32_t amountLists = 0;
    uint32_t amountWords = 0;
    for (uint32_t i = 0; i < table->sizeOfTable; i++)
    {
        if (table->lenghtList[i] != 0)
        {
            avgLengthList += table->lenghtList[i];
            amountLists++;
        }

        List *currElemList = table->lists[i];
        while (currElemList != NULL)
        {
            uint32_t currNumberReplies = getValueWord(table, currElemList->key);
            amountWords ++;
            if (currNumberReplies > maxNumberReplies)
            {
                maxNumberReplies = currNumberReplies;
            }
            currElemList = currElemList->ptrNext;
        }
    }

    for (uint32_t i = 0; i < table->sizeOfTable; i++)
    {
        List *currElemList = table->lists[i];
        while (currElemList != NULL)
        {
            uint32_t currNumberReplies = getValueWord(table, currElemList->key);
            if (currNumberReplies >= maxNumberReplies)
            {
                fprintf(stderr, "%s %d\n", currElemList->key, currElemList->numberReplies);
            }
            currElemList = currElemList->ptrNext;
        }
    }
    //printf("Total words are %d\n", amountWords);
    //printf("Average length of Lists is %d", avgLengthList/amountLists);

}
void printWords(char *key, uint32_t numReplies)
{
    fprintf(stdout, "%s %d\n", key, numReplies);
}
void iterate(HashTable *table)
{
    for (uint32_t i = 0; i < table->sizeOfTable; i++)
    {
        List *currElemList = table->lists[i];
        while (currElemList != NULL)
        {
            uint32_t currNumberReplies = getValueWord(table, currElemList->key);
            printWords(currElemList->key, currNumberReplies);
            currElemList = currElemList->ptrNext;
        }
    }
}

void deleteTable(HashTable *table)
{
    for (uint32_t i = 0; i < table->sizeOfTable; i++)
    {
        List *currElemList = table->lists[i];
        while(currElemList != NULL)
        {
            List *buff = currElemList;
            currElemList = currElemList->ptrNext;
            free(buff->key);
            free(buff);
        }
    }
    free(table->lenghtList);
    free(table->lists);
}

int main(int argc, char *argv[])
{
    if (argc > 1 || argv[0] == NULL)
    {
        printErrors("Incorrect parameters input", 1);
    }
    /*FILE *bookFile;
    bookFile = fopen("book.txt", "r");
    if (NULL == bookFile)
    {
        printf("Error: cannot open file");
        return 1;
    }*/
    uint32_t numberOfReplies = 1;
    char word[MAX_WORD_LENGTH];
    HashTable table = newHashTable(MAX_SIZE_ARR);

    while (scanf("%s", word) != EOF)
    {
        if (strcmp(word, "endText") == 0)
        {
            break;
        }
        deletePunctuation(word);
        addWord(word, &table, numberOfReplies);
    }

    iterate(&table);
    getStat(&table);
    deleteTable(&table);


    return 0;
}

