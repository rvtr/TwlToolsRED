#!/bin/sh

flag_tool="./MenuSkipFlagTool_Checker.exe"
output_dir="output/"

for file in $*
do
	$flag_tool $file ${output_dir}${file%.srl}_1_flag.srl 1
	$flag_tool $file ${output_dir}${file%.srl}_0_flag.srl 0
	$flag_tool ${output_dir}${file%.srl}_1_flag.srl ${output_dir}${file%.srl}_1_disable.srl 1 -d
	$flag_tool ${output_dir}${file%.srl}_1_flag.srl ${output_dir}${file%.srl}_0_disable.srl 0 -d
done
