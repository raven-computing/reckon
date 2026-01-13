#include <stdio.h>
#include <stdbool.h>

int main(void) {
    printf("Some text\n");
    bool b = false;
    int i = 42;
    switch(i) {
        case 0:
            break;
        case 1:
            b = true;
            break;
        default:
            ++i;
            break;
    }
    printf("%d\n", i);
    printf("%s\n", b ? "true" : "false");
    return 0;
}
