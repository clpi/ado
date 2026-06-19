#include "ado_runtime.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <curl/curl.h>
#include <time.h>
#include <unistd.h>

AdoArray ado_make_array(int *init, int count) {
    AdoArray arr;
    arr.len = count;
    arr.cap = count > 0 ? count : 4;
    arr.data = malloc(arr.cap * sizeof(int));
    for (int i = 0; i < count; i++) {
        arr.data[i] = init[i];
    }
    return arr;
}

void ado_push(AdoArray *arr, int val) {
    if (arr->len >= arr->cap) {
        arr->cap = arr->cap ? arr->cap * 2 : 4;
        arr->data = realloc(arr->data, arr->cap * sizeof(int));
    }
    arr->data[arr->len++] = val;
}

int ado_abs(int x) {
    return x < 0 ? -x : x;
}

int ado_min(int a, int b) {
    return a < b ? a : b;
}

int ado_max(int a, int b) {
    return a > b ? a : b;
}

int ado_clamp(int x, int lo, int hi) {
    if (x < lo) return lo;
    if (x > hi) return hi;
    return x;
}

int ado_pow(int base, int exp) {
    int result = 1;
    while (exp) {
        if (exp & 1) result *= base;
        base *= base;
        exp >>= 1;
    }
    return result;
}

int ado_sign(int x) {
    return x > 0 ? 1 : (x < 0 ? -1 : 0);
}

int ado_is_even(int x) {
    return x % 2 == 0;
}

int ado_is_odd(int x) {
    return x % 2 != 0;
}

int ado_gcd(int a, int b) {
    while (b) {
        int t = b;
        b = a % b;
        a = t;
    }
    return a;
}

int ado_lcm(int a, int b) {
    return a / ado_gcd(a, b) * b;
}

int ado_factorial(int n) {
    int r = 1;
    for (int i = 2; i <= n; i++) r *= i;
    return r;
}

int ado_fib(int n) {
    int a = 0, b = 1;
    for (int i = 0; i < n; i++) {
        int t = b;
        b = a + b;
        a = t;
    }
    return a;
}

int ado_sum(AdoArray a) {
    int s = 0;
    for (int i = 0; i < a.len; i++) s += a.data[i];
    return s;
}

int ado_avg(AdoArray a) {
    return a.len ? ado_sum(a) / a.len : 0;
}

int ado_contains(AdoArray a, int val) {
    for (int i = 0; i < a.len; i++) {
        if (a.data[i] == val) return 1;
    }
    return 0;
}

int ado_count_if(AdoArray a, int val) {
    int c = 0;
    for (int i = 0; i < a.len; i++) {
        if (a.data[i] == val) c++;
    }
    return c;
}

int ado_find(AdoArray a, int val) {
    for (int i = 0; i < a.len; i++) {
        if (a.data[i] == val) return i;
    }
    return -1;
}

int ado_all(AdoArray a) {
    for (int i = 0; i < a.len; i++) {
        if (!a.data[i]) return 0;
    }
    return 1;
}

int ado_any(AdoArray a) {
    for (int i = 0; i < a.len; i++) {
        if (a.data[i]) return 1;
    }
    return 0;
}

int ado_pop(AdoArray *arr) {
    return arr->data[--arr->len];
}

int ado_reverse(AdoArray *arr) {
    for (int i = 0; i < arr->len / 2; i++) {
        int t = arr->data[i];
        arr->data[i] = arr->data[arr->len - 1 - i];
        arr->data[arr->len - 1 - i] = t;
    }
    return 0;
}

int ado_remove(AdoArray *arr, int idx) {
    for (int i = idx; i < arr->len - 1; i++) {
        arr->data[i] = arr->data[i + 1];
    }
    arr->len--;
    return 0;
}

int ado_insert(AdoArray *arr, int idx, int val) {
    if (arr->len >= arr->cap) {
        arr->cap = arr->cap ? arr->cap * 2 : 4;
        arr->data = realloc(arr->data, arr->cap * sizeof(int));
    }
    for (int i = arr->len; i > idx; i--) {
        arr->data[i] = arr->data[i - 1];
    }
    arr->data[idx] = val;
    arr->len++;
    return 0;
}

AdoArray ado_take(AdoArray a, int n) {
    int c = n < a.len ? n : a.len;
    AdoArray r = ado_make_array((int[]){}, c);
    for (int i = 0; i < c; i++) r.data[i] = a.data[i];
    return r;
}

AdoArray ado_drop(AdoArray a, int n) {
    int c = n < a.len ? a.len - n : 0;
    AdoArray r = ado_make_array((int[]){}, c);
    for (int i = 0; i < c; i++) r.data[i] = a.data[i + n];
    return r;
}

AdoArray ado_concat(AdoArray a, AdoArray b) {
    AdoArray r = ado_make_array((int[]){}, a.len + b.len);
    for (int i = 0; i < a.len; i++) r.data[i] = a.data[i];
    for (int j = 0; j < b.len; j++) r.data[a.len + j] = b.data[j];
    return r;
}

AdoArray ado_fill(int n, int v) {
    AdoArray a;
    a.len = n;
    a.cap = n > 0 ? n : 4;
    a.data = malloc(a.cap * sizeof(int));
    for (int i = 0; i < n; i++) a.data[i] = v;
    return a;
}

AdoArray ado_filter(AdoArray a, int v) {
    AdoArray r = ado_make_array((int[]){}, 0);
    for (int i = 0; i < a.len; i++) {
        if (a.data[i] != v) {
            ado_push(&r, a.data[i]);
        }
    }
    return r;
}

static size_t ado_http_write(void *contents, size_t size, size_t nmemb, void *userp) {
    size_t total = size * nmemb;
    char **buf = (char **)userp;
    size_t cur = *buf ? strlen(*buf) : 0;
    size_t new_total = cur + total;
    if (new_total >= 4096) total = 4096 - cur;
    if (total > 0) {
        *buf = realloc(*buf, new_total + 1);
        memcpy(*buf + cur, contents, total);
        (*buf)[new_total] = '\0';
    }
    return size * nmemb;
}

static long ado_http_perform(const char *url, const char *method, const char *post_data) {
    CURL *curl = curl_easy_init();
    if (!curl) return -1;
    char *resp = NULL;
    curl_easy_setopt(curl, CURLOPT_URL, url);
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, ado_http_write);
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &resp);
    curl_easy_setopt(curl, CURLOPT_FOLLOWLOCATION, 1L);
    curl_easy_setopt(curl, CURLOPT_MAXREDIRS, 3L);
    curl_easy_setopt(curl, CURLOPT_USERAGENT, "ado-runtime/1.0");
    if (method && strcmp(method, "POST") == 0) {
        curl_easy_setopt(curl, CURLOPT_POST, 1L);
        if (post_data) curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    } else if (method && strcmp(method, "PUT") == 0) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "PUT");
        if (post_data) curl_easy_setopt(curl, CURLOPT_POSTFIELDS, post_data);
    } else if (method && strcmp(method, "DELETE") == 0) {
        curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, "DELETE");
    } else {
        curl_easy_setopt(curl, CURLOPT_HTTPGET, 1L);
    }
    CURLcode rc = curl_easy_perform(curl);
    long code = -1;
    if (rc == CURLE_OK) curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &code);
    curl_easy_cleanup(curl);
    if (resp) {
        fprintf(stderr, "%s", resp);
        free(resp);
    }
    if (rc != CURLE_OK) {
        fprintf(stderr, "ado_http: %s: %s\n", url, curl_easy_strerror(rc));
        return -1;
    }
    return code;
}

int ado_http_get(const char *url) {
    long code = ado_http_perform(url, "GET", NULL);
    return (int)code;
}

int ado_http_post(const char *url, const char *body) {
    long code = ado_http_perform(url, "POST", body);
    return (int)code;
}

int ado_http_put(const char *url, const char *body) {
    long code = ado_http_perform(url, "PUT", body);
    return (int)code;
}

int ado_http_delete(const char *url) {
    long code = ado_http_perform(url, "DELETE", NULL);
    return (int)code;
}

int ado_http_status(const char *url) {
    long code = ado_http_perform(url, "GET", NULL);
    return (int)code;
}

/* OS module */
int ado_getenv(const char *name) {
    char *val = getenv(name);
    if (!val) return 0;
    printf("%s", val);
    return 1;
}

int ado_exit(int code) {
    exit(code);
    return 0;
}

/* IO module */
int ado_read_file(const char *path) {
    FILE *f = fopen(path, "r");
    if (!f) return -1;
    char buf[4096];
    size_t n;
    while ((n = fread(buf, 1, sizeof(buf) - 1, f))) {
        buf[n] = '\0';
        printf("%s", buf);
    }
    fclose(f);
    return 0;
}

int ado_write_file(const char *path, int content) {
    FILE *f = fopen(path, "w");
    if (!f) return -1;
    fprintf(f, "%d", content);
    fclose(f);
    return 0;
}

int ado_file_exists(const char *path) {
    FILE *f = fopen(path, "r");
    if (f) { fclose(f); return 1; }
    return 0;
}

/* STD module */
int ado_sleep(int ms) {
#ifdef _WIN32
    Sleep(ms);
#else
    usleep(ms * 1000);
#endif
    return 0;
}

int ado_time(void) {
    return (int)time(NULL);
}

int ado_random(int max) {
    return rand() % max;
}

/* Memory control */
int ado_capacity(AdoArray a) {
    return a.cap;
}

int ado_reserve(AdoArray *arr, int cap) {
    if (cap <= arr->cap) return 0;
    arr->data = realloc(arr->data, cap * sizeof(int));
    arr->cap = cap;
    return 0;
}

int ado_shrink_to_fit(AdoArray *arr) {
    if (arr->cap > arr->len) {
        arr->data = realloc(arr->data, arr->len * sizeof(int));
        arr->cap = arr->len;
    }
    return 0;
}

void *ado_alloc(int size) {
    return malloc(size);
}

void ado_free(void *ptr) {
    free(ptr);
}

/* Collections - higher order operations */
AdoArray ado_map(AdoArray a, int func_idx) {
    (void)func_idx;
    return a;
}

AdoArray ado_flat_map(AdoArray a, int func_idx) {
    (void)func_idx;
    return a;
}

AdoArray ado_flatten(AdoArray a) {
    AdoArray r;
    r.len = 0;
    r.cap = a.len > 0 ? a.len : 4;
    r.data = malloc(r.cap * sizeof(int));
    for (int i = 0; i < a.len; i++) {
        if (a.data[i] < 0) continue;
        if (r.len >= r.cap) {
            r.cap = r.cap ? r.cap * 2 : 4;
            r.data = realloc(r.data, r.cap * sizeof(int));
        }
        r.data[r.len++] = a.data[i];
    }
    return r;
}

AdoArray ado_group_by(AdoArray a, int key_func_idx) {
    (void)key_func_idx;
    return a;
}

static void ado_qsort(int *d, int lo, int hi) {
    if (lo >= hi) return;
    int pivot = d[hi];
    int i = lo - 1;
    for (int j = lo; j < hi; j++) {
        if (d[j] <= pivot) {
            i++;
            int t = d[i]; d[i] = d[j]; d[j] = t;
        }
    }
    int t = d[i + 1]; d[i + 1] = d[hi]; d[hi] = t;
    ado_qsort(d, lo, i);
    ado_qsort(d, i + 2, hi);
}

int ado_sort(AdoArray *a) {
    if (a->len > 1) ado_qsort(a->data, 0, a->len - 1);
    return 0;
}

AdoArray ado_bsort(AdoArray a) {
    int n = a.len;
    int *b = malloc(n * sizeof(int));
    memcpy(b, a.data, n * sizeof(int));
    for (int i = 0; i < n - 1; i++) {
        for (int j = 0; j < n - i - 1; j++) {
            if (b[j] > b[j + 1]) {
                int t = b[j];
                b[j] = b[j + 1];
                b[j + 1] = t;
            }
        }
    }
    AdoArray r;
    r.data = b;
    r.len = n;
    r.cap = n;
    return r;
}

AdoArray ado_unique(AdoArray a) {
    if (a.len <= 1) {
        AdoArray r = ado_make_array((int[]){}, a.len);
        for (int i = 0; i < a.len; i++) r.data[i] = a.data[i];
        r.len = a.len;
        return r;
    }
    ado_sort(&a);
    AdoArray r = ado_make_array((int[]){}, a.len);
    r.data[r.len++] = a.data[0];
    for (int i = 1; i < a.len; i++) {
        if (a.data[i] != a.data[i - 1]) {
            r.data[r.len++] = a.data[i];
        }
    }
    return r;
}

AdoArray ado_reduce(AdoArray a, int init, int func_idx) {
    (void)func_idx;
    int r = init;
    for (int i = 0; i < a.len; i++) {
        r = r + a.data[i];
    }
    return ado_make_array((int[]){r}, 1);
}

/* Metaprogramming */
int ado_typeof(AdoArray a) {
    (void)a;
    return 0;
}

int ado_sizeof(int x) {
    return sizeof(x);
}

int ado_reflect(AdoArray a) {
    printf("Array(len=%d, cap=%d)", a.len, a.cap);
    return 0;
}

int ado_exec(const char *code) {
    (void)code;
    return -1;
}
