#ifndef _OBID_H_
#define _OBID_H_

#define OBID_NUMBER_INDEX(c) (0 - '0' + c)
#define OBID_UPPER_LETTER_INDEX(c) (10 - 'A' + c)
#define OBID_LOWER_LETTER_INDEX(c) (36 - 'a' + c)
#define OBID_UNDERLINE_INDEX 62
#define OBID_SEPARATOR_INDEX 63

#define OBID_CHAR_COUNT 64

#define OBID_MAX_WORD_LENGTH 40

/*
 * Each characters in this model will be assigned with a specified
 * index. Following shows how characters are mapped to index.
 *
 *   '0' ~ '9' => 0 ~ 9
 *   'A' ~ 'Z' => 10 ~ 35
 *   'a' ~ 'z' => 36 ~ 61
 *         '_' => 62
 *       other => 63
 *
 * Function f(c1,c2,...,cn) is the probability of the occurance of
 * sequence c1,c2,...,cn in normal texts. ci may be a character or the
 * index of it. The number of parameters of f is just the dimension of
 * the model.
 *
 */
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
