#include "obid.h"
#include <stdlib.h>
#include <string.h>
#include <assert.h>

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
	
	p++;
    }
}

void obid_load_model(obid_model_t * model, const char * file)
{
}

void obid_save_model(obid_model_t * model, const char * file)
{
}

double obid_check_word(obid_model_t * model, const char * word)
{

}
