#include "obid.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>

#define MAX_LETTER_NUMBER 40

void usage()
{
    printf("Usage:\n"
           "  obid-demo <options>\n"
           "\n"
           "Option:\n"
           "  -v,--verbose, enable verbose mode.\n"
           "  -m,--model, specify model file, default is \"obid.model\"\n"
           "  -S,--summary, print the average frequency summary\n"
           "\n");
}

int main(int argc, char* argv[])
{
    int i, j, g;
    double f;
    char * model_file = "obid.model";
    char line[512];
    char ch;
    obid_model_t * model = NULL;
    char letters[MAX_LETTER_NUMBER];
    double farray[MAX_LETTER_NUMBER];
    obid_result_t result = {0.0, letters, farray, MAX_LETTER_NUMBER};
    double avg_f[MAX_LETTER_NUMBER];
    int avg_n[MAX_LETTER_NUMBER];
    int opt_verbose = 0;
    int opt_summary = 0;

    /* options descriptor */
    static struct option longopts[] = {
            { "verbose",    no_argument,            NULL,           'v' },
            { "model",      required_argument,      NULL,           'm' },
            { "summary",    no_argument,            NULL,           'S' },
            { "help",       no_argument,            NULL,           'h' },
            { NULL,         0,                      NULL,           0 }
    };

    while ((ch = getopt_long(argc, argv, "vm:Sh", longopts, NULL)) != -1) {
        switch (ch) {
        case 'v':
            opt_verbose = 1;
            break;
        case 'S':
            opt_summary = 1;
            break;
        case 'm':
            model_file = optarg;
            break;
        case 'h':
        default:
            usage();
            exit(1);
        }
    }
    argc -= optind;
    argv += optind;

    model = obid_create_model(3);
    obid_load_model(model, model_file);

    printf("Loaded model file: %s\n", model_file);
    if (opt_verbose) {
        printf("Enabled verbose mode\n");
    }

    if (opt_summary) {
        for (i = 0; i < MAX_LETTER_NUMBER; i++) {
            avg_f[i] = 0.0;
            avg_n[i] = 0;
        }
    }

    while (1 == scanf("%s", line)) {
        obid_check_word(model, &result, line);

        for (i = 0; i < result.flen; i++) {
            f = result.farray[i];
            g = (int)(f == 0.0 ? 0 : 100* sqrt(sqrt(f)));

            if (opt_summary) {
                avg_f[result.flen] += f;
                avg_n[result.flen] += 1;
            }

            if (opt_verbose) {
                printf(" [%02d] %c: %.8f [", i, result.letters[i], f);
                for (j = 0; j < 100; j++)
                    if (j < g)
                        printf(".");
                    else
                        printf(" ");
                printf("]\n");
            }

        }
    }

    if (opt_summary) {
        printf("Word Len.    Avg. Frequency\n");
        printf("---------    --------------\n");
        for (i = 0; i < MAX_LETTER_NUMBER; i++) {
            printf("%9d    %.8f\n", i, avg_f[i] / avg_n[i]);
        }
    }

    obid_destroy_model(model);
    return 0;
}
