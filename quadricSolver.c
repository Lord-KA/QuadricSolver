#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>        //TODO delete if unused
#include <string.h>
#include <stdarg.h>
#include <math.h>



#define KEY_ESCAPE  0x001b
#define KEY_ENTER   0x000a
#define KEY_UP      0x0105
#define KEY_DOWN    0x0106
#define KEY_LEFT    0x0107
#define KEY_RIGHT   0x0108

#define CURFORWARD(n) printf("\033[%dC", (n))
#define CURBACKWARD(n) printf("\033[%dD", (n))

static const double TOL = 1e-3;
 
static const size_t HISTORY_LEN = 64;   //TODO add Makefile; add `make install` option; add history save support
static const size_t HISTORY_SIZE = 128; 


struct history
{
    char** log;
    
    size_t end;
    size_t cur;

    bool isActive;
    bool isEmpty;
};

void history_construct(struct history *h)
{
    h->isActive = true;
    h->isEmpty = true;
    h->log = malloc(sizeof(char*) * HISTORY_LEN);

    for (size_t i = 0; i < HISTORY_LEN; ++i) {
        h->log[i] = malloc(sizeof(char) * HISTORY_SIZE);
        if (!h->log[i]) {
            fprintf(stderr, "WARNING: history initialization failed\n");
            h->isActive = false;
        }
        else 
            h->log[i][0] = 0;
    }
    h->end = 0;
    h->cur = 0;
}

void history_destruct(struct history *h)
{
    for (size_t i = 0; i < HISTORY_LEN; ++i) {
        if (h->log[i])
            free(h->log[i]);
    }
}

void history_put(struct history *h, char* elem)
{
    strcpy(h->log[h->end], elem);
    h->end = (h->end + 1) % HISTORY_LEN;
}

char* history_get(struct history *h, size_t n) 
{
    if (!h->isActive || n > HISTORY_LEN)
        return NULL;

    //if (h->log[(h->end - n) % HISTORY_LEN ][0])
    return h->log[(h->end - n) % HISTORY_LEN];
    // else 
        return NULL;
}

void history_list(struct history *h)
{
    if (!h->isActive)
        return;

    size_t i = 1;
    while (h->log[(h->end - i) % HISTORY_LEN][0]) {
        printf("%d: %s\n", i, h->log[(h->end - i) % HISTORY_LEN]);
        ++i;
    }
}


bool doubleCheck(size_t num, ...)
{
    va_list args;
    va_start(args, num);
    

    for (int i = 0; i < num; ++i) {
        if (isnan(va_arg(args, double))){   //TODO maybe add something else
            va_end(args);
            return false;
        }
    }

    va_end(args);
    return true;
}


int main() 
{
    printf("==============================\n");
    printf("== QuadricSolver by Lord-KA ==\n");
    printf("==============================\n");

    char keyword[32];
    char input[HISTORY_SIZE];

    struct history *h = malloc(sizeof(struct history));
    history_construct(h);

    while (true) {
        printf(">>> ");

        gets(input);


        sscanf(input, "%s", keyword);

        if (strcmp(keyword, "help") == 0)
            printf("This is a (very usefull) help message.\n"); //TODO write reasonable help

        else if (strcmp(keyword, "history") == 0) {
            char select[32];
            int num;
            if ((sscanf(input, "%s %s %d", keyword, select, &num) == 3) && (strcmp(select, "select") == 0)) {
                char* command = history_get(h, num);
                if (command)
                    printf("%s", command);
                
            }
            else {
                history_list(h);
            }
            
            printf("`%s` should equal `select`\n", select);                                                             //TODO finish select option

            




        }


        else if (strcmp(keyword, "exit") == 0) 
            break;                                  
        
        else if (strcmp(keyword, "solve") == 0) {
            double a = 0, b = 0, c = 0;
            sscanf(input, "%s %lf %lf %lf", keyword, &a, &b, &c);

            if (!doubleCheck(3,  a, b, c)) {
                printf("Bad input. Type 'help' for additional info.\n");
                continue;
            }
            
            double result_1 = NAN;
            double result_2 = NAN;
            if (fabs(a) < TOL){         // Linear case
                result_1 = -b / a;
                if (fabs(result_1) < TOL)
                    result_1 = 0;
            }

            else {                      // Quadric case
                double det = b * b - 4 * a * c;
                if (fabs(det) < TOL) {
                    result_1 = -b / 2 / a;
                    result_2 = result_1;
                }
                else if (det > 0) { 
                    result_1 = (-b - sqrt(det)) / 2 / a;
                    result_2 = (-b + sqrt(det)) / 2 / a;
                }
            }
            
            
            printf("%.2f x^2 + %.2f x + %.2f = 0  <=>  x \\in ", a, b, c);
            if (isnan(result_1))
                printf("\\emptyset\n");
            else if(isnan(result_2))
                printf("{ %lf }\n", result_1);
            else 
                printf("{ %lf, %lf }\n", result_1, result_2);
        }

        history_put(h, input);
    }
    history_list(h);
    history_destruct(h);
}
