#include "obid.h"
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <getopt.h>
#include <string.h>
#include <assert.h>

int opt_verbose = 0;
int opt_summary = 0;
int opt_train = 0;
char * opt_model_file = "obid.model";

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
           "  obid-demo -T|--train file1 file2 ...\n"
           "\n"
           "Option:\n"
           "  -v,--verbose, enable verbose mode.\n"
           "  -m,--model, specify model file, default is \"obid.model\"\n"
           "  -S,--summary, print the average frequency summary\n"
           "  -T,--train, train the model with given files\n"
           "\n");
}

void parse_options(int *pargc, char **(*pargv))
{
    int argc = *pargc;
    char **argv = *pargv;
    char ch;

    /* options descriptor */
    static struct option longopts[] = {
            { "verbose",    no_argument,            NULL,           'v' },
            { "model",      required_argument,      NULL,           'm' },
            { "summary",    no_argument,            NULL,           'S' },
            { "train",      no_argument,            NULL,           'T' },
            { "help",       no_argument,            NULL,           'h' },
            { NULL,         0,                      NULL,           0 }
    };

    while ((ch = getopt_long(argc, argv, "vm:STh", longopts, NULL)) != -1) {
        switch (ch) {
        case 'v':
            opt_verbose = 1;
            break;
        case 'S':
            opt_summary = 1;
            break;
        case 'T':
            opt_train = 1;
            break;
        case 'm':
            opt_model_file = optarg;
            break;
        case 'h':
        default:
            usage();
            exit(1);
        }
    }
    *pargc -= optind;
    *pargv += optind;
}

int train(int argc, char *argv[])
{
    char * training_file;
    char * buffer;
    obid_model_t * model = obid_create_model(3);
    int i;
    
    if (0 == access(opt_model_file, F_OK)) {
        obid_load_model(model, opt_model_file);
    }
    
    for (i = 0; i < argc; i++) {
        training_file = argv[i];
        printf("[%d/%d] training %s ...\n", i + 1, argc, training_file);
        readfile(training_file, &buffer);
        obid_train(model, buffer);
        free(buffer);
    }

    obid_save_model(model, opt_model_file);
    obid_destroy_model(model);
    
    return 0;
}

int interactive(int argc, char *argv[])
{
    int i, j, g, len;
    double f, h, t;
    char * line = NULL;
    obid_model_t * model = NULL;
    char letters[OBID_MAX_WORD_LENGTH];
    double farray[OBID_MAX_WORD_LENGTH];
    obid_result_t result = {0.0, letters, farray, OBID_MAX_WORD_LENGTH};
    double avg_f[OBID_MAX_WORD_LENGTH];
    double avg_f2[OBID_MAX_WORD_LENGTH];
    int avg_n[OBID_MAX_WORD_LENGTH];
    FILE *fin = stdin;

    model = obid_create_model(3);
    obid_load_model(model, opt_model_file);

    if (argc >= 1) {
        fin = fopen(argv[0], "r");
        if (!fin) {
            printf("can't open file %s\n", argv[0]);
            exit(1);
        }
    }

    printf("Loaded model file: %s\n", opt_model_file);
    if (opt_verbose) {
        printf("Enabled verbose mode\n");
    }

    if (opt_summary) {
        for (i = 0; i < OBID_MAX_WORD_LENGTH; i++) {
            avg_f[i] = 0.0;
            avg_f2[i] = 0.0;
            avg_n[i] = 0;
        }
    }

    while ((line = readline(fin))) {
        len = strlen(line);
        result.flen = OBID_MAX_WORD_LENGTH; // set the max length for the arrays in result.
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

        if (opt_summary && len < OBID_MAX_WORD_LENGTH) {
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
        for (i = 0; i < OBID_MAX_WORD_LENGTH; i++) {
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

int main(int argc, char* argv[])
{
    parse_options(&argc, &argv);

    if (opt_train) {
        return train(argc, argv);
    } else {
        return interactive(argc, argv);
    }
}
