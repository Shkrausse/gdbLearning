#include <stdio.h>
#include <string.h>

void copy_input(char* input) {
    char buffer[8];
    // Fix: strncpy limits the number of characters copied
    strncpy(buffer, input, sizeof(buffer) - 1);
    buffer[sizeof(buffer) - 1] = '\0'; // always null terminate
    printf("Buffer contains: %s\n", buffer);
}

int main() {
    char* long_input = "ThisStringIsWayTooLongForTheBuffer";
    copy_input(long_input);
    return 0;
}
