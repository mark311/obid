#include "obid.h"
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int readfile(const char * filename, char **buffer)
{
    FILE *f = fopen(filename, "rb");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);

    char *string = malloc(fsize + 1);
    fread(string, fsize, 1, f);
    fclose(f);

    string[fsize] = 0;
    *buffer = string;

    return 1;
}

int main(int argc, char* args[])
{
    char * model_file;
    char * training_file;
    char * buffer;
    obid_model_t * model = obid_create_model(3);
    int i;
    
    if (argc < 3) {
        printf("Usage: obid-train <model-file> <training-files> ...\n");
        return -1;
    }
    model_file = args[1];

    if (0 == access(model_file, F_OK)) {
        obid_load_model(model, model_file);
    }
    
    for (i = 2; i < argc; i++) {
        training_file = args[i];
        printf("[%d/%d] training %s ...\n", i - 1, argc - 2, training_file);
        readfile(training_file, &buffer);
        obid_train(model, buffer);
        free(buffer);
    }

    obid_save_model(model, model_file);
    obid_destroy_model(model);
    
    return 0;
}
