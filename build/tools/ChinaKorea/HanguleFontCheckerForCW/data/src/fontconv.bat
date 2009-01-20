
perl -w fonttbl.pl -o ..\..\src\fontgrpdata.c -i %NITROSYSTEM_ROOT%\tools\win\fontcvtr\xlor\KS_X1001_X1003.xlor

%NITROSYSTEM_ROOT%\tools\win\fontcvtr\fontcvtrc -i bmp -if HANGUL_15x15.bmp -ib 2 -io %NITROSYSTEM_ROOT%\tools\win\fontcvtr\xlor\KS_X1001_X1003.xlor -o nitro -of ..\HANGUL_15x15.NFTR -ol 0 -ow 15 -or 0 >15x15.log

%NITROSYSTEM_ROOT%\tools\win\fontcvtr\fontcvtrc -i bmp -if HANGUL_14x14.bmp -ib 2 -io %NITROSYSTEM_ROOT%\tools\win\fontcvtr\xlor\KS_X1001_X1003.xlor -o nitro -of ..\HANGUL_14x14.NFTR -ol 0 -ow 14 -or 0 >14x14.log

%NITROSYSTEM_ROOT%\tools\win\fontcvtr\fontcvtrc -i bmp -if HANGUL_11x12.bmp -ib 2 -io %NITROSYSTEM_ROOT%\tools\win\fontcvtr\xlor\KS_X1001_X1003.xlor -o nitro -of ..\HANGUL_11x12.NFTR -ol 0 -ow 11 -or 0 >11x12.log

%NITROSYSTEM_ROOT%\tools\win\fontcvtr\fontcvtrc -i bmp -if HANGUL_11x11.bmp -ib 2 -io %NITROSYSTEM_ROOT%\tools\win\fontcvtr\xlor\KS_X1001_X1003.xlor -o nitro -of ..\HANGUL_11x11.NFTR -ol 0 -ow 11 -or 0 >11x11.log

%NITROSYSTEM_ROOT%\tools\win\fontcvtr\fontcvtrc -i bmp -if HANGUL_10x10.bmp -ib 2 -io %NITROSYSTEM_ROOT%\tools\win\fontcvtr\xlor\KS_X1001_X1003.xlor -o nitro -of ..\HANGUL_10x10.NFTR -ol 0 -ow 10 -or 0 >10x10.log

%NITROSYSTEM_ROOT%\tools\win\fontcvtr\fontcvtrc -i bmp -if HANGUL_9x9.bmp   -ib 2 -io %NITROSYSTEM_ROOT%\tools\win\fontcvtr\xlor\KS_X1001_X1003.xlor -o nitro -of ..\HANGUL_9x9.NFTR -ol   0 -ow 9  -or 0 >9x9.log

