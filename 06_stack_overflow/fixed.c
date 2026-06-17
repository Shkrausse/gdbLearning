#include <stdio.h>

// Fix: base case stops the recursion at 0
int countdown(int n) {
    if (n < 0) {
        printf("Done!\n");
        return 0;
    }
    printf("Counting: %d\n", n);
    return countdown(n - 1);
}

int main() {
    countdown(10);
    return 0;
}
