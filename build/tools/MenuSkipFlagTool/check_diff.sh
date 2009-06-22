#!/bin/sh

flag_tool="./MenuSkipFlagTool.exe"
output_dir="output/"

for file in $*
do
	echo "== diff $file ${output_dir}${file%.srl}_0_flag.srl"
	diff $file ${output_dir}${file%.srl}_0_flag.srl
	echo "== diff $file ${output_dir}${file%.srl}_1_disable.srl"
	diff $file ${output_dir}${file%.srl}_1_disable.srl
	echo "== diff ${output_dir}${file%.srl}_1_flag.srl ${output_dir}${file%.srl}_0_disable.srl"
	diff ${output_dir}${file%.srl}_1_flag.srl ${output_dir}${file%.srl}_0_disable.srl
done
