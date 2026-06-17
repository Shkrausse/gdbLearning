#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

Node* find(Node* head, int val) {
    while (head != NULL) {
        if (head->data == val) return head;
        head = head->next;
    }
    return NULL; // not found
}

int main() {
    Node* list = NULL; // empty list

    // BUG: find() returns NULL because list is empty,
    // then we dereference the NULL pointer
    Node* result = find(list, 42);
    printf("Found value: %d\n", result->data);

    return 0;
}
