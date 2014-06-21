#!/bin/bash
make -f ece454.mk clean
RETVAL=$?
if [ $RETVAL -ne 0  ]; then
	echo "ERROR: make clean failed!"
	exit 1
else
	echo "make clean successful!"
fi

make -f ece454.mk all
RETVAL=$?
if [ $RETVAL -ne 0  ]; then
	echo "ERROR: make all failed!"
	exit 1
else
	echo "make all successful!"
fi
