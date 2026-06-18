#include <stdio.h>

int main(void) {
    int total = 0;
    for (int i = 0; i < 50000000; i++) {
        total += i;
    }
    printf("sum: %d\n", total);
    return total;
}
