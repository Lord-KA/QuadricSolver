#define AUTOTESTS
#include "./quadricSolver.h"
#include "gtest/gtest.h"



TEST(History, Manual)
{
    initscr();


    history* h;
    
    history_construct(h, stdscr);

    history_put(h, "first line");
    
    history_put(h, "second line");
    
    history_put(h, "third line");

    // history_put(h, "1234567890123456789012345678901234567890123456789012345678901234567890123456789000");

    history_put(h, "fifth line");
    
    history_put(h, "sixth line");

    history_list(h);

    history_destruct(h);

    // getch();
    clear();
    endwin();
}


TEST(History, Ranges)
{
    initscr();


    history* h;
    
    history_construct(h, stdscr);

    history_put(h, "first line");
    
    history_put(h, "second line");
    
    history_put(h, "third line");

    //history_put(h, "1234567890123456789012345678901234567890123456789012345678901234567890123456789000");

    history_put(h, "fifth line");

    for (size_t i = 0; i < 100; ++i) {
        history_put(h, "Same f****g string");
    }

    history_list(h);

    history_destruct(h);

    // getch();
    clear();
    endwin();
}


TEST(QuadricSolver, Manual)
{
    double result_1 = NAN, result_2 = NAN;
    bool result_eq_inf = false;
    quadricSolver(1, 2, 1, &result_1, &result_2, &result_eq_inf);
    EXPECT_FLOAT_EQ(result_1, -1);
    EXPECT_FLOAT_EQ(result_2, -1);
    EXPECT_FALSE(result_eq_inf);
    

    result_1 = NAN, result_2 = NAN;
    result_eq_inf = false;
    quadricSolver(1, 0, 0, &result_1, &result_2, &result_eq_inf);
    EXPECT_NEAR(result_1, 0, TOL);
    EXPECT_NEAR(result_2, 0, TOL);
    EXPECT_FALSE(result_eq_inf);


    result_1 = NAN, result_2 = NAN;
    result_eq_inf = false;
    quadricSolver(14, -97, 113, &result_1, &result_2, &result_eq_inf);
    EXPECT_NEAR(result_1, 1.4819, TOL);
    EXPECT_NEAR(result_2, 5.4467, TOL);
    EXPECT_FALSE(result_eq_inf);


    result_1 = NAN, result_2 = NAN;
    result_eq_inf = false;
    quadricSolver(1400, -97, 113, &result_1, &result_2, &result_eq_inf);
    EXPECT_TRUE(isnan(result_1));
    EXPECT_TRUE(isnan(result_2));
    EXPECT_FALSE(result_eq_inf);


    result_1 = NAN, result_2 = NAN;
    result_eq_inf = false;
    quadricSolver(0, -97, 113, &result_1, &result_2, &result_eq_inf);
    EXPECT_NEAR(result_1, 1.1649, TOL);
    EXPECT_TRUE(isnan(result_2));
    EXPECT_FALSE(result_eq_inf);


    result_1 = NAN, result_2 = NAN;
    result_eq_inf = false;
    quadricSolver(0, 0, 113, &result_1, &result_2, &result_eq_inf);
    EXPECT_TRUE(isnan(result_1));
    EXPECT_TRUE(isnan(result_2));
    EXPECT_TRUE(result_eq_inf);
}

/*
TEST(QuadricSolver, Ranges)         //TODO add Ranged tests 
{
    for (float a = -10; a < 10; a += 0.01)
        for (float b = -10; b < 10; b += 0.01)
            for (float c = -10; c < 10; c += 0.01) {    
                if (doubleCheck(3,  a, b, c)) {
                double result_1 = NAN, result_2 = NAN;
                bool result_eq_inf = false;
                quadricSolver(0, -97, 113, &result_1, &result_2, &result_eq_inf);
                EXPECT_NEAR(result_1, 1.1649, TOL);
                EXPECT_TRUE(isnan(result_2));
                EXPECT_FALSE(result_eq_inf);
                }
    }
}
*/                                  //TODO add random tests
