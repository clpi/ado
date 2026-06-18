#include <stdio.h>

int main(void) {
    long total = 0;
    for (long i = 0; i < 50000000; i++) {
        total += i;
    }
    printf("sum: %ld\n", total);
    return (int)total;
}
