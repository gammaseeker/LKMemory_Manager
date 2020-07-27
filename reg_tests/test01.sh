#!/bin/bash

cd /home/jjiemjitpolc/hw2-jjiemjitpolc
RED='\033[0;31m'
ORANGE='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${ORANGE} Test 1 double malloc detection${NC}"
./lkm_t_double_malloc 
if [ $? -eq 1 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 2 LKM_REG test${NC}"
./lkm_t_reg_malloc
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 3 LKM_INIT test${NC}"
./lkm_t_init_malloc
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 4 LKM_UNDER test${NC}"
./lkm_t_under
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 5 LKM_OVER test${NC}"
./lkm_t_over
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 6 LKM_INIT || LKM_UNDER test${NC}"
./lkm_t_init_under
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 7 LKM_INIT || LKM_OVER test${NC}"
./lkm_t_init_over
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 8 LKM_UNDER || LKM_OVER test${NC}"
./lkm_t_under_over
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 9 LKM_INIT || LKM_UNDER || LKM_OVER test${NC}"
./lkm_t_init_under_over
if [ $? -eq 0 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi
