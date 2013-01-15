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
    { Private 宣言 }
    function CheckSrlHeader:integer;
  public
    { Public 宣言 }
  end;

  TRom_Fat = record
    top:Longword;  // file region start
    bottom:Longword;  //         end;
  end;

  TRomh = record
    title: array[1..12] of Char;
    game_code: array[1..4] of Byte;
//0x10
    dumy:array[1..16]of Byte;//ツールで使用しない部分

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
    dumy2:array[1..8] of Byte;//ツールで使用しない部分

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
  SrlDir: String;//srlファイルの存在するディレクトリ名

implementation

function TForm1.CheckSrlHeader: integer;
var
  Rec: TSearchRec;
  s: String;
  i,n,Count: integer;
  fsrl: file;
  romh: TRomh;
  noc_size,noc_sectors,size,sectors,nums,max_sectors,total_sectors: integer;
  fat: TRom_Fat;

begin
  {$I-}
  Count :=0;
  //指定ディレクトリからファイルを取り出す
  // ** 内容で判別しないので違うファイル置かないよう注意 **
  if FindFirst(SrlDir + '*.*', faAnyFile, Rec) = 0 then
  try
    repeat
       //フォルダやカレントディレクトリや親ディレクトリは対象外で
       if not((Rec.Attr and faDirectory > 0)) and
             (Rec.Name <> '.') and (Rec.Name <> '..') then
         begin
           Inc(Count);
           ListBox_log.Items.Add('file: '+Rec.Name);
           AssignFile (fsrl,SrlDir+Rec.Name);
           FileMode := fmOpenRead;//リード専用指定
           Reset(fsrl,1);//ファイル開く
           if IOResult = 0 then begin
             BlockRead(fsrl,romh,SizeOf(TRomh),n);
             if SizeOf(TRomh) <> n then
               s := ' size too small'
             else begin
               SetLength(s,12);
               for i:=1 to 12 do begin
                if romh.title[i] = char(0) then break;
                s[i] := romh.title[i];
               end;
               if i<12 then Setlength(s,i);
               ListBox_log.Items.Add('title = '+s);

             //オーバレイチェック
              if  romh.main_ovt_size >0 then begin
//DHT_OVERLAY_MAX     (512*1024)
                total_sectors := 0;
                nums := romh.main_ovt_size div 32;// 32 = Nitro用(Twlは33)
                noc_size :=0;
                noc_sectors := 0;
                for  i := 0 to nums-1 do begin
                  seek( fsrl,romh.fat_offset+sizeof(TRom_Fat)*i);
                  BlockRead( fsrl,fat,sizeof(fat),n);
                  size := fat.bottom - fat.top;
                  if total_sectors >= 1024 then begin//MAX超えたファイルは検証外に加算
                    s := 'OL['+inttostr(i)+'] $'+ inttohex(fat.top,8)+' - $'+
                      inttohex(fat.bottom,8)+' :size '+ inttostr(size);
                    noc_size := noc_size + size;
                    noc_sectors := noc_sectors + ((size+511) and $fffffe00) div 512;
                    next;
                  end;
                 //max_sectors = (DHT_OVERLAY_MAX/512 - total_sectors) / (nums-i);
                  max_sectors := (1024 - total_sectors) div (nums-i);
                  if n <> sizeof(fat) then begin
                    s := s+'Cannot read fat id='+ inttostr(i);
                    break;
                  end;
                  sectors := ( ((fat.bottom+511) and $fffffe00)- fat.top) div 512;
                  if sectors > max_sectors then begin
                    size := size - max_sectors*512;//検証されない残りサイズ
                    s := 'OL['+inttostr(i)+'] $'+ inttohex(fat.top+max_sectors*512,8)+' - $'+
                      inttohex(fat.bottom,8)+' :size '+ inttostr(size);
                    ListBox_log.Items.Add(s);
                    noc_size := noc_size+size;
                    noc_sectors := noc_sectors + (max_sectors-sectors);
                  end;
                  total_sectors := total_sectors + sectors;
                end;
                if i = nums then begin
                  ListBox_log.Items.Add('total sectors = '+inttostr(total_sectors));
                  ListBox_log.Items.Add('no check sectors = '+inttostr(noc_sectors));
                  ListBox_log.Items.Add('no check size = '+inttostr(noc_size));
                  if noc_size > 0 then s:= 'NG'
                  else s:= 'OK';
                end;
              end else s := 'no OVL';
             end;
           end else s := ' Open Error';
           CloseFile(fsrl);
           ListBox_log.Items.Add(s);

         end;
    until (FindNext(Rec) <> 0);
  finally
    FindClose(Rec);
  end;
  {$I+}
  Result :=Count;
end;

{$R *.dfm}

procedure TForm1.Button1Click(Sender: TObject);
var
  FileCount :integer;
begin
  ListBox_log.Clear;
  //指定ディレクトリのsrlファイルをチェック
  FileCount := CheckSrlHeader;
  if FileCount>0 then begin
    ListBox_log.Items.Add(' ------------------------ ');
    ListBox_log.Items.Add('total '+inttostr(FileCount)+' files');
  end else
    ListBox_log.Items.Add(' file not ditect');

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
  //イベント利用してSrlDirNameとステータスバーの設定
  DirListBox.Drive := DriveBox.Drive;
end;

end.
