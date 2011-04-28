# ---------------------------------------
#
# �o�͐؂�ւ�(�K���ҏW���ĉ�����)
#
# ---------------------------------------

# true �̏ꍇ�� �ʏ��
# false �̏ꍇ�� �����
normal_flg = false


# ---------------------------------------
#
# ����
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

# �����쐬
src = open(input_file_name)
dst = open(backup_file_name,"w")

cont = src.read
dst.write(cont)

src.close
dst.close

# 
# ���`�F�b�N�����J�n
# 
copy_in_file  = open(input_file_name,"r")
out_file = open(temp_output_file_name,"w+")

while (inLine = copy_in_file.gets)
	# ���s�폜
	inLine.chomp!

	# 1�s�̕�������u,�v�ŕ�����
	inCulm = inLine.split(/,/)

	# out�t�@�C����擪����T�[�`���ďd���`�F�b�N
	# �t�@�C���|�C���^���ŏ��ɖ߂�
	out_file.rewind
	isEntry = false
	while (outLine = out_file.gets)
		outLine.chomp!
		outCulm = outLine.split(/,/)

		# �d�����������Ƃ�
		if (outCulm[0] == inCulm[0]) then
			isEntry = true
			break
		end
	end
	
	# �n�b�V���l�������G���g���͂����̏����őS�Č����o��
	# out_file�ɓo�^���Ȃ�����
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

		# �璷��SDK�o�[�W�������͏Ȃ�
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


# ���`���(�w�b�_�t�@�C���쐬)
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

	# culm[0] -> hash�l
	# culm[1] -> SDK�o�[�W������

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

