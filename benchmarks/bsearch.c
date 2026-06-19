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

static int binary_search(Array arr, int target) {
    int lo = 0, hi = arr.len - 1;
    while (lo <= hi) {
        int mid = (lo + hi) / 2;
        if (arr.data[mid] == target) return mid;
        if (arr.data[mid] < target) lo = mid + 1;
        else hi = mid - 1;
    }
    return -1;
}

int main(void) {
    int n = 100000;
    Array arr = make_array((int[]){}, 0);
    for (int i = 0; i < n; i++) push(&arr, i);
    int total = 0;
    for (int i = 0; i < 50000; i++) {
        int idx = i * 2;
        int found = binary_search(arr, idx);
        if (found >= 0) total += found;
    }
    printf("total: %d\n", total);
    return (int)total;
}
