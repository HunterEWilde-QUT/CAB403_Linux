// Based on the Week 3 hashtable exercise
#include <inttypes.h> // for portable integer declarations
#include <stdbool.h>  // for bool type
#include <stdio.h>    // for printf()
#include <stdlib.h>   // for malloc(), free(), NULL
#include <string.h>   // for strcmp()
#include <pthread.h>

#define THREADS 4
#define BUCKETS 100
#define BUCKET_SIZE (BUCKETS / THREADS)

// An item inserted into a hash table.
// As hash collisions can occur, multiple items can exist in one bucket.
// Therefore, each bucket is a linked list of items that hashes to that bucket.
typedef struct item {
    char key[4];
    int value;
    struct item *next;
} item_t;

// A hash table mapping a string to an integer.
typedef struct htab {
    item_t **buckets;
    size_t size;
} htab_t;

typedef struct Hash {
    htab_t *h;
    int search;
    int start;
    int end;
} threadvals;

/* function prototypes */
void item_print(item_t *);
bool htab_init(htab_t *, size_t);
size_t djb_hash(char *);
size_t htab_index(htab_t *, char *);
item_t *htab_bucket(htab_t *, char *);
item_t *htab_find(htab_t *, char *);
bool htab_add(htab_t *, char *, int);
void htab_print(htab_t *);
void htab_delete(htab_t *, char *);
void htab_destroy(htab_t *);
void *htab_search_value(void *);

int main(int argc, char **argv) {
    // create a hash table with BUCKETS many buckets
    htab_t h;
    if (!htab_init(&h, BUCKETS)) {
        printf("Failed to initialise hash table\n");
        return EXIT_FAILURE;
    }

    // add 10000 items to hash table.
    char key[4];
    for (int i = 0; i < 10000; i++) {
        // Generate a unique key for each item
        key[0] = (i % 26) + 'A';
        key[1] = ((i / 26) % 26) + 'A';
        key[2] = ((i / 676) % 26) + 'A';
        key[3] = '\0';
        // Generate a value (0-99) for each item
        htab_add(&h, key, rand() % BUCKETS);
    }

    // Get search value from command line (0 if none provided)
    int search = 0;
    if (argc >= 2) search = atoi(argv[1]);

    pthread_t threads[THREADS];
    threadvals data[THREADS];
    for (int i = 0; i < THREADS; i++) {
        data[i].search = search;
        data[i].start = i * BUCKET_SIZE;
        data[i].end = data[i].start + BUCKET_SIZE;
        data[i].h = &h;
        pthread_create(&threads[i], NULL, htab_search_value, &data[i]);
    }
    htab_destroy(&h);
    return EXIT_SUCCESS;
}

void item_print(item_t *i) {
    printf("key=%s value=%d", i->key, i->value);
}

// Initialise a new hash table with n buckets.
// pre: true
// post: (return == false AND allocation of table failed)
//       OR (all buckets are null pointers)
bool htab_init(htab_t *h, size_t n) {
    h->size = n;
    h->buckets = (item_t **)calloc(n, sizeof(item_t *));
    return h->buckets != 0;
}

// The Bernstein hash function.
// A very fast hash function that works well in practice.
size_t djb_hash(char *s) {
    size_t hash = 5381;
    int c;
    while ((c = *s++) != '\0') {
        // hash = hash * 33 + c
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

// Calculate the offset for the bucket for key in hash table.
size_t htab_index(htab_t *h, char *key) {
    return djb_hash(key) % h->size;
}

// Find pointer to head of list for key in hash table.
item_t *htab_bucket(htab_t *h, char *key) {
    return h->buckets[htab_index(h, key)];
}

// Find an item for key in hash table.
// pre: true
// post: (return == NULL AND item not found)
//       OR (strcmp(return->key, key) == 0)
item_t *htab_find(htab_t *h, char *key) {
    for (item_t *i = htab_bucket(h, key); i != NULL; i = i->next) {
        if (strcmp(i->key, key)) {
            // found the key
            return i;
        }
    }
    return NULL;
}

// Add a key with value to the hash table.
// pre: htab_find(h, key) == NULL
// post: (return == false AND allocation of new item failed)
//       OR (htab_find(h, key) != NULL)
bool htab_add(htab_t *h, char *key, int value) {
    // allocate new item
    item_t *newhead = (item_t *)malloc(sizeof(item_t));
    if (newhead == NULL) {
        return false;
    }
    strncpy(newhead->key, key, 4);
    newhead->value = value;

    // hash key and place item in appropriate bucket
    size_t bucket = htab_index(h, key);
    newhead->next = h->buckets[bucket];
    h->buckets[bucket] = newhead;
    return true;
}

// Print the hash table.
// pre: true
// post: hash table is printed to screen
void htab_print(htab_t *h) {
    printf("hash table with %ld buckets\n", h->size);
    for (size_t i = 0; i < h->size; ++i) {
        printf("bucket %ld: ", i);
        if (h->buckets[i] == NULL) {
            printf("empty\n");
        } else {
            for (item_t *j = h->buckets[i]; j != NULL; j = j->next) {
                item_print(j);
                if (j->next != NULL) {
                    printf(" -> ");
                }
            }
            printf("\n");
        }
    }
    return;
}

// Delete an item with key from the hash table.
// pre: htab_find(h, key) != NULL
// post: htab_find(h, key) == NULL
void htab_delete(htab_t *h, char *key) {
    item_t *head = htab_bucket(h, key);
    item_t *current = head;
    item_t *previous = NULL;
    while (current != NULL) {
        if (strcmp(current->key, key) == 0) {
            if (previous == NULL) {
                // first item in list
                h->buckets[htab_index(h, key)] = current->next;
            }
            else
            {
                previous->next = current->next;
            }
            free(current);
            break;
        }
        previous = current;
        current = current->next;
    }
    return;
}

// Destroy an initialised hash table.
// pre: htab_init(h)
// post: all memory for hash table is released
void htab_destroy(htab_t *h) {
    // free linked lists
    for (size_t i = 0; i < h->size; ++i) {
        item_t *bucket = h->buckets[i];
        while (bucket != NULL) {
            item_t *next = bucket->next;
            free(bucket);
            bucket = next;
        }
    }

    // free buckets array
    free(h->buckets);
    h->buckets = NULL;
    h->size = 0;
    return;
}

// Iterates through each bucket of hashtable 'h', printing out keys with value 'search'
// pre: htab_init(h)
// post: each key with value 'search' has been printed to stdout
void *htab_search_value(void *ptr) {
    threadvals *data;
    data = ptr;
    for (size_t i = data->start; i < data->end; ++i) {
        for (item_t *bucket = data->h->buckets[i]; bucket != NULL; bucket = bucket->next) {
            if (bucket->value == data->search) {
                printf("%s ", bucket->key);
            }
        }
    }
    printf("\n");
    return;
}
