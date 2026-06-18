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
static int sum_arr(Array a) { int s = 0; for (int i = 0; i < a.len; i++) s += a.data[i]; return s; }
static void reverse_arr(Array a) { for (int i = 0; i < a.len / 2; i++) { int t = a.data[i]; a.data[i] = a.data[a.len - 1 - i]; a.data[a.len - 1 - i] = t; } }

int main(void) {
    int n = 20000;
    Array arr = make_array((int[]){}, 0);
    for (int i = 0; i < n; i++) push(&arr, i);
    int total = sum_arr(arr);
    reverse_arr(arr);
    int check = arr.data[0];
    for (int i = 0; i < 1000; i++) push(&arr, i);
    printf("total: %d first: %d final_len: %d\n", total, check, arr.len);
    return total;
}
