#include "obid.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>

#define MAX_LETTER_NUMBER 40

static char* readline(FILE *input)
{
    static int max_line_len = 1024;
    static char *line = NULL;
    int len;
    char *c;

    if (input == NULL) {
        free(line);
        return NULL;
    }

    if (line == NULL) {
        line = (char*) malloc(max_line_len);
    }

    if (fgets(line,max_line_len,input) == NULL)
        return NULL;

    while (strrchr(line,'\n') == NULL)
    {
        max_line_len *= 2;
        line = (char *) realloc(line,max_line_len);
        len = (int) strlen(line);
        if(fgets(line+len,max_line_len-len,input) == NULL)
            break;
    }

    // strip the last '\n'
    c = strrchr(line, '\n');
    assert(c);
    *c = '\0';

    return line;
}

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
    int i, j, g, len;
    double f, h, t;
    char * model_file = "obid.model";
    char * line = NULL;
    char ch;
    obid_model_t * model = NULL;
    char letters[MAX_LETTER_NUMBER];
    double farray[MAX_LETTER_NUMBER];
    obid_result_t result = {0.0, letters, farray, MAX_LETTER_NUMBER};
    double avg_f[MAX_LETTER_NUMBER];
    double avg_f2[MAX_LETTER_NUMBER];
    int avg_n[MAX_LETTER_NUMBER];
    int opt_verbose = 0;
    int opt_summary = 0;
    FILE *fin = stdin;

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

    if (argc >= 1) {
        fin = fopen(argv[0], "r");
        if (!fin) {
            printf("can't open file %s\n", argv[0]);
            exit(1);
        }
    }

    printf("Loaded model file: %s\n", model_file);
    if (opt_verbose) {
        printf("Enabled verbose mode\n");
    }

    if (opt_summary) {
        for (i = 0; i < MAX_LETTER_NUMBER; i++) {
            avg_f[i] = 0.0;
            avg_f2[i] = 0.0;
            avg_n[i] = 0;
        }
    }

    while ((line = readline(fin))) {
        len = strlen(line);
        result.flen = MAX_LETTER_NUMBER; // set the max length for the arrays in result.
        obid_check_word(model, &result, line);

        if (opt_verbose) {
            for (i = 0; i < result.flen; i++) {
                f = result.farray[i];
                g = (int)(f == 0.0 ? 0 : 100* sqrt(sqrt(f)));

                printf(" [%02d] %c: %.8f [", i, result.letters[i], f);
                for (j = 0; j < 100; j++)
                    if (j < g)
                        printf(".");
                    else
                        printf(" ");
                printf("]\n");
            }
        }

        if (opt_summary && len < MAX_LETTER_NUMBER) {
            avg_f[len] += result.obf;
            avg_f2[len] += result.obf * result.obf;
            avg_n[len] += 1;
            if (opt_verbose) {
                printf("=(%02d)=== %.8f %s\n\n", len, result.obf, line);
            }
        }
    }

    if (opt_summary) {
        printf("Word Len.\tAvg. Frequency\tStD. Frequency\t Min Frequency\t Max Frequency\n");
        printf("---------\t--------------\t--------------\t--------------\t--------------\n");
        for (i = 0; i < MAX_LETTER_NUMBER; i++) {
            f = avg_f[i] / avg_n[i];
            t = sqrt(avg_f2[i] / avg_n[i] - f * f);
            printf("%9d\t%14.8f\t%14.8f\t%14.8f\t%14.8f\n", i, f, t, f - t / 2, f + t / 2);
        }
    }

    obid_destroy_model(model);
    if (fin != stdin) {
        fclose(fin);
    }

    // free line buffer
    readline(NULL);

    return 0;
}
