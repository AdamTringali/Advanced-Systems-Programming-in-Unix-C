#!/bin/sh
# comment


# say whats running
echo ~~~~~~~~~~~~~~~~~~~~~~~~test01.sh~~~~~~~~~~~~~~~~
echo ~~~~~~~~~~~~~~~~~~~~~~~~test01.c~~~~~~~~~~~~~~~~
echo testing lkmalloc and lkfree with printing LKR_MATCH reports
echo
#in shell scrips, the last exit stat of a pgrm is recorded.
#in variable $?
gcc -o test1 test_scripts/test01.c lkmalloc.c
./test1 #&>/dev/null 
if test $? = 0; then
    echo this test is OK
else
    echo this test FAILED
fi

echo
echo ~~~~~~~~~~~~~~~~~~~~~~~~test02.c~~~~~~~~~~~~~~~~
echo testing lkfree on an invalid address LKR_ORPHAN_FREE reports
echo
#in shell scrips, the last exit stat of a pgrm is recorded.
#in variable $?
gcc -o test2 test_scripts/test02.c lkmalloc.c
./test2 #&>/dev/null 
if test $? = 0; then
    echo this test is OK
else
    echo this test FAILED
fi

echo
echo ~~~~~~~~~~~~~~~~~~~~~~~~test03.c~~~~~~~~~~~~~~~~
echo testing lkfree on an invalid address with LKF_ERROR flag LKR_NONE
echo
#in shell scrips, the last exit stat of a pgrm is recorded.
#in variable $?
gcc -o test3 test_scripts/test03.c lkmalloc.c 
./test3 #&>/dev/null 
if test $? = 0; then
    echo this test FAILED
else
    echo this test is OK
fi


echo
echo ~~~~~~~~~~~~~~~~~~~~~~~~test04.c~~~~~~~~~~~~~~~~
echo testing lkmalloc with LKM_INIT flag LKR_NONE
echo
#in shell scrips, the last exit stat of a pgrm is recorded.
#in variable $?
gcc -o test4 test_scripts/test04.c lkmalloc.c
./test4 #&>/dev/null 
if test $? = 0; then
    echo this test is OK
else
    echo this test FAILED
fi