#include <stdio.h>

// BUG: No base case — recurses forever until stack overflows
int countdown(int n) {
    printf("Counting: %d\n", n);
    return countdown(n - 1);
}

int main() {
    countdown(10);
    return 0;
}
