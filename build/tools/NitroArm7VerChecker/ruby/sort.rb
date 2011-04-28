# ---------------------------------------
#
# 出力切り替え(適時編集して下さい)
#
# ---------------------------------------

# true の場合は 通常版
# false の場合は 特殊版
normal_flg = false


# ---------------------------------------
#
# 処理
#
# ---------------------------------------

if normal_flg == true then
	input_file_name = "data_normal.txt"
	output_file_name = "SdkInfoData_normal.h"
else
	input_file_name = "data_special.txt"
	output_file_name = "SdkInfoData_special.h"
end

temp_output_file_name = "temp_output.txt"
backup_file_name = "data.txt.BAK"

# 複製作成
src = open(input_file_name)
dst = open(backup_file_name,"w")

cont = src.read
dst.write(cont)

src.close
dst.close

# 
# 被りチェック処理開始
# 
copy_in_file  = open(input_file_name,"r")
out_file = open(temp_output_file_name,"w+")

while (inLine = copy_in_file.gets)
	# 改行削除
	inLine.chomp!

	# 1行の文字列を「,」で分ける
	inCulm = inLine.split(/,/)

	# outファイルを先頭からサーチして重複チェック
	# ファイルポインタを最初に戻す
	out_file.rewind
	isEntry = false
	while (outLine = out_file.gets)
		outLine.chomp!
		outCulm = outLine.split(/,/)

		# 重複があったとき
		if (outCulm[0] == inCulm[0]) then
			isEntry = true
			break
		end
	end
	
	# ハッシュ値が同じエントリはここの処理で全て見つけ出す
	# out_fileに登録がなかった
	if isEntry == false then
		in_file = open(backup_file_name,"r")

		entry = ""

		while (line = in_file.gets)
			line.chomp!
			culm = line.split(/,/)

			if (culm[0] == inCulm[0]) then
				temp = entry.split(/\//)
				flg = false
				temp.each{|elem|
					if elem == culm[1] then
						flg = true
						break
					end
				}

				if flg == false then
					entry += culm[1] + "/"
				end
			end
		end

		# 冗長なSDKバージョン名は省く
		if normal_flg != true then
			write_entry = ""
			current_sdk_ver = ""
			a = entry.split(/\//)
			a.each{|elem|
				b = elem.split(/;/)
				if current_sdk_ver != b[0] then
					current_sdk_ver = b[0]
					write_entry += b[0]
				end
				write_entry += "[" + b[1] + "]" + "/"
			}
			entry = write_entry
		end

		out_file.seek(0, IO::SEEK_END)
		out_file.write(inCulm[0] + "," + entry + "\n")
		# puts ("  new entry")

		in_file.close
	end

end	

out_file.close
copy_in_file.close

File.delete(backup_file_name)


# 成形作業(ヘッダファイル作成)
src_file = open(temp_output_file_name,"r")
dst_file = open(output_file_name,"w+")

dst_file.write("#include <twl.h>\n\n")
dst_file.write("#define SDK_INFO_NUM \t\t2794\n")
dst_file.write("#define SDK_INFO_NAME_SIZE\t0x200\n\n")
dst_file.write("typedef struct Arm7Info\n")
dst_file.write("{\n")
dst_file.write("\tunsigned char hash[MATH_SHA1_DIGEST_SIZE];\n")
dst_file.write("\tchar name[SDK_INFO_NAME_SIZE];\n")
dst_file.write("}\n")
dst_file.write("Arm7Info;\n\n")
dst_file.write("static Arm7Info s_sdk_info[ SDK_INFO_NUM ] =\n")
dst_file.write("{" + "\n")

while (inLine = src_file.gets)
	inLine.chomp!
	culm = inLine.split(/,/)

	# culm[0] -> hash値
	# culm[1] -> SDKバージョン名

	elem = culm[0].unpack("a2a2a2a2a2a2a2a2a2a2a2a2a2a2a2a2a2a2a2a2")

	hash_data = ""

	elem.each{|data|
		hash_data += "0x" + data + ", "
	}
	
	line_data = "\t{{" + hash_data + "}, \"" + culm[1] + "\",},"

	dst_file.write(line_data + "\n")
end	

dst_file.write("};" + "\n")

src_file.close
dst_file.close

