RM = rm.exe -f

TADFILES=$(wildcard *.tad)

TAD_TABLE_FILE = taddev_table.txt
TAD_MAKE_INC_FILE = ../../copy_dst/MakefileTadDev.inc

$(TAD_TABLE_FILE):  $(TADFILES)
		../../bin/make_tad_table.exe -dir . -o $@ -var MAKEROM_TADDEV_ROMFILES -fdir taddevs -mk $(TAD_MAKE_INC_FILE)



.PHONY: clean clobber
clean clobber:
	$(RM) $(TAD_TABLE_FILE) $(TAD_MAKE_INC_FILE)
