object Form1: TForm1
  Left = 301
  Top = 145
  Width = 555
  Height = 278
  Caption = 'find areas  not validated  in A9_overlay'
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
    Left = 16
    Top = 160
    Width = 75
    Height = 25
    Caption = #38283#22987
    TabOrder = 1
    OnClick = Button1Click
  end
  object DriveBox: TDriveComboBox
    Left = 384
    Top = 72
    Width = 145
    Height = 18
    TabOrder = 2
    OnChange = DriveBoxChange
  end
  object DirListBox: TDirectoryListBox
    Left = 384
    Top = 96
    Width = 145
    Height = 97
    ItemHeight = 16
    TabOrder = 3
    OnChange = DirListBoxChange
  end
  object StatusBar: TStatusBar
    Left = 0
    Top = 210
    Width = 547
    Height = 19
    Panels = <>
    SimplePanel = True
  end
  object StaticText3: TStaticText
    Left = 104
    Top = 168
    Width = 246
    Height = 16
    Caption = #65288#27880#24847#65289#25351#23450#12487#12451#12524#12463#12488#12522#12395'srl'#20197#22806#12399#32622#12363#12394#12356#12371#12392
    TabOrder = 5
  end
  object RG_select: TRadioGroup
    Left = 408
    Top = 8
    Width = 113
    Height = 57
    Caption = #12497#12473#25351#23450
    ItemIndex = 1
    Items.Strings = (
      'srl '#26684#32013#22580#25152
      #12525#12464#12288#20986#21147#20808)
    TabOrder = 6
  end
  object StatusBar1: TStatusBar
    Left = 0
    Top = 229
    Width = 547
    Height = 19
    Panels = <>
    SimplePanel = True
  end
end
