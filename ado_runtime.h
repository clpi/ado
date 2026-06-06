#ifndef ADO_RUNTIME_H
#define ADO_RUNTIME_H

#include <stdlib.h>
#include <string.h>

typedef struct {
    int *data;
    int len;
    int cap;
} AdoArray;

AdoArray ado_make_array(int *init, int count);
void ado_push(AdoArray *arr, int val);

int ado_abs(int x);
int ado_min(int a, int b);
int ado_max(int a, int b);
int ado_clamp(int x, int lo, int hi);
int ado_pow(int base, int exp);
int ado_sign(int x);
int ado_is_even(int x);
int ado_is_odd(int x);
int ado_gcd(int a, int b);
int ado_lcm(int a, int b);
int ado_factorial(int n);
int ado_fib(int n);
int ado_sum(AdoArray a);
int ado_avg(AdoArray a);
int ado_contains(AdoArray a, int val);
int ado_count_if(AdoArray a, int val);
int ado_find(AdoArray a, int val);
int ado_all(AdoArray a);
int ado_any(AdoArray a);
int ado_pop(AdoArray *arr);
int ado_reverse(AdoArray *arr);
int ado_remove(AdoArray *arr, int idx);
int ado_insert(AdoArray *arr, int idx, int val);
AdoArray ado_take(AdoArray a, int n);
AdoArray ado_drop(AdoArray a, int n);
AdoArray ado_concat(AdoArray a, AdoArray b);
AdoArray ado_fill(int n, int v);
AdoArray ado_filter(AdoArray a, int v);

int ado_http_get(const char *url);
int ado_http_post(const char *url, const char *body);
int ado_http_put(const char *url, const char *body);
int ado_http_delete(const char *url);
int ado_http_status(const char *url);

/* OS module */
int ado_getenv(const char *name);
int ado_exit(int code);

/* IO module */
int ado_read_file(const char *path);
int ado_write_file(const char *path, int content);
int ado_file_exists(const char *path);

/* STD module */
int ado_sleep(int ms);
int ado_time();
int ado_random(int max);

/* Memory control */
int ado_capacity(AdoArray a);
int ado_reserve(AdoArray *arr, int cap);
int ado_shrink_to_fit(AdoArray *arr);
void *ado_alloc(int size);
void ado_free(void *ptr);

/* Collections - higher order operations */
AdoArray ado_map(AdoArray a, int func_idx);
AdoArray ado_flat_map(AdoArray a, int func_idx);
AdoArray ado_flatten(AdoArray a);
AdoArray ado_group_by(AdoArray a, int key_func_idx);
int ado_sort(AdoArray *a);
AdoArray ado_bsort(AdoArray a);
AdoArray ado_unique(AdoArray a);
AdoArray ado_reduce(AdoArray a, int init, int func_idx);

/* Metaprogramming */
int ado_typeof(AdoArray a);
int ado_sizeof(int x);
int ado_reflect(AdoArray a);
int ado_exec(const char *code);

#endif
