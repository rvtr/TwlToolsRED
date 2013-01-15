object Form1: TForm1
  Left = 310
  Top = 126
  Width = 574
  Height = 259
  Caption = 'Form1'
  Color = clBtnFace
  Font.Charset = SHIFTJIS_CHARSET
  Font.Color = clWindowText
  Font.Height = -12
  Font.Name = #65325#65331' '#65328#12468#12471#12483#12463
  Font.Style = []
  OldCreateOrder = False
  OnCreate = FormCreate
  PixelsPerInch = 96
  TextHeight = 12
  object ListBox_log: TListBox
    Left = 16
    Top = 16
    Width = 353
    Height = 129
    ItemHeight = 12
    TabOrder = 0
  end
  object Button1: TButton
    Left = 48
    Top = 168
    Width = 75
    Height = 25
    Caption = #38283#22987
    TabOrder = 1
    OnClick = Button1Click
  end
  object DriveBox: TDriveComboBox
    Left = 392
    Top = 40
    Width = 145
    Height = 18
    TabOrder = 2
    OnChange = DriveBoxChange
  end
  object DirListBox: TDirectoryListBox
    Left = 392
    Top = 64
    Width = 145
    Height = 97
    ItemHeight = 16
    TabOrder = 3
    OnChange = DirListBoxChange
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 210
    Width = 566
    Height = 19
    Panels = <>
    SimplePanel = True
  end
  object StaticText1: TStaticText
    Left = 184
    Top = 176
    Width = 283
    Height = 16
    Caption = #65288#27880#24847#65289#12525#12464#12399#23455#34892#12501#12449#12452#12523#12398#12354#12427#12487#12451#12524#12463#12488#12522#12395#20986#12375#12414#12377
    TabOrder = 5
  end
  object StaticText2: TStaticText
    Left = 416
    Top = 16
    Width = 95
    Height = 16
    BevelInner = bvLowered
    BevelOuter = bvRaised
    BorderStyle = sbsSingle
    Caption = 'srl'#12288#12398#22580#25152#12434#25351#23450
    TabOrder = 6
  end
  object StaticText3: TStaticText
    Left = 184
    Top = 192
    Width = 254
    Height = 16
    Caption = #65288#27880#24847#65298#65289#25351#23450#12487#12451#12524#12463#12488#12522#12395'srl'#20197#22806#12399#32622#12363#12394#12356#12371#12392
    TabOrder = 7
  end
end
