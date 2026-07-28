#include "../src/main.h"
#include "../src/utils.h"
#include <stdio.h>

// utils.h
static void t_str_to_lowercase(void) {
    char s[] = "StRiNg";
    str_to_lowercase(s, strlen(s));
    printf("%s\n", s);
}

// utils.h
static void t_time_functions(void) {
    char s[] = "2024";
    char s2[] = "2024-06-11";

    time_t r = time_t_from_iso_ymd(s);
    time_t r2 = time_t_from_iso_ymd(s);

    static size_t size = 256;
    char r_buff[size];
    format_time_t_year(r_buff, size, &r, true);
    char r2_buff[size];
    format_time_t_year(r2_buff, size, &r2, true);

    printf("year: %s, date: %s", r_buff, r2_buff);
}

// utils.h
static void t_expand_home_path(void) {
    char *p = expand_home_path("~/dotfiles/wezterm");
    printf("expanded path: %s\n", p);
    free(p);
}

// utils.h
static void t_read_entire_file(void) {
    FILE *f = fopen("./test.ani", "r");
    char *content = read_entire_file(f);
    printf("%s\n", content);
    free(content);
}

// utils.h
static void t_trim_str(void) {
    char s[] = "   string   /t";
    trim_str(s);
    assert(strcmp(s, "string"));
    printf("%s", s);
}

typedef void (*test_fn)(void);
static const test_fn tests[] = {
    [0] = NULL,
    [1] = t_str_to_lowercase,
    [2] = t_time_functions,
    [3] = t_expand_home_path,
    [4] = t_read_entire_file,
    [5] = t_trim_str,
};
void _run_test(int n) {
    size_t count = sizeof(tests) / sizeof(tests[0]);
    if (n > 0 && (size_t)n < count && tests[n]) {
        tests[n]();
        exit(0);
    }
}
