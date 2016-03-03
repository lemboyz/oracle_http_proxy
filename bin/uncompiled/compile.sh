#!/bin/sh -x

for file in `ls *.lua`; do
	echo $file
	luac $file
	mv luac.out ../script/$file
done
