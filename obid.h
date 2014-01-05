#ifndef _OBID_H_
#define _OBID_H_

#define OBID_NUMBER_INDEX(c) (0 - '0' + c)
#define OBID_UPPER_LETTER_INDEX(c) (10 - 'A' + c)
#define OBID_LOWER_LETTER_INDEX(c) (36 - 'a' + c)
#define OBID_UNDERLINE_INDEX 62
#define OBID_SEPARATOR_INDEX 63

#define OBID_CHAR_COUNT 64

#define OBID_MAX_WORD_LENGTH 40

typedef struct {
    unsigned long *f;
    unsigned long n;
    int d;
} obid_model_t;

typedef struct {
    double obf;
    char *letters;
    double *farray;
    int flen;
} obid_result_t;

obid_model_t * obid_create_model(int d);
void obid_destroy_model(obid_model_t * model);
void obid_train(obid_model_t * model, const char * text);
int obid_load_model(obid_model_t * model, const char * file);
int obid_save_model(obid_model_t * model, const char * file);
int obid_check_word(obid_model_t * model, obid_result_t * result, const char * word);

#endif //_OBID_H_
