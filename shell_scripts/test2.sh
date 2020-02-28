#!/bin/sh
# comment

# say what's running
echo ~~~~~~~~~~~~~~~~~test2.sh~~~~~~~~~~~~~~~~~
./filesec -e -d files/in.txt files/out.txt >/dev/null 2>&1
# in shell scripts, the last exit status of a prog is recorded in a special
# variable called $?
#exit $?
if test $? > 0 ; then
	echo this test is OK 
else
	echo this test FAILED
fi
#echo unwritten test

