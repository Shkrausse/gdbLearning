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

void delete_list(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL; // Fix: null out the caller's pointer
}

int main() {
    Node* list = NULL;
    append(&list, 10);
    append(&list, 20);
    append(&list, 30);

    int saved = list->data; // Fix: read BEFORE freeing
    delete_list(&list);
    printf("Data before deletion: %d\n", saved);

    if (list == NULL) {
        printf("List successfully deleted.\n");
    }

    return 0;
}
