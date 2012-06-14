
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "natsort.h"

static int
cmp(const void *p1, const void *p2) {
    return strcmp(* (char * const *) p1, * (char * const *) p2);
}

static int
cmp_alt2(const void *p1, const void *p2) {
    char *s1 = *(char * const *) p1;
    char *s2 = *(char * const *) p2;
    while (1) {
        char c1 = *(s1++);
        char c2 = *(s2++);
        if (c1 < c2) return -1;
        if (c1 > c2) return 1;
        if (c1 == '\0') return 0;
    }
}

static int
cmp_alt(const void *p1, const void *p2) {
    char *s1 = *(char * const *) p1;
    char *s2 = *(char * const *) p2;
    while (1) {
        char c1 = *(s1++);
        char c2 = *(s2++);
        if (c1 == c2) { 
            if (c1 == '\0') return 0;
        }
        else {
            if (c1 < c2) return -1;
            if (c1 > c2) return 1;
        }
    }
}

static int
rcmp(const void *p1, const void *p2) {
    return strcmp(* (char * const *) p2, * (char * const *) p1);
}

static int
natcmp(const void *p1, const void *p2) {
    char *s1 = *(char * const *) p1;
    char *s2 = *(char * const *) p2;
    char c1, c2;

    while (1) {
        c1 = *(s1++);
        c2 = *(s2++);
        if ((c1 <= '9') && (c2 <= '9') && (c1 >= '0') && (c2 >= '0')) {
            size_t l1, l2;
            char *e1, *e2;
            while (c1 == '0') c1 = *(s1++);
            while (c2 == '0') c2 = *(s2++);
            for (e1 = s1; (c1 >= '0') && (c1 <= '9'); c1 = *(e1++));
            for (e2 = s2; (c2 >= '0') && (c2 <= '9'); c2 = *(e2++));
            if (e1 - s1 < e2 - s2) return -1;
            if (e1 - s1 > e2 - s2) return 1;
            s1--; s2--; e1--;
            while (s1 < e1) {
                c1 = *(s1++);
                c2 = *(s2++);
                if (c1 < c2) return -1;
                if (c1 > c2) return 1;
            }
        }
        else if (c1 < c2) return -1;
        else if (c1 > c2) return 1;
        else if (c1 == '\0') return 0;
    }
}

static int
natcmp_alt(const void *p1, const void *p2) {
    char *s1 = *(char * const *) p1;
    char *s2 = *(char * const *) p2;
    char c1, c2;
    while (1) {
        char c1 = *(s1++);
        char c2 = *(s2++);
        if (c1 == c2) { 
            if (c1 == '\0') return 0;
        }
        if ((c1 <= '9') && (c2 <= '9') && (c1 >= '0') && (c2 >= '0')) {
            if (c1 == '0') {
                char *b1 = *(char * const *) p1;
                char *p = s1 - 2;
                //while ((p >= b1)
            }
        }
    }
}

int
natsort(char **base, size_t nmemb, int flags) {
    int i, j;
    char **keys;
    char *p;
    size_t len;

    int ignore_case    = (flags & NATSORT_IGNORE_CASE    ? 1 : 0);
    int ignore_symbols = (flags & NATSORT_IGNORE_SYMBOLS ? 1 : 0);
    int reverse        = (flags & NATSORT_REVERSE        ? 1 : 0);
    int simple         = (flags & NATSORT_SIMPLE         ? 1 : 0);
    int alt            = (flags & NATSORT_ALTCMP         ? 1 : 0);
    int alt2           = (flags & NATSORT_ALTCMP2        ? 1 : 0);
    int debug          = (flags & NATSORT_DEBUG          ? 1 : 0);

    if (simple) {
        if (debug) fprintf(stderr, "using natcmp\n");
        qsort(base, nmemb, sizeof(char *), &natcmp);
        return 0;
    }

    for (len = i = 0; i < nmemb; i++) {
        const char *s = base[i];
        char c;

        if (ignore_symbols) while ((c = *(s++)) && !isalnum(c));
        else c = *(s++);

        if (c == 0) {
            len++;
            continue;
        }

        do {
            if (isalpha(c)) {
                /* len is token len plus one if ignore_symbols */
                while (isalpha(c = *(s++))) len++;
                len += ignore_symbols + 1;
            }
            else if (isdigit(c)) {
                size_t digits;
                /* len is number len plus encoded length len plus one
                 * if ignore_symbols */
                while (c == '0') c = *(s++);
                for (digits = 0; isdigit(c); c = *(s++)) digits++;
                len += digits + digits / 9 + 1 + ignore_symbols;
            }
            else {
                /* len is 0 if ignore_symbols or the token len
                 * otherwise */
                if (ignore_symbols)
                    while((c = *(s++)) && !isalnum(c));
                else {
                    while((c = *(s++)) && !isalnum(c)) len++;
                    len++;
                }
            }
        } while (c);

        if (!ignore_symbols) len++;
    }

    keys = (char**)malloc(nmemb * sizeof(char *) * 2 + len);
    if (keys == NULL) return -1;

    p = (char *)(keys + nmemb);
    for (i = 0; i < nmemb; i++) {
        const char *s = base[i];
        char *f = (char *)(base + i);
        char *t = (char *)(base + i + 1);
        char c;

        /* copy pointer to string before the key */
        while (f < t) *(p++) = *(f++);

        /* save pointer to key into keys */
        keys[i] = p;

        /* skip symbols at the beginning */
        if (ignore_symbols) while((c = *(s++)) && !isalnum(c));
        else c = *(s++);

        /* empty string has an empty key */
        if (c == 0) {
            *(p++) = '\0';
            continue;
        }

        do {
            if (isalpha(c)) {
                if (ignore_case) {
                    *(p++) = tolower(c);
                    while (isalpha(c = *(s++))) *(p++) = tolower(c);
                }
                else {
                    *(p++) = c;
                    while (isalpha(c = *(s++))) *(p++) = c;
                }
                if (ignore_symbols) *(p++) = ' ';
            }
            else if (isdigit(c)) {
                size_t digits;
                const char *f, *t;
                while (c == '0') c = *(s++);
                f = s - 1;
                for (digits = 0; isdigit(c); c = *(s++)) digits++;
                for (digits; digits >= 9; digits -= 9) *(p++) = '9';
                *(p++) = '0' + digits;
                for (t = s - 1; f < t; ) *(p++) = *(f++);
                if (ignore_symbols) *(p++) = ' ';
            }
            else {
                if (ignore_symbols)
                    while((c = *(s++)) && !isalnum(c));
                else {
                    *(p++) = c;
                    while((c = *(s++)) && !isalnum(c)) *(p++) = c;
                }
            }
        } while (c);

        if (ignore_symbols) p[-1] = '\0';
        else *(p++) = '\0';
    }

    if (p != ((char*)keys) + nmemb * sizeof(char *) * 2 + len)
        fprintf(stderr, "unexpected p value %p, expected %p (diff: %ld)\n",
               p,
               ((char*)keys) + nmemb * sizeof(char *) * 2 + len,
               p - (((char*)keys) + nmemb * sizeof(char *) * 2 + len));
    else if (debug) 
        fprintf(stderr, "also unexpectedly p value matched the calculated one %p!\n", p);

    qsort(keys, nmemb, sizeof(char *), (reverse ? &rcmp : (alt ? &cmp_alt : (alt2 ? &cmp_alt2 : &cmp))));
    for (i = 0; i < nmemb; i++)
        memcpy((char *)(base + i), keys[i] - sizeof(char *), sizeof(char *));
    
    free(keys);
    return 0;
}

