#include "quadricSolver.h"

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
           
            quadricSolver(a, b, c, &result_1, &result_2);

            printw("%.2f x^2 + %.2f x + %.2f = 0  <=>  x \\in ", a, b, c);
            if (isnan(result_1))
                printw("\\emptyset\n");
            else if(isnan(result_2))
                printw("{ %lf }\n", result_1);
            else 
                printw("{ %lf, %lf }\n", result_1, result_2);
        }
        else {
            printw("Bad input. Type 'help' for additional info.\n");
        }
        refresh();
        history_put(h, input);
    }
    history_list(h);
    history_destruct(h);

    endwin();
}
