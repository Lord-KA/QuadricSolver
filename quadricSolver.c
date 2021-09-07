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

 
static const size_t HISTORY_LEN = 64;   //TODO add history support for the terminal
static const size_t HISTORY_SIZE = 128; 


struct history
{
    char* log[HISTORY_LEN];
    
    size_t end;
    size_t cur;

    bool isActive;
    bool isEmpty;
};

void history_construct(struct history *h)
{
    h->isActive = true;
    h->isEmpty = true;
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

    if (h->log[(h->end - n) % HISTORY_LEN ][0])
        return h->log[(h->end - n) % HISTORY_LEN];
    else 
        return NULL;
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
    while (true) {
        printf(">>> ");
        scanf("%s", &keyword);
        
        if (strcmp(keyword, "help") == 0)
            printf("This is a (very usefull) help.\n"); //TODO write reasonable help

        else if (strcmp(keyword, "exit") == 0) 
            break;                                  // breaking because I want to use gprintf.flush()
        
        else if (strcmp(keyword, "solve") == 0) {
            double a = 0, b = 0, c = 0;
            scanf("%lf %lf %lf", &a, &b, &c);

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
    }


}
