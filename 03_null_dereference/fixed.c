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
    return NULL;
}

int main() {
    Node* list = NULL;

    Node* result = find(list, 42);
    // Fix: always check for NULL before dereferencing
    if (result == NULL) {
        printf("Value not found in list.\n");
    } else {
        printf("Found value: %d\n", result->data);
    }

    return 0;
}
