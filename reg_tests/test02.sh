#!/bin/bash

cd /home/jjiemjitpolc/hw2-jjiemjitpolc
RED='\033[0;31m'
ORANGE='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${ORANGE} Test 1 double free detection${NC}"
./lkf_t_double_free
if [ $? -eq 2 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 2 free NULL${NC}"
./lkf_t_null
if [ $? -eq 3 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 3 regular free${NC}"
./lkf_t_reg
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 4 free unalloc ptr${NC}"
./lkf_t_unalloc
if [ $? -eq 4 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 5 free APPROX flag set${NC}"
./lkf_t_approx_pass
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 6 free APPROX flag not set${NC}"
./lkf_t_approx_fail
if [ $? -eq 6 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 7 APPROX and WARN set${NC}"
./lkf_t_warn
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 8 UNKNOWN set${NC}"
./lkf_t_unknown
if [ $? -eq 4 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 9 WARN ERROR set${NC}"
./lkf_t_warn_error
if [ $? -eq 1 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 10 UNKNOWN ERROR set${NC}"
./lkf_t_unknown_error
if [ $? -eq 1 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi
