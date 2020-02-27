#!/bin/sh
# comment

# say what's running
echo testing that -e and -d flags cannot be together
./filesec -e -d tests/in.txt tests/out.txt
# in shell scripts, the last exit status of a prog is recorded in a special
# variable called $?
#exit $?
if test $? > 0 ; then
	echo this test is OK
else
	echo this test FAILED
fi

echo testing that -e flag works with -p passfile flag
./filesec -e -p tests/pass.txt tests/in.txt tests/out.txt

if test $? = 0 ; then
	echo this test is OK
else
	echo this test FAILED
fi

echo testing that -e or -d flag must be provided
./filesec tests/in.txt tests/out.txt

if test $? > 0 ; then
	echo this test is OK
else
	echo this test FAILED
fi