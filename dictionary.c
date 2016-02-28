/****************************************************************************
 * dictionary.c
 *
 * Computer Science 50
 * Problem Set 5
 *
 * Implements a dictionary's functionality.
 ***************************************************************************/

#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "dictionary.h"

// Prototypes
bool addword(char* word);
void addtolinkedlist(node* new, node* first);
int hash(char* key, char first, int length);
hashtable* makehash(int size);

// setup hash array, etc.
hashtable* abchash;
long long dictlen = 0;

/**
 * Returns true if word is in dictionary else false.
 */
bool check(const char* word)
{
    // prepare to search
    int len = strlen(word);
    // minisculize word as temp string before search
    char* temp = malloc((len + 1) * sizeof(char));
    for(int i = 0; i < len; i++)
        temp[i] = tolower(word[i]);
    temp[len] = '\0';
    // create pointer to appropriate part of hashtable
    int hashindex = hash(&temp[0], 'A', ABCLEN);
    node* ptr = abchash->table[hashindex];
    
    // search dictionary for word
    
    while (ptr != NULL)
    {
        if (strcmp(ptr->word,temp) == 0)
        {
            // exit with match
            free(temp);
            return true;
        }
        else
        {
            ptr = ptr->next;
        }
    }
    
    // exit with no match
    free(temp);
    return false;
}

/**
 * Loads dictionary into memory.  Returns true if successful else false.
 */
bool load(const char* dictionary)
{
    // open file at dictionary
    FILE* file = fopen(dictionary, "r");
    
    // check for unreadable file
    if (file == NULL)
        return false;
    // if readable
    else
    {
        // prepare for reading loop
        abchash = makehash(ABCLEN);
        char temp[LENGTH + 1];
        int wordlen = 0;
        
        // start reading loop until end of file
        for (int c = fgetc(file); c != EOF; c = fgetc(file))
        {
            // if end of word (\n)
            if (c == '\n')
            {
                // strip garbage from temp and add to dictionary
                char word[wordlen + 1];
                for (int i = 0; i < wordlen; i++)
                    word[i] = temp[i];
                word[wordlen] = '\0';
                addword(word);
                
                // prepare for next word
                dictlen++;
                wordlen = 0;
            }
            // read char of word   
            else
            {
                temp[wordlen] = c;
                wordlen++;
            }
            
        // end of file reached
        }
        
        // return successful reading
        return true;
    }
}

/**
 * Returns number of words in dictionary if loaded else 0 if not yet loaded.
 */
unsigned int size(void)
{
    return dictlen;
}

/**
 * Unloads dictionary from memory.  Returns true if successful else false.
 */
bool unload(void)
{
    // create node pointer and temp node
    node* ptr = NULL;
    node* temp = NULL;
    
    // iterate through table in abchash
    for (int i = 0; i < abchash->size; i++)
    {
        // prepare to free
        ptr = abchash->table[i];
        temp = NULL;
    
        // move through linked list, freeing nodes
        while (ptr != NULL)
        {
            temp = ptr;
            ptr = ptr->next;
            free(temp->word);
            free(temp);
        }
        
        // go to next value in hashtable
    }
    
    // free hashtable
    free(abchash->table); // TODO why isn't this working!
    free(abchash); // TODO why isn't this working!
    
/* I'm missing something with these free()s. Even after they're freed, they
still print a value other than NULL in GDB. Furthermore, valgrind tells me all
heap blocks are freed and that there are 0 errors. What am I missing? */
    
    return true;
}

/**
 * Creates new node to store word then adds node to linked list within
   hash. Returns success true/false.
 */

bool addword(char* word) 
{
    // Create node to store word
    node* new = malloc(sizeof(node));
    
    if (new == NULL)
    {
        return false;
    }

    // initialize new node
/* I was having troubles here with overwriting until I read
http://stackoverflow.com/questions/16432698/trouble-with-insertion-of-c-string
-into-linked-list. Pointers are still murky for me. */
    new->word = malloc(strlen(word)+1);
    strcpy(new->word, word);
    new->length = strlen(word);
    new->first = word[0];
    new->second = word[1];
    new->next = NULL;
    
    // find hash value
    int hashed = hash(&new->first, 'A', ABCLEN);
        
    // insert new in linked list within hashtable
    // if first word
    if (abchash->table[hashed] == NULL)
    {
        abchash->table[hashed] = new;
        return true;
    }
    // dictionary is alphabetized already so...
    // add word to end of dictionary
    else
    {
        node* predptr = abchash->table[hashed];
        while (true)
        {
            // if at end
            if (predptr->next == NULL)
            {
                predptr->next = new;
                return true;
            }
            // go to next word in dict
            predptr = predptr->next;
        }
    }   
}

/**
 * Returns a hash value given the hash table's first value and size. 
 */
int hash(char* key, char first, int length)
{
    // hash on first letter of string
    int hash = toupper(key[0]) - first;
    return hash % length;
}

/**
 * Creates a hashtable structure and initial entries. 
 */
hashtable* makehash(int size)
{
    // check for invalid size
    if (size < 1)
        return NULL;
    
    // allocate memory for hashtable
    hashtable* new = malloc(sizeof(hashtable));
    new->table = malloc(size * sizeof(node));
    
    // initialize elements
    new->size = size;
    for (int i = 0; i < size; i++)
    {
        new->table[i] = NULL;
    }
    
    return new;
}
