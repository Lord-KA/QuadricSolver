#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <stdbool.h>        //TODO delete if unused
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <ncurses.h>


static const double TOL = 1e-3;
static const double GRAPH_EPS = 1;
 
static const size_t HISTORY_LEN = 64;   //TODO add Makefile; add `make install` option; add history save support
static const size_t HISTORY_SIZE = 128; 

WINDOW *create_newwin(int height, int width, int starty, int startx)
{	WINDOW *local_win;

	local_win = newwin(height, width, starty, startx);
	box(local_win, 0 , 0);		/* 0, 0 gives default characters 
					 * for the vertical and horizontal
					 * lines			*/
	wrefresh(local_win);		/* Show that box 		*/

	return local_win;
}

void destroy_win(WINDOW *local_win)
{	
	/* box(local_win, ' ', ' '); : This won't produce the desired
	 * result of erasing the window. It will leave it's four corners 
	 * and so an ugly remnant of window. 
	 */
	wborder(local_win, ' ', ' ', ' ',' ',' ',' ',' ',' ');
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
	wrefresh(local_win);
	delwin(local_win);
}

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
            printw(stderr, "WARNING: history initialization failed\n");
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
        printw("%d: %s\n", i, h->log[(h->end - i) % HISTORY_LEN]);
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

void printGraph(double a, double b, double c)
{
    double dimentionsDiff = 2.3;
    long long windowHight = 41;
    long long windowWidth = dimentionsDiff * windowHight;
    
    addstr("\n");
    for (size_t i = 0; i < windowHight; ++i) {          // Printing coordinate grid
        if (i == windowHight / 2) {
            for (size_t j = 0; j < windowWidth / 2; ++j)
                addstr("-");
            addstr("+");
            for (size_t j = 0; j < windowWidth / 2; ++j)
                addstr("-");
            addstr("\n");
        }
        printw("%*s|", windowWidth / 2, "");            //TODO change '|', '-' for actual borders
        printw("%*s\n", windowWidth / 2, "");
    }


    long long xCorrection = 3;                          // TMP? for getting above previous outp
    for (int i = 0; i < windowHight; ++i) {             // Printing the graph
        double y = i - windowHight / 2;
        for (int j = 0; j < windowWidth; ++j) {
            double x = j - windowWidth / 2;
            x /= dimentionsDiff;
            if (fabs(x*x*a + x*b + c - y) < GRAPH_EPS) {
                mvaddch(windowHight - i + xCorrection, windowWidth - j, '.');
            }
            /*
            else if (i != windowHight / 2 && j != windowWidth / 2) {
                printw(" ");
            }
            else if (i == windowHight / 2 && j != windowWidth / 2) {
                printw("|");
            }
            */
        }
    }
    mvaddch(windowHight, windowWidth, ' ');

    refresh();
    getch();                // TMP, so the window doesn't close
}


int main() 
{
    initscr();

    printw("==============================\n");
    printw("== QuadricSolver by Lord-KA ==\n");
    printw("==============================\n");
    refresh();

    char keyword[32];
    char input[HISTORY_SIZE];

    struct history *h = malloc(sizeof(struct history));
    history_construct(h);

    // WINDOW *win = create_newwin();
    // printGraph(1, 7, -3);

    while (true) { 
        printw(">>> ");
        refresh();

        getstr(input);


        sscanf(input, "%s", keyword);

        if (strcmp(keyword, "help") == 0)
            printw("This is a (very usefull) help message.\n"); //TODO write reasonable help

        else if (strcmp(keyword, "history") == 0) {
            char select[32];
            int num;
            if ((sscanf(input, "%s %s %d", keyword, select, &num) == 3) && (strcmp(select, "select") == 0)) {   //TODO finish autocomplit/history-complit
                char* command = history_get(h, num);
                if (command)
                    addstr(command);
                
            }
            else {
                history_list(h);
            }
       }


        else if (strcmp(keyword, "exit") == 0) 
            break;                                  

        else if (strcmp(keyword, "clear") == 0) 
            clear();             
        
        else if (strcmp(keyword, "plot") == 0) {
            double a = 0, b = 0, c = 0;
            if (sscanf(input, "%s %lf %lf %lf", keyword, &a, &b, &c) != 4 || !doubleCheck(3,  a, b, c)) {
                printw("Bad input. Type 'help' for additional info.\n");
                continue;
            }
            printGraph(a, b, c);
        }
            

        else if (strcmp(keyword, "solve") == 0) {
            double a = 0, b = 0, c = 0;
            if (sscanf(input, "%s %lf %lf %lf", keyword, &a, &b, &c) != 4 || !doubleCheck(3,  a, b, c)) {
                printw("Bad input. Type 'help' for additional info.\n");
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
            
            
            printw("%.2f x^2 + %.2f x + %.2f = 0  <=>  x \\in ", a, b, c);
            if (isnan(result_1))
                printw("\\emptyset\n");
            else if(isnan(result_2))
                printw("{ %lf }\n", result_1);
            else 
                printw("{ %lf, %lf }\n", result_1, result_2);
        }
        refresh();
        history_put(h, input);
    }
    history_list(h);
    history_destruct(h);

    endwin();
}
