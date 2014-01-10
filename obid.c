#include "obid.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <ctype.h>
#include <math.h>

#define CODE2CHAR(c) ((c) == 63 ? '|' :         \
                      ((c) == 62 ? '_' :         \
                       ((c) >= 36 ? 'a' - 36 + c :      \
                        ((c) >= 10 ? 'A' - 10 + c :     \
                         '0' - 0 + (c)))))
#define COMPONENT(i,k) ((i >> (6 * k)) & 63)

static inline int _obid_index_of_char(const char c)
{
    if (isdigit(c))
        return OBID_NUMBER_INDEX(c);
    else if (isupper(c))
        return OBID_UPPER_LETTER_INDEX(c);
    else if (islower(c))
        return OBID_LOWER_LETTER_INDEX(c);
    else if (c == '_')
        return OBID_UNDERLINE_INDEX;
    else
        return OBID_SEPARATOR_INDEX;
}

obid_model_t * obid_create_model(int d)
{
    assert(d >= 2);

    obid_model_t * model = (obid_model_t*) malloc(sizeof(obid_model_t));
    size_t l = 1;
    int i;
    for (i = 0; i < d; i++) {
        l *= OBID_CHAR_COUNT;
    }
    model->f = (unsigned long*) malloc(sizeof(unsigned long) * l);
    memset(model->f, 0, sizeof(unsigned long) * l);
    model->d = d;
    model->n = 0;
    return model;
}

void obid_destroy_model(obid_model_t * model)
{
    free(model);
}

void obid_train(obid_model_t * model, const char * text)
{
    const char * p = text;
    int i, index, index0, index1, cc;
    int len;

    index = 0;
    cc = 1;
    for (i = 0; i < model->d; i++) {
        index *= OBID_CHAR_COUNT;
        index += OBID_SEPARATOR_INDEX;
        cc *= OBID_CHAR_COUNT;
    }
    index0 = index;
    assert(index < cc);

    while (*p) {
        index1 = _obid_index_of_char(*p);
        if (index1 == OBID_SEPARATOR_INDEX) {
            index = index0;
        } else {
            index = index * OBID_CHAR_COUNT % cc + index1;
            model->f[index]++;
            model->n++;

#ifdef DEBUG
            printf("index = %d, model->f[ %d %d %d ] '%c%c%c' = %lu, n = %lu\n", index,
                   COMPONENT(index, 2),
                   COMPONENT(index, 1),
                   COMPONENT(index, 0),
                   CODE2CHAR(COMPONENT(index, 2)),
                   CODE2CHAR(COMPONENT(index, 1)),
                   CODE2CHAR(COMPONENT(index, 0)),
                   model->f[index],
                   model->n);
#endif
        }
    
        p++;
    }
}

int obid_load_model(obid_model_t * model, const char * file)
{
    FILE * fin;
    size_t l = 1;
    int i;

    fin = fopen(file, "r");
    if (!fin) {
        return 0;
    }

    fscanf(fin, "%lu %d", &model->n, &model->d);
    
    for (i = 0; i < model->d; i++) {
        l *= OBID_CHAR_COUNT;
    }
    for (i = 0; i < l; i++) {
        fscanf(fin, "%lu", &model->f[i]);
    }
    
    fclose(fin);
    return 1;
}

int obid_save_model(obid_model_t * model, const char * file)
{
    FILE * fout;
    size_t l = 1;
    int i;

    fout = fopen(file, "w");
    if (!fout) {
        return 0;
    }

    fprintf(fout, "%lu %d\n", model->n, model->d);
    
    for (i = 0; i < model->d; i++) {
        l *= OBID_CHAR_COUNT;
    }
    for (i = 0; i < l; i++) {
        fprintf(fout, "%lu\n", model->f[i]);
    }

    fclose(fout);
    return 1;
}

int obid_check_word(obid_model_t * model, obid_result_t * result, const char * word)
{
    const char * p = word;
    int i, j, index, index0, cc, len, start;
    double f, fsum;
    int f_verbose = 0;

    if (result->letters && result->farray && result->flen > 0) {
        f_verbose = 1;
    }

    len = strlen(word);
    fsum = 0;
    j = 0;

    index = 0;
    cc = 1;
    start  = len > model->d ? 0 : len - model->d;

    for (i = start; i < start + model->d; i++) {
        index *= OBID_CHAR_COUNT;
        index += (i < 0 ? OBID_SEPARATOR_INDEX : _obid_index_of_char(word[i]));
        cc *= OBID_CHAR_COUNT;

        if (f_verbose) {
            result->letters[j] = (i < 0 ? '|' : word[i]);
            result->farray[j] = -1.0;
            j++;
        }
    }

    f = 1.0 * model->f[index] / model->n;
    fsum += f;
    if (f_verbose) {
        result->farray[j - 1] = f;
    }

    for (p = word + start + model->d; p != word + len; p++) {
        index = index * OBID_CHAR_COUNT % cc + _obid_index_of_char(*p);
        f = 1.0 * model->f[index] / model->n;
        fsum += f;

        if (f_verbose && j < result->flen) {
            result->letters[j] = *p;
            result->farray[j] = f;
            j++;
        }
    }

    if (f_verbose)
        result->flen = (j > result->flen ? result->flen : j);

    result->obf = fsum / (p - word);
    return 1;
}
