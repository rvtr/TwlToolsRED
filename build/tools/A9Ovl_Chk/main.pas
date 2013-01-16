unit main;

interface

uses
  Windows, Messages, SysUtils, Variants, Classes, Graphics, Controls, Forms,
  Dialogs, StdCtrls, FileCtrl, ComCtrls, ShellCtrls;

type
  TForm1 = class(TForm)
    ListBox_log: TListBox;
    Button1: TButton;
    DriveBox: TDriveComboBox;
    DirListBox: TDirectoryListBox;
    StatusBar: TStatusBar;
    StaticText1: TStaticText;
    StaticText2: TStaticText;
    StaticText3: TStaticText;
    procedure Button1Click(Sender: TObject);
    procedure DriveBoxChange(Sender: TObject);
    procedure DirListBoxChange(Sender: TObject);
    procedure FormCreate(Sender: TObject);
  private
    { Private �錾 }
    function CheckSrlHeader:integer;
  public
    { Public �錾 }
  end;

  TRom_Fat = record
    top:Longword;  // file region start
    bottom:Longword;  //         end;
  end;

  TRomh = record //ROM�w�b�_(nitro_romheader.h���)
    title: array[1..12] of Char;
    game_code: array[1..4] of Char;
//0x10
    maker_code: array[1..2] of Byte;
    machine_code: Byte;
    rom_type: Byte;
    rom_size: Byte;
    resv_A: array[1..8] of Byte;  //Reserve(All 0)
    region: Byte;
        // b0 = china,b1 = koria

    rom_version: Byte;
    comp_boot_area: Byte;//b7 =A9,b6=A7

    //
    // 0x020 for Static modules (Section:B)
    //ARM9
    A9_main_rom_offset: Longword;           // ROM offset
    A9_main_entry_address: Longword;        // Entry point
    A9_main_ram_address: Longword;          // RAM address
    A9_main_size: Longword;                 // Module size

    //ARM7
    A7_main_rom_offset: Longword;           // ROM offset
    A7_main_entry_address: Longword;        // Entry point
    A7_main_ram_address: Longword;          // RAM address
    A7_main_size: Longword;                 // Module size

//0x40
    dumy2:array[1..8] of Byte;//�c�[���Ŏg�p���Ȃ�����

    //
    // 0x048 for File Allocation Table[FAT] (Section:E)
    //
    fat_offset: Longword;        // ROM offset
    fat_size:Longword;           // Table size

    //
    // 0x050 for Overlay Tables[OVT] (Section:D)
    //
    //  ARM9
    main_ovt_offset: Longword;   // ROM offset
    main_ovt_size:Longword;             // Table size

  end;

var
  Form1: TForm1;
  SrlDir: String;//srl�t�@�C���̑��݂���f�B���N�g����

implementation

function TForm1.CheckSrlHeader: integer;
var
  Rec: TSearchRec;
  sf,s,ss: String;
  i,n,Count: integer;
  fsrl: file;
  flog:TextFile;
  romh: TRomh;
  noc_size,noc_sectors,size,sectors,nums,max_sectors,total_sectors: integer;
  fat: TRom_Fat;
  top,btm,all_sectors: Longword;

begin
  {$I-}
  //���O�t�@�C���쐬
  ListBox_log.Items.Add('Create Log file');
  s := ExtractFilePath(Application.Exename)+'log.txt';
  AssignFile (flog,s);
  Rewrite(flog);
  if IOResult <> 0 then begin
    Count := -1;
    ListBox_log.Items.Add('faild')
  end else begin
   Count := 0;
   //�w��f�B���N�g������t�@�C�������o��
   // ** ���e�Ŕ��ʂ��Ȃ��̂ňႤ�t�@�C���u���Ȃ��悤���� **
   if FindFirst(SrlDir + '*.*', faAnyFile, Rec) = 0 then
   try
    repeat
       //�t�H���_��J�����g�f�B���N�g����e�f�B���N�g���͑ΏۊO
       if not((Rec.Attr and faDirectory > 0)) and
             (Rec.Name <> '.') and (Rec.Name <> '..') and (Rec.name<>'log.txt')then
         begin
           Inc(Count);
           //ListBox_log.Items.Add('**********************************');
           Writeln(flog,'**********************************');
           sf := 'file: '+Rec.Name;
           //ListBox_log.Items.Add(sf);
           Writeln(flog,sf);
           AssignFile (fsrl,SrlDir+Rec.Name);
           FileMode := fmOpenRead;//���[�h��p�w��
           Reset(fsrl,1);//�t�@�C���J��
           if IOResult = 0 then begin
             BlockRead(fsrl,romh,SizeOf(TRomh),n);//�w�b�_���[�h
             if SizeOf(TRomh) <> n then
               s := ' header read error'
             else begin
               //SetLength(s,12);
               s := '[            ( )]';
               for i:=1 to 12 do begin
                   if romh.title[i] = char(0) then break;
                   s[i+1] := romh.title[i];
               end;
               s[15] := romh.game_code[4];//id�̂S������(�d���n)
               //ListBox_log.Items.Add(s); //title
               Writeln(flog,s);
               s := 'game_code =     ';
               for i:=1 to 4 do s[12+i] := romh.game_code[i];
               //ListBox_log.Items.Add(s); //game code
               //ListBox_log.Items.Add('version = '+inttostr(romh.rom_version));
               //ListBox_log.Items.Add(' ');
               Writeln(flog,s);
               Writeln(flog,'version = '+inttostr(romh.rom_version));
               Writeln(flog);//���s

             //�I�[�o���C�`�F�b�N
              if  romh.main_ovt_size >31 then begin
//DHT_OVERLAY_MAX     (512*1024)
                total_sectors := 0;
                all_sectors := 0;
                nums := romh.main_ovt_size div 32;//Ovl count
                noc_size :=0;//�͈͊O�g�[�^��size
                noc_sectors := 0;// ���Z�N�^
                for  i := 0 to nums-1 do begin
                  top := 0;btm := 0;
                  seek( fsrl,romh.fat_offset+sizeof(TRom_Fat)*i);
                  BlockRead( fsrl,fat,sizeof(fat),n);//top,bottom�擾
                  if n <> sizeof(fat) then begin
                    s := s+'Cannot read fat id='+ inttostr(i);
                    break;
                  end;
                  //max_sectors = (DHT_OVERLAY_MAX/512 - total_sectors) / (nums-i);
                  if (total_sectors < 1024)then max_sectors := (1024 - total_sectors) div (nums-i)
                  else max_sectors := 0;
                  btm := fat.bottom and $fffffe00; //512�P��
                  if (fat.bottom and $1ff)<>0 then inc(btm,512);//������Ɋۂ߂�
                  if max_sectors = 0 then begin//�c���؃T�C�Y�Ȃ���Ίۂ��ƌ��؊O
                    top := fat.top;
                    ss := '(A9 Overlay �̑S��)';
                  end else begin
                    top := fat.top and $fffffe00; //512�P��
                    size := fat.top and $01ff;
                    if size<>0 then begin
                      inc(top,512);////������Ɋۂ߂�
                      //�擪�̖����ؕ��� .. �s�v�H
                      s := 'offset'+inttostr(i)+' = 0x'+ inttohex(fat.top,8)
                         +' ; 0x'+ inttohex(fat.top,8)
                         +'-0x' + inttohex(top-1,8)+ss;
                      //ListBox_log.Items.Add(s);
                      Writeln(flog,s);
                      s := 'length'+ inttostr(i) + ' = 0x' + inttohex(size,4)
                        + ' ; '+ inttostr(size)+' bytes';
                      //ListBox_log.Items.Add(s);
                      Writeln(flog,s);
                      inc(noc_size,512);
                      inc(noc_sectors,1);
                      inc(all_sectors,1);
                    end;
                    size := btm - top;
                    sectors := size shr 9;//div 512
                    inc(all_sectors,sectors);
                    if sectors > max_sectors then begin //�ő劄���T�C�Y��
                      sectors := sectors - max_sectors;//�c��Z�N�^��
                      inc(top,max_sectors*512);//�c��擪
                      inc(total_sectors,max_sectors);
                      ss := '(A9 Overlay �̈ꕔ)';
                    end else begin //���߂Ȃ�
                      inc(total_sectors,sectors);
                      sectors := 0;
                    end;
                  end;
                  if sectors > 0 then begin//�c�肠��
                    size := sectors shl 9;
                    s := 'offset'+inttostr(i)+' = 0x'+ inttohex(top,8)
                         +' ; 0x'+ inttohex(top,8)
                         +'-0x' + inttohex(btm-1,8)+ss;
                    //ListBox_log.Items.Add(s);
                    Writeln(flog,s);
                    s := 'length'+ inttostr(i) + ' = 0x' + inttohex(size,4)
                        + ' ; '+ inttostr(size)+' bytes';
                    //ListBox_log.Items.Add(s);
                    Writeln(flog,s);
                    inc(noc_size,size);
                    inc(noc_sectors,sectors);
                  end;
                end;
                if i = nums then begin //���f�Ȃ�
                  //ListBox_log.Items.Add(' ');
                  //ListBox_log.Items.Add('total sectors = '+inttostr(all_sectors));
                  //ListBox_log.Items.Add('no check sectors = '+inttostr(noc_sectors));
                  //ListBox_log.Items.Add('no check size = '+inttostr(noc_size));
                  Writeln(flog); //���s
                  Writeln(flog,'total sectors = '+inttostr(all_sectors));
                  Writeln(flog,'check sectors = '+inttostr(total_sectors));
                  Writeln(flog,'no check sectors = '+inttostr(noc_sectors));
                  Writeln(flog,'no check size = '+inttostr(noc_size));
                  if noc_size > 0 then s:= 'NG'
                  else s:= 'OK';
                end;
              end else s := 'no OVL';
             end;
           end else s := ' Open Error';
           CloseFile(fsrl);
           Writeln(flog,s);//����
           if (s='OK') or (s='NG') then s := 'done';
           ss := sf + ' : ' + s;
           ListBox_log.Items.Add(ss);
          end;
    until (FindNext(Rec) <> 0);
   finally
    FindClose(Rec);
   end;
   CloseFile(flog);
  end;
  {$I+}
  Result :=Count;
end;

{$R *.dfm}

procedure TForm1.Button1Click(Sender: TObject);
var
  FileCount :integer;
  s :string;
begin
  ListBox_log.Clear;
  //�w��f�B���N�g����srl�t�@�C�����`�F�b�N
  FileCount := CheckSrlHeader;
  if FileCount>0 then begin
    ListBox_log.Items.Add(' ------------------------------ ');
    s := 'total '+inttostr(FileCount)+' files';
  end else if FileCount = 0 then s := ' file not ditect' ;
  ListBox_log.Items.Add(s);

end;



procedure TForm1.DriveBoxChange(Sender: TObject);
begin
  DirListBox.Drive := DriveBox.Drive;
end;

procedure TForm1.DirListBoxChange(Sender: TObject);
begin
  SrlDir := IncludeTrailingPathDelimiter(DirListBox.Directory);
  StatusBar.SimpleText := '[Target Dir] ' + DirListBox.Directory;
end;


procedure TForm1.FormCreate(Sender: TObject);
begin
  //�C�x���g���p����SrlDirName�ƃX�e�[�^�X�o�[�̐ݒ�
  DirListBox.Drive := DriveBox.Drive;
end;

end.
