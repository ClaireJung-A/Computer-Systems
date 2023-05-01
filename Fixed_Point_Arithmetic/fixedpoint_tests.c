#include <stdio.h>
#include <stdlib.h>
#include "fixedpoint.h"
#include "tctest.h"

// Test fixture object, has some useful values for testing
typedef struct {
    Fixedpoint zero;
    Fixedpoint one;
    Fixedpoint one_half;
    Fixedpoint one_fourth;
    Fixedpoint large1;
    Fixedpoint large2;
    Fixedpoint max;
} TestObjs;

// functions to create and destroy the test fixture
TestObjs *setup(void);
void cleanup(TestObjs *objs);

// test functions
void test_whole_part(TestObjs *objs);
void test_frac_part(TestObjs *objs);
void test_is_zero(TestObjs *objs);
void test_create_from_hex(TestObjs *objs);
void test_format_as_hex(TestObjs *objs);
void test_negate(TestObjs *objs);
void test_add(TestObjs *objs);
void test_sub(TestObjs *objs);
void test_is_overflow_pos(TestObjs *objs);
void test_is_err(TestObjs *objs);

int main(int argc, char **argv) {
    // if a testname was specified on the command line, only that
    // test function will be executed
    if (argc > 1) {
        tctest_testname_to_execute = argv[1];
    }

    TEST_INIT();

    TEST(test_whole_part);
    TEST(test_frac_part);
    TEST(test_is_zero);
    TEST(test_create_from_hex);
    TEST(test_format_as_hex);
    TEST(test_negate);
    TEST(test_add);
    TEST(test_sub);
    TEST(test_is_overflow_pos);
    TEST(test_is_err);

    // IMPORTANT: if you add additional test functions (which you should!),
    // make sure they are included here.  E.g., if you add a test function
    // "my_awesome_tests", you should add
    //
    //   TEST(my_awesome_tests);
    //
    // here. This ensures that your test function will actually be executed.

    TEST_FINI();
}

TestObjs *setup(void) {
    TestObjs *objs = malloc(sizeof(TestObjs));

    objs->zero = fixedpoint_create(0UL);
    objs->one = fixedpoint_create(1UL);
    objs->one_half = fixedpoint_create2(0UL, 0x8000000000000000UL);
    objs->one_fourth = fixedpoint_create2(0UL, 0x4000000000000000UL);
    objs->large1 = fixedpoint_create2(0x4b19efceaUL, 0xec9a1e2418UL);
    objs->large2 = fixedpoint_create2(0xfcbf3d5UL, 0x4d1a23c24fafUL);
    objs->max = fixedpoint_create2(0xFFFFFFFFFFFFFFFFUL, 0xFFFFFFFFFFFFFFFFUL);

    return objs;
}

void cleanup(TestObjs *objs) {
    free(objs);
}

void test_whole_part(TestObjs *objs) {
    ASSERT(0UL == fixedpoint_whole_part(objs->zero));
    ASSERT(1UL == fixedpoint_whole_part(objs->one));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
    ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
    ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
}

void test_frac_part(TestObjs *objs) {
    ASSERT(0UL == fixedpoint_frac_part(objs->zero));
    ASSERT(0UL == fixedpoint_frac_part(objs->one));
    ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
    ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
    ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
    ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_is_zero(TestObjs *objs) {
    ASSERT(1 == fixedpoint_is_zero(objs->zero));
    ASSERT(0 == fixedpoint_is_zero(objs->one));
    ASSERT(0 == fixedpoint_is_zero(objs->one_half));
    ASSERT(0 == fixedpoint_is_zero(objs->one_fourth));
    ASSERT(0 == fixedpoint_is_zero(objs->large1));
    ASSERT(0 == fixedpoint_is_zero(objs->large2));
}

void test_create_from_hex(TestObjs *objs) {
    (void) objs;

    Fixedpoint val1 = fixedpoint_create_from_hex("f6a5865.00f2");
    ASSERT(fixedpoint_is_valid(val1));
    ASSERT(!fixedpoint_is_err(val1));
    ASSERT(0xf6a5865UL == fixedpoint_whole_part(val1));
    ASSERT(0x00f2000000000000UL == fixedpoint_frac_part(val1));
    ASSERT(!fixedpoint_is_neg(val1));

    Fixedpoint val2 = fixedpoint_create_from_hex("0");
    ASSERT(fixedpoint_is_valid(val2));
    ASSERT(!fixedpoint_is_err(val2));
    ASSERT(0UL == fixedpoint_whole_part(val2));
    ASSERT(0UL == fixedpoint_frac_part(val2));
    ASSERT(!fixedpoint_is_neg(val2));

    Fixedpoint val3 = fixedpoint_create_from_hex("-3a");
    ASSERT(fixedpoint_is_valid(val3));
    ASSERT(!fixedpoint_is_err(val3));
    ASSERT(0x3a == fixedpoint_whole_part(val3));
    ASSERT(0UL == fixedpoint_frac_part(val3));
    ASSERT(fixedpoint_is_neg(val3));

    Fixedpoint val4 = fixedpoint_create_from_hex("-f6a5865.028d01000bc");
    ASSERT(fixedpoint_is_valid(val4));
    ASSERT(!fixedpoint_is_err(val4));
    ASSERT(0xf6a5865UL == fixedpoint_whole_part(val4));
    ASSERT(0x028d01000bc00000UL == fixedpoint_frac_part(val4));
    ASSERT(fixedpoint_is_neg(val4));

    Fixedpoint val5 = fixedpoint_create_from_hex("UXYZ");
    ASSERT(fixedpoint_is_err(val5));

    Fixedpoint val6 = fixedpoint_create_from_hex("-imt.0192");
    ASSERT(fixedpoint_is_err(val6));

    Fixedpoint val7 = fixedpoint_create_from_hex("-.028d01000bc");
    ASSERT(fixedpoint_is_valid(val7));
    ASSERT(!fixedpoint_is_err(val7));
    ASSERT(0UL == fixedpoint_whole_part(val7));
    ASSERT(0x028d01000bc00000UL == fixedpoint_frac_part(val7));
    ASSERT(fixedpoint_is_neg(val7));

    Fixedpoint val8 = fixedpoint_create_from_hex(".028d01000bc");
    ASSERT(fixedpoint_is_valid(val8));
    ASSERT(!fixedpoint_is_err(val8));
    ASSERT(0UL == fixedpoint_whole_part(val8));
    ASSERT(0x028d01000bc00000UL == fixedpoint_frac_part(val8));
    ASSERT(!fixedpoint_is_neg(val8));

    Fixedpoint val9 = fixedpoint_create_from_hex("BcF.");
    ASSERT(fixedpoint_is_valid(val9));
    ASSERT(!fixedpoint_is_err(val9));
    ASSERT(0xbcfUL == fixedpoint_whole_part(val9));
    ASSERT(0UL == fixedpoint_frac_part(val9));
    ASSERT(!fixedpoint_is_neg(val9));

    Fixedpoint valA = fixedpoint_create_from_hex("-BcF.");
    ASSERT(fixedpoint_is_valid(valA));
    ASSERT(!fixedpoint_is_err(valA));
    ASSERT(0xbcfUL == fixedpoint_whole_part(valA));
    ASSERT(0UL == fixedpoint_frac_part(valA));
    ASSERT(fixedpoint_is_neg(valA));

    Fixedpoint valb = fixedpoint_create_from_hex("123");
    ASSERT(fixedpoint_is_valid(valb));
    ASSERT(!fixedpoint_is_err(valb));
    ASSERT(0x123UL == fixedpoint_whole_part(valb));
    ASSERT(0UL == fixedpoint_frac_part(valb));
    ASSERT(!fixedpoint_is_neg(valb));

    Fixedpoint valc = fixedpoint_create_from_hex("-123");
    ASSERT(fixedpoint_is_valid(valc));
    ASSERT(!fixedpoint_is_err(valc));
    ASSERT(0x123UL == fixedpoint_whole_part(valc));
    ASSERT(0UL == fixedpoint_frac_part(valc));
    ASSERT(fixedpoint_is_neg(valc));

    Fixedpoint vald = fixedpoint_create_from_hex("ABACAB");
    ASSERT(fixedpoint_is_valid(vald));
    ASSERT(!fixedpoint_is_err(vald));
    ASSERT(0xabacabUL == fixedpoint_whole_part(vald));
    ASSERT(0UL == fixedpoint_frac_part(vald));
    ASSERT(!fixedpoint_is_neg(vald));

    Fixedpoint vale = fixedpoint_create_from_hex("1");
    ASSERT(fixedpoint_is_valid(vale));
    ASSERT(!fixedpoint_is_err(vale));
    ASSERT(1UL == fixedpoint_whole_part(vale));
    ASSERT(0UL == fixedpoint_frac_part(vale));
    ASSERT(!fixedpoint_is_neg(vale));

    Fixedpoint valf = fixedpoint_create_from_hex("0.00001");
    ASSERT(fixedpoint_is_valid(valf));
    ASSERT(!fixedpoint_is_err(valf));
    ASSERT(0UL == fixedpoint_whole_part(valf));
    ASSERT(0x0000100000000000UL == fixedpoint_frac_part(valf));
    ASSERT(!fixedpoint_is_neg(valf));
}

void test_format_as_hex(TestObjs *objs) {
    char *s;

    s = fixedpoint_format_as_hex(objs->zero);
    ASSERT(0 == strcmp(s, "0"));
    free(s);

    s = fixedpoint_format_as_hex(objs->one);
    ASSERT(0 == strcmp(s, "1"));
    free(s);

    s = fixedpoint_format_as_hex(objs->one_half);
    ASSERT(0 == strcmp(s, "0.8"));
    free(s);

    s = fixedpoint_format_as_hex(objs->one_fourth);
    ASSERT(0 == strcmp(s, "0.4"));
    free(s);

    s = fixedpoint_format_as_hex(objs->large1);
    ASSERT(0 == strcmp(s, "4b19efcea.000000ec9a1e2418"));
    free(s);

    s = fixedpoint_format_as_hex(objs->large2);
    ASSERT(0 == strcmp(s, "fcbf3d5.00004d1a23c24faf"));
    free(s);
}

void test_negate(TestObjs *objs) {
    // none of the test fixture objects are negative
    ASSERT(!fixedpoint_is_neg(objs->zero));
    ASSERT(!fixedpoint_is_neg(objs->one));
    ASSERT(!fixedpoint_is_neg(objs->one_half));
    ASSERT(!fixedpoint_is_neg(objs->one_fourth));
    ASSERT(!fixedpoint_is_neg(objs->large1));
    ASSERT(!fixedpoint_is_neg(objs->large2));

    // negate the test fixture values
    Fixedpoint zero_neg = fixedpoint_negate(objs->zero);
    Fixedpoint one_neg = fixedpoint_negate(objs->one);
    Fixedpoint one_half_neg = fixedpoint_negate(objs->one_half);
    Fixedpoint one_fourth_neg = fixedpoint_negate(objs->one_fourth);
    Fixedpoint large1_neg = fixedpoint_negate(objs->large1);
    Fixedpoint large2_neg = fixedpoint_negate(objs->large2);

    // zero does not become negative when negated
    ASSERT(!fixedpoint_is_neg(zero_neg));

    // all of the other values should have become negative when negated
    ASSERT(fixedpoint_is_neg(one_neg));
    ASSERT(fixedpoint_is_neg(one_half_neg));
    ASSERT(fixedpoint_is_neg(one_fourth_neg));
    ASSERT(fixedpoint_is_neg(large1_neg));
    ASSERT(fixedpoint_is_neg(large2_neg));

    // magnitudes should stay the same
    ASSERT(0UL == fixedpoint_whole_part(objs->zero));
    ASSERT(1UL == fixedpoint_whole_part(objs->one));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
    ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
    ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
    ASSERT(0UL == fixedpoint_frac_part(objs->zero));
    ASSERT(0UL == fixedpoint_frac_part(objs->one));
    ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
    ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
    ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
    ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));

    //initialization
    Fixedpoint zero_pos = fixedpoint_negate(zero_neg);
    Fixedpoint one_pos = fixedpoint_negate(one_neg);
    Fixedpoint one_half_pos = fixedpoint_negate(one_half_neg);
    Fixedpoint one_fourth_pos = fixedpoint_negate(one_fourth_neg);
    Fixedpoint large1_pos = fixedpoint_negate(large1_neg);
    Fixedpoint large2_pos = fixedpoint_negate(large2_neg);

    // check if all values are negated
    ASSERT(!fixedpoint_is_neg(one_pos));
    ASSERT(!fixedpoint_is_neg(one_half_pos));
    ASSERT(!fixedpoint_is_neg(one_fourth_pos));
    ASSERT(!fixedpoint_is_neg(large1_pos));
    ASSERT(!fixedpoint_is_neg(large2_pos));
    ASSERT(!fixedpoint_is_neg(zero_pos));

    // check if magnitudes are the same
    ASSERT(0UL == fixedpoint_whole_part(objs->zero));
    ASSERT(1UL == fixedpoint_whole_part(objs->one));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_half));
    ASSERT(0UL == fixedpoint_whole_part(objs->one_fourth));
    ASSERT(0x4b19efceaUL == fixedpoint_whole_part(objs->large1));
    ASSERT(0xfcbf3d5UL == fixedpoint_whole_part(objs->large2));
    ASSERT(0UL == fixedpoint_frac_part(objs->zero));
    ASSERT(0UL == fixedpoint_frac_part(objs->one));
    ASSERT(0x8000000000000000UL == fixedpoint_frac_part(objs->one_half));
    ASSERT(0x4000000000000000UL == fixedpoint_frac_part(objs->one_fourth));
    ASSERT(0xec9a1e2418UL == fixedpoint_frac_part(objs->large1));
    ASSERT(0x4d1a23c24fafUL == fixedpoint_frac_part(objs->large2));
}

void test_add(TestObjs *objs) {
    (void) objs;

    Fixedpoint lhs, rhs, sum;

    lhs = fixedpoint_create_from_hex("-c7252a193ae07.7a51de9ea0538c5");
    rhs = fixedpoint_create_from_hex("d09079.1e6d601");
    sum = fixedpoint_add(lhs, rhs);
    ASSERT(fixedpoint_is_neg(sum));
    ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
    ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));

    // large pos + small neg
    lhs = fixedpoint_create_from_hex("c7252a193ae07.7a51de9ea0538c5");
    rhs = fixedpoint_create_from_hex("-d09079.1e6d601");
    sum = fixedpoint_add(lhs, rhs);
    ASSERT(0xc7252a0c31d8eUL == fixedpoint_whole_part(sum));
    ASSERT(0x5be47e8ea0538c50UL == fixedpoint_frac_part(sum));
}

void test_sub(TestObjs *objs) {
    (void) objs;

    Fixedpoint lhs, rhs, diff;

    lhs = fixedpoint_create_from_hex("-ccf35aa3a04a3b.b105");
    rhs = fixedpoint_create_from_hex("f676e8.58");
    diff = fixedpoint_sub(lhs, rhs);
    ASSERT(fixedpoint_is_neg(diff));
    ASSERT(0xccf35aa496c124UL == fixedpoint_whole_part(diff));
    ASSERT(0x0905000000000000UL == fixedpoint_frac_part(diff));
}

void test_is_overflow_pos(TestObjs *objs) {
    Fixedpoint sum;

    sum = fixedpoint_add(objs->max, objs->one);
    ASSERT(fixedpoint_is_overflow_pos(sum));

    sum = fixedpoint_add(objs->one, objs->max);
    ASSERT(fixedpoint_is_overflow_pos(sum));

    Fixedpoint negative_one = fixedpoint_negate(objs->one);

    sum = fixedpoint_sub(objs->max, negative_one);
    ASSERT(fixedpoint_is_overflow_pos(sum));

    Fixedpoint fp_neg = fixedpoint_negate(objs->max);
    sum = fixedpoint_sub(objs->one, fp_neg);
    ASSERT(fixedpoint_is_overflow_pos(sum));

    Fixedpoint fp2;

    fp2 = fixedpoint_double(objs->max);
    ASSERT(fixedpoint_is_overflow_pos(fp2));
}

void test_is_err(TestObjs *objs) {
    (void) objs;

    // too many characters
    Fixedpoint err1 = fixedpoint_create_from_hex("88888888888888889.6666666666666666");
    ASSERT(fixedpoint_is_err(err1));

    // too many characters
    Fixedpoint err2 = fixedpoint_create_from_hex("6666666666666666.88888888888888889");
    ASSERT(fixedpoint_is_err(err2));

    // this one is actually fine
    Fixedpoint err3 = fixedpoint_create_from_hex("-6666666666666666.8888888888888888");
    ASSERT(fixedpoint_is_valid(err3));
    ASSERT(!fixedpoint_is_err(err3));

    // whole part is too large
    Fixedpoint err4 = fixedpoint_create_from_hex("88888888888888889");
    ASSERT(fixedpoint_is_err(err4));

    // fractional part is too large
    Fixedpoint err5 = fixedpoint_create_from_hex("7.88888888888888889");
    ASSERT(fixedpoint_is_err(err5));

    // invalid hex digits in whole part
    Fixedpoint err6 = fixedpoint_create_from_hex("123xabc.4");
    ASSERT(fixedpoint_is_err(err6));

    // invalid hex digits in fractional part
    Fixedpoint err7 = fixedpoint_create_from_hex("7.0?4");
    ASSERT(fixedpoint_is_err(err7));

    // negative sign in wrong position
    Fixedpoint err9 = fixedpoint_create_from_hex("abd.-1234");
    ASSERT(fixedpoint_is_err(err9));

    // too many periods
    Fixedpoint err10 = fixedpoint_create_from_hex("a..b");
    ASSERT(fixedpoint_is_err(err10));

    //more than one negative sign
    Fixedpoint err11 = fixedpoint_create_from_hex("--abc,123");
    ASSERT(fixedpoint_is_err(err11));

    // empty
    Fixedpoint err8 = fixedpoint_create_from_hex("");
    ASSERT(fixedpoint_is_err(err8));
}
