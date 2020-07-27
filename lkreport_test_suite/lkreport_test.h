#ifndef LKREPORT_TEST_H
#define LKREPORT_TEST_H

int lkreport_wrapper(char *filepath, int flag);
int test_write_header();
int test_orphan_free();
int test_bad_free();
int test_match();
int test_serious();
int test_rdouble_free();
int test_approx();

#endif /*LKREPORT_TEST_H*/
