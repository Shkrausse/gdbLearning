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

void delete_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    // Bug: head is not set to NULL after this returns
}

int main() {
    Node* list = NULL;
    append(&list, 10);
    append(&list, 20);
    append(&list, 30);

    delete_list(list);

    // BUG: Use-after-free — list was already freed
    printf("Accessing list after deletion: %d\n", list->data);

    return 0;
}
