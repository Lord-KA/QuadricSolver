#ifndef QUADRICSOLVER_H
#define QUADRICSOLVER_H

/**
 * @file Header for quadricSolver application
 */

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>        
#include <string.h>
#include <stdarg.h>
#include <math.h>
#include <ctype.h>

#include <ncurses.h>
/**
 * @fn double sign(double x)
 * @brief return sign of double x
 * @param x a double number
 */

double sign(double x) { 
    if (x < 0) return -1; 
    if (x > 0) return 1; 
    else return 0;
}

static const double TOL = 1e-3; //> Tolerance for double calculations
static const double GRAPH_TOL = 1; //> Tolerance for printing the graph
 
static const size_t HISTORY_LENGHT = 64;  //> the max number of entries in history           //TODO add Makefile; add `make install` option; add history save support
static const size_t MAX_CMD_LENGHT = 64;  //> the max len of an entry


#define ALT_BACKSPACE 127       //> macro for backspace entry recognition by NCurses

//==========================================
// Command history struct

/**
 * @struct History
 * @brief logs commands run in app
 */
struct History
{
    /** 
     * @brief pointer to log window
     */
    WINDOW *localWin; /** pointer to log window */


    /** 
     * @brief pointer to char* that is interpreted as char[HISTORY_LENGHT][MAX_CMD_LENGHT + 1] for comfy usage
     */
    char (*log)[HISTORY_LENGHT][MAX_CMD_LENGHT + 1]; /** pointer to char* that is interpreted as char[HISTORY_LENGHT][MAX_CMD_LENGHT + 1] for comfy usage */
    
    
    /** 
     * @brief index of string after the last
     */
    size_t end; /** index of string after the last */


    /** 
     * @brief bool flag states that history-saving is active
     */
    bool isActive; /** bool flag states that history-saving is active */
};

/**
 * @fn static void History_construct(struct History *h, WINDOW *historyWin)
 * @brief creates new history struct
 * Creates new history struct, if failes to allocate memory, sets isActive to false
 * @param h pointer to history struct to write results in
 * @param historyWin pointer to NCurses WINDOW in which log is shown
 */
static void History_construct(struct History *h, WINDOW *historyWin)                       //TODO ask DED if `History_*`is a good naming strategy
{
    h->localWin = historyWin;
    h->isActive = true;
    h->log = (char (*)[HISTORY_LENGHT][MAX_CMD_LENGHT + 1])calloc(HISTORY_LENGHT * (MAX_CMD_LENGHT + 1), sizeof(char));

    h->end = 0;
}

/**
 * @fn static void History_destruct(struct History *h)
 * @brief destroys history struct
 * @param h pointer to history struct to destroy
 */
static void History_destruct(struct History *h)
{
    if (h->log)
       free(h->log);
}

/**
 * @fn static void History_put(struct History *h, char *elem)
 * @brief adds new entry to history 
 * Adds new entry to history log, rewrites the oldest entry if nessesary
 * @param h pointer to history struct to write results in   
 * @param elem pointer to string with command to log
 */
static void History_put(struct History *h, char *elem)
{
    strcpy((*h->log)[h->end], elem);
    h->end = (h->end + 1) % HISTORY_LENGHT;
}

/**
 * @fn static char* History_get(struct History *h, size_t n)
 * @brief gets an entry from history
 * Gets an entry from history log, returns NULL on failure
 * @param h pointer to history struct to write results in   
 * @param n serial number of needed entry in log
 * @return pointer to string if succeeds, NULL otherwise
 */
static char* History_get(struct History *h, size_t n)          //TODO change one letter vars
{
    if (!h->isActive || n > HISTORY_LENGHT)
        return NULL;

    if ((*h->log)[(h->end - n) % HISTORY_LENGHT ][0])
        return ((*h->log)[(h->end - n) % HISTORY_LENGHT]);
    else 
        return NULL;
}

/**
 * @fn static void History_list(struct History *h)
 * @brief lists all history log
 * Lists all history log in localWin window
 * @param h pointer to history struct to write results in   
 */
static void History_list(struct History *h)
{
    if (!h->isActive)
        return;

    size_t i = 1;
    while ((*h->log)[(h->end - i) % HISTORY_LENGHT][0] && i != HISTORY_LENGHT) {
        wprintw(h->localWin, "%d: %s\n", i, (*h->log)[(h->end - i) % HISTORY_LENGHT]);
        ++i;
    }
}

//==========================================
// WINDOW management

/**
 * @fn static WINDOW* createWin(int height, int width, int starty, int startx)
 * @brief creates new window
 * Creates new NCurses window with specified parameters
 * @param height height of the window
 * @param width  width of the window
 * @param starty y coordinate of starting of the window
 * @param startx x coordinate of starting of the window
 * @return pointer to the new window
 */
static WINDOW *createWin(int height, int width, int starty, int startx)
{	
    WINDOW *localWin;

	localWin = newwin(height, width, starty, startx);
	box(localWin, 0 , 0);		/* 0, 0 gives default characters 
					             * for the vertical and horizontal
            					 * lines			                */
	wrefresh(localWin);		    

	return localWin;
}

/**
 * @fn static void destroyWin(WINDOW *localWin)
 * @brief destroys a window
 * Destroys an NCurses window and clears its insides
 */
static void destroyWin(WINDOW *localWin)
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
	wclear(localWin);
    wrefresh(localWin);
	delwin(localWin);
}

/**     
 * @fn static void mvwreadline(WINDOW *localWin, History *h, size_t starty, size_t startx, char *buffer, size_t buflen) 
 * @brief smart readline function with keybind support
 * Smart readline function with KEY_*, BACKSPACE, ENTER etc support
 * Read up to buflen characters into `buffer`.
 * A terminating '\0' character is added after the input.
 * @param localWin NCurses WINDOW to read from
 * @param starty y coordinate to read from
 * @param startx x coordinate to read from
 * @param buffer pointer to store input
 * @param buflen size of the buffer
 */
static void mvwreadline(WINDOW *localWin, History *h, size_t starty, size_t startx, char *buffer, size_t buflen)          //TODO refactor
{                                                                                                                         //TODO emplement saving unentered command     //TODO clean up the screen after each setting new command from history
    keypad(localWin, TRUE);
    ++buflen;       
    size_t old_curs = curs_set(1);
    size_t pos = 0;
    size_t len = 0;
    size_t historyPos = 0;

    while (true) {
        int c;

        buffer[len] = ' ';
        mvwaddnstr(localWin, starty, startx, buffer, len + 1);
        wmove(localWin, starty, startx + pos);
        c = wgetch(localWin);

        if (c == KEY_ENTER || c == '\n' || c == '\r') {
            break;
        } 
        else if (isprint(c)) {
            if (pos < buflen - 1) {
                memmove(buffer + pos + 1, buffer + pos, len - pos);
                buffer[pos++] = c;
                len += 1;
            } 
            else
                beep();
        }
        else if (c == KEY_LEFT) {
            if (pos > 0) {
                pos -= 1;
            }
            else 
                beep();
        } 
        else if (c == KEY_UP) {
            char *command;
            if (command = History_get(h, historyPos + 1)) {
                memset(buffer, ' ', buflen);

                strncpy(buffer, command, MAX_CMD_LENGHT);
                len = strlen(command);
                pos = strlen(command);
                ++historyPos;
            }
        }
        else if (c == KEY_DOWN) {
            char *command;
            if (historyPos - 1 != -1 && (command = History_get(h, historyPos - 1))) {
                memset(buffer, ' ', buflen);
                strncpy(buffer, command, MAX_CMD_LENGHT);
                len = strlen(command);
                pos = strlen(command);
                --historyPos;
            }
        }
        else if (c == KEY_RIGHT) {
            if (pos < len)
                pos += 1;
            else 
                beep();
        }
        else if (c == ALT_BACKSPACE) {
            if (pos > 0) {
                memmove(buffer+pos-1, buffer+pos, len-pos);
                pos -= 1;
                len -= 1;
            } 
            else 
                beep();
        }     
        else if (c == KEY_DC) {
            if (pos < len) {
                memmove(buffer+pos, buffer+pos+1, len-pos-1);
                len -= 1;
            }
            else 
                beep();
        }
        else 
            beep();

    }
    buffer[len] = '\0';
    if (old_curs != ERR)
        curs_set(old_curs);
}

//==========================================
// Main 

/**
 * @fn bool doubleValidate(size_t num, ...)
 * @brief checks correctness of all num of doubles
 * Checks correctness of all num of doubles 
 * @param num the number of entered doubles
 * @param ... va_list of doubles
 * @return true if all doubles are fine, false otherwise
 */
bool doubleValidate(size_t num, ...)            //TODO ask DED if it is reasonable to use abstrat func
{
    va_list args;
    va_start(args, num);
    

    for (int i = 0; i < num; ++i) {
        if (isnan(va_arg(args, double))){       //TODO maybe add something else?
            va_end(args);
            return false;
        }
    }

    va_end(args);
    return true;
}

/**
 * @fn void printGraph(double a, double b, double c)
 * @brief prints parabola y == a * x^2 + b * x + c
 * Creates NCurses windows and prints parabola y == a * x^2 + b * x + c there.
 * Uses GRAPH_EPS when compares doubles
 * @param a coefficient at x^2
 * @param b coefficient at x
 * @param c intercept
 */
void printGraph(double a, double b, double c)
{
    WINDOW* sideWin = createWin(LINES - 5, COLS / 2 + 1,  0, COLS / 2);
    WINDOW* plotWin = createWin(LINES - 15, COLS / 2 - 8, 5, COLS / 2 + 5);
	wborder(plotWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');

    double dimentionsDiff = 2.3;
    long long windowHight = LINES - 15;
    long long windowWidth = COLS / 2 - 10;

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
            else if (fabs(x*x*a + x*b + c - y) < GRAPH_TOL) {
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

/**
 * @fn void quadricSolver(double a, double b, double c, double *result_1, double *result_2, bool *result_eq_inf)
 * @brief solves quadric equasion
 * Solves quadric equasion and puts results in pointers
 * @param a coefficient at x^2
 * @param b coefficient at x
 * @param c intercept
 * @param result_1 pointer to result value one, before execution *result_1 == NAN
 * @param result_2 pointer to result value two, before execution *result_2 == NAN
 * @param result_eq_inf pointer to flag which states if there is inf num of solutions
 */
void quadricSolver(double a, double b, double c, double *result_1, double *result_2, bool *result_eq_inf)
{
    if (fabs(a) < TOL) {
        if (fabs(b) < TOL) {
            if (fabs(c) < TOL)
                *result_eq_inf = true;              
            else
                return;
        }
        else {
            *result_1 = -c / b;
            *result_2 = *result_1;
        }
    }

   else {                                                       // Quadric case
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
