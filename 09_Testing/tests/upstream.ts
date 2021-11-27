#include <check.h>
#include "buf.h"

#suite InitializationFree
#test initialization_free_test
    float *a = 0;
    ck_assert_int_eq(buf_capacity(a), 0);
    ck_assert_int_eq(buf_size(a), 0);

    buf_push(a, 1.3f);
    ck_assert_int_eq(buf_size(a), 1);
    /* No ck_assert_float_eq in old version of check lib */
    ck_assert_int_eq(a[0] - 1.3f != 0.0f, 0);

    buf_clear(a);
    ck_assert_int_eq(buf_size(a), 0);
    ck_assert_ptr_ne(a, NULL);

    buf_free(a);
    ck_assert_ptr_eq(a, NULL);

    buf_clear(a);
    ck_assert_int_eq(buf_size(a), 0);
    ck_assert_ptr_eq(a, NULL);

#suite Push
#test push_test
    long *ai = 0;
    for (int i = 0; i < 10000; i++)
        buf_push(ai, i);  
    ck_assert_int_eq(buf_size(ai), 10000);
    int match = 0;
    for (int i = 0; i < (int)(buf_size(ai)); i++)
        match += ai[i] == i;
    ck_assert_int_eq(match, 10000);
    buf_free(ai);

#suite GrowTrunc
#test grow_trunc_test
    long *ai = 0;
    buf_grow(ai, 1000);
    ck_assert_int_eq(buf_capacity(ai), 1000);
    ck_assert_int_eq(buf_size(ai), 0);
    buf_trunc(ai, 100);
    ck_assert_int_eq(buf_capacity(ai), 100);
    buf_free(ai);

#suite Pop
#test pop_test
    float *a = 0;
    buf_push(a, 1.1);
    buf_push(a, 1.2);
    buf_push(a, 1.3);
    buf_push(a, 1.4);
    ck_assert_int_eq(buf_size(a), 4);
    ck_assert_int_eq(buf_pop(a) - 1.4f != 0.0f, 0);
    buf_trunc(a, 3);
    ck_assert_int_eq(buf_size(a), 3);
    ck_assert_int_eq(buf_pop(a) - 1.3f != 0.0f, 0);
    ck_assert_int_eq(buf_pop(a) - 1.2f != 0.0f, 0);
    ck_assert_int_eq(buf_pop(a) - 1.1f != 0.0f, 0);
    ck_assert_int_eq(buf_size(a), 0);
    buf_free(a);
