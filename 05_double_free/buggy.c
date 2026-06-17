#include <stdio.h>
#include <stdlib.h>

typedef struct Node {
    int data;
    struct Node* next;
} Node;

void delete_list(Node* head) {
    Node* current = head;
    while (current != NULL) {
        Node* next = current->next;
        free(current);
        current = next;
    }
}

int main() {
    Node* list = (Node*)malloc(sizeof(Node));
    list->data = 99;
    list->next = NULL;

    printf("Node value: %d\n", list->data);

    delete_list(list);  // first free — correct
    delete_list(list);  // BUG: double free — list already freed!

    return 0;
}
