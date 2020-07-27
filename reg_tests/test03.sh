#!/bin/bash

cd /home/jjiemjitpolc/hw2-jjiemjitpolc
RED='\033[0;31m'
ORANGE='\033[0;33m'
GREEN='\033[0;32m'
NC='\033[0m'

echo -e "${ORANGE} Test 1 ORPHAN_FREE set${NC}"
./lkr_t_orphan_free
if [ $? -eq 2 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 2 BAD_FREE set${NC}"
./lkr_t_bad_free
if [ $? -eq 2 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 3 MATCH set${NC}"
./lkr_t_match
if [ $? -eq 4 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 4 SERIOUS set${NC}"
./lkr_t_serious
if [ $? -eq 2 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 5 DOUBLE FREE set${NC}"
./lkr_t_double_free
if [ $? -eq 1 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi

echo -e "${ORANGE} Test 6 APPROX set${NC}"
./lkr_t_approx
if [ $? -eq 2 ]; then
    echo -e "${GREEN} PASSED!${NC}"
else
    echo -e "${RED} FAILED! ${NC}"
    exit 1
fi
