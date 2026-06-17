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
        // BUG: list goes out of scope here but memory is never freed
        // Every iteration leaks a Node
    }
    printf("Done — but we leaked 5 nodes worth of memory!\n");
    return 0;
}
