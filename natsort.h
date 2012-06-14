
#ifndef NATSORT_H

#define NATSORT_IGNORE_CASE    1
#define NATSORT_IGNORE_SYMBOLS 2
#define NATSORT_REVERSE        4
#define NATSORT_SIMPLE         8
#define NATSORT_ALTCMP        16
#define NATSORT_ALTCMP2       32
#define NATSORT_DEBUG         64

int
natsort(char **base, size_t nmemb, int flags);


#endif /* NATSORT_H */
