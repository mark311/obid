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

obid_model_t * obid_create_model(int d)
{
    obid_model_t * model = (obid_model_t*) malloc(sizeof(obid_model_t));
    size_t l = 1;
    for (int i = 0; i < d; i++) {
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
    int index, index0, cc;

    index = 0;
    cc = 1;
    for (int i = 0; i < model->d; i++) {
        index *= OBID_CHAR_COUNT;
        index += OBID_SEPARATOR_INDEX;
        cc *= OBID_CHAR_COUNT;
    }
    assert(index < cc);

    while (*p) {
        if (isdigit(*p))
            index0 = OBID_NUMBER_INDEX(*p);
        else if (isupper(*p))
            index0 = OBID_UPPER_LETTER_INDEX(*p);
        else if (islower(*p))
            index0 = OBID_LOWER_LETTER_INDEX(*p);
        else if (*p == '_')
            index0 = OBID_UNDERLINE_INDEX;
        else
            index0 = OBID_SEPARATOR_INDEX;

        index = index * OBID_CHAR_COUNT % cc + index0;
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
    
        p++;
    }
}

int obid_load_model(obid_model_t * model, const char * file)
{
    FILE * fin;
    size_t l = 1;

    fin = fopen(file, "r");
    if (!fin) {
        return 0;
    }

    fscanf(fin, "%lu %d", &model->n, &model->d);
    
    for (int i = 0; i < model->d; i++) {
        l *= OBID_CHAR_COUNT;
    }
    for (int i = 0; i < l; i++) {
        fscanf(fin, "%lu", &model->f[i]);
    }
    
    fclose(fin);
    return 1;
}

int obid_save_model(obid_model_t * model, const char * file)
{
    FILE * fout;
    size_t l = 1;

    fout = fopen(file, "w");
    if (!fout) {
        return 0;
    }

    fprintf(fout, "%lu %d\n", model->n, model->d);
    
    for (int i = 0; i < model->d; i++) {
        l *= OBID_CHAR_COUNT;
    }
    for (int i = 0; i < l; i++) {
        fprintf(fout, "%lu\n", model->f[i]);
    }

    fclose(fout);
    return 1;
}

double obid_check_word(obid_model_t * model, const char * word)
{
    const char * p = word;
    int index, index0, cc;

#ifdef DEBUG
    printf("Probs for word: %s\n", word);
#endif

    index = 0;
    cc = 1;
    for (int i = 0; i < model->d; i++) {
        index *= OBID_CHAR_COUNT;
        index += OBID_SEPARATOR_INDEX;
        cc *= OBID_CHAR_COUNT;
    }
    assert(index < cc);

    while (*p) {
        if (isdigit(*p))
            index0 = OBID_NUMBER_INDEX(*p);
        else if (isupper(*p))
            index0 = OBID_UPPER_LETTER_INDEX(*p);
        else if (islower(*p))
            index0 = OBID_LOWER_LETTER_INDEX(*p);
        else if (*p == '_')
            index0 = OBID_UNDERLINE_INDEX;
        else
            index0 = OBID_SEPARATOR_INDEX;

        index = index * OBID_CHAR_COUNT % cc + index0;
#ifdef DEBUG
        double f = 1.0 * model->f[index] / model->n;
        int g = (int)(f == 0.0 ? 0 : 100* sqrt(sqrt(f)));
        printf(" [%02ld] %c: %.8f [", (p - word), *p, f);
        for (int i = 0; i < 100; i++)
            if (i < g)
                printf(".");
            else
                printf(" ");
        printf("]\n");
#endif
    
        p++;
    }
    return 0.0;
}
