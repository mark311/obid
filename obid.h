#ifndef _OBID_H_
#define _OBID_H_

#define OBID_NUMBER_INDEX(c) (0 - '0' + c)
#define OBID_UPPER_LETTER_INDEX(c) (10 - 'A' + c)
#define OBID_LOWER_LETTER_INDEX(c) (36 - 'a' + c)
#define OBID_UNDERLINE_INDEX 62
#define OBID_SEPARATOR_INDEX 63

#define OBID_CHAR_COUNT 64

typedef struct {
    unsigned long *f;
    unsigned long n;
    int d;
} obid_model_t;

#endif //_OBID_H_
