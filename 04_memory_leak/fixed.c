#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

void append(Node** head, int val) {
    Node* new_node = (Node*)malloc(sizeof(Node));
    new_node->data = val;
    new_node->next = *head;
    *head = new_node;
}

int main() {
    for (int i = 0; i < 5; i++) {
        Node* list = NULL;
        append(&list, i * 10);
        printf("Created node with value: %d\n", list->data);
        // Fix: free the memory before it goes out of scope
        free(list);
        list = NULL;
    }
    printf("Done — no memory leaked!\n");
    return 0;
}
