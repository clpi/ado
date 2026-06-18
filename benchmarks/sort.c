#include <stdio.h>
#include <stdlib.h>

typedef struct { int *data; int len; int cap; } Array;

static Array make_array(int *init, int c) {
    Array a; a.len = c; a.cap = c > 0 ? c : 4; a.data = malloc(a.cap * sizeof(int));
    for (int i = 0; i < c; i++) a.data[i] = init[i];
    return a;
}
static void push(Array *a, int v) {
    if (a->len >= a->cap) { a->cap = a->cap ? a->cap * 2 : 4; a->data = realloc(a->data, a->cap * sizeof(int)); }
    a->data[a->len++] = v;
}

static int cmp(const void *a, const void *b) {
    return (*(int*)a - *(int*)b);
}

int main(void) {
    int n = 5000;
    Array arr = make_array((int[]){}, 0);
    for (int i = 0; i < n; i++) push(&arr, n - i);
    qsort(arr.data, arr.len, sizeof(int), cmp);
    int ok = 1;
    for (int i = 0; i < n - 1; i++) {
        if (arr.data[i] > arr.data[i + 1]) { ok = 0; break; }
    }
    printf("sorted: %d\n", ok);
    return ok;
}
