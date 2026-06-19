#include <stdio.h>
#include <stdlib.h>

int main(void) {
    int n = 64, nn = n * n;
    int *a = malloc(nn * sizeof(int));
    int *b = malloc(nn * sizeof(int));
    int *c = malloc(nn * sizeof(int));
    for (int i = 0; i < nn; i++) {
        a[i] = i;
        b[i] = nn - i;
        c[i] = 0;
    }
    for (int i = 0; i < n; i++) {
        for (int j = 0; j < n; j++) {
            int sum = 0;
            for (int k = 0; k < n; k++) {
                sum += a[i * n + k] * b[k * n + j];
            }
            c[i * n + j] = sum;
        }
    }
    printf("result: %d %d\n", c[0], c[nn - 1]);
    return c[0];
}
