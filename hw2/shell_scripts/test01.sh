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

echo
echo ~~~~~~~~~~~~~~~~~~~~~~~~test5.c~~~~~~~~~~~~~~~~
echo TESTS MALLOC LKM_OVER 0x2 and all lkreport flags
echo
#in shell scrips, the last exit stat of a pgrm is recorded.
#in variable $?
gcc -o test5 test_scripts/test5.c lkmalloc.c
./test5 #&>/dev/null 
if test $? = 0; then
    echo this test is OK
else
    echo this test FAILED
fi

echo
echo ~~~~~~~~~~~~~~~~~~~~~~~~test6.c~~~~~~~~~~~~~~~~
echo TESTS MALLOC WITH LKM_UNDER WITH LKR_MATCHES FLAG
echo
#in shell scrips, the last exit stat of a pgrm is recorded.
#in variable $?
gcc -o test6 test_scripts/test6.c lkmalloc.c
./test6 #&>/dev/null 
if test $? = 0; then
    echo this test is OK
else
    echo this test FAILED
fi