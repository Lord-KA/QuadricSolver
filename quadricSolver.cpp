#include "quadricSolver.h"

int main() 
{
    initscr();

    cbreak();
    noecho();

    WINDOW* logWin = createWin(LINES - 5, COLS / 2, 0, 0);
    wborder(logWin, ' ', ' ', ' ',' ',' ',' ',' ',' ');

    wprintw(logWin, "==============================\n");
    wprintw(logWin, "== QuadricSolver by Lord-KA ==\n");
    wprintw(logWin, "==============================\n");
    wrefresh(logWin);

    char keyword[MAX_CMD_LENGHT];
    char input  [MAX_CMD_LENGHT];

    struct History *h = (History*)calloc(1, sizeof(struct History));
    History_construct(h, logWin);

    WINDOW *inputWin;
    while (true) { 
        inputWin = createWin(5, COLS, LINES - 5, 0);

        mvwprintw(inputWin, 2, 2, ">>> ");
        wrefresh(inputWin);

        mvwreadline(inputWin, h, 2, 6, input, MAX_CMD_LENGHT);

        wprintw(logWin, ">>> %s\n", input);
        
        sscanf(input, "%s", keyword);

        if (strncmp(keyword, "help", MAX_CMD_LENGHT) == 0)
            wprintw(logWin, "This is a (very usefull) help message.\n"); //TODO write reasonable help

        else if (strncmp(keyword, "history", MAX_CMD_LENGHT) == 0) {
            History_list(h);
       }


        else if (strncmp(keyword, "exit", MAX_CMD_LENGHT) == 0) 
            break;                                  

        else if (strcmp(keyword, "clear") == 0) 
            wclear(logWin);             
        
        else if (strcmp(keyword, "plot") == 0) {
            double a = 0, b = 0, c = 0;
            if (sscanf(input, "%s %lf %lf %lf", keyword, &a, &b, &c) != 4 || !doubleValidate(3,  a, b, c)) {
                wprintw(logWin, "Bad input. Type 'help' for additional info.\n");
            }
            else {
                printGraph(a, b, c);
            }
        }
            

        else if (strcmp(keyword, "solve") == 0) {
            double a = 0, b = 0, c = 0;
            if (sscanf(input, "%s %lf %lf %lf", keyword, &a, &b, &c) != 4 || !doubleValidate(3,  a, b, c)) {
                wprintw(logWin, "Bad input. Type 'help' for additional info.\n");
            }
            else {
                double result_1 = NAN;
                double result_2 = NAN;
                bool result_eq_inf = false;
           
                quadricSolver(a, b, c, &result_1, &result_2, &result_eq_inf);

                wprintw(logWin, "%.2f x^2 + %.2f x + %.2f = 0  <=>  x \\in ", a, b, c);
                if (result_eq_inf)
                    wprintw(logWin, "\\R \n");
                else if (isnan(result_1))
                    wprintw(logWin, "\\emptyset\n");
                else if(isnan(result_2) || fabs(result_1 - result_2) < TOL)
                    wprintw(logWin, "{ %lf }\n", result_1);
                else 
                    wprintw(logWin, "{ %lf, %lf }\n", result_1, result_2);
            }
        }
        else {
            wprintw(logWin, "Bad input. Type 'help' for additional info.\n");
        }
        
        destroyWin(inputWin);
        wrefresh(logWin);
        History_put(h, input);
    }
    History_list(h);
    History_destruct(h);
    free(h);

    destroyWin(logWin);

    endwin();
}
