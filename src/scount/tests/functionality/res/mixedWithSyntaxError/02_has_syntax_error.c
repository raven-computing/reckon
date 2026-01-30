//
// This source code file contains a syntax error.
//

#include <stdio.h>

void someFunction(int x) {
    printf("This function is missing a closing brace\n");
    return x + 1;
    // Syntax error below: missing closing brace

int main(int argc, char** argv) {
    printf("Hello\n");
    int i = someFunction(i);
    printf("i=%d\n", i);
    return 0;
}
