#include <stdio.h>
#include <string.h>

void copy_input(char* input) {
    char buffer[8];
    // BUG: strcpy does not check the length of input
    // If input > 8 chars, this overflows the buffer
    strcpy(buffer, input);
    printf("Buffer contains: %s\n", buffer);
}

int main() {
    char* long_input = "ThisStringIsWayTooLongForTheBuffer";
    copy_input(long_input);
    return 0;
}
