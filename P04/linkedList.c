#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node *next;
} node;

void linkedListTraversal(node *);

int main() {
    node *n1;
    node *n2;
    node *n3;
    node *n4;

    // Allocate memory
    n1 = malloc(sizeof(node));
    n2 = malloc(sizeof(node));
    n3 = malloc(sizeof(node));
    n4 = malloc(sizeof(node));

    // Define node data
    n1->data = 1;
    n2->data = 2;
    n3->data = 3;
    n4->data = 4;

    // Link elements
    n1->next = n2;
    n2->next = n3;
    n3->next = n4;
    n4->next = NULL;

    linkedListTraversal(n1);

    return 0;
}

void linkedListTraversal(node *ptr) {
    printf("List contents:\n");
    while(ptr != NULL) {
        printf("Element: %d\n", ptr->data);
        ptr = ptr->next;
    }
}
