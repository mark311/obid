#include "obid.h"
#include <stdio.h>
#include <stdlib.h>

int main(int argc, char* args[])
{
    char * model_file;
    char line[512];
    obid_model_t * model = obid_create_model(3);
    
    if (argc < 2) {
        printf("Usage: obid-demo <model-file>\n");
        return -1;
    }
    model_file = args[1];
    obid_load_model(model, model_file);

    while (1 == scanf("%s", line)) {
        obid_check_word(model, line);
    }
    obid_destroy_model(model);
    return 0;
}
