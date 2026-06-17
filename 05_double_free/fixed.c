#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

void delete_list(Node** head) {
    Node* current = *head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
    *head = NULL; // Fix: null out after free
}

int main() {
    Node* list = (Node*)malloc(sizeof(Node));
    list->data = 99;
    list->next = NULL;

    printf("Node value: %d\n", list->data);

    delete_list(&list); // first free
    // Fix: list is now NULL so second call is safe
    if (list != NULL) {
        delete_list(&list);
    } else {
        printf("List already freed — skipping second delete.\n");
    }

    return 0;
}
