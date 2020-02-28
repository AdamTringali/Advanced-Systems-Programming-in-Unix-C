#!/bin/sh
# comment

# say what's running
echo ~~~~~~~~~~~~~~~~~test1.sh~~~~~~~~~~~~~~~~~
echo testing that -e and -d flags cannot be together
./filesec -e -d files/in.txt files/out.txt >/dev/null 2>&1
# in shell scripts, the last exit status of a prog is recorded in a special
# variable called $?
#exit $?
if test $? > 0 ; then
	echo this test is OK 
else
	echo this test FAILED
fi

echo testing that -e flag works with -p passfile flag
./filesec -e -p files/pass.txt files/in.txt files/out.txt >/dev/null 2>&1

if test $? = 0 ; then
	echo this test is OK $?
else
	echo this test FAILED $?
fi

echo testing that -e or -d flag must be provided
./filesec files/in.txt files/out.txt >/dev/null 2>&1

if test $? > 0 ; then
	echo this test is OK
else
	echo this test FAILED
fi