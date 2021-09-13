#ifndef QUADRICSOLVER_H
#define QUADRICSOLVER_H

#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>        
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <ncurses.h>

double sign(double x) { 
    if (x < 0) return -1; 
    if (x > 0) return 1; 
    else return 0;
}

static const double TOL = 1e-3;
static const double GRAPH_EPS = 1;
 
static const size_t HISTORY_LENGHT = 64;   //TODO add Makefile; add `make install` option; add history save support
static const size_t MAX_CMD_LENGHT = 64; 


//==========================================
// Command history struct

struct history
{
    WINDOW *localWin;

    char (*log)[HISTORY_LENGHT][MAX_CMD_LENGHT + 1];
    
    size_t end;
    size_t cur;

    bool isActive;
    bool isEmpty;
};

void history_construct(struct history *h, WINDOW* historyWin)
{
    h->localWin = historyWin;
    h->isActive = true;
    h->isEmpty = true;
    h->log = (char (*)[HISTORY_LENGHT][MAX_CMD_LENGHT + 1])calloc(HISTORY_LENGHT * (MAX_CMD_LENGHT + 1), sizeof(char));

    h->end = 0;
    h->cur = 0;
}

void history_destruct(struct history *h)
{
    if (h->log)
       free(h->log);
}

void history_put(struct history *h, char* elem)
{
    strcpy((*h->log)[h->end], elem);
    h->end = (h->end + 1) % HISTORY_LENGHT;
}

char* history_get(struct history *h, size_t n) 
{
    if (!h->isActive || n > HISTORY_LENGHT)
        return NULL;

    if (*h->log[(h->end - n) % HISTORY_LENGHT ][0])
        return ((*h->log)[(h->end - n) % HISTORY_LENGHT]);
    else 
        return NULL;
}

void history_list(struct history *h)
{
    if (!h->isActive)
        return;

    size_t i = 1;
    while ((*h->log)[(h->end - i) % HISTORY_LENGHT][0] && i != HISTORY_LENGHT) {
        // printf("Here %d times!\n", i);
        wprintw(h->localWin, "%d: %s\n", i, (*h->log)[(h->end - i) % HISTORY_LENGHT]);
        ++i;
    }
}

//==========================================
// WINDOW management

WINDOW *createWin(int height, int width, int starty, int startx)
{	WINDOW *localWin;

	localWin = newwin(height, width, starty, startx);
	box(localWin, 0 , 0);		/* 0, 0 gives default characters 
					             * for the vertical and horizontal
            					 * lines			                */
	wrefresh(localWin);		    

	return localWin;
}

void destroyWin(WINDOW *localWin)
{	
	wborder(localWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');

	/* The parameters taken are 
	 * 1. win: the window on which to operate
	 * 2. ls: character to be used for the left side of the window 
	 * 3. rs: character to be used for the right side of the window 
	 * 4. ts: character to be used for the top side of the window 
	 * 5. bs: character to be used for the bottom side of the window 
	 * 6. tl: character to be used for the top left corner of the window 
	 * 7. tr: character to be used for the top right corner of the window 
	 * 8. bl: character to be used for the bottom left corner of the window 
	 * 9. br: character to be used for the bottom right corner of the window
	 */
	
    wrefresh(localWin);
	delwin(localWin);
}

//==========================================
// Main 

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

void printGraph(double a, double b, double c)
{
    WINDOW* sideWin = createWin(LINES - 5, COLS / 2 + 1,  0, COLS / 2);
    WINDOW* plotWin = createWin(LINES - 15, COLS / 2 - 8, 5, COLS / 2 + 5);
	wborder(plotWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');

    double dimentionsDiff = 2.3;
    long long windowHight = LINES - 15;
    long long windowWidth = COLS / 2 - 10;
    //mvwprintw(plotWin, 5, 10, "Plot: y = %.2f x^2 + %.2f x + %.2f \n\n", a, b, c);

    for (int i = 0; i < windowHight; ++i) {             // Printing the graph
        double y = -(i - windowHight / 2);
        for (int j = 0; j < windowWidth; ++j) {
            double x = j - windowWidth / 2;
            x /= dimentionsDiff;
            if (i == 0 && j == windowWidth / 2 - 1) {
                wprintw(plotWin, "Y");
            }
            else if (i == windowHight / 2 + 1 && j == windowWidth - 1) {
                wprintw(plotWin, "X");
            }
            else if (fabs(x*x*a + x*b + c - y) < GRAPH_EPS) {
                wprintw(plotWin, ".");
            }
            else if (i != windowHight / 2 && j != windowWidth / 2) {
                waddch(plotWin, ' ');
            }
            else if (i == windowHight / 2 && j != windowWidth / 2) {
                if (j == windowWidth - 1)
                    waddch(plotWin, ACS_RARROW);
                else
                    waddch(plotWin, ACS_HLINE);
            }
            else if (i != windowHight / 2 && j == windowWidth / 2) {
                if (i == 0)
                    waddch(plotWin, ACS_UARROW);
                else
                    waddch(plotWin, ACS_VLINE);
                
            }
            else if (i == windowHight / 2 && j == windowWidth / 2) {
                waddch(plotWin, ACS_PLUS);
            }
            if (j == windowWidth - 1)
                waddch(plotWin, '\n');
        }
    }

    wrefresh(plotWin);
    delwin(plotWin);
    delwin(sideWin);
}

void quadricSolver(double a, double b, double c, double *result_1, double *result_2, bool *result_eq_inf)
{
    if (fabs(a) < TOL) {
        if (fabs(b) < TOL) {
            *result_eq_inf = true;
        }
        else {
            *result_1 = -c / b;
        }
    }

   else {                      // Quadric case
        double det = b * b - 4 * a * c;
        if (fabs(det) < TOL) {
            *result_1 = -0.5 * b / a;
            *result_2 = *result_1;
        }
        else if (det > 0) { 
            double interim = -0.5 * (b + sign(b) * sqrt(det));
            *result_1 = c / interim;
            *result_2 = interim / a;
        }
    }
}

#endif
