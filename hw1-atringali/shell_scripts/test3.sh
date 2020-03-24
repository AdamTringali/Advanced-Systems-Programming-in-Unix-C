
#!/bin/sh


echo ~~~~~~~~~~~~~~~~~test3.sh~~~~~~~~~~~~~~~~~
echo test to verify that the decryption key must be the same as the encryption key
./filesec -e -p files/pass.txt files/in.txt files/out.txt >/dev/null 2>&1
./filesec -d -p files/pass2.txt files/out.txt files/decrypted.txt >/dev/null 2>&1

if test $? > 0 ; then
	echo this test is OK
else
	echo this test FAILED
fi

echo test to verify that the decryption is the same as the encryption key
./filesec -e -p files/pass.txt files/in.txt files/out.txt >/dev/null 2>&1
./filesec -d -p files/pass.txt files/out.txt files/decrypted.txt >/dev/null 2>&1

if test $? = 0 ; then
	echo this test is OK
else
	echo this test FAILED
fi
