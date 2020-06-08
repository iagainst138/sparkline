#include <ctype.h>
#include <math.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#define MAX_INT_CHARS 20
#define MAXPOINTS 10
#define NUM_BARS 9

const char bars[NUM_BARS][5] = {"_", "\u2581", "\u2582", "\u2583", "\u2584", "\u2585", "\u2586", "\u2587", "\u2588"};

void append_array(float points[], int n, int length) {
    int i;
    for(i=0;i<length-1;i++) {
        points[i] = points[i+1];
    }
    points[i] = n;
}

void reset_array(char num_chars[], int length) {
    int i;
    for(i=0;i<length;i++) {
        num_chars[i] = '\0';
    }
}

void print_sparkline(float points[], int length, bool quiet) {
    int i;
    int j;
    int max = points[0];
    int min = points[0];

    for(i=0;i<length;i++) {
        if(points[i] > max)
            max = points[i];
        else if(points[i] < min)
            min = points[i];
    }

    // clear the entire line
    printf("\033[2K");
    printf("\r");

    for(i=0;i<length;i++){
        if(max == 0 && min == 0) {
            j = 0;
        } else if(min == max) {
            j = NUM_BARS-1;
        } else {
            j = (int)ceil((points[i]-min)/(max-min)*(NUM_BARS-1));
        }
        printf("%s", bars[j]);
    }

    if(quiet) {
        printf(" ");
    } else {
        printf(" min:%d max:%d [", min, max);
        for(i=0;i<length;i++) {
            if(i == length-1)
                printf("%.1f", points[i]);
            else
                printf("%.1f ", points[i]);
        }
        printf("]");
    }
}

int run(int maxpoints, int length, float values[], bool quiet) {
    int i = 0;
    int next_int = 0;
    char c = 0;
    char num_chars[MAX_INT_CHARS] = {'\0'};
    float points[maxpoints];

    // initialize all points to zero
    int j = 0;
    for(j=0;j<maxpoints;j++)
        points[j] = 0;

    // add values passed as args
    for(j=0;j<length;j++)
        append_array(points, values[j], maxpoints);

    // set output to not be line buffered
    setvbuf(stdout, NULL, _IONBF, 0);

    if(isatty(fileno(stdin))) {
        if(length == 0) {
            fprintf(stderr, "nothing to do, no arguments passed\n");
            return 0;
        }
        print_sparkline(points, maxpoints, quiet);
    } else {
        while((c = fgetc(stdin)) && c != EOF) {
            if(isdigit(c) && c != EOF) {
                num_chars[i++] = c;
            } else if(c == ' ' || c == '\t' || c == '\n' || c == EOF) {
                next_int = atof(num_chars);
                append_array(points, next_int, maxpoints);
                reset_array(num_chars, MAX_INT_CHARS);
                print_sparkline(points, maxpoints, quiet);
                i = 0;
            }
        }
    }

    printf("\n");

    return 0;
}

int main(int argc, char *argv[]) {
    int opt;
    int n = MAXPOINTS;
    bool quiet = false;

    while ((opt = getopt(argc, argv, "hqn:")) != -1) {
        switch (opt) {
            case 'n': n = atoi(optarg);break;
            case 'q': quiet = true;break;
            case 'h':
            default:
                  fprintf(stderr, "usage: %s [-n <num points to track>] [-q quiet]\n", argv[0]);
                  exit(EXIT_FAILURE);
        }
    }

    int i;
    int l = argc-optind;
    float values[l];
    for(i=optind;i<argc;i++)
        append_array(values, atof(argv[i]), l);
    if(isatty(fileno(stdin)))
        n = l;
    run(n, l, values, quiet);
 }
