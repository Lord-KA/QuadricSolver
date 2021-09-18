#define AUTOTESTS
#include "./quadricSolver.h"
#include "gtest/gtest.h"

static const double TEST_TOL = 1e-2;

TEST(History, Manual)
{
    initscr();


    History history;
    History *h = &history;
    
    History_construct(h, nullptr);

    History_put(h, "first line");
    
    History_put(h, "second line");
    
    History_put(h, "third line");

    // History_put(h, "1234567890123456789012345678901234567890123456789012345678901234567890123456789000");

    History_put(h, "fifth line");
    
    History_put(h, "sixth line");

    // History_list(h);

    char *command = History_get(h, 2);
    if (command)
        printf("%s\n", command);
    else
        printf("NULL\n");
 
    History_destruct(h);

    getch();
    clear();
    endwin();
}


TEST(History, Ranges)
{
    initscr();


    History history;
    History *h = &history;
    
    History_construct(h, stdscr);

    History_put(h, "first line");
    
    History_put(h, "second line");
    
    History_put(h, "third line");

    // History_put(h, "1234567890123456789012345678901234567890123456789012345678901234567890123456789000");

    History_put(h, "fifth line");

    for (size_t i = 0; i < 100; ++i) {
        History_put(h, "Same f****g string");
    }

    History_list(h);

    History_destruct(h);

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
    EXPECT_NEAR(result_2, 1.1649, TOL);
    EXPECT_FALSE(result_eq_inf);


    result_1 = NAN, result_2 = NAN;
    result_eq_inf = false;
    quadricSolver(0, 0, 113, &result_1, &result_2, &result_eq_inf);
    EXPECT_TRUE(isnan(result_1));
    EXPECT_TRUE(isnan(result_2));
    EXPECT_FALSE(result_eq_inf);


    result_1 = NAN, result_2 = NAN;
    result_eq_inf = false;
    quadricSolver(0, 0, 0, &result_1, &result_2, &result_eq_inf);
    EXPECT_TRUE(isnan(result_1));
    EXPECT_TRUE(isnan(result_2));
    EXPECT_TRUE(result_eq_inf);

}


/*
TEST(QuadricSolver, Ranges)         //TODO add Ranged tests 
{
    for (float a = -5; a < 5; a += 0.035)
        for (float b = -5; b < 5; b += 0.035)
            for (float c = -5; c < 5; c += 0.035) {    
                if (doubleValidate(3,  a, b, c)) {
                    double result_1 = NAN, result_2 = NAN;
                    bool result_eq_inf = false;
                    quadricSolver(a, b, c, &result_1, &result_2, &result_eq_inf);
                    if (doubleValidate(2,  result_1, result_2)) {
                        printf("%f, %f, %f\n", a, b, c);
                        assert(fabs(result_1 * result_1 * a + result_1 * b + c) < TEST_TOL);
                        EXPECT_TRUE(fabs(result_1 * result_1 * a + result_1 * b + c) < TEST_TOL);
                        EXPECT_TRUE(fabs(result_2 * result_2 * a + result_2 * b + c) < TEST_TOL);
                    }

                }
    }
}


TEST(QuadricSolver, Random)         
{
    for (size_t i = 0; i < 1e8; ++i) {
            

        if (doubleValidate(3,  a, b, c)) {
            double result_1 = NAN, result_2 = NAN;
            bool result_eq_inf = false;
            quadricSolver(a, b, c, &result_1, &result_2, &result_eq_inf);
            if (doubleValidate(2,  result_1, result_2)) {
                EXPECT_TRUE(fabs(result_1 * result_1 * a + result_1 * b + c) < TOL);
                EXPECT_TRUE(fabs(result_2 * result_2 * a + result_2 * b + c) < TOL);
            }
        }
    }
}                                 //TODO add random tests
*/
