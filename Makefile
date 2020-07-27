SRC = LKmalloc.c
OBJ = LKmalloc.o hash_table.o metadata.o 
OBJTESTDEP = lkmalloc_test.o lkfree_test.o lkreport_test.o
DEP = lkmalloc.o hash_table.o metadata.o
OBJ_LKM_TEST = lkm_t_double_malloc.o lkm_t_reg.o lkm_t_init.o lkm_t_under.o lkm_t_over.o lkm_t_init_under.o lkm_t_init_over.o lkm_t_under_over.o lkm_t_init_under_over.o
OBJ_LKF_TEST = lkf_t_double_free.o lkf_t_null.o lkf_t_reg.o lkf_t_unalloc.o lkf_t_approx_pass.o lkf_t_approx_fail.o lkf_t_warn.o lkf_t_unknown.o lkf_t_warn_error.o lkf_t_unknown_error.o
OBJ_LKR_TEST = lkr_t_orphan_free.o lkr_t_bad_free.o lkr_t_match.o lkr_t_serious.o lkr_t_double_free.o lkr_t_approx.o
OBJCLEAN = $(OBJTESTDEP) $(OBJ_LKM_TEST) $(OBJ_LKF_TEST) $(OBJ_LKR_TEST)
TESTBIN = \
lkm_t_double_malloc lkm_t_reg_malloc lkm_t_init_malloc lkm_t_under lkm_t_over lkm_t_init_under lkm_t_init_over lkm_t_under_over lkm_t_init_under_over \
lkf_t_double_free lkf_t_null lkf_t_reg lkf_t_unalloc lkf_t_approx_pass lkf_t_approx_fail lkf_t_warn lkf_t_unknown lkf_t_warn_error lkf_t_unknown_error lkr_t_orphan_free lkr_t_bad_free lkr_t_match lkr_t_serious lkr_t_double_free lkr_t_approx

TESTPROG = lkmalloc_test.o lkfree_test.o lkreport_test.o
CC = gcc
CFLAGS = -g -O2 -Wall -Werror
HEADERS = LKmalloc.h hash_table.h metadata.h
LDFLAGS = lkmalloc.a -lm  
LKM_T = lkmalloc_test_suite/
LKF_T = lkfree_test_suite/
LKR_T = lkreport_test_suite/

depend: lib $(DEP) $(HEADERS)

lib: $(DEP) $(HEADERS)
	ar -vq lkmalloc.a LKmalloc.o hash_table.o metadata.o

lkmalloc: lkmalloc.o $(HEADERS)
	$(CC) -o lkmalloc $(OBJ)

lkmalloc.o: LKmalloc.c 
	$(CC) $(CFLAGS) -c LKmalloc.c

lkmalloc_test.o: $(LKM_T)lkmalloc_test.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkmalloc_test.c

lkfree_test.o: $(LKF_T)lkfree_test.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkfree_test.c

lkreport_test.o: $(LKR_T)lkreport_test.c
	$(CC) $(CFLAGS) -c $(LKR_T)lkreport_test.c

hash_t.o: hash_table.c
	$(CC) $(CFLAGS) -c hash_table.c

metadata.o: metadata.c
	$(CC) $(CFLAGS) -c metadata.c

lkm_tests: depend $(TESTPROG) $(OBJ_LKM_TEST) $(OBJ_LKF_TEST) $(OBJ_LKR_TEST)
	$(CC) -o lkm_t_double_malloc $(OBJTESTDEP) lkm_t_double_malloc.o $(LDFLAGS)
	$(CC) -o lkm_t_reg_malloc $(OBJTESTDEP) lkm_t_reg.o $(LDFLAGS)
	$(CC) -o lkm_t_init_malloc $(OBJTESTDEP) lkm_t_init.o $(LDFLAGS)
	$(CC) -o lkm_t_under $(OBJTESTDEP) lkm_t_under.o $(LDFLAGS)
	$(CC) -o lkm_t_over $(OBJTESTDEP) lkm_t_over.o $(LDFLAGS)
	$(CC) -o lkm_t_init_under $(OBJTESTDEP) lkm_t_init_under.o $(LDFLAGS)
	$(CC) -o lkm_t_init_over $(OBJTESTDEP) lkm_t_init_over.o $(LDFLAGS)
	$(CC) -o lkm_t_under_over $(OBJTESTDEP) lkm_t_under_over.o $(LDFLAGS)
	$(CC) -o lkm_t_init_under_over $(OBJTESTDEP) lkm_t_init_under_over.o $(LDFLAGS)
	$(CC) -o lkf_t_double_free $(OBJTESTDEP) lkf_t_double_free.o $(LDFLAGS)
	$(CC) -o lkf_t_null $(OBJTESTDEP) lkf_t_null.o $(LDFLAGS)
	$(CC) -o lkf_t_reg $(OBJTESTDEP) lkf_t_reg.o $(LDFLAGS)
	$(CC) -o lkf_t_unalloc $(OBJTESTDEP) lkf_t_unalloc.o $(LDFLAGS)
	$(CC) -o lkf_t_approx_pass $(OBJTESTDEP) lkf_t_approx_pass.o $(LDFLAGS)
	$(CC) -o lkf_t_approx_fail $(OBJTESTDEP) lkf_t_approx_fail.o $(LDFLAGS)
	$(CC) -o lkf_t_warn $(OBJTESTDEP) lkf_t_warn.o $(LDFLAGS)
	$(CC) -o lkf_t_unknown $(OBJTESTDEP) lkf_t_unknown.o $(LDFLAGS)
	$(CC) -o lkf_t_warn_error $(OBJTESTDEP) lkf_t_warn_error.o $(LDFLAGS)
	$(CC) -o lkf_t_unknown_error $(OBJTESTDEP) lkf_t_unknown_error.o $(LDFLAGS)
	$(CC) -o lkr_t_orphan_free $(OBJTESTDEP) lkr_t_orphan_free.o $(LDFLAGS)
	$(CC) -o lkr_t_bad_free $(OBJTESTDEP) lkr_t_bad_free.o $(LDFLAGS)
	$(CC) -o lkr_t_match $(OBJTESTDEP) lkr_t_match.o $(LDFLAGS)
	$(CC) -o lkr_t_serious $(OBJTESTDEP) lkr_t_serious.o $(LDFLAGS)
	$(CC) -o lkr_t_double_free $(OBJTESTDEP) lkr_t_double_free.o $(LDFLAGS)
	$(CC) -o lkr_t_approx $(OBJTESTDEP) lkr_t_approx.o $(LDFLAGS)

tests: lkm_tests
	./reg_tests/test01.sh
	./reg_tests/test02.sh
	./reg_tests/test03.sh

clean:
	rm -f lkmalloc
	rm -f hash_table_test
	rm -f metadata_test
	rm -f LKmalloc.o
	rm -f hash_table.o
	rm -f metadata.o
	rm -f $(TESTBIN)
	rm -f $(OBJCLEAN)

lkm_t_double_malloc.o: $(LKM_T)lkm_t_double_malloc.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_double_malloc.c

lkm_t_reg.o: $(LKM_T)lkm_t_reg.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_reg.c

lkm_t_init.o: $(LKM_T)lkm_t_init.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_init.c

lkm_t_under.o: $(LKM_T)lkm_t_under.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_under.c

lkm_t_over.o: $(LKM_T)lkm_t_over.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_over.c

lkm_t_init_under.o: $(LKM_T)lkm_t_init_under.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_init_under.c

lkm_t_init_over.o: $(LKM_T)lkm_t_init_over.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_init_over.c

lkm_t_under_over.o: $(LKM_T)lkm_t_under_over.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_under_over.c

lkm_t_init_under_over.o: $(LKM_T)lkm_t_init_under_over.c
	$(CC) $(CFLAGS) -c $(LKM_T)lkm_t_init_under_over.c

lkf_t_double_free.o: $(LKF_T)lkf_t_double_free.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_double_free.c

lkf_t_null.o: $(LKF_T)lkf_t_null.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_null.c

lkf_t_reg.o: $(LKF_T)lkf_t_reg.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_reg.c

lkf_t_unalloc.o: $(LKF_T)lkf_t_unalloc.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_unalloc.c

lkf_t_approx_pass.o: $(LKF_T)lkf_t_approx_pass.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_approx_pass.c

lkf_t_approx_fail.o: $(LKF_T)lkf_t_approx_fail.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_approx_fail.c

lkf_t_warn.o: $(LKF_T)lkf_t_warn.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_warn.c

lkf_t_unknown.o: $(LKF_T)lkf_t_unknown.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_unknown.c

lkf_t_warn_error.o: $(LKF_T)lkf_t_warn_error.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_warn_error.c

lkf_t_unknown_error.o: $(LKF_T)lkf_t_unknown_error.c
	$(CC) $(CFLAGS) -c $(LKF_T)lkf_t_unknown_error.c

lkr_t_orphan_free.o: $(LKR_T)lkr_t_orphan_free.c
	$(CC) $(CFLAGS) -c $(LKR_T)lkr_t_orphan_free.c

lkr_t_bad_free.o: $(LKR_T)lkr_t_bad_free.c
	$(CC) $(CFLAGS) -c $(LKR_T)lkr_t_bad_free.c

lkr_t_match.o: $(LKR_T)lkr_t_match.c
	$(CC) $(CFLAGS) -c $(LKR_T)lkr_t_match.c

lkr_t_serious.o: $(LKR_T)lkr_t_serious.c
	$(CC) $(CFLAGS) -c $(LKR_T)lkr_t_serious.c

lkr_t_double_free.o: $(LKR_T)lkr_t_double_free.c
	$(CC) $(CFLAGS) -c $(LKR_T)lkr_t_double_free.c

lkr_t_approx.o: $(LKR_T)lkr_t_approx.c
	$(CC) $(CFLAGS) -c $(LKR_T)lkr_t_approx.c

hash_table_test: metadata.o hash_t.o test_progs/hash_table_test.c
	$(CC) $(CFLAGS) -o hash_table_test test_progs/hash_table_test.c metadata.o hash_table.o $(LDFLAGS)

metadata_test: metadata.o test_progs/metadata_test.c
	$(CC) $(CFLAGS) -o metadata_test test_progs/metadata_test.c metadata.o

macro_test: test_progs/macro1.c test_progs/macro2.c
	$(CC) $(CFLAGS) -o macro_test test_progs/macro1.c test_progs/macro2.c
