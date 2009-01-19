#pragma once

#include <apptype.h>
#include "common.h"
#include "srl.h"
#include "deliverable.h"
#include "crc_whole.h"
#include "utility.h"
#include "lang.h"
#include "FormError.h"

namespace MasterEditorTWL {

	using namespace System;
	using namespace System::ComponentModel;
	using namespace System::Collections;
	using namespace System::Windows::Forms;
	using namespace System::Data;
	using namespace System::Drawing;

	/// <summary>
	/// Form1 の概要
	///
	/// 警告: このクラスの名前を変更する場合、このクラスが依存するすべての .resx ファイルに関連付けられた
	///          マネージ リソース コンパイラ ツールに対して 'Resource File Name' プロパティを
	///          変更する必要があります。この変更を行わないと、
	///          デザイナと、このフォームに関連付けられたローカライズ済みリソースとが、
	///          正しく相互に利用できなくなります。
	/// </summary>
	public ref class Form1 : public System::Windows::Forms::Form
	{
	/////////////////////////////////////////////
	// 独自追加フィールド
	/////////////////////////////////////////////
	private:
		// SRL情報(ROMヘッダを含む)
		RCSrl ^hSrl;

		// マスタ書類
		RCDeliverable ^hDeliv;

		// TADを読み込んだかどうか(SRLを読み込んだ場合はfalse)
		System::Boolean IsLoadTad;

		// 書類出力モード(ノーマルXML or XML Spread Sheet)
		System::Boolean IsSpreadSheet;

		// リードオンリーモード
		System::Boolean IsReadOnly;

		// 非表示項目の有無で大きさを変えるグループボックスのもともとの大きさ
		System::Drawing::Size SizeGboxExFlags;
		System::Boolean IsVisibleGboxExFlags;

		// 入力エラー情報
		System::Collections::Generic::List<RCMrcError ^> ^hErrorList;
		System::Collections::Generic::List<RCMrcError ^> ^hWarnList;

		// SRLに登録されないROM仕様を読み込み時の状態に戻せる仕組み
		System::Boolean IsCheckedUGC;			// 読み込み時にチェックされていたか
		System::Boolean IsCheckedPhotoEx;

	/////////////////////////////////////////////
	// VC自動追加フィールド
	/////////////////////////////////////////////
	private: System::Windows::Forms::GroupBox^  gboxCRC;
	private: System::Windows::Forms::TextBox^  tboxWholeCRC;

	private: System::Windows::Forms::Label^  labTitleName;
	private: System::Windows::Forms::Label^  labGameCode;
	private: System::Windows::Forms::TextBox^  tboxGameCode;
	private: System::Windows::Forms::Label^  labMakerCode;
	private: System::Windows::Forms::TextBox^  tboxMakerCode;
	private: System::Windows::Forms::Label^  labRomType;
	private: System::Windows::Forms::ComboBox^  combBackup;
	private: System::Windows::Forms::Label^  labRomSize;
	private: System::Windows::Forms::TextBox^  tboxRomLatency;
	private: System::Windows::Forms::Label^  labBackup;
	private: System::Windows::Forms::Label^  labPlatform;
	private: System::Windows::Forms::Label^  labRomCRC;
	private: System::Windows::Forms::Label^  labHeaderCRC;
	private: System::Windows::Forms::TextBox^  tboxHeaderCRC;
	private: System::Windows::Forms::TextBox^  tboxBackupOther;
	private: System::Windows::Forms::Label^  labCaption;
	private: System::Windows::Forms::TextBox^  tboxCaption;



	private: System::Windows::Forms::TextBox^  tboxRomSize;





























































	private: System::Windows::Forms::Label^  labOFLC;
	private: System::Windows::Forms::Label^  labPEGI_BBFC;
	private: System::Windows::Forms::Label^  labPEGI_PRT;
	private: System::Windows::Forms::Label^  labPEGI;
	private: System::Windows::Forms::Label^  labUSK;
	private: System::Windows::Forms::Label^  labESRB;
	private: System::Windows::Forms::Label^  labCERO;







	private: System::Windows::Forms::ComboBox^  combOFLC;
	private: System::Windows::Forms::ComboBox^  combPEGI_BBFC;
	private: System::Windows::Forms::ComboBox^  combPEGI_PRT;
	private: System::Windows::Forms::ComboBox^  combPEGI;
	private: System::Windows::Forms::ComboBox^  combUSK;
	private: System::Windows::Forms::ComboBox^  combESRB;
	private: System::Windows::Forms::ComboBox^  combCERO;








	private: System::Windows::Forms::Label^  labParentalRating;




































	private: System::Windows::Forms::CheckBox^  cboxIsEULA;



























	private: System::Windows::Forms::CheckBox^  cboxIsInputPerson2;
	private: System::Windows::Forms::GroupBox^  gboxPerson2;
	private: System::Windows::Forms::Label^  labNTSC2Sur;
	private: System::Windows::Forms::TextBox^  tboxNTSC2;
	private: System::Windows::Forms::Label^  labNTSC2Pre;
	private: System::Windows::Forms::TextBox^  tboxFax2;
	private: System::Windows::Forms::TextBox^  tboxMail2;
	private: System::Windows::Forms::TextBox^  tboxTel2;
	private: System::Windows::Forms::TextBox^  tboxFurigana2;
	private: System::Windows::Forms::TextBox^  tboxPerson2;
	private: System::Windows::Forms::TextBox^  tboxDepart2;
	private: System::Windows::Forms::Label^  labDepart2;
	private: System::Windows::Forms::TextBox^  tboxCompany2;
	private: System::Windows::Forms::Label^  labMail2;
	private: System::Windows::Forms::Label^  labFax2;
	private: System::Windows::Forms::Label^  labTel2;
	private: System::Windows::Forms::Label^  labFurigana2;
	private: System::Windows::Forms::Label^  labPerson2;
	private: System::Windows::Forms::Label^  labCompany2;
	private: System::Windows::Forms::GroupBox^  gboxPerson1;
	private: System::Windows::Forms::Label^  labNTSC1Sur;
	private: System::Windows::Forms::Label^  labNTSC1Pre;
	private: System::Windows::Forms::TextBox^  tboxNTSC1;
	private: System::Windows::Forms::TextBox^  tboxFax1;
	private: System::Windows::Forms::TextBox^  tboxMail1;
	private: System::Windows::Forms::TextBox^  tboxTel1;
	private: System::Windows::Forms::TextBox^  tboxFurigana1;
	private: System::Windows::Forms::TextBox^  tboxPerson1;
	private: System::Windows::Forms::TextBox^  tboxDepart1;
	private: System::Windows::Forms::Label^  labDepart1;
	private: System::Windows::Forms::TextBox^  tboxCompany1;
	private: System::Windows::Forms::Label^  labMail1;
	private: System::Windows::Forms::Label^  labFax1;
	private: System::Windows::Forms::Label^  labTel1;
	private: System::Windows::Forms::Label^  labFurigana1;
	private: System::Windows::Forms::Label^  labPerson1;
	private: System::Windows::Forms::Label^  labCompany1;

	private: System::Windows::Forms::TextBox^  tboxRemasterVer;
	private: System::Windows::Forms::TextBox^  tboxProductCode2;

	private: System::Windows::Forms::TextBox^  tboxProductCode1;
	private: System::Windows::Forms::TextBox^  tboxProductName;









	private: System::Windows::Forms::Label^  labProductCode2;
	private: System::Windows::Forms::Label^  labProductCode1;
	private: System::Windows::Forms::DateTimePicker^  dateSubmit;
	private: System::Windows::Forms::DateTimePicker^  dateRelease;

	private: System::Windows::Forms::CheckBox^  cboxRemasterVerE;
	private: System::Windows::Forms::Label^  labCapSubmitVer;
	private: System::Windows::Forms::NumericUpDown^  numSubmitVersion;
	private: System::Windows::Forms::Label^  labSubmitVer;
	private: System::Windows::Forms::Label^  labRemasterVer;


	private: System::Windows::Forms::GroupBox^  gboxUsage;
	private: System::Windows::Forms::TextBox^  tboxUsageOther;
	private: System::Windows::Forms::RadioButton^  rUsageOther;
	private: System::Windows::Forms::RadioButton^  rUsageDst;
	private: System::Windows::Forms::RadioButton^  rUsageSample;
	private: System::Windows::Forms::RadioButton^  rUsageSale;

	private: System::Windows::Forms::GroupBox^  gboxSubmitWay;
	private: System::Windows::Forms::RadioButton^  rSubmitHand;
	private: System::Windows::Forms::RadioButton^  rSubmitPost;

	private: System::Windows::Forms::Label^  labSubmiteDate;
	private: System::Windows::Forms::Label^  labReleaseDate;
	private: System::Windows::Forms::Label^  labProductCode;
	private: System::Windows::Forms::Label^  labProductName;
private: System::Windows::Forms::ComboBox^  combRegion;



















private: System::Windows::Forms::Label^  labRegion;









private: System::Windows::Forms::Label^  labProductCode2Foreign;
private: System::Windows::Forms::CheckBox^  cboxReleaseForeign;
private: System::Windows::Forms::Label^  labProductNameForeign;
private: System::Windows::Forms::TextBox^  tboxProductNameForeign;
private: System::Windows::Forms::Label^  labProductCode1Foreign;
private: System::Windows::Forms::TextBox^  tboxProductCode1Foreign;
private: System::Windows::Forms::Label^  labProductCodeForeign;
private: System::Windows::Forms::TextBox^  tboxProductCode2Foreign1;














private: System::Windows::Forms::GroupBox^  gboxShared2Size;
private: System::Windows::Forms::Label^  labShared2Size5;
private: System::Windows::Forms::Label^  labShared2Size4;
private: System::Windows::Forms::Label^  labShared2Size3;
private: System::Windows::Forms::Label^  labShared2Size2;
private: System::Windows::Forms::Label^  labShared2Size1;
private: System::Windows::Forms::Label^  labShared2Size0;






private: System::Windows::Forms::TextBox^  tboxShared2Size5;
private: System::Windows::Forms::TextBox^  tboxShared2Size4;
private: System::Windows::Forms::TextBox^  tboxShared2Size3;
private: System::Windows::Forms::TextBox^  tboxShared2Size2;
private: System::Windows::Forms::TextBox^  tboxShared2Size1;
private: System::Windows::Forms::TextBox^  tboxShared2Size0;
private: System::Windows::Forms::CheckBox^  cboxIsShared2;

private: System::Windows::Forms::Label^  labLib;
private: System::Windows::Forms::TextBox^  tboxSDK;
private: System::Windows::Forms::Label^  labSDK;

private: System::Windows::Forms::GroupBox^  gboxTWLExInfo;



private: System::Windows::Forms::Label^  labByte1;
private: System::Windows::Forms::Label^  labHex4;
private: System::Windows::Forms::Label^  labHex3;
private: System::Windows::Forms::TextBox^  tboxIsCodec;
private: System::Windows::Forms::Label^  labIsCodec;
private: System::Windows::Forms::Label^  labNormalRomOffset;
private: System::Windows::Forms::TextBox^  tboxNormalRomOffset;
private: System::Windows::Forms::CheckBox^  cboxIsSubBanner;
private: System::Windows::Forms::Label^  labKeyTableRomOffset;

private: System::Windows::Forms::TextBox^  tboxPrivateSize;
private: System::Windows::Forms::Label^  labPrivateSize;
private: System::Windows::Forms::TextBox^  tboxKeyTableRomOffset;
private: System::Windows::Forms::CheckBox^  cboxIsNormalJump;
private: System::Windows::Forms::CheckBox^  cboxIsTmpJump;
private: System::Windows::Forms::Label^  labPublicSize;
private: System::Windows::Forms::TextBox^  tboxPublicSize;


private: System::Windows::Forms::GroupBox^  gboxAccess;
private: System::Windows::Forms::Label^  labAccessOther;
private: System::Windows::Forms::TextBox^  tboxAccessOther;
private: System::Windows::Forms::TextBox^  tboxIsGameCardOn;
private: System::Windows::Forms::Label^  labIsGameCardOn;
private: System::Windows::Forms::CheckBox^  cboxIsNAND;
private: System::Windows::Forms::CheckBox^  cboxIsSD;



private: System::Windows::Forms::GroupBox^  gboxTitleID;
private: System::Windows::Forms::Label^  labHex2;

private: System::Windows::Forms::TextBox^  tboxTitleIDLo;
private: System::Windows::Forms::Label^  labTitleIDLo;
private: System::Windows::Forms::Label^  labTitleIDHi;
private: System::Windows::Forms::TextBox^  tboxTitleIDHi;
private: System::Windows::Forms::TextBox^  tboxAppTypeOther;

private: System::Windows::Forms::Label^  labAppTypeOther;

private: System::Windows::Forms::Label^  labCaptionEx;
private: System::Windows::Forms::TextBox^  tboxCaptionEx;



private: System::Windows::Forms::TextBox^  tboxProductCode2Foreign3;

private: System::Windows::Forms::TextBox^  tboxProductCode2Foreign2;
private: System::Windows::Forms::Label^  labMultiForeign1;



private: System::Windows::Forms::GroupBox^  gboxProd;



private: System::Windows::Forms::MenuStrip^  menuStripAbove;

private: System::Windows::Forms::ToolStripMenuItem^  stripFile;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemOpenRom;

private: System::Windows::Forms::ToolStripMenuItem^  stripItemSaveTemp;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemLoadTemp;
private: System::Windows::Forms::ToolStripMenuItem^  stripMaster;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemMasterRom;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemSheet;
private: System::Windows::Forms::ToolStripMenuItem^  stripLang;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemEnglish;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemJapanese;
private: System::Windows::Forms::TabControl^  tabMain;
private: System::Windows::Forms::TabPage^  tabRomInfo;


private: System::Windows::Forms::TabPage^  tabTWLInfo;
private: System::Windows::Forms::TabPage^  tabRomEditInfo;
private: System::Windows::Forms::TabPage^  tabSubmitInfo;







private: System::Windows::Forms::TabPage^  tabCompanyInfo;

private: System::Windows::Forms::GroupBox^  gboxForeign;
private: System::Windows::Forms::GroupBox^  gboxExFlags;





private: System::Windows::Forms::GroupBox^  gboxParental;
private: System::Windows::Forms::TextBox^  tboxGuideRomInfo;
private: System::Windows::Forms::TextBox^  tboxGuideTWLInfo;
private: System::Windows::Forms::TextBox^  tboxGuideRomEditInfo;
private: System::Windows::Forms::TextBox^  tboxGuideSubmitInfo;
private: System::Windows::Forms::TextBox^  tboxGuideCompanyInfo;










private: System::Windows::Forms::Label^  labFile;
public: System::Windows::Forms::DataGridView^  gridError;
private: System::Windows::Forms::TabPage^  tabErrorInfo;
public: 
private: 

public: 




public: System::Windows::Forms::DataGridView^  gridWarn;
private: System::Windows::Forms::TextBox^  tboxGuideErrorInfo;
public: 
private: 

public: 




private: System::Windows::Forms::GroupBox^  gboxErrorTiming;
private: System::Windows::Forms::Label^  labWarn;
private: System::Windows::Forms::Label^  labError;
private: System::Windows::Forms::RadioButton^  rErrorCurrent;

private: System::Windows::Forms::RadioButton^  rErrorReading;








private: System::Windows::Forms::Label^  labMultiForeign2;
private: System::Windows::Forms::DataGridView^  gridLibrary;










private: System::Windows::Forms::Label^  labAssemblyVersion;
private: System::Windows::Forms::Label^  labArbit4;
private: System::Windows::Forms::Label^  labArbit3;
private: System::Windows::Forms::Label^  labArbit2;
private: System::Windows::Forms::Label^  labArbit1;
private: System::Windows::Forms::Button^  butSetBack;
private: System::Windows::Forms::Label^  labAppType;

private: System::Windows::Forms::Label^  labMedia;
private: System::Windows::Forms::TextBox^  tboxAppType;

private: System::Windows::Forms::TextBox^  tboxMedia;
private: System::Windows::Forms::Label^  labProductNameLimit;
private: System::Windows::Forms::Label^  labProductNameLimitForeign;























private: System::Windows::Forms::DataGridViewTextBoxColumn^  colLibPublisher;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colLibName;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemMiddlewareXml;
private: System::Windows::Forms::ToolStripMenuItem^  stripItemMiddlewareHtml;
private: System::Windows::Forms::ToolStripSeparator^  stripItemSepFile1;
private: System::Windows::Forms::ToolStripSeparator^  stripItemSepMaster1;
private: System::Windows::Forms::GroupBox^  gboxOtherSpec;
private: System::Windows::Forms::CheckBox^  cboxIsUGC;
private: System::Windows::Forms::CheckBox^  cboxIsPhotoEx;




private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorName;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorBegin;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorEnd;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colErrorCause;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnName;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnBegin;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnEnd;
private: System::Windows::Forms::DataGridViewTextBoxColumn^  colWarnCause;
private: System::Windows::Forms::GroupBox^  gboxLaunch;
private: System::Windows::Forms::Label^  labConnectIcon;
private: System::Windows::Forms::TextBox^  tboxConnectIcon;
private: System::Windows::Forms::Label^  labIsGameCardOn2;
private: System::Windows::Forms::CheckBox^  cboxIsUnnecessaryRating;
private: System::Windows::Forms::TabPage^  tabNandSizeInfo;

private: System::Windows::Forms::Label^  labSumSize2;



private: System::Windows::Forms::Label^  labSrlSize;
private: System::Windows::Forms::Label^  labSubBannerSize;
private: System::Windows::Forms::Label^  labTmdSize;
private: System::Windows::Forms::Label^  labNandSize;


private: System::Windows::Forms::Label^  labNandSizeFS;
private: System::Windows::Forms::Label^  labSumSize;
private: System::Windows::Forms::Label^  labShopBlock;
private: System::Windows::Forms::Label^  labShopBlock2;
private: System::Windows::Forms::GroupBox^  gboxNandSize;
private: System::Windows::Forms::TextBox^  tboxShopBlock;

private: System::Windows::Forms::TextBox^  tboxSumSize;

private: System::Windows::Forms::TextBox^  tboxTmdSizeFS;

private: System::Windows::Forms::TextBox^  tboxSubBannerSizeFS;

private: System::Windows::Forms::TextBox^  tboxPrivateSizeFS;

private: System::Windows::Forms::TextBox^  tboxPublicSizeFS;

private: System::Windows::Forms::TextBox^  tboxSrlSizeFS;

private: System::Windows::Forms::TextBox^  tboxTmdSize;

private: System::Windows::Forms::TextBox^  tboxSubBannerSize;



private: System::Windows::Forms::Label^  labShopBlockUnit;
private: System::Windows::Forms::Label^  labArrow5;

private: System::Windows::Forms::Label^  labArrow4;

private: System::Windows::Forms::Label^  labArrow3;



private: System::Windows::Forms::Label^  labArrow2;

private: System::Windows::Forms::Label^  labArrow1;
private: System::Windows::Forms::TextBox^  tboxGuideNandSizeInfo;


private: System::Windows::Forms::TextBox^  tboxSrlSize;
























































































	private: System::Windows::Forms::TextBox^  tboxPlatform;

	public:
		Form1(void)
		{
			InitializeComponent();
			//
			//TODO: ここにコンストラクタ コードを追加します
			//
			this->construct();
		}

	protected:
		/// <summary>
		/// 使用中のリソースをすべてクリーンアップします。
		/// </summary>
		~Form1()
		{
			this->destruct();

			if (components)
			{
				delete components;
			}
		}
	private: System::Windows::Forms::TextBox^  tboxFile;
	protected: 


	private: System::Windows::Forms::GroupBox^  gboxSrl;
	private: System::Windows::Forms::TextBox^  tboxTitleName;



	private:
		/// <summary>
		/// 必要なデザイナ変数です。
		/// </summary>
		System::ComponentModel::Container ^components;

#pragma region Windows Form Designer generated code
		/// <summary>
		/// デザイナ サポートに必要なメソッドです。このメソッドの内容を
		/// コード エディタで変更しないでください。
		/// </summary>
		void InitializeComponent(void)
		{
			System::ComponentModel::ComponentResourceManager^  resources = (gcnew System::ComponentModel::ComponentResourceManager(Form1::typeid));
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle1 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle2 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle3 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			System::Windows::Forms::DataGridViewCellStyle^  dataGridViewCellStyle4 = (gcnew System::Windows::Forms::DataGridViewCellStyle());
			this->tboxFile = (gcnew System::Windows::Forms::TextBox());
			this->gboxSrl = (gcnew System::Windows::Forms::GroupBox());
			this->tboxRemasterVer = (gcnew System::Windows::Forms::TextBox());
			this->tboxRomSize = (gcnew System::Windows::Forms::TextBox());
			this->tboxPlatform = (gcnew System::Windows::Forms::TextBox());
			this->labPlatform = (gcnew System::Windows::Forms::Label());
			this->tboxRomLatency = (gcnew System::Windows::Forms::TextBox());
			this->labRomSize = (gcnew System::Windows::Forms::Label());
			this->labRomType = (gcnew System::Windows::Forms::Label());
			this->tboxMakerCode = (gcnew System::Windows::Forms::TextBox());
			this->cboxRemasterVerE = (gcnew System::Windows::Forms::CheckBox());
			this->labMakerCode = (gcnew System::Windows::Forms::Label());
			this->labGameCode = (gcnew System::Windows::Forms::Label());
			this->tboxGameCode = (gcnew System::Windows::Forms::TextBox());
			this->labTitleName = (gcnew System::Windows::Forms::Label());
			this->labRemasterVer = (gcnew System::Windows::Forms::Label());
			this->tboxTitleName = (gcnew System::Windows::Forms::TextBox());
			this->labBackup = (gcnew System::Windows::Forms::Label());
			this->combBackup = (gcnew System::Windows::Forms::ComboBox());
			this->tboxBackupOther = (gcnew System::Windows::Forms::TextBox());
			this->gboxCRC = (gcnew System::Windows::Forms::GroupBox());
			this->labRomCRC = (gcnew System::Windows::Forms::Label());
			this->labHeaderCRC = (gcnew System::Windows::Forms::Label());
			this->tboxHeaderCRC = (gcnew System::Windows::Forms::TextBox());
			this->tboxWholeCRC = (gcnew System::Windows::Forms::TextBox());
			this->labCaption = (gcnew System::Windows::Forms::Label());
			this->tboxCaption = (gcnew System::Windows::Forms::TextBox());
			this->labOFLC = (gcnew System::Windows::Forms::Label());
			this->labPEGI_BBFC = (gcnew System::Windows::Forms::Label());
			this->labPEGI_PRT = (gcnew System::Windows::Forms::Label());
			this->labPEGI = (gcnew System::Windows::Forms::Label());
			this->labUSK = (gcnew System::Windows::Forms::Label());
			this->labESRB = (gcnew System::Windows::Forms::Label());
			this->labCERO = (gcnew System::Windows::Forms::Label());
			this->combOFLC = (gcnew System::Windows::Forms::ComboBox());
			this->combPEGI_BBFC = (gcnew System::Windows::Forms::ComboBox());
			this->combPEGI_PRT = (gcnew System::Windows::Forms::ComboBox());
			this->combPEGI = (gcnew System::Windows::Forms::ComboBox());
			this->combUSK = (gcnew System::Windows::Forms::ComboBox());
			this->combESRB = (gcnew System::Windows::Forms::ComboBox());
			this->combCERO = (gcnew System::Windows::Forms::ComboBox());
			this->labParentalRating = (gcnew System::Windows::Forms::Label());
			this->labRegion = (gcnew System::Windows::Forms::Label());
			this->cboxIsEULA = (gcnew System::Windows::Forms::CheckBox());
			this->combRegion = (gcnew System::Windows::Forms::ComboBox());
			this->cboxIsInputPerson2 = (gcnew System::Windows::Forms::CheckBox());
			this->gboxPerson2 = (gcnew System::Windows::Forms::GroupBox());
			this->labArbit4 = (gcnew System::Windows::Forms::Label());
			this->labArbit3 = (gcnew System::Windows::Forms::Label());
			this->labNTSC2Sur = (gcnew System::Windows::Forms::Label());
			this->tboxNTSC2 = (gcnew System::Windows::Forms::TextBox());
			this->labFax2 = (gcnew System::Windows::Forms::Label());
			this->labNTSC2Pre = (gcnew System::Windows::Forms::Label());
			this->tboxFax2 = (gcnew System::Windows::Forms::TextBox());
			this->tboxMail2 = (gcnew System::Windows::Forms::TextBox());
			this->tboxTel2 = (gcnew System::Windows::Forms::TextBox());
			this->tboxFurigana2 = (gcnew System::Windows::Forms::TextBox());
			this->tboxPerson2 = (gcnew System::Windows::Forms::TextBox());
			this->tboxDepart2 = (gcnew System::Windows::Forms::TextBox());
			this->labDepart2 = (gcnew System::Windows::Forms::Label());
			this->tboxCompany2 = (gcnew System::Windows::Forms::TextBox());
			this->labMail2 = (gcnew System::Windows::Forms::Label());
			this->labTel2 = (gcnew System::Windows::Forms::Label());
			this->labFurigana2 = (gcnew System::Windows::Forms::Label());
			this->labPerson2 = (gcnew System::Windows::Forms::Label());
			this->labCompany2 = (gcnew System::Windows::Forms::Label());
			this->gboxPerson1 = (gcnew System::Windows::Forms::GroupBox());
			this->labArbit2 = (gcnew System::Windows::Forms::Label());
			this->labArbit1 = (gcnew System::Windows::Forms::Label());
			this->labNTSC1Sur = (gcnew System::Windows::Forms::Label());
			this->labFax1 = (gcnew System::Windows::Forms::Label());
			this->labNTSC1Pre = (gcnew System::Windows::Forms::Label());
			this->tboxNTSC1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxFax1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxMail1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxTel1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxFurigana1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxPerson1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxDepart1 = (gcnew System::Windows::Forms::TextBox());
			this->labDepart1 = (gcnew System::Windows::Forms::Label());
			this->tboxCompany1 = (gcnew System::Windows::Forms::TextBox());
			this->labMail1 = (gcnew System::Windows::Forms::Label());
			this->labTel1 = (gcnew System::Windows::Forms::Label());
			this->labFurigana1 = (gcnew System::Windows::Forms::Label());
			this->labPerson1 = (gcnew System::Windows::Forms::Label());
			this->labCompany1 = (gcnew System::Windows::Forms::Label());
			this->tboxProductCode2 = (gcnew System::Windows::Forms::TextBox());
			this->tboxProductCode1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxProductName = (gcnew System::Windows::Forms::TextBox());
			this->labProductCode2 = (gcnew System::Windows::Forms::Label());
			this->labProductCode1 = (gcnew System::Windows::Forms::Label());
			this->dateSubmit = (gcnew System::Windows::Forms::DateTimePicker());
			this->dateRelease = (gcnew System::Windows::Forms::DateTimePicker());
			this->gboxUsage = (gcnew System::Windows::Forms::GroupBox());
			this->tboxUsageOther = (gcnew System::Windows::Forms::TextBox());
			this->rUsageOther = (gcnew System::Windows::Forms::RadioButton());
			this->rUsageDst = (gcnew System::Windows::Forms::RadioButton());
			this->rUsageSample = (gcnew System::Windows::Forms::RadioButton());
			this->rUsageSale = (gcnew System::Windows::Forms::RadioButton());
			this->gboxSubmitWay = (gcnew System::Windows::Forms::GroupBox());
			this->rSubmitHand = (gcnew System::Windows::Forms::RadioButton());
			this->rSubmitPost = (gcnew System::Windows::Forms::RadioButton());
			this->labSubmiteDate = (gcnew System::Windows::Forms::Label());
			this->labReleaseDate = (gcnew System::Windows::Forms::Label());
			this->labProductCode = (gcnew System::Windows::Forms::Label());
			this->labProductName = (gcnew System::Windows::Forms::Label());
			this->labCapSubmitVer = (gcnew System::Windows::Forms::Label());
			this->numSubmitVersion = (gcnew System::Windows::Forms::NumericUpDown());
			this->labSubmitVer = (gcnew System::Windows::Forms::Label());
			this->labMultiForeign1 = (gcnew System::Windows::Forms::Label());
			this->tboxProductCode2Foreign3 = (gcnew System::Windows::Forms::TextBox());
			this->tboxProductCode2Foreign2 = (gcnew System::Windows::Forms::TextBox());
			this->labProductCode2Foreign = (gcnew System::Windows::Forms::Label());
			this->cboxReleaseForeign = (gcnew System::Windows::Forms::CheckBox());
			this->labProductNameForeign = (gcnew System::Windows::Forms::Label());
			this->tboxProductNameForeign = (gcnew System::Windows::Forms::TextBox());
			this->labProductCode1Foreign = (gcnew System::Windows::Forms::Label());
			this->tboxProductCode1Foreign = (gcnew System::Windows::Forms::TextBox());
			this->labProductCodeForeign = (gcnew System::Windows::Forms::Label());
			this->tboxProductCode2Foreign1 = (gcnew System::Windows::Forms::TextBox());
			this->gboxShared2Size = (gcnew System::Windows::Forms::GroupBox());
			this->labShared2Size5 = (gcnew System::Windows::Forms::Label());
			this->labShared2Size4 = (gcnew System::Windows::Forms::Label());
			this->labShared2Size3 = (gcnew System::Windows::Forms::Label());
			this->labShared2Size2 = (gcnew System::Windows::Forms::Label());
			this->labShared2Size1 = (gcnew System::Windows::Forms::Label());
			this->labShared2Size0 = (gcnew System::Windows::Forms::Label());
			this->tboxShared2Size5 = (gcnew System::Windows::Forms::TextBox());
			this->tboxShared2Size4 = (gcnew System::Windows::Forms::TextBox());
			this->tboxShared2Size3 = (gcnew System::Windows::Forms::TextBox());
			this->tboxShared2Size2 = (gcnew System::Windows::Forms::TextBox());
			this->tboxShared2Size1 = (gcnew System::Windows::Forms::TextBox());
			this->tboxShared2Size0 = (gcnew System::Windows::Forms::TextBox());
			this->cboxIsShared2 = (gcnew System::Windows::Forms::CheckBox());
			this->labLib = (gcnew System::Windows::Forms::Label());
			this->tboxSDK = (gcnew System::Windows::Forms::TextBox());
			this->labSDK = (gcnew System::Windows::Forms::Label());
			this->gboxTWLExInfo = (gcnew System::Windows::Forms::GroupBox());
			this->labByte1 = (gcnew System::Windows::Forms::Label());
			this->labHex4 = (gcnew System::Windows::Forms::Label());
			this->labHex3 = (gcnew System::Windows::Forms::Label());
			this->tboxIsCodec = (gcnew System::Windows::Forms::TextBox());
			this->labIsCodec = (gcnew System::Windows::Forms::Label());
			this->labKeyTableRomOffset = (gcnew System::Windows::Forms::Label());
			this->tboxKeyTableRomOffset = (gcnew System::Windows::Forms::TextBox());
			this->labNormalRomOffset = (gcnew System::Windows::Forms::Label());
			this->tboxNormalRomOffset = (gcnew System::Windows::Forms::TextBox());
			this->tboxPrivateSize = (gcnew System::Windows::Forms::TextBox());
			this->labPrivateSize = (gcnew System::Windows::Forms::Label());
			this->labPublicSize = (gcnew System::Windows::Forms::Label());
			this->tboxPublicSize = (gcnew System::Windows::Forms::TextBox());
			this->cboxIsSubBanner = (gcnew System::Windows::Forms::CheckBox());
			this->cboxIsNormalJump = (gcnew System::Windows::Forms::CheckBox());
			this->cboxIsTmpJump = (gcnew System::Windows::Forms::CheckBox());
			this->gboxAccess = (gcnew System::Windows::Forms::GroupBox());
			this->labIsGameCardOn2 = (gcnew System::Windows::Forms::Label());
			this->labAccessOther = (gcnew System::Windows::Forms::Label());
			this->tboxAccessOther = (gcnew System::Windows::Forms::TextBox());
			this->tboxIsGameCardOn = (gcnew System::Windows::Forms::TextBox());
			this->labIsGameCardOn = (gcnew System::Windows::Forms::Label());
			this->cboxIsNAND = (gcnew System::Windows::Forms::CheckBox());
			this->cboxIsSD = (gcnew System::Windows::Forms::CheckBox());
			this->gboxTitleID = (gcnew System::Windows::Forms::GroupBox());
			this->labAppType = (gcnew System::Windows::Forms::Label());
			this->tboxAppType = (gcnew System::Windows::Forms::TextBox());
			this->labHex2 = (gcnew System::Windows::Forms::Label());
			this->tboxTitleIDLo = (gcnew System::Windows::Forms::TextBox());
			this->labTitleIDLo = (gcnew System::Windows::Forms::Label());
			this->labTitleIDHi = (gcnew System::Windows::Forms::Label());
			this->tboxTitleIDHi = (gcnew System::Windows::Forms::TextBox());
			this->tboxAppTypeOther = (gcnew System::Windows::Forms::TextBox());
			this->labAppTypeOther = (gcnew System::Windows::Forms::Label());
			this->labMedia = (gcnew System::Windows::Forms::Label());
			this->tboxMedia = (gcnew System::Windows::Forms::TextBox());
			this->labCaptionEx = (gcnew System::Windows::Forms::Label());
			this->tboxCaptionEx = (gcnew System::Windows::Forms::TextBox());
			this->gboxProd = (gcnew System::Windows::Forms::GroupBox());
			this->menuStripAbove = (gcnew System::Windows::Forms::MenuStrip());
			this->stripFile = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemOpenRom = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemSepFile1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->stripItemSaveTemp = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemLoadTemp = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripMaster = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemSheet = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemSepMaster1 = (gcnew System::Windows::Forms::ToolStripSeparator());
			this->stripItemMasterRom = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemMiddlewareXml = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemMiddlewareHtml = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripLang = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemEnglish = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->stripItemJapanese = (gcnew System::Windows::Forms::ToolStripMenuItem());
			this->tabMain = (gcnew System::Windows::Forms::TabControl());
			this->tabRomInfo = (gcnew System::Windows::Forms::TabPage());
			this->gridLibrary = (gcnew System::Windows::Forms::DataGridView());
			this->colLibPublisher = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colLibName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->tboxGuideRomInfo = (gcnew System::Windows::Forms::TextBox());
			this->tabTWLInfo = (gcnew System::Windows::Forms::TabPage());
			this->gboxLaunch = (gcnew System::Windows::Forms::GroupBox());
			this->labConnectIcon = (gcnew System::Windows::Forms::Label());
			this->tboxConnectIcon = (gcnew System::Windows::Forms::TextBox());
			this->tboxGuideTWLInfo = (gcnew System::Windows::Forms::TextBox());
			this->gboxExFlags = (gcnew System::Windows::Forms::GroupBox());
			this->tabNandSizeInfo = (gcnew System::Windows::Forms::TabPage());
			this->tboxGuideNandSizeInfo = (gcnew System::Windows::Forms::TextBox());
			this->gboxNandSize = (gcnew System::Windows::Forms::GroupBox());
			this->tboxSrlSize = (gcnew System::Windows::Forms::TextBox());
			this->labShopBlockUnit = (gcnew System::Windows::Forms::Label());
			this->labArrow5 = (gcnew System::Windows::Forms::Label());
			this->labArrow4 = (gcnew System::Windows::Forms::Label());
			this->labArrow3 = (gcnew System::Windows::Forms::Label());
			this->labArrow2 = (gcnew System::Windows::Forms::Label());
			this->labArrow1 = (gcnew System::Windows::Forms::Label());
			this->tboxShopBlock = (gcnew System::Windows::Forms::TextBox());
			this->tboxSumSize = (gcnew System::Windows::Forms::TextBox());
			this->tboxTmdSizeFS = (gcnew System::Windows::Forms::TextBox());
			this->tboxSubBannerSizeFS = (gcnew System::Windows::Forms::TextBox());
			this->tboxPrivateSizeFS = (gcnew System::Windows::Forms::TextBox());
			this->tboxPublicSizeFS = (gcnew System::Windows::Forms::TextBox());
			this->tboxSrlSizeFS = (gcnew System::Windows::Forms::TextBox());
			this->tboxTmdSize = (gcnew System::Windows::Forms::TextBox());
			this->tboxSubBannerSize = (gcnew System::Windows::Forms::TextBox());
			this->labNandSize = (gcnew System::Windows::Forms::Label());
			this->labShopBlock2 = (gcnew System::Windows::Forms::Label());
			this->labNandSizeFS = (gcnew System::Windows::Forms::Label());
			this->labShopBlock = (gcnew System::Windows::Forms::Label());
			this->labSrlSize = (gcnew System::Windows::Forms::Label());
			this->labSumSize2 = (gcnew System::Windows::Forms::Label());
			this->labSumSize = (gcnew System::Windows::Forms::Label());
			this->labSubBannerSize = (gcnew System::Windows::Forms::Label());
			this->labTmdSize = (gcnew System::Windows::Forms::Label());
			this->tabRomEditInfo = (gcnew System::Windows::Forms::TabPage());
			this->gboxOtherSpec = (gcnew System::Windows::Forms::GroupBox());
			this->cboxIsUGC = (gcnew System::Windows::Forms::CheckBox());
			this->cboxIsPhotoEx = (gcnew System::Windows::Forms::CheckBox());
			this->butSetBack = (gcnew System::Windows::Forms::Button());
			this->tboxGuideRomEditInfo = (gcnew System::Windows::Forms::TextBox());
			this->gboxParental = (gcnew System::Windows::Forms::GroupBox());
			this->cboxIsUnnecessaryRating = (gcnew System::Windows::Forms::CheckBox());
			this->tabSubmitInfo = (gcnew System::Windows::Forms::TabPage());
			this->labProductNameLimit = (gcnew System::Windows::Forms::Label());
			this->tboxGuideSubmitInfo = (gcnew System::Windows::Forms::TextBox());
			this->gboxForeign = (gcnew System::Windows::Forms::GroupBox());
			this->labProductNameLimitForeign = (gcnew System::Windows::Forms::Label());
			this->labMultiForeign2 = (gcnew System::Windows::Forms::Label());
			this->tabCompanyInfo = (gcnew System::Windows::Forms::TabPage());
			this->tboxGuideCompanyInfo = (gcnew System::Windows::Forms::TextBox());
			this->tabErrorInfo = (gcnew System::Windows::Forms::TabPage());
			this->tboxGuideErrorInfo = (gcnew System::Windows::Forms::TextBox());
			this->gboxErrorTiming = (gcnew System::Windows::Forms::GroupBox());
			this->rErrorCurrent = (gcnew System::Windows::Forms::RadioButton());
			this->rErrorReading = (gcnew System::Windows::Forms::RadioButton());
			this->labWarn = (gcnew System::Windows::Forms::Label());
			this->labError = (gcnew System::Windows::Forms::Label());
			this->gridWarn = (gcnew System::Windows::Forms::DataGridView());
			this->colWarnName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colWarnBegin = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colWarnEnd = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colWarnCause = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->gridError = (gcnew System::Windows::Forms::DataGridView());
			this->colErrorName = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colErrorBegin = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colErrorEnd = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->colErrorCause = (gcnew System::Windows::Forms::DataGridViewTextBoxColumn());
			this->labFile = (gcnew System::Windows::Forms::Label());
			this->labAssemblyVersion = (gcnew System::Windows::Forms::Label());
			this->gboxSrl->SuspendLayout();
			this->gboxCRC->SuspendLayout();
			this->gboxPerson2->SuspendLayout();
			this->gboxPerson1->SuspendLayout();
			this->gboxUsage->SuspendLayout();
			this->gboxSubmitWay->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSubmitVersion))->BeginInit();
			this->gboxShared2Size->SuspendLayout();
			this->gboxTWLExInfo->SuspendLayout();
			this->gboxAccess->SuspendLayout();
			this->gboxTitleID->SuspendLayout();
			this->gboxProd->SuspendLayout();
			this->menuStripAbove->SuspendLayout();
			this->tabMain->SuspendLayout();
			this->tabRomInfo->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridLibrary))->BeginInit();
			this->tabTWLInfo->SuspendLayout();
			this->gboxLaunch->SuspendLayout();
			this->gboxExFlags->SuspendLayout();
			this->tabNandSizeInfo->SuspendLayout();
			this->gboxNandSize->SuspendLayout();
			this->tabRomEditInfo->SuspendLayout();
			this->gboxOtherSpec->SuspendLayout();
			this->gboxParental->SuspendLayout();
			this->tabSubmitInfo->SuspendLayout();
			this->gboxForeign->SuspendLayout();
			this->tabCompanyInfo->SuspendLayout();
			this->tabErrorInfo->SuspendLayout();
			this->gboxErrorTiming->SuspendLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridWarn))->BeginInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridError))->BeginInit();
			this->SuspendLayout();
			// 
			// tboxFile
			// 
			resources->ApplyResources(this->tboxFile, L"tboxFile");
			this->tboxFile->Name = L"tboxFile";
			this->tboxFile->ReadOnly = true;
			// 
			// gboxSrl
			// 
			this->gboxSrl->Controls->Add(this->tboxRemasterVer);
			this->gboxSrl->Controls->Add(this->tboxRomSize);
			this->gboxSrl->Controls->Add(this->tboxPlatform);
			this->gboxSrl->Controls->Add(this->labPlatform);
			this->gboxSrl->Controls->Add(this->tboxRomLatency);
			this->gboxSrl->Controls->Add(this->labRomSize);
			this->gboxSrl->Controls->Add(this->labRomType);
			this->gboxSrl->Controls->Add(this->tboxMakerCode);
			this->gboxSrl->Controls->Add(this->cboxRemasterVerE);
			this->gboxSrl->Controls->Add(this->labMakerCode);
			this->gboxSrl->Controls->Add(this->labGameCode);
			this->gboxSrl->Controls->Add(this->tboxGameCode);
			this->gboxSrl->Controls->Add(this->labTitleName);
			this->gboxSrl->Controls->Add(this->labRemasterVer);
			this->gboxSrl->Controls->Add(this->tboxTitleName);
			resources->ApplyResources(this->gboxSrl, L"gboxSrl");
			this->gboxSrl->Name = L"gboxSrl";
			this->gboxSrl->TabStop = false;
			// 
			// tboxRemasterVer
			// 
			resources->ApplyResources(this->tboxRemasterVer, L"tboxRemasterVer");
			this->tboxRemasterVer->Name = L"tboxRemasterVer";
			this->tboxRemasterVer->ReadOnly = true;
			// 
			// tboxRomSize
			// 
			resources->ApplyResources(this->tboxRomSize, L"tboxRomSize");
			this->tboxRomSize->Name = L"tboxRomSize";
			this->tboxRomSize->ReadOnly = true;
			// 
			// tboxPlatform
			// 
			resources->ApplyResources(this->tboxPlatform, L"tboxPlatform");
			this->tboxPlatform->Name = L"tboxPlatform";
			this->tboxPlatform->ReadOnly = true;
			// 
			// labPlatform
			// 
			resources->ApplyResources(this->labPlatform, L"labPlatform");
			this->labPlatform->Name = L"labPlatform";
			// 
			// tboxRomLatency
			// 
			resources->ApplyResources(this->tboxRomLatency, L"tboxRomLatency");
			this->tboxRomLatency->Name = L"tboxRomLatency";
			this->tboxRomLatency->ReadOnly = true;
			// 
			// labRomSize
			// 
			resources->ApplyResources(this->labRomSize, L"labRomSize");
			this->labRomSize->Name = L"labRomSize";
			// 
			// labRomType
			// 
			resources->ApplyResources(this->labRomType, L"labRomType");
			this->labRomType->Name = L"labRomType";
			// 
			// tboxMakerCode
			// 
			resources->ApplyResources(this->tboxMakerCode, L"tboxMakerCode");
			this->tboxMakerCode->Name = L"tboxMakerCode";
			this->tboxMakerCode->ReadOnly = true;
			// 
			// cboxRemasterVerE
			// 
			resources->ApplyResources(this->cboxRemasterVerE, L"cboxRemasterVerE");
			this->cboxRemasterVerE->Name = L"cboxRemasterVerE";
			this->cboxRemasterVerE->UseVisualStyleBackColor = true;
			this->cboxRemasterVerE->CheckedChanged += gcnew System::EventHandler(this, &Form1::cboxRemasterVerE_CheckedChanged);
			// 
			// labMakerCode
			// 
			resources->ApplyResources(this->labMakerCode, L"labMakerCode");
			this->labMakerCode->Name = L"labMakerCode";
			// 
			// labGameCode
			// 
			resources->ApplyResources(this->labGameCode, L"labGameCode");
			this->labGameCode->Name = L"labGameCode";
			// 
			// tboxGameCode
			// 
			resources->ApplyResources(this->tboxGameCode, L"tboxGameCode");
			this->tboxGameCode->Name = L"tboxGameCode";
			this->tboxGameCode->ReadOnly = true;
			// 
			// labTitleName
			// 
			resources->ApplyResources(this->labTitleName, L"labTitleName");
			this->labTitleName->Name = L"labTitleName";
			// 
			// labRemasterVer
			// 
			resources->ApplyResources(this->labRemasterVer, L"labRemasterVer");
			this->labRemasterVer->Name = L"labRemasterVer";
			// 
			// tboxTitleName
			// 
			resources->ApplyResources(this->tboxTitleName, L"tboxTitleName");
			this->tboxTitleName->Name = L"tboxTitleName";
			this->tboxTitleName->ReadOnly = true;
			// 
			// labBackup
			// 
			resources->ApplyResources(this->labBackup, L"labBackup");
			this->labBackup->Name = L"labBackup";
			// 
			// combBackup
			// 
			this->combBackup->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combBackup->FormattingEnabled = true;
			this->combBackup->Items->AddRange(gcnew cli::array< System::Object^  >(9) {resources->GetString(L"combBackup.Items"), resources->GetString(L"combBackup.Items1"), 
				resources->GetString(L"combBackup.Items2"), resources->GetString(L"combBackup.Items3"), resources->GetString(L"combBackup.Items4"), 
				resources->GetString(L"combBackup.Items5"), resources->GetString(L"combBackup.Items6"), resources->GetString(L"combBackup.Items7"), 
				resources->GetString(L"combBackup.Items8")});
			resources->ApplyResources(this->combBackup, L"combBackup");
			this->combBackup->Name = L"combBackup";
			this->combBackup->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::combBackup_SelectedIndexChanged);
			// 
			// tboxBackupOther
			// 
			resources->ApplyResources(this->tboxBackupOther, L"tboxBackupOther");
			this->tboxBackupOther->Name = L"tboxBackupOther";
			// 
			// gboxCRC
			// 
			this->gboxCRC->Controls->Add(this->labRomCRC);
			this->gboxCRC->Controls->Add(this->labHeaderCRC);
			this->gboxCRC->Controls->Add(this->tboxHeaderCRC);
			this->gboxCRC->Controls->Add(this->tboxWholeCRC);
			resources->ApplyResources(this->gboxCRC, L"gboxCRC");
			this->gboxCRC->Name = L"gboxCRC";
			this->gboxCRC->TabStop = false;
			// 
			// labRomCRC
			// 
			resources->ApplyResources(this->labRomCRC, L"labRomCRC");
			this->labRomCRC->Name = L"labRomCRC";
			// 
			// labHeaderCRC
			// 
			resources->ApplyResources(this->labHeaderCRC, L"labHeaderCRC");
			this->labHeaderCRC->Name = L"labHeaderCRC";
			// 
			// tboxHeaderCRC
			// 
			resources->ApplyResources(this->tboxHeaderCRC, L"tboxHeaderCRC");
			this->tboxHeaderCRC->Name = L"tboxHeaderCRC";
			this->tboxHeaderCRC->ReadOnly = true;
			// 
			// tboxWholeCRC
			// 
			resources->ApplyResources(this->tboxWholeCRC, L"tboxWholeCRC");
			this->tboxWholeCRC->Name = L"tboxWholeCRC";
			this->tboxWholeCRC->ReadOnly = true;
			// 
			// labCaption
			// 
			resources->ApplyResources(this->labCaption, L"labCaption");
			this->labCaption->Name = L"labCaption";
			// 
			// tboxCaption
			// 
			resources->ApplyResources(this->tboxCaption, L"tboxCaption");
			this->tboxCaption->Name = L"tboxCaption";
			// 
			// labOFLC
			// 
			resources->ApplyResources(this->labOFLC, L"labOFLC");
			this->labOFLC->Name = L"labOFLC";
			// 
			// labPEGI_BBFC
			// 
			resources->ApplyResources(this->labPEGI_BBFC, L"labPEGI_BBFC");
			this->labPEGI_BBFC->Name = L"labPEGI_BBFC";
			// 
			// labPEGI_PRT
			// 
			resources->ApplyResources(this->labPEGI_PRT, L"labPEGI_PRT");
			this->labPEGI_PRT->Name = L"labPEGI_PRT";
			// 
			// labPEGI
			// 
			resources->ApplyResources(this->labPEGI, L"labPEGI");
			this->labPEGI->Name = L"labPEGI";
			// 
			// labUSK
			// 
			resources->ApplyResources(this->labUSK, L"labUSK");
			this->labUSK->Name = L"labUSK";
			// 
			// labESRB
			// 
			resources->ApplyResources(this->labESRB, L"labESRB");
			this->labESRB->Name = L"labESRB";
			// 
			// labCERO
			// 
			resources->ApplyResources(this->labCERO, L"labCERO");
			this->labCERO->Name = L"labCERO";
			// 
			// combOFLC
			// 
			this->combOFLC->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combOFLC->FormattingEnabled = true;
			this->combOFLC->Items->AddRange(gcnew cli::array< System::Object^  >(5) {resources->GetString(L"combOFLC.Items"), resources->GetString(L"combOFLC.Items1"), 
				resources->GetString(L"combOFLC.Items2"), resources->GetString(L"combOFLC.Items3"), resources->GetString(L"combOFLC.Items4")});
			resources->ApplyResources(this->combOFLC, L"combOFLC");
			this->combOFLC->Name = L"combOFLC";
			// 
			// combPEGI_BBFC
			// 
			this->combPEGI_BBFC->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combPEGI_BBFC->FormattingEnabled = true;
			this->combPEGI_BBFC->Items->AddRange(gcnew cli::array< System::Object^  >(10) {resources->GetString(L"combPEGI_BBFC.Items"), 
				resources->GetString(L"combPEGI_BBFC.Items1"), resources->GetString(L"combPEGI_BBFC.Items2"), resources->GetString(L"combPEGI_BBFC.Items3"), 
				resources->GetString(L"combPEGI_BBFC.Items4"), resources->GetString(L"combPEGI_BBFC.Items5"), resources->GetString(L"combPEGI_BBFC.Items6"), 
				resources->GetString(L"combPEGI_BBFC.Items7"), resources->GetString(L"combPEGI_BBFC.Items8"), resources->GetString(L"combPEGI_BBFC.Items9")});
			resources->ApplyResources(this->combPEGI_BBFC, L"combPEGI_BBFC");
			this->combPEGI_BBFC->Name = L"combPEGI_BBFC";
			// 
			// combPEGI_PRT
			// 
			this->combPEGI_PRT->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combPEGI_PRT->FormattingEnabled = true;
			this->combPEGI_PRT->Items->AddRange(gcnew cli::array< System::Object^  >(7) {resources->GetString(L"combPEGI_PRT.Items"), 
				resources->GetString(L"combPEGI_PRT.Items1"), resources->GetString(L"combPEGI_PRT.Items2"), resources->GetString(L"combPEGI_PRT.Items3"), 
				resources->GetString(L"combPEGI_PRT.Items4"), resources->GetString(L"combPEGI_PRT.Items5"), resources->GetString(L"combPEGI_PRT.Items6")});
			resources->ApplyResources(this->combPEGI_PRT, L"combPEGI_PRT");
			this->combPEGI_PRT->Name = L"combPEGI_PRT";
			// 
			// combPEGI
			// 
			this->combPEGI->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combPEGI->FormattingEnabled = true;
			this->combPEGI->Items->AddRange(gcnew cli::array< System::Object^  >(7) {resources->GetString(L"combPEGI.Items"), resources->GetString(L"combPEGI.Items1"), 
				resources->GetString(L"combPEGI.Items2"), resources->GetString(L"combPEGI.Items3"), resources->GetString(L"combPEGI.Items4"), 
				resources->GetString(L"combPEGI.Items5"), resources->GetString(L"combPEGI.Items6")});
			resources->ApplyResources(this->combPEGI, L"combPEGI");
			this->combPEGI->Name = L"combPEGI";
			// 
			// combUSK
			// 
			this->combUSK->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combUSK->FormattingEnabled = true;
			this->combUSK->Items->AddRange(gcnew cli::array< System::Object^  >(6) {resources->GetString(L"combUSK.Items"), resources->GetString(L"combUSK.Items1"), 
				resources->GetString(L"combUSK.Items2"), resources->GetString(L"combUSK.Items3"), resources->GetString(L"combUSK.Items4"), resources->GetString(L"combUSK.Items5")});
			resources->ApplyResources(this->combUSK, L"combUSK");
			this->combUSK->Name = L"combUSK";
			// 
			// combESRB
			// 
			this->combESRB->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combESRB->FormattingEnabled = true;
			this->combESRB->Items->AddRange(gcnew cli::array< System::Object^  >(7) {resources->GetString(L"combESRB.Items"), resources->GetString(L"combESRB.Items1"), 
				resources->GetString(L"combESRB.Items2"), resources->GetString(L"combESRB.Items3"), resources->GetString(L"combESRB.Items4"), 
				resources->GetString(L"combESRB.Items5"), resources->GetString(L"combESRB.Items6")});
			resources->ApplyResources(this->combESRB, L"combESRB");
			this->combESRB->Name = L"combESRB";
			// 
			// combCERO
			// 
			this->combCERO->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combCERO->FormattingEnabled = true;
			this->combCERO->Items->AddRange(gcnew cli::array< System::Object^  >(6) {resources->GetString(L"combCERO.Items"), resources->GetString(L"combCERO.Items1"), 
				resources->GetString(L"combCERO.Items2"), resources->GetString(L"combCERO.Items3"), resources->GetString(L"combCERO.Items4"), 
				resources->GetString(L"combCERO.Items5")});
			resources->ApplyResources(this->combCERO, L"combCERO");
			this->combCERO->Name = L"combCERO";
			// 
			// labParentalRating
			// 
			resources->ApplyResources(this->labParentalRating, L"labParentalRating");
			this->labParentalRating->Name = L"labParentalRating";
			// 
			// labRegion
			// 
			resources->ApplyResources(this->labRegion, L"labRegion");
			this->labRegion->Name = L"labRegion";
			// 
			// cboxIsEULA
			// 
			resources->ApplyResources(this->cboxIsEULA, L"cboxIsEULA");
			this->cboxIsEULA->Name = L"cboxIsEULA";
			this->cboxIsEULA->UseVisualStyleBackColor = true;
			// 
			// combRegion
			// 
			this->combRegion->DropDownStyle = System::Windows::Forms::ComboBoxStyle::DropDownList;
			this->combRegion->FormattingEnabled = true;
			this->combRegion->Items->AddRange(gcnew cli::array< System::Object^  >(5) {resources->GetString(L"combRegion.Items"), resources->GetString(L"combRegion.Items1"), 
				resources->GetString(L"combRegion.Items2"), resources->GetString(L"combRegion.Items3"), resources->GetString(L"combRegion.Items4")});
			resources->ApplyResources(this->combRegion, L"combRegion");
			this->combRegion->Name = L"combRegion";
			this->combRegion->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::combRegion_SelectedIndexChanged);
			// 
			// cboxIsInputPerson2
			// 
			resources->ApplyResources(this->cboxIsInputPerson2, L"cboxIsInputPerson2");
			this->cboxIsInputPerson2->Name = L"cboxIsInputPerson2";
			this->cboxIsInputPerson2->UseVisualStyleBackColor = true;
			this->cboxIsInputPerson2->CheckedChanged += gcnew System::EventHandler(this, &Form1::cboxIsInputPerson2_CheckedChanged);
			// 
			// gboxPerson2
			// 
			this->gboxPerson2->Controls->Add(this->labArbit4);
			this->gboxPerson2->Controls->Add(this->labArbit3);
			this->gboxPerson2->Controls->Add(this->labNTSC2Sur);
			this->gboxPerson2->Controls->Add(this->tboxNTSC2);
			this->gboxPerson2->Controls->Add(this->labFax2);
			this->gboxPerson2->Controls->Add(this->labNTSC2Pre);
			this->gboxPerson2->Controls->Add(this->tboxFax2);
			this->gboxPerson2->Controls->Add(this->tboxMail2);
			this->gboxPerson2->Controls->Add(this->tboxTel2);
			this->gboxPerson2->Controls->Add(this->tboxFurigana2);
			this->gboxPerson2->Controls->Add(this->tboxPerson2);
			this->gboxPerson2->Controls->Add(this->tboxDepart2);
			this->gboxPerson2->Controls->Add(this->labDepart2);
			this->gboxPerson2->Controls->Add(this->tboxCompany2);
			this->gboxPerson2->Controls->Add(this->labMail2);
			this->gboxPerson2->Controls->Add(this->labTel2);
			this->gboxPerson2->Controls->Add(this->labFurigana2);
			this->gboxPerson2->Controls->Add(this->labPerson2);
			this->gboxPerson2->Controls->Add(this->labCompany2);
			resources->ApplyResources(this->gboxPerson2, L"gboxPerson2");
			this->gboxPerson2->Name = L"gboxPerson2";
			this->gboxPerson2->TabStop = false;
			// 
			// labArbit4
			// 
			resources->ApplyResources(this->labArbit4, L"labArbit4");
			this->labArbit4->Name = L"labArbit4";
			// 
			// labArbit3
			// 
			resources->ApplyResources(this->labArbit3, L"labArbit3");
			this->labArbit3->Name = L"labArbit3";
			// 
			// labNTSC2Sur
			// 
			resources->ApplyResources(this->labNTSC2Sur, L"labNTSC2Sur");
			this->labNTSC2Sur->Name = L"labNTSC2Sur";
			// 
			// tboxNTSC2
			// 
			resources->ApplyResources(this->tboxNTSC2, L"tboxNTSC2");
			this->tboxNTSC2->Name = L"tboxNTSC2";
			// 
			// labFax2
			// 
			resources->ApplyResources(this->labFax2, L"labFax2");
			this->labFax2->Name = L"labFax2";
			// 
			// labNTSC2Pre
			// 
			resources->ApplyResources(this->labNTSC2Pre, L"labNTSC2Pre");
			this->labNTSC2Pre->Name = L"labNTSC2Pre";
			// 
			// tboxFax2
			// 
			resources->ApplyResources(this->tboxFax2, L"tboxFax2");
			this->tboxFax2->Name = L"tboxFax2";
			// 
			// tboxMail2
			// 
			resources->ApplyResources(this->tboxMail2, L"tboxMail2");
			this->tboxMail2->Name = L"tboxMail2";
			// 
			// tboxTel2
			// 
			resources->ApplyResources(this->tboxTel2, L"tboxTel2");
			this->tboxTel2->Name = L"tboxTel2";
			// 
			// tboxFurigana2
			// 
			resources->ApplyResources(this->tboxFurigana2, L"tboxFurigana2");
			this->tboxFurigana2->Name = L"tboxFurigana2";
			// 
			// tboxPerson2
			// 
			resources->ApplyResources(this->tboxPerson2, L"tboxPerson2");
			this->tboxPerson2->Name = L"tboxPerson2";
			// 
			// tboxDepart2
			// 
			resources->ApplyResources(this->tboxDepart2, L"tboxDepart2");
			this->tboxDepart2->Name = L"tboxDepart2";
			// 
			// labDepart2
			// 
			resources->ApplyResources(this->labDepart2, L"labDepart2");
			this->labDepart2->Name = L"labDepart2";
			// 
			// tboxCompany2
			// 
			resources->ApplyResources(this->tboxCompany2, L"tboxCompany2");
			this->tboxCompany2->Name = L"tboxCompany2";
			// 
			// labMail2
			// 
			resources->ApplyResources(this->labMail2, L"labMail2");
			this->labMail2->Name = L"labMail2";
			// 
			// labTel2
			// 
			resources->ApplyResources(this->labTel2, L"labTel2");
			this->labTel2->Name = L"labTel2";
			// 
			// labFurigana2
			// 
			resources->ApplyResources(this->labFurigana2, L"labFurigana2");
			this->labFurigana2->Name = L"labFurigana2";
			// 
			// labPerson2
			// 
			resources->ApplyResources(this->labPerson2, L"labPerson2");
			this->labPerson2->Name = L"labPerson2";
			// 
			// labCompany2
			// 
			resources->ApplyResources(this->labCompany2, L"labCompany2");
			this->labCompany2->Name = L"labCompany2";
			// 
			// gboxPerson1
			// 
			this->gboxPerson1->Controls->Add(this->labArbit2);
			this->gboxPerson1->Controls->Add(this->labArbit1);
			this->gboxPerson1->Controls->Add(this->labNTSC1Sur);
			this->gboxPerson1->Controls->Add(this->labFax1);
			this->gboxPerson1->Controls->Add(this->labNTSC1Pre);
			this->gboxPerson1->Controls->Add(this->tboxNTSC1);
			this->gboxPerson1->Controls->Add(this->tboxFax1);
			this->gboxPerson1->Controls->Add(this->tboxMail1);
			this->gboxPerson1->Controls->Add(this->tboxTel1);
			this->gboxPerson1->Controls->Add(this->tboxFurigana1);
			this->gboxPerson1->Controls->Add(this->tboxPerson1);
			this->gboxPerson1->Controls->Add(this->tboxDepart1);
			this->gboxPerson1->Controls->Add(this->labDepart1);
			this->gboxPerson1->Controls->Add(this->tboxCompany1);
			this->gboxPerson1->Controls->Add(this->labMail1);
			this->gboxPerson1->Controls->Add(this->labTel1);
			this->gboxPerson1->Controls->Add(this->labFurigana1);
			this->gboxPerson1->Controls->Add(this->labPerson1);
			this->gboxPerson1->Controls->Add(this->labCompany1);
			resources->ApplyResources(this->gboxPerson1, L"gboxPerson1");
			this->gboxPerson1->Name = L"gboxPerson1";
			this->gboxPerson1->TabStop = false;
			// 
			// labArbit2
			// 
			resources->ApplyResources(this->labArbit2, L"labArbit2");
			this->labArbit2->Name = L"labArbit2";
			// 
			// labArbit1
			// 
			resources->ApplyResources(this->labArbit1, L"labArbit1");
			this->labArbit1->Name = L"labArbit1";
			// 
			// labNTSC1Sur
			// 
			resources->ApplyResources(this->labNTSC1Sur, L"labNTSC1Sur");
			this->labNTSC1Sur->Name = L"labNTSC1Sur";
			// 
			// labFax1
			// 
			resources->ApplyResources(this->labFax1, L"labFax1");
			this->labFax1->Name = L"labFax1";
			// 
			// labNTSC1Pre
			// 
			resources->ApplyResources(this->labNTSC1Pre, L"labNTSC1Pre");
			this->labNTSC1Pre->Name = L"labNTSC1Pre";
			// 
			// tboxNTSC1
			// 
			resources->ApplyResources(this->tboxNTSC1, L"tboxNTSC1");
			this->tboxNTSC1->Name = L"tboxNTSC1";
			// 
			// tboxFax1
			// 
			resources->ApplyResources(this->tboxFax1, L"tboxFax1");
			this->tboxFax1->Name = L"tboxFax1";
			// 
			// tboxMail1
			// 
			resources->ApplyResources(this->tboxMail1, L"tboxMail1");
			this->tboxMail1->Name = L"tboxMail1";
			// 
			// tboxTel1
			// 
			resources->ApplyResources(this->tboxTel1, L"tboxTel1");
			this->tboxTel1->Name = L"tboxTel1";
			// 
			// tboxFurigana1
			// 
			resources->ApplyResources(this->tboxFurigana1, L"tboxFurigana1");
			this->tboxFurigana1->Name = L"tboxFurigana1";
			// 
			// tboxPerson1
			// 
			resources->ApplyResources(this->tboxPerson1, L"tboxPerson1");
			this->tboxPerson1->Name = L"tboxPerson1";
			// 
			// tboxDepart1
			// 
			resources->ApplyResources(this->tboxDepart1, L"tboxDepart1");
			this->tboxDepart1->Name = L"tboxDepart1";
			// 
			// labDepart1
			// 
			resources->ApplyResources(this->labDepart1, L"labDepart1");
			this->labDepart1->Name = L"labDepart1";
			// 
			// tboxCompany1
			// 
			resources->ApplyResources(this->tboxCompany1, L"tboxCompany1");
			this->tboxCompany1->Name = L"tboxCompany1";
			// 
			// labMail1
			// 
			resources->ApplyResources(this->labMail1, L"labMail1");
			this->labMail1->Name = L"labMail1";
			// 
			// labTel1
			// 
			resources->ApplyResources(this->labTel1, L"labTel1");
			this->labTel1->Name = L"labTel1";
			// 
			// labFurigana1
			// 
			resources->ApplyResources(this->labFurigana1, L"labFurigana1");
			this->labFurigana1->Name = L"labFurigana1";
			// 
			// labPerson1
			// 
			resources->ApplyResources(this->labPerson1, L"labPerson1");
			this->labPerson1->Name = L"labPerson1";
			// 
			// labCompany1
			// 
			resources->ApplyResources(this->labCompany1, L"labCompany1");
			this->labCompany1->Name = L"labCompany1";
			// 
			// tboxProductCode2
			// 
			resources->ApplyResources(this->tboxProductCode2, L"tboxProductCode2");
			this->tboxProductCode2->Name = L"tboxProductCode2";
			// 
			// tboxProductCode1
			// 
			resources->ApplyResources(this->tboxProductCode1, L"tboxProductCode1");
			this->tboxProductCode1->Name = L"tboxProductCode1";
			// 
			// tboxProductName
			// 
			resources->ApplyResources(this->tboxProductName, L"tboxProductName");
			this->tboxProductName->Name = L"tboxProductName";
			// 
			// labProductCode2
			// 
			resources->ApplyResources(this->labProductCode2, L"labProductCode2");
			this->labProductCode2->Name = L"labProductCode2";
			// 
			// labProductCode1
			// 
			resources->ApplyResources(this->labProductCode1, L"labProductCode1");
			this->labProductCode1->Name = L"labProductCode1";
			// 
			// dateSubmit
			// 
			this->dateSubmit->Format = System::Windows::Forms::DateTimePickerFormat::Short;
			resources->ApplyResources(this->dateSubmit, L"dateSubmit");
			this->dateSubmit->MaxDate = System::DateTime(2099, 12, 31, 0, 0, 0, 0);
			this->dateSubmit->MinDate = System::DateTime(2008, 1, 1, 0, 0, 0, 0);
			this->dateSubmit->Name = L"dateSubmit";
			this->dateSubmit->Value = System::DateTime(2008, 7, 30, 17, 43, 18, 405);
			// 
			// dateRelease
			// 
			this->dateRelease->Format = System::Windows::Forms::DateTimePickerFormat::Short;
			resources->ApplyResources(this->dateRelease, L"dateRelease");
			this->dateRelease->MaxDate = System::DateTime(2099, 12, 31, 0, 0, 0, 0);
			this->dateRelease->MinDate = System::DateTime(2008, 1, 1, 0, 0, 0, 0);
			this->dateRelease->Name = L"dateRelease";
			this->dateRelease->Value = System::DateTime(2008, 7, 30, 17, 43, 18, 420);
			// 
			// gboxUsage
			// 
			this->gboxUsage->Controls->Add(this->tboxUsageOther);
			this->gboxUsage->Controls->Add(this->rUsageOther);
			this->gboxUsage->Controls->Add(this->rUsageDst);
			this->gboxUsage->Controls->Add(this->rUsageSample);
			this->gboxUsage->Controls->Add(this->rUsageSale);
			resources->ApplyResources(this->gboxUsage, L"gboxUsage");
			this->gboxUsage->Name = L"gboxUsage";
			this->gboxUsage->TabStop = false;
			// 
			// tboxUsageOther
			// 
			resources->ApplyResources(this->tboxUsageOther, L"tboxUsageOther");
			this->tboxUsageOther->Name = L"tboxUsageOther";
			// 
			// rUsageOther
			// 
			resources->ApplyResources(this->rUsageOther, L"rUsageOther");
			this->rUsageOther->Name = L"rUsageOther";
			this->rUsageOther->UseVisualStyleBackColor = true;
			this->rUsageOther->CheckedChanged += gcnew System::EventHandler(this, &Form1::rUsageOther_CheckedChanged);
			// 
			// rUsageDst
			// 
			resources->ApplyResources(this->rUsageDst, L"rUsageDst");
			this->rUsageDst->Name = L"rUsageDst";
			this->rUsageDst->UseVisualStyleBackColor = true;
			// 
			// rUsageSample
			// 
			resources->ApplyResources(this->rUsageSample, L"rUsageSample");
			this->rUsageSample->Name = L"rUsageSample";
			this->rUsageSample->UseVisualStyleBackColor = true;
			// 
			// rUsageSale
			// 
			resources->ApplyResources(this->rUsageSale, L"rUsageSale");
			this->rUsageSale->Checked = true;
			this->rUsageSale->Name = L"rUsageSale";
			this->rUsageSale->TabStop = true;
			this->rUsageSale->UseVisualStyleBackColor = true;
			// 
			// gboxSubmitWay
			// 
			this->gboxSubmitWay->Controls->Add(this->rSubmitHand);
			this->gboxSubmitWay->Controls->Add(this->rSubmitPost);
			resources->ApplyResources(this->gboxSubmitWay, L"gboxSubmitWay");
			this->gboxSubmitWay->Name = L"gboxSubmitWay";
			this->gboxSubmitWay->TabStop = false;
			// 
			// rSubmitHand
			// 
			resources->ApplyResources(this->rSubmitHand, L"rSubmitHand");
			this->rSubmitHand->Name = L"rSubmitHand";
			this->rSubmitHand->UseVisualStyleBackColor = true;
			// 
			// rSubmitPost
			// 
			resources->ApplyResources(this->rSubmitPost, L"rSubmitPost");
			this->rSubmitPost->Checked = true;
			this->rSubmitPost->Name = L"rSubmitPost";
			this->rSubmitPost->TabStop = true;
			this->rSubmitPost->UseVisualStyleBackColor = true;
			// 
			// labSubmiteDate
			// 
			resources->ApplyResources(this->labSubmiteDate, L"labSubmiteDate");
			this->labSubmiteDate->Name = L"labSubmiteDate";
			// 
			// labReleaseDate
			// 
			resources->ApplyResources(this->labReleaseDate, L"labReleaseDate");
			this->labReleaseDate->Name = L"labReleaseDate";
			// 
			// labProductCode
			// 
			resources->ApplyResources(this->labProductCode, L"labProductCode");
			this->labProductCode->Name = L"labProductCode";
			// 
			// labProductName
			// 
			resources->ApplyResources(this->labProductName, L"labProductName");
			this->labProductName->Name = L"labProductName";
			// 
			// labCapSubmitVer
			// 
			resources->ApplyResources(this->labCapSubmitVer, L"labCapSubmitVer");
			this->labCapSubmitVer->Name = L"labCapSubmitVer";
			// 
			// numSubmitVersion
			// 
			resources->ApplyResources(this->numSubmitVersion, L"numSubmitVersion");
			this->numSubmitVersion->Maximum = System::Decimal(gcnew cli::array< System::Int32 >(4) {15, 0, 0, 0});
			this->numSubmitVersion->Name = L"numSubmitVersion";
			// 
			// labSubmitVer
			// 
			resources->ApplyResources(this->labSubmitVer, L"labSubmitVer");
			this->labSubmitVer->Name = L"labSubmitVer";
			// 
			// labMultiForeign1
			// 
			resources->ApplyResources(this->labMultiForeign1, L"labMultiForeign1");
			this->labMultiForeign1->Name = L"labMultiForeign1";
			// 
			// tboxProductCode2Foreign3
			// 
			resources->ApplyResources(this->tboxProductCode2Foreign3, L"tboxProductCode2Foreign3");
			this->tboxProductCode2Foreign3->Name = L"tboxProductCode2Foreign3";
			// 
			// tboxProductCode2Foreign2
			// 
			resources->ApplyResources(this->tboxProductCode2Foreign2, L"tboxProductCode2Foreign2");
			this->tboxProductCode2Foreign2->Name = L"tboxProductCode2Foreign2";
			// 
			// labProductCode2Foreign
			// 
			resources->ApplyResources(this->labProductCode2Foreign, L"labProductCode2Foreign");
			this->labProductCode2Foreign->Name = L"labProductCode2Foreign";
			// 
			// cboxReleaseForeign
			// 
			resources->ApplyResources(this->cboxReleaseForeign, L"cboxReleaseForeign");
			this->cboxReleaseForeign->Name = L"cboxReleaseForeign";
			this->cboxReleaseForeign->UseVisualStyleBackColor = true;
			this->cboxReleaseForeign->CheckedChanged += gcnew System::EventHandler(this, &Form1::cboxReleaseForeign_CheckedChanged);
			// 
			// labProductNameForeign
			// 
			resources->ApplyResources(this->labProductNameForeign, L"labProductNameForeign");
			this->labProductNameForeign->Name = L"labProductNameForeign";
			// 
			// tboxProductNameForeign
			// 
			resources->ApplyResources(this->tboxProductNameForeign, L"tboxProductNameForeign");
			this->tboxProductNameForeign->Name = L"tboxProductNameForeign";
			// 
			// labProductCode1Foreign
			// 
			resources->ApplyResources(this->labProductCode1Foreign, L"labProductCode1Foreign");
			this->labProductCode1Foreign->Name = L"labProductCode1Foreign";
			// 
			// tboxProductCode1Foreign
			// 
			resources->ApplyResources(this->tboxProductCode1Foreign, L"tboxProductCode1Foreign");
			this->tboxProductCode1Foreign->Name = L"tboxProductCode1Foreign";
			// 
			// labProductCodeForeign
			// 
			resources->ApplyResources(this->labProductCodeForeign, L"labProductCodeForeign");
			this->labProductCodeForeign->Name = L"labProductCodeForeign";
			// 
			// tboxProductCode2Foreign1
			// 
			resources->ApplyResources(this->tboxProductCode2Foreign1, L"tboxProductCode2Foreign1");
			this->tboxProductCode2Foreign1->Name = L"tboxProductCode2Foreign1";
			// 
			// gboxShared2Size
			// 
			this->gboxShared2Size->Controls->Add(this->labShared2Size5);
			this->gboxShared2Size->Controls->Add(this->labShared2Size4);
			this->gboxShared2Size->Controls->Add(this->labShared2Size3);
			this->gboxShared2Size->Controls->Add(this->labShared2Size2);
			this->gboxShared2Size->Controls->Add(this->labShared2Size1);
			this->gboxShared2Size->Controls->Add(this->labShared2Size0);
			this->gboxShared2Size->Controls->Add(this->tboxShared2Size5);
			this->gboxShared2Size->Controls->Add(this->tboxShared2Size4);
			this->gboxShared2Size->Controls->Add(this->tboxShared2Size3);
			this->gboxShared2Size->Controls->Add(this->tboxShared2Size2);
			this->gboxShared2Size->Controls->Add(this->tboxShared2Size1);
			this->gboxShared2Size->Controls->Add(this->tboxShared2Size0);
			this->gboxShared2Size->Controls->Add(this->cboxIsShared2);
			resources->ApplyResources(this->gboxShared2Size, L"gboxShared2Size");
			this->gboxShared2Size->Name = L"gboxShared2Size";
			this->gboxShared2Size->TabStop = false;
			// 
			// labShared2Size5
			// 
			resources->ApplyResources(this->labShared2Size5, L"labShared2Size5");
			this->labShared2Size5->Name = L"labShared2Size5";
			// 
			// labShared2Size4
			// 
			resources->ApplyResources(this->labShared2Size4, L"labShared2Size4");
			this->labShared2Size4->Name = L"labShared2Size4";
			// 
			// labShared2Size3
			// 
			resources->ApplyResources(this->labShared2Size3, L"labShared2Size3");
			this->labShared2Size3->Name = L"labShared2Size3";
			// 
			// labShared2Size2
			// 
			resources->ApplyResources(this->labShared2Size2, L"labShared2Size2");
			this->labShared2Size2->Name = L"labShared2Size2";
			// 
			// labShared2Size1
			// 
			resources->ApplyResources(this->labShared2Size1, L"labShared2Size1");
			this->labShared2Size1->Name = L"labShared2Size1";
			// 
			// labShared2Size0
			// 
			resources->ApplyResources(this->labShared2Size0, L"labShared2Size0");
			this->labShared2Size0->Name = L"labShared2Size0";
			// 
			// tboxShared2Size5
			// 
			resources->ApplyResources(this->tboxShared2Size5, L"tboxShared2Size5");
			this->tboxShared2Size5->Name = L"tboxShared2Size5";
			this->tboxShared2Size5->ReadOnly = true;
			// 
			// tboxShared2Size4
			// 
			resources->ApplyResources(this->tboxShared2Size4, L"tboxShared2Size4");
			this->tboxShared2Size4->Name = L"tboxShared2Size4";
			this->tboxShared2Size4->ReadOnly = true;
			// 
			// tboxShared2Size3
			// 
			resources->ApplyResources(this->tboxShared2Size3, L"tboxShared2Size3");
			this->tboxShared2Size3->Name = L"tboxShared2Size3";
			this->tboxShared2Size3->ReadOnly = true;
			// 
			// tboxShared2Size2
			// 
			resources->ApplyResources(this->tboxShared2Size2, L"tboxShared2Size2");
			this->tboxShared2Size2->Name = L"tboxShared2Size2";
			this->tboxShared2Size2->ReadOnly = true;
			// 
			// tboxShared2Size1
			// 
			resources->ApplyResources(this->tboxShared2Size1, L"tboxShared2Size1");
			this->tboxShared2Size1->Name = L"tboxShared2Size1";
			this->tboxShared2Size1->ReadOnly = true;
			// 
			// tboxShared2Size0
			// 
			resources->ApplyResources(this->tboxShared2Size0, L"tboxShared2Size0");
			this->tboxShared2Size0->Name = L"tboxShared2Size0";
			this->tboxShared2Size0->ReadOnly = true;
			// 
			// cboxIsShared2
			// 
			resources->ApplyResources(this->cboxIsShared2, L"cboxIsShared2");
			this->cboxIsShared2->Name = L"cboxIsShared2";
			this->cboxIsShared2->UseVisualStyleBackColor = true;
			// 
			// labLib
			// 
			resources->ApplyResources(this->labLib, L"labLib");
			this->labLib->Name = L"labLib";
			// 
			// tboxSDK
			// 
			resources->ApplyResources(this->tboxSDK, L"tboxSDK");
			this->tboxSDK->Name = L"tboxSDK";
			this->tboxSDK->ReadOnly = true;
			// 
			// labSDK
			// 
			resources->ApplyResources(this->labSDK, L"labSDK");
			this->labSDK->Name = L"labSDK";
			// 
			// gboxTWLExInfo
			// 
			this->gboxTWLExInfo->Controls->Add(this->labByte1);
			this->gboxTWLExInfo->Controls->Add(this->labHex4);
			this->gboxTWLExInfo->Controls->Add(this->tboxIsCodec);
			this->gboxTWLExInfo->Controls->Add(this->labIsCodec);
			this->gboxTWLExInfo->Controls->Add(this->labKeyTableRomOffset);
			this->gboxTWLExInfo->Controls->Add(this->tboxKeyTableRomOffset);
			resources->ApplyResources(this->gboxTWLExInfo, L"gboxTWLExInfo");
			this->gboxTWLExInfo->Name = L"gboxTWLExInfo";
			this->gboxTWLExInfo->TabStop = false;
			// 
			// labByte1
			// 
			resources->ApplyResources(this->labByte1, L"labByte1");
			this->labByte1->Name = L"labByte1";
			// 
			// labHex4
			// 
			resources->ApplyResources(this->labHex4, L"labHex4");
			this->labHex4->Name = L"labHex4";
			// 
			// labHex3
			// 
			resources->ApplyResources(this->labHex3, L"labHex3");
			this->labHex3->Name = L"labHex3";
			// 
			// tboxIsCodec
			// 
			resources->ApplyResources(this->tboxIsCodec, L"tboxIsCodec");
			this->tboxIsCodec->Name = L"tboxIsCodec";
			this->tboxIsCodec->ReadOnly = true;
			// 
			// labIsCodec
			// 
			resources->ApplyResources(this->labIsCodec, L"labIsCodec");
			this->labIsCodec->Name = L"labIsCodec";
			// 
			// labKeyTableRomOffset
			// 
			resources->ApplyResources(this->labKeyTableRomOffset, L"labKeyTableRomOffset");
			this->labKeyTableRomOffset->Name = L"labKeyTableRomOffset";
			// 
			// tboxKeyTableRomOffset
			// 
			resources->ApplyResources(this->tboxKeyTableRomOffset, L"tboxKeyTableRomOffset");
			this->tboxKeyTableRomOffset->Name = L"tboxKeyTableRomOffset";
			this->tboxKeyTableRomOffset->ReadOnly = true;
			// 
			// labNormalRomOffset
			// 
			resources->ApplyResources(this->labNormalRomOffset, L"labNormalRomOffset");
			this->labNormalRomOffset->Name = L"labNormalRomOffset";
			// 
			// tboxNormalRomOffset
			// 
			resources->ApplyResources(this->tboxNormalRomOffset, L"tboxNormalRomOffset");
			this->tboxNormalRomOffset->Name = L"tboxNormalRomOffset";
			this->tboxNormalRomOffset->ReadOnly = true;
			// 
			// tboxPrivateSize
			// 
			resources->ApplyResources(this->tboxPrivateSize, L"tboxPrivateSize");
			this->tboxPrivateSize->Name = L"tboxPrivateSize";
			this->tboxPrivateSize->ReadOnly = true;
			// 
			// labPrivateSize
			// 
			resources->ApplyResources(this->labPrivateSize, L"labPrivateSize");
			this->labPrivateSize->Name = L"labPrivateSize";
			// 
			// labPublicSize
			// 
			resources->ApplyResources(this->labPublicSize, L"labPublicSize");
			this->labPublicSize->Name = L"labPublicSize";
			// 
			// tboxPublicSize
			// 
			resources->ApplyResources(this->tboxPublicSize, L"tboxPublicSize");
			this->tboxPublicSize->Name = L"tboxPublicSize";
			this->tboxPublicSize->ReadOnly = true;
			// 
			// cboxIsSubBanner
			// 
			resources->ApplyResources(this->cboxIsSubBanner, L"cboxIsSubBanner");
			this->cboxIsSubBanner->Name = L"cboxIsSubBanner";
			this->cboxIsSubBanner->UseVisualStyleBackColor = true;
			// 
			// cboxIsNormalJump
			// 
			resources->ApplyResources(this->cboxIsNormalJump, L"cboxIsNormalJump");
			this->cboxIsNormalJump->Name = L"cboxIsNormalJump";
			this->cboxIsNormalJump->UseVisualStyleBackColor = true;
			// 
			// cboxIsTmpJump
			// 
			resources->ApplyResources(this->cboxIsTmpJump, L"cboxIsTmpJump");
			this->cboxIsTmpJump->Name = L"cboxIsTmpJump";
			this->cboxIsTmpJump->UseVisualStyleBackColor = true;
			// 
			// gboxAccess
			// 
			this->gboxAccess->Controls->Add(this->labIsGameCardOn2);
			this->gboxAccess->Controls->Add(this->labAccessOther);
			this->gboxAccess->Controls->Add(this->tboxAccessOther);
			this->gboxAccess->Controls->Add(this->tboxIsGameCardOn);
			this->gboxAccess->Controls->Add(this->labIsGameCardOn);
			this->gboxAccess->Controls->Add(this->cboxIsNAND);
			this->gboxAccess->Controls->Add(this->cboxIsSD);
			resources->ApplyResources(this->gboxAccess, L"gboxAccess");
			this->gboxAccess->Name = L"gboxAccess";
			this->gboxAccess->TabStop = false;
			// 
			// labIsGameCardOn2
			// 
			resources->ApplyResources(this->labIsGameCardOn2, L"labIsGameCardOn2");
			this->labIsGameCardOn2->Name = L"labIsGameCardOn2";
			// 
			// labAccessOther
			// 
			resources->ApplyResources(this->labAccessOther, L"labAccessOther");
			this->labAccessOther->Name = L"labAccessOther";
			// 
			// tboxAccessOther
			// 
			resources->ApplyResources(this->tboxAccessOther, L"tboxAccessOther");
			this->tboxAccessOther->Name = L"tboxAccessOther";
			this->tboxAccessOther->ReadOnly = true;
			// 
			// tboxIsGameCardOn
			// 
			resources->ApplyResources(this->tboxIsGameCardOn, L"tboxIsGameCardOn");
			this->tboxIsGameCardOn->Name = L"tboxIsGameCardOn";
			this->tboxIsGameCardOn->ReadOnly = true;
			// 
			// labIsGameCardOn
			// 
			resources->ApplyResources(this->labIsGameCardOn, L"labIsGameCardOn");
			this->labIsGameCardOn->Name = L"labIsGameCardOn";
			// 
			// cboxIsNAND
			// 
			resources->ApplyResources(this->cboxIsNAND, L"cboxIsNAND");
			this->cboxIsNAND->Name = L"cboxIsNAND";
			this->cboxIsNAND->UseVisualStyleBackColor = true;
			// 
			// cboxIsSD
			// 
			resources->ApplyResources(this->cboxIsSD, L"cboxIsSD");
			this->cboxIsSD->Name = L"cboxIsSD";
			this->cboxIsSD->UseVisualStyleBackColor = true;
			// 
			// gboxTitleID
			// 
			this->gboxTitleID->Controls->Add(this->labAppType);
			this->gboxTitleID->Controls->Add(this->tboxAppType);
			this->gboxTitleID->Controls->Add(this->labHex2);
			this->gboxTitleID->Controls->Add(this->tboxTitleIDLo);
			this->gboxTitleID->Controls->Add(this->labTitleIDLo);
			this->gboxTitleID->Controls->Add(this->labTitleIDHi);
			this->gboxTitleID->Controls->Add(this->tboxTitleIDHi);
			this->gboxTitleID->Controls->Add(this->tboxAppTypeOther);
			this->gboxTitleID->Controls->Add(this->labAppTypeOther);
			resources->ApplyResources(this->gboxTitleID, L"gboxTitleID");
			this->gboxTitleID->Name = L"gboxTitleID";
			this->gboxTitleID->TabStop = false;
			// 
			// labAppType
			// 
			resources->ApplyResources(this->labAppType, L"labAppType");
			this->labAppType->Name = L"labAppType";
			// 
			// tboxAppType
			// 
			resources->ApplyResources(this->tboxAppType, L"tboxAppType");
			this->tboxAppType->Name = L"tboxAppType";
			this->tboxAppType->ReadOnly = true;
			// 
			// labHex2
			// 
			resources->ApplyResources(this->labHex2, L"labHex2");
			this->labHex2->Name = L"labHex2";
			// 
			// tboxTitleIDLo
			// 
			resources->ApplyResources(this->tboxTitleIDLo, L"tboxTitleIDLo");
			this->tboxTitleIDLo->Name = L"tboxTitleIDLo";
			this->tboxTitleIDLo->ReadOnly = true;
			// 
			// labTitleIDLo
			// 
			resources->ApplyResources(this->labTitleIDLo, L"labTitleIDLo");
			this->labTitleIDLo->Name = L"labTitleIDLo";
			// 
			// labTitleIDHi
			// 
			resources->ApplyResources(this->labTitleIDHi, L"labTitleIDHi");
			this->labTitleIDHi->Name = L"labTitleIDHi";
			// 
			// tboxTitleIDHi
			// 
			resources->ApplyResources(this->tboxTitleIDHi, L"tboxTitleIDHi");
			this->tboxTitleIDHi->Name = L"tboxTitleIDHi";
			this->tboxTitleIDHi->ReadOnly = true;
			// 
			// tboxAppTypeOther
			// 
			resources->ApplyResources(this->tboxAppTypeOther, L"tboxAppTypeOther");
			this->tboxAppTypeOther->Name = L"tboxAppTypeOther";
			this->tboxAppTypeOther->ReadOnly = true;
			// 
			// labAppTypeOther
			// 
			resources->ApplyResources(this->labAppTypeOther, L"labAppTypeOther");
			this->labAppTypeOther->Name = L"labAppTypeOther";
			// 
			// labMedia
			// 
			resources->ApplyResources(this->labMedia, L"labMedia");
			this->labMedia->Name = L"labMedia";
			// 
			// tboxMedia
			// 
			resources->ApplyResources(this->tboxMedia, L"tboxMedia");
			this->tboxMedia->Name = L"tboxMedia";
			this->tboxMedia->ReadOnly = true;
			// 
			// labCaptionEx
			// 
			resources->ApplyResources(this->labCaptionEx, L"labCaptionEx");
			this->labCaptionEx->Name = L"labCaptionEx";
			// 
			// tboxCaptionEx
			// 
			resources->ApplyResources(this->tboxCaptionEx, L"tboxCaptionEx");
			this->tboxCaptionEx->Name = L"tboxCaptionEx";
			this->tboxCaptionEx->ReadOnly = true;
			// 
			// gboxProd
			// 
			this->gboxProd->Controls->Add(this->combBackup);
			this->gboxProd->Controls->Add(this->labBackup);
			this->gboxProd->Controls->Add(this->tboxBackupOther);
			resources->ApplyResources(this->gboxProd, L"gboxProd");
			this->gboxProd->Name = L"gboxProd";
			this->gboxProd->TabStop = false;
			// 
			// menuStripAbove
			// 
			this->menuStripAbove->Items->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(3) {this->stripFile, this->stripMaster, 
				this->stripLang});
			resources->ApplyResources(this->menuStripAbove, L"menuStripAbove");
			this->menuStripAbove->Name = L"menuStripAbove";
			// 
			// stripFile
			// 
			this->stripFile->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(4) {this->stripItemOpenRom, 
				this->stripItemSepFile1, this->stripItemSaveTemp, this->stripItemLoadTemp});
			this->stripFile->Name = L"stripFile";
			resources->ApplyResources(this->stripFile, L"stripFile");
			// 
			// stripItemOpenRom
			// 
			this->stripItemOpenRom->Name = L"stripItemOpenRom";
			resources->ApplyResources(this->stripItemOpenRom, L"stripItemOpenRom");
			this->stripItemOpenRom->Click += gcnew System::EventHandler(this, &Form1::stripItemOpenRom_Click);
			// 
			// stripItemSepFile1
			// 
			this->stripItemSepFile1->Name = L"stripItemSepFile1";
			resources->ApplyResources(this->stripItemSepFile1, L"stripItemSepFile1");
			// 
			// stripItemSaveTemp
			// 
			this->stripItemSaveTemp->Name = L"stripItemSaveTemp";
			resources->ApplyResources(this->stripItemSaveTemp, L"stripItemSaveTemp");
			this->stripItemSaveTemp->Click += gcnew System::EventHandler(this, &Form1::stripItemSaveTemp_Click);
			// 
			// stripItemLoadTemp
			// 
			this->stripItemLoadTemp->Name = L"stripItemLoadTemp";
			resources->ApplyResources(this->stripItemLoadTemp, L"stripItemLoadTemp");
			this->stripItemLoadTemp->Click += gcnew System::EventHandler(this, &Form1::stripItemLoadTemp_Click);
			// 
			// stripMaster
			// 
			this->stripMaster->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(5) {this->stripItemSheet, 
				this->stripItemSepMaster1, this->stripItemMasterRom, this->stripItemMiddlewareXml, this->stripItemMiddlewareHtml});
			this->stripMaster->Name = L"stripMaster";
			resources->ApplyResources(this->stripMaster, L"stripMaster");
			// 
			// stripItemSheet
			// 
			this->stripItemSheet->Name = L"stripItemSheet";
			resources->ApplyResources(this->stripItemSheet, L"stripItemSheet");
			this->stripItemSheet->Click += gcnew System::EventHandler(this, &Form1::stripItemSheet_Click);
			// 
			// stripItemSepMaster1
			// 
			this->stripItemSepMaster1->Name = L"stripItemSepMaster1";
			resources->ApplyResources(this->stripItemSepMaster1, L"stripItemSepMaster1");
			// 
			// stripItemMasterRom
			// 
			this->stripItemMasterRom->Name = L"stripItemMasterRom";
			resources->ApplyResources(this->stripItemMasterRom, L"stripItemMasterRom");
			this->stripItemMasterRom->Click += gcnew System::EventHandler(this, &Form1::stripItemMasterRom_Click);
			// 
			// stripItemMiddlewareXml
			// 
			this->stripItemMiddlewareXml->Name = L"stripItemMiddlewareXml";
			resources->ApplyResources(this->stripItemMiddlewareXml, L"stripItemMiddlewareXml");
			this->stripItemMiddlewareXml->Click += gcnew System::EventHandler(this, &Form1::stripItemMiddlewareXml_Click);
			// 
			// stripItemMiddlewareHtml
			// 
			this->stripItemMiddlewareHtml->Name = L"stripItemMiddlewareHtml";
			resources->ApplyResources(this->stripItemMiddlewareHtml, L"stripItemMiddlewareHtml");
			this->stripItemMiddlewareHtml->Click += gcnew System::EventHandler(this, &Form1::stripItemMiddlewareHtml_Click);
			// 
			// stripLang
			// 
			this->stripLang->DropDownItems->AddRange(gcnew cli::array< System::Windows::Forms::ToolStripItem^  >(2) {this->stripItemEnglish, 
				this->stripItemJapanese});
			this->stripLang->Name = L"stripLang";
			resources->ApplyResources(this->stripLang, L"stripLang");
			// 
			// stripItemEnglish
			// 
			this->stripItemEnglish->Name = L"stripItemEnglish";
			resources->ApplyResources(this->stripItemEnglish, L"stripItemEnglish");
			this->stripItemEnglish->Click += gcnew System::EventHandler(this, &Form1::stripItemEnglish_Click);
			// 
			// stripItemJapanese
			// 
			this->stripItemJapanese->Checked = true;
			this->stripItemJapanese->CheckState = System::Windows::Forms::CheckState::Checked;
			this->stripItemJapanese->Name = L"stripItemJapanese";
			resources->ApplyResources(this->stripItemJapanese, L"stripItemJapanese");
			this->stripItemJapanese->Click += gcnew System::EventHandler(this, &Form1::stripItemJapanese_Click);
			// 
			// tabMain
			// 
			this->tabMain->Controls->Add(this->tabRomInfo);
			this->tabMain->Controls->Add(this->tabTWLInfo);
			this->tabMain->Controls->Add(this->tabNandSizeInfo);
			this->tabMain->Controls->Add(this->tabRomEditInfo);
			this->tabMain->Controls->Add(this->tabSubmitInfo);
			this->tabMain->Controls->Add(this->tabCompanyInfo);
			this->tabMain->Controls->Add(this->tabErrorInfo);
			resources->ApplyResources(this->tabMain, L"tabMain");
			this->tabMain->Name = L"tabMain";
			this->tabMain->SelectedIndex = 0;
			this->tabMain->SelectedIndexChanged += gcnew System::EventHandler(this, &Form1::tabMain_SelectedIndexChanged);
			// 
			// tabRomInfo
			// 
			this->tabRomInfo->Controls->Add(this->gridLibrary);
			this->tabRomInfo->Controls->Add(this->tboxGuideRomInfo);
			this->tabRomInfo->Controls->Add(this->gboxSrl);
			this->tabRomInfo->Controls->Add(this->labLib);
			this->tabRomInfo->Controls->Add(this->labCaptionEx);
			this->tabRomInfo->Controls->Add(this->tboxCaptionEx);
			this->tabRomInfo->Controls->Add(this->gboxCRC);
			this->tabRomInfo->Controls->Add(this->tboxSDK);
			this->tabRomInfo->Controls->Add(this->labSDK);
			resources->ApplyResources(this->tabRomInfo, L"tabRomInfo");
			this->tabRomInfo->Name = L"tabRomInfo";
			this->tabRomInfo->UseVisualStyleBackColor = true;
			// 
			// gridLibrary
			// 
			this->gridLibrary->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCellsExceptHeaders;
			this->gridLibrary->BackgroundColor = System::Drawing::SystemColors::Control;
			this->gridLibrary->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->gridLibrary->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(2) {this->colLibPublisher, 
				this->colLibName});
			dataGridViewCellStyle1->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle1->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle1->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle1->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle1->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle1->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle1->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->gridLibrary->DefaultCellStyle = dataGridViewCellStyle1;
			resources->ApplyResources(this->gridLibrary, L"gridLibrary");
			this->gridLibrary->Name = L"gridLibrary";
			this->gridLibrary->ReadOnly = true;
			this->gridLibrary->RowHeadersVisible = false;
			this->gridLibrary->RowTemplate->Height = 21;
			// 
			// colLibPublisher
			// 
			resources->ApplyResources(this->colLibPublisher, L"colLibPublisher");
			this->colLibPublisher->Name = L"colLibPublisher";
			this->colLibPublisher->ReadOnly = true;
			// 
			// colLibName
			// 
			this->colLibName->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			resources->ApplyResources(this->colLibName, L"colLibName");
			this->colLibName->Name = L"colLibName";
			this->colLibName->ReadOnly = true;
			// 
			// tboxGuideRomInfo
			// 
			this->tboxGuideRomInfo->BackColor = System::Drawing::SystemColors::Info;
			resources->ApplyResources(this->tboxGuideRomInfo, L"tboxGuideRomInfo");
			this->tboxGuideRomInfo->Name = L"tboxGuideRomInfo";
			this->tboxGuideRomInfo->ReadOnly = true;
			// 
			// tabTWLInfo
			// 
			this->tabTWLInfo->Controls->Add(this->gboxLaunch);
			this->tabTWLInfo->Controls->Add(this->tboxGuideTWLInfo);
			this->tabTWLInfo->Controls->Add(this->labHex3);
			this->tabTWLInfo->Controls->Add(this->gboxExFlags);
			this->tabTWLInfo->Controls->Add(this->gboxShared2Size);
			this->tabTWLInfo->Controls->Add(this->gboxTWLExInfo);
			this->tabTWLInfo->Controls->Add(this->gboxAccess);
			this->tabTWLInfo->Controls->Add(this->gboxTitleID);
			this->tabTWLInfo->Controls->Add(this->tboxNormalRomOffset);
			this->tabTWLInfo->Controls->Add(this->labNormalRomOffset);
			resources->ApplyResources(this->tabTWLInfo, L"tabTWLInfo");
			this->tabTWLInfo->Name = L"tabTWLInfo";
			this->tabTWLInfo->UseVisualStyleBackColor = true;
			// 
			// gboxLaunch
			// 
			this->gboxLaunch->Controls->Add(this->labConnectIcon);
			this->gboxLaunch->Controls->Add(this->tboxConnectIcon);
			this->gboxLaunch->Controls->Add(this->cboxIsEULA);
			resources->ApplyResources(this->gboxLaunch, L"gboxLaunch");
			this->gboxLaunch->Name = L"gboxLaunch";
			this->gboxLaunch->TabStop = false;
			// 
			// labConnectIcon
			// 
			resources->ApplyResources(this->labConnectIcon, L"labConnectIcon");
			this->labConnectIcon->Name = L"labConnectIcon";
			// 
			// tboxConnectIcon
			// 
			resources->ApplyResources(this->tboxConnectIcon, L"tboxConnectIcon");
			this->tboxConnectIcon->Name = L"tboxConnectIcon";
			this->tboxConnectIcon->ReadOnly = true;
			// 
			// tboxGuideTWLInfo
			// 
			this->tboxGuideTWLInfo->BackColor = System::Drawing::SystemColors::Info;
			resources->ApplyResources(this->tboxGuideTWLInfo, L"tboxGuideTWLInfo");
			this->tboxGuideTWLInfo->Name = L"tboxGuideTWLInfo";
			this->tboxGuideTWLInfo->ReadOnly = true;
			// 
			// gboxExFlags
			// 
			this->gboxExFlags->Controls->Add(this->cboxIsNormalJump);
			this->gboxExFlags->Controls->Add(this->cboxIsTmpJump);
			this->gboxExFlags->Controls->Add(this->cboxIsSubBanner);
			resources->ApplyResources(this->gboxExFlags, L"gboxExFlags");
			this->gboxExFlags->Name = L"gboxExFlags";
			this->gboxExFlags->TabStop = false;
			// 
			// tabNandSizeInfo
			// 
			this->tabNandSizeInfo->Controls->Add(this->tboxGuideNandSizeInfo);
			this->tabNandSizeInfo->Controls->Add(this->labMedia);
			this->tabNandSizeInfo->Controls->Add(this->tboxMedia);
			this->tabNandSizeInfo->Controls->Add(this->gboxNandSize);
			resources->ApplyResources(this->tabNandSizeInfo, L"tabNandSizeInfo");
			this->tabNandSizeInfo->Name = L"tabNandSizeInfo";
			this->tabNandSizeInfo->UseVisualStyleBackColor = true;
			// 
			// tboxGuideNandSizeInfo
			// 
			this->tboxGuideNandSizeInfo->BackColor = System::Drawing::SystemColors::Info;
			resources->ApplyResources(this->tboxGuideNandSizeInfo, L"tboxGuideNandSizeInfo");
			this->tboxGuideNandSizeInfo->Name = L"tboxGuideNandSizeInfo";
			this->tboxGuideNandSizeInfo->ReadOnly = true;
			// 
			// gboxNandSize
			// 
			this->gboxNandSize->Controls->Add(this->tboxSrlSize);
			this->gboxNandSize->Controls->Add(this->labShopBlockUnit);
			this->gboxNandSize->Controls->Add(this->labArrow5);
			this->gboxNandSize->Controls->Add(this->labArrow4);
			this->gboxNandSize->Controls->Add(this->labArrow3);
			this->gboxNandSize->Controls->Add(this->labArrow2);
			this->gboxNandSize->Controls->Add(this->labArrow1);
			this->gboxNandSize->Controls->Add(this->tboxShopBlock);
			this->gboxNandSize->Controls->Add(this->tboxSumSize);
			this->gboxNandSize->Controls->Add(this->tboxTmdSizeFS);
			this->gboxNandSize->Controls->Add(this->tboxSubBannerSizeFS);
			this->gboxNandSize->Controls->Add(this->tboxPrivateSizeFS);
			this->gboxNandSize->Controls->Add(this->tboxPublicSizeFS);
			this->gboxNandSize->Controls->Add(this->tboxSrlSizeFS);
			this->gboxNandSize->Controls->Add(this->tboxTmdSize);
			this->gboxNandSize->Controls->Add(this->tboxSubBannerSize);
			this->gboxNandSize->Controls->Add(this->labNandSize);
			this->gboxNandSize->Controls->Add(this->labShopBlock2);
			this->gboxNandSize->Controls->Add(this->labNandSizeFS);
			this->gboxNandSize->Controls->Add(this->tboxPrivateSize);
			this->gboxNandSize->Controls->Add(this->labShopBlock);
			this->gboxNandSize->Controls->Add(this->labSrlSize);
			this->gboxNandSize->Controls->Add(this->labPrivateSize);
			this->gboxNandSize->Controls->Add(this->labSumSize2);
			this->gboxNandSize->Controls->Add(this->tboxPublicSize);
			this->gboxNandSize->Controls->Add(this->labPublicSize);
			this->gboxNandSize->Controls->Add(this->labSumSize);
			this->gboxNandSize->Controls->Add(this->labSubBannerSize);
			this->gboxNandSize->Controls->Add(this->labTmdSize);
			resources->ApplyResources(this->gboxNandSize, L"gboxNandSize");
			this->gboxNandSize->Name = L"gboxNandSize";
			this->gboxNandSize->TabStop = false;
			// 
			// tboxSrlSize
			// 
			resources->ApplyResources(this->tboxSrlSize, L"tboxSrlSize");
			this->tboxSrlSize->Name = L"tboxSrlSize";
			this->tboxSrlSize->ReadOnly = true;
			// 
			// labShopBlockUnit
			// 
			resources->ApplyResources(this->labShopBlockUnit, L"labShopBlockUnit");
			this->labShopBlockUnit->Name = L"labShopBlockUnit";
			// 
			// labArrow5
			// 
			resources->ApplyResources(this->labArrow5, L"labArrow5");
			this->labArrow5->Name = L"labArrow5";
			// 
			// labArrow4
			// 
			resources->ApplyResources(this->labArrow4, L"labArrow4");
			this->labArrow4->Name = L"labArrow4";
			// 
			// labArrow3
			// 
			resources->ApplyResources(this->labArrow3, L"labArrow3");
			this->labArrow3->Name = L"labArrow3";
			// 
			// labArrow2
			// 
			resources->ApplyResources(this->labArrow2, L"labArrow2");
			this->labArrow2->Name = L"labArrow2";
			// 
			// labArrow1
			// 
			resources->ApplyResources(this->labArrow1, L"labArrow1");
			this->labArrow1->Name = L"labArrow1";
			// 
			// tboxShopBlock
			// 
			resources->ApplyResources(this->tboxShopBlock, L"tboxShopBlock");
			this->tboxShopBlock->Name = L"tboxShopBlock";
			this->tboxShopBlock->ReadOnly = true;
			// 
			// tboxSumSize
			// 
			resources->ApplyResources(this->tboxSumSize, L"tboxSumSize");
			this->tboxSumSize->Name = L"tboxSumSize";
			this->tboxSumSize->ReadOnly = true;
			// 
			// tboxTmdSizeFS
			// 
			resources->ApplyResources(this->tboxTmdSizeFS, L"tboxTmdSizeFS");
			this->tboxTmdSizeFS->Name = L"tboxTmdSizeFS";
			this->tboxTmdSizeFS->ReadOnly = true;
			// 
			// tboxSubBannerSizeFS
			// 
			resources->ApplyResources(this->tboxSubBannerSizeFS, L"tboxSubBannerSizeFS");
			this->tboxSubBannerSizeFS->Name = L"tboxSubBannerSizeFS";
			this->tboxSubBannerSizeFS->ReadOnly = true;
			// 
			// tboxPrivateSizeFS
			// 
			resources->ApplyResources(this->tboxPrivateSizeFS, L"tboxPrivateSizeFS");
			this->tboxPrivateSizeFS->Name = L"tboxPrivateSizeFS";
			this->tboxPrivateSizeFS->ReadOnly = true;
			// 
			// tboxPublicSizeFS
			// 
			resources->ApplyResources(this->tboxPublicSizeFS, L"tboxPublicSizeFS");
			this->tboxPublicSizeFS->Name = L"tboxPublicSizeFS";
			this->tboxPublicSizeFS->ReadOnly = true;
			// 
			// tboxSrlSizeFS
			// 
			resources->ApplyResources(this->tboxSrlSizeFS, L"tboxSrlSizeFS");
			this->tboxSrlSizeFS->Name = L"tboxSrlSizeFS";
			this->tboxSrlSizeFS->ReadOnly = true;
			// 
			// tboxTmdSize
			// 
			resources->ApplyResources(this->tboxTmdSize, L"tboxTmdSize");
			this->tboxTmdSize->Name = L"tboxTmdSize";
			this->tboxTmdSize->ReadOnly = true;
			// 
			// tboxSubBannerSize
			// 
			resources->ApplyResources(this->tboxSubBannerSize, L"tboxSubBannerSize");
			this->tboxSubBannerSize->Name = L"tboxSubBannerSize";
			this->tboxSubBannerSize->ReadOnly = true;
			// 
			// labNandSize
			// 
			resources->ApplyResources(this->labNandSize, L"labNandSize");
			this->labNandSize->Name = L"labNandSize";
			// 
			// labShopBlock2
			// 
			resources->ApplyResources(this->labShopBlock2, L"labShopBlock2");
			this->labShopBlock2->Name = L"labShopBlock2";
			// 
			// labNandSizeFS
			// 
			resources->ApplyResources(this->labNandSizeFS, L"labNandSizeFS");
			this->labNandSizeFS->Name = L"labNandSizeFS";
			// 
			// labShopBlock
			// 
			resources->ApplyResources(this->labShopBlock, L"labShopBlock");
			this->labShopBlock->Name = L"labShopBlock";
			// 
			// labSrlSize
			// 
			resources->ApplyResources(this->labSrlSize, L"labSrlSize");
			this->labSrlSize->Name = L"labSrlSize";
			// 
			// labSumSize2
			// 
			resources->ApplyResources(this->labSumSize2, L"labSumSize2");
			this->labSumSize2->Name = L"labSumSize2";
			// 
			// labSumSize
			// 
			resources->ApplyResources(this->labSumSize, L"labSumSize");
			this->labSumSize->Name = L"labSumSize";
			// 
			// labSubBannerSize
			// 
			resources->ApplyResources(this->labSubBannerSize, L"labSubBannerSize");
			this->labSubBannerSize->Name = L"labSubBannerSize";
			// 
			// labTmdSize
			// 
			resources->ApplyResources(this->labTmdSize, L"labTmdSize");
			this->labTmdSize->Name = L"labTmdSize";
			// 
			// tabRomEditInfo
			// 
			this->tabRomEditInfo->Controls->Add(this->gboxOtherSpec);
			this->tabRomEditInfo->Controls->Add(this->butSetBack);
			this->tabRomEditInfo->Controls->Add(this->tboxGuideRomEditInfo);
			this->tabRomEditInfo->Controls->Add(this->gboxParental);
			resources->ApplyResources(this->tabRomEditInfo, L"tabRomEditInfo");
			this->tabRomEditInfo->Name = L"tabRomEditInfo";
			this->tabRomEditInfo->UseVisualStyleBackColor = true;
			// 
			// gboxOtherSpec
			// 
			this->gboxOtherSpec->Controls->Add(this->cboxIsUGC);
			this->gboxOtherSpec->Controls->Add(this->cboxIsPhotoEx);
			resources->ApplyResources(this->gboxOtherSpec, L"gboxOtherSpec");
			this->gboxOtherSpec->Name = L"gboxOtherSpec";
			this->gboxOtherSpec->TabStop = false;
			// 
			// cboxIsUGC
			// 
			resources->ApplyResources(this->cboxIsUGC, L"cboxIsUGC");
			this->cboxIsUGC->Name = L"cboxIsUGC";
			this->cboxIsUGC->UseVisualStyleBackColor = true;
			// 
			// cboxIsPhotoEx
			// 
			resources->ApplyResources(this->cboxIsPhotoEx, L"cboxIsPhotoEx");
			this->cboxIsPhotoEx->Name = L"cboxIsPhotoEx";
			this->cboxIsPhotoEx->UseVisualStyleBackColor = true;
			// 
			// butSetBack
			// 
			resources->ApplyResources(this->butSetBack, L"butSetBack");
			this->butSetBack->Name = L"butSetBack";
			this->butSetBack->UseVisualStyleBackColor = true;
			this->butSetBack->Click += gcnew System::EventHandler(this, &Form1::butSetBack_Click);
			// 
			// tboxGuideRomEditInfo
			// 
			this->tboxGuideRomEditInfo->BackColor = System::Drawing::SystemColors::Info;
			resources->ApplyResources(this->tboxGuideRomEditInfo, L"tboxGuideRomEditInfo");
			this->tboxGuideRomEditInfo->Name = L"tboxGuideRomEditInfo";
			this->tboxGuideRomEditInfo->ReadOnly = true;
			// 
			// gboxParental
			// 
			this->gboxParental->Controls->Add(this->cboxIsUnnecessaryRating);
			this->gboxParental->Controls->Add(this->labRegion);
			this->gboxParental->Controls->Add(this->combRegion);
			this->gboxParental->Controls->Add(this->labParentalRating);
			this->gboxParental->Controls->Add(this->labOFLC);
			this->gboxParental->Controls->Add(this->labPEGI_BBFC);
			this->gboxParental->Controls->Add(this->combPEGI_BBFC);
			this->gboxParental->Controls->Add(this->combCERO);
			this->gboxParental->Controls->Add(this->labCERO);
			this->gboxParental->Controls->Add(this->combOFLC);
			this->gboxParental->Controls->Add(this->labPEGI_PRT);
			this->gboxParental->Controls->Add(this->combPEGI_PRT);
			this->gboxParental->Controls->Add(this->combESRB);
			this->gboxParental->Controls->Add(this->labESRB);
			this->gboxParental->Controls->Add(this->labPEGI);
			this->gboxParental->Controls->Add(this->combPEGI);
			this->gboxParental->Controls->Add(this->combUSK);
			this->gboxParental->Controls->Add(this->labUSK);
			resources->ApplyResources(this->gboxParental, L"gboxParental");
			this->gboxParental->Name = L"gboxParental";
			this->gboxParental->TabStop = false;
			// 
			// cboxIsUnnecessaryRating
			// 
			resources->ApplyResources(this->cboxIsUnnecessaryRating, L"cboxIsUnnecessaryRating");
			this->cboxIsUnnecessaryRating->Name = L"cboxIsUnnecessaryRating";
			this->cboxIsUnnecessaryRating->UseVisualStyleBackColor = true;
			this->cboxIsUnnecessaryRating->CheckedChanged += gcnew System::EventHandler(this, &Form1::cboxIsUnnecessaryRating_CheckedChanged);
			// 
			// tabSubmitInfo
			// 
			this->tabSubmitInfo->Controls->Add(this->labProductNameLimit);
			this->tabSubmitInfo->Controls->Add(this->tboxGuideSubmitInfo);
			this->tabSubmitInfo->Controls->Add(this->gboxForeign);
			this->tabSubmitInfo->Controls->Add(this->tboxCaption);
			this->tabSubmitInfo->Controls->Add(this->labSubmitVer);
			this->tabSubmitInfo->Controls->Add(this->labCaption);
			this->tabSubmitInfo->Controls->Add(this->tboxProductCode2);
			this->tabSubmitInfo->Controls->Add(this->labCapSubmitVer);
			this->tabSubmitInfo->Controls->Add(this->gboxUsage);
			this->tabSubmitInfo->Controls->Add(this->numSubmitVersion);
			this->tabSubmitInfo->Controls->Add(this->gboxProd);
			this->tabSubmitInfo->Controls->Add(this->gboxSubmitWay);
			this->tabSubmitInfo->Controls->Add(this->tboxProductCode1);
			this->tabSubmitInfo->Controls->Add(this->labSubmiteDate);
			this->tabSubmitInfo->Controls->Add(this->dateRelease);
			this->tabSubmitInfo->Controls->Add(this->tboxProductName);
			this->tabSubmitInfo->Controls->Add(this->labReleaseDate);
			this->tabSubmitInfo->Controls->Add(this->dateSubmit);
			this->tabSubmitInfo->Controls->Add(this->labProductCode2);
			this->tabSubmitInfo->Controls->Add(this->labProductCode);
			this->tabSubmitInfo->Controls->Add(this->labProductName);
			this->tabSubmitInfo->Controls->Add(this->labProductCode1);
			resources->ApplyResources(this->tabSubmitInfo, L"tabSubmitInfo");
			this->tabSubmitInfo->Name = L"tabSubmitInfo";
			this->tabSubmitInfo->UseVisualStyleBackColor = true;
			// 
			// labProductNameLimit
			// 
			resources->ApplyResources(this->labProductNameLimit, L"labProductNameLimit");
			this->labProductNameLimit->Name = L"labProductNameLimit";
			// 
			// tboxGuideSubmitInfo
			// 
			this->tboxGuideSubmitInfo->BackColor = System::Drawing::SystemColors::Info;
			resources->ApplyResources(this->tboxGuideSubmitInfo, L"tboxGuideSubmitInfo");
			this->tboxGuideSubmitInfo->Name = L"tboxGuideSubmitInfo";
			this->tboxGuideSubmitInfo->ReadOnly = true;
			// 
			// gboxForeign
			// 
			this->gboxForeign->Controls->Add(this->labProductNameLimitForeign);
			this->gboxForeign->Controls->Add(this->labMultiForeign2);
			this->gboxForeign->Controls->Add(this->cboxReleaseForeign);
			this->gboxForeign->Controls->Add(this->labMultiForeign1);
			this->gboxForeign->Controls->Add(this->tboxProductCode2Foreign1);
			this->gboxForeign->Controls->Add(this->tboxProductCode2Foreign3);
			this->gboxForeign->Controls->Add(this->labProductCodeForeign);
			this->gboxForeign->Controls->Add(this->tboxProductCode2Foreign2);
			this->gboxForeign->Controls->Add(this->tboxProductCode1Foreign);
			this->gboxForeign->Controls->Add(this->labProductCode2Foreign);
			this->gboxForeign->Controls->Add(this->labProductCode1Foreign);
			this->gboxForeign->Controls->Add(this->tboxProductNameForeign);
			this->gboxForeign->Controls->Add(this->labProductNameForeign);
			resources->ApplyResources(this->gboxForeign, L"gboxForeign");
			this->gboxForeign->Name = L"gboxForeign";
			this->gboxForeign->TabStop = false;
			// 
			// labProductNameLimitForeign
			// 
			resources->ApplyResources(this->labProductNameLimitForeign, L"labProductNameLimitForeign");
			this->labProductNameLimitForeign->Name = L"labProductNameLimitForeign";
			// 
			// labMultiForeign2
			// 
			resources->ApplyResources(this->labMultiForeign2, L"labMultiForeign2");
			this->labMultiForeign2->Name = L"labMultiForeign2";
			// 
			// tabCompanyInfo
			// 
			this->tabCompanyInfo->Controls->Add(this->tboxGuideCompanyInfo);
			this->tabCompanyInfo->Controls->Add(this->cboxIsInputPerson2);
			this->tabCompanyInfo->Controls->Add(this->gboxPerson1);
			this->tabCompanyInfo->Controls->Add(this->gboxPerson2);
			resources->ApplyResources(this->tabCompanyInfo, L"tabCompanyInfo");
			this->tabCompanyInfo->Name = L"tabCompanyInfo";
			this->tabCompanyInfo->UseVisualStyleBackColor = true;
			// 
			// tboxGuideCompanyInfo
			// 
			this->tboxGuideCompanyInfo->BackColor = System::Drawing::SystemColors::Info;
			resources->ApplyResources(this->tboxGuideCompanyInfo, L"tboxGuideCompanyInfo");
			this->tboxGuideCompanyInfo->Name = L"tboxGuideCompanyInfo";
			this->tboxGuideCompanyInfo->ReadOnly = true;
			// 
			// tabErrorInfo
			// 
			this->tabErrorInfo->Controls->Add(this->tboxGuideErrorInfo);
			this->tabErrorInfo->Controls->Add(this->gboxErrorTiming);
			this->tabErrorInfo->Controls->Add(this->labWarn);
			this->tabErrorInfo->Controls->Add(this->labError);
			this->tabErrorInfo->Controls->Add(this->gridWarn);
			this->tabErrorInfo->Controls->Add(this->gridError);
			resources->ApplyResources(this->tabErrorInfo, L"tabErrorInfo");
			this->tabErrorInfo->Name = L"tabErrorInfo";
			this->tabErrorInfo->UseVisualStyleBackColor = true;
			// 
			// tboxGuideErrorInfo
			// 
			this->tboxGuideErrorInfo->BackColor = System::Drawing::SystemColors::Info;
			resources->ApplyResources(this->tboxGuideErrorInfo, L"tboxGuideErrorInfo");
			this->tboxGuideErrorInfo->Name = L"tboxGuideErrorInfo";
			this->tboxGuideErrorInfo->ReadOnly = true;
			// 
			// gboxErrorTiming
			// 
			this->gboxErrorTiming->Controls->Add(this->rErrorCurrent);
			this->gboxErrorTiming->Controls->Add(this->rErrorReading);
			resources->ApplyResources(this->gboxErrorTiming, L"gboxErrorTiming");
			this->gboxErrorTiming->Name = L"gboxErrorTiming";
			this->gboxErrorTiming->TabStop = false;
			// 
			// rErrorCurrent
			// 
			resources->ApplyResources(this->rErrorCurrent, L"rErrorCurrent");
			this->rErrorCurrent->Name = L"rErrorCurrent";
			this->rErrorCurrent->TabStop = true;
			this->rErrorCurrent->UseVisualStyleBackColor = true;
			this->rErrorCurrent->CheckedChanged += gcnew System::EventHandler(this, &Form1::rErrorCurrent_CheckedChanged);
			// 
			// rErrorReading
			// 
			resources->ApplyResources(this->rErrorReading, L"rErrorReading");
			this->rErrorReading->Checked = true;
			this->rErrorReading->Name = L"rErrorReading";
			this->rErrorReading->TabStop = true;
			this->rErrorReading->UseVisualStyleBackColor = true;
			this->rErrorReading->CheckedChanged += gcnew System::EventHandler(this, &Form1::rErrorReading_CheckedChanged);
			// 
			// labWarn
			// 
			resources->ApplyResources(this->labWarn, L"labWarn");
			this->labWarn->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->labWarn->Name = L"labWarn";
			// 
			// labError
			// 
			resources->ApplyResources(this->labError, L"labError");
			this->labError->ForeColor = System::Drawing::SystemColors::ActiveCaption;
			this->labError->Name = L"labError";
			// 
			// gridWarn
			// 
			this->gridWarn->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCellsExceptHeaders;
			this->gridWarn->BackgroundColor = System::Drawing::SystemColors::Control;
			this->gridWarn->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->gridWarn->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {this->colWarnName, 
				this->colWarnBegin, this->colWarnEnd, this->colWarnCause});
			dataGridViewCellStyle2->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle2->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle2->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle2->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle2->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle2->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle2->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->gridWarn->DefaultCellStyle = dataGridViewCellStyle2;
			this->gridWarn->GridColor = System::Drawing::SystemColors::Control;
			resources->ApplyResources(this->gridWarn, L"gridWarn");
			this->gridWarn->Name = L"gridWarn";
			this->gridWarn->ReadOnly = true;
			this->gridWarn->RowHeadersVisible = false;
			this->gridWarn->RowTemplate->Height = 21;
			// 
			// colWarnName
			// 
			resources->ApplyResources(this->colWarnName, L"colWarnName");
			this->colWarnName->Name = L"colWarnName";
			this->colWarnName->ReadOnly = true;
			// 
			// colWarnBegin
			// 
			resources->ApplyResources(this->colWarnBegin, L"colWarnBegin");
			this->colWarnBegin->Name = L"colWarnBegin";
			this->colWarnBegin->ReadOnly = true;
			// 
			// colWarnEnd
			// 
			resources->ApplyResources(this->colWarnEnd, L"colWarnEnd");
			this->colWarnEnd->Name = L"colWarnEnd";
			this->colWarnEnd->ReadOnly = true;
			// 
			// colWarnCause
			// 
			this->colWarnCause->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			resources->ApplyResources(this->colWarnCause, L"colWarnCause");
			this->colWarnCause->Name = L"colWarnCause";
			this->colWarnCause->ReadOnly = true;
			// 
			// gridError
			// 
			dataGridViewCellStyle3->BackColor = System::Drawing::Color::White;
			this->gridError->AlternatingRowsDefaultCellStyle = dataGridViewCellStyle3;
			this->gridError->AutoSizeRowsMode = System::Windows::Forms::DataGridViewAutoSizeRowsMode::AllCellsExceptHeaders;
			this->gridError->BackgroundColor = System::Drawing::SystemColors::Control;
			this->gridError->ColumnHeadersHeightSizeMode = System::Windows::Forms::DataGridViewColumnHeadersHeightSizeMode::AutoSize;
			this->gridError->Columns->AddRange(gcnew cli::array< System::Windows::Forms::DataGridViewColumn^  >(4) {this->colErrorName, 
				this->colErrorBegin, this->colErrorEnd, this->colErrorCause});
			dataGridViewCellStyle4->Alignment = System::Windows::Forms::DataGridViewContentAlignment::MiddleLeft;
			dataGridViewCellStyle4->BackColor = System::Drawing::SystemColors::Window;
			dataGridViewCellStyle4->Font = (gcnew System::Drawing::Font(L"MS UI Gothic", 9, System::Drawing::FontStyle::Regular, System::Drawing::GraphicsUnit::Point, 
				static_cast<System::Byte>(128)));
			dataGridViewCellStyle4->ForeColor = System::Drawing::SystemColors::ControlText;
			dataGridViewCellStyle4->SelectionBackColor = System::Drawing::SystemColors::Highlight;
			dataGridViewCellStyle4->SelectionForeColor = System::Drawing::SystemColors::HighlightText;
			dataGridViewCellStyle4->WrapMode = System::Windows::Forms::DataGridViewTriState::True;
			this->gridError->DefaultCellStyle = dataGridViewCellStyle4;
			this->gridError->GridColor = System::Drawing::SystemColors::Control;
			resources->ApplyResources(this->gridError, L"gridError");
			this->gridError->Name = L"gridError";
			this->gridError->ReadOnly = true;
			this->gridError->RowHeadersVisible = false;
			this->gridError->RowTemplate->Height = 21;
			// 
			// colErrorName
			// 
			resources->ApplyResources(this->colErrorName, L"colErrorName");
			this->colErrorName->Name = L"colErrorName";
			this->colErrorName->ReadOnly = true;
			// 
			// colErrorBegin
			// 
			resources->ApplyResources(this->colErrorBegin, L"colErrorBegin");
			this->colErrorBegin->Name = L"colErrorBegin";
			this->colErrorBegin->ReadOnly = true;
			// 
			// colErrorEnd
			// 
			resources->ApplyResources(this->colErrorEnd, L"colErrorEnd");
			this->colErrorEnd->Name = L"colErrorEnd";
			this->colErrorEnd->ReadOnly = true;
			// 
			// colErrorCause
			// 
			this->colErrorCause->AutoSizeMode = System::Windows::Forms::DataGridViewAutoSizeColumnMode::Fill;
			resources->ApplyResources(this->colErrorCause, L"colErrorCause");
			this->colErrorCause->Name = L"colErrorCause";
			this->colErrorCause->ReadOnly = true;
			// 
			// labFile
			// 
			resources->ApplyResources(this->labFile, L"labFile");
			this->labFile->Name = L"labFile";
			// 
			// labAssemblyVersion
			// 
			resources->ApplyResources(this->labAssemblyVersion, L"labAssemblyVersion");
			this->labAssemblyVersion->ForeColor = System::Drawing::SystemColors::ControlText;
			this->labAssemblyVersion->Name = L"labAssemblyVersion";
			// 
			// Form1
			// 
			this->AllowDrop = true;
			resources->ApplyResources(this, L"$this");
			this->AutoScaleMode = System::Windows::Forms::AutoScaleMode::Font;
			this->Controls->Add(this->labAssemblyVersion);
			this->Controls->Add(this->labFile);
			this->Controls->Add(this->tabMain);
			this->Controls->Add(this->tboxFile);
			this->Controls->Add(this->menuStripAbove);
			this->MainMenuStrip = this->menuStripAbove;
			this->Name = L"Form1";
			this->DragDrop += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::Form1_DragDrop);
			this->DragEnter += gcnew System::Windows::Forms::DragEventHandler(this, &Form1::Form1_DragEnter);
			this->gboxSrl->ResumeLayout(false);
			this->gboxSrl->PerformLayout();
			this->gboxCRC->ResumeLayout(false);
			this->gboxCRC->PerformLayout();
			this->gboxPerson2->ResumeLayout(false);
			this->gboxPerson2->PerformLayout();
			this->gboxPerson1->ResumeLayout(false);
			this->gboxPerson1->PerformLayout();
			this->gboxUsage->ResumeLayout(false);
			this->gboxUsage->PerformLayout();
			this->gboxSubmitWay->ResumeLayout(false);
			this->gboxSubmitWay->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->numSubmitVersion))->EndInit();
			this->gboxShared2Size->ResumeLayout(false);
			this->gboxShared2Size->PerformLayout();
			this->gboxTWLExInfo->ResumeLayout(false);
			this->gboxTWLExInfo->PerformLayout();
			this->gboxAccess->ResumeLayout(false);
			this->gboxAccess->PerformLayout();
			this->gboxTitleID->ResumeLayout(false);
			this->gboxTitleID->PerformLayout();
			this->gboxProd->ResumeLayout(false);
			this->gboxProd->PerformLayout();
			this->menuStripAbove->ResumeLayout(false);
			this->menuStripAbove->PerformLayout();
			this->tabMain->ResumeLayout(false);
			this->tabRomInfo->ResumeLayout(false);
			this->tabRomInfo->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridLibrary))->EndInit();
			this->tabTWLInfo->ResumeLayout(false);
			this->tabTWLInfo->PerformLayout();
			this->gboxLaunch->ResumeLayout(false);
			this->gboxLaunch->PerformLayout();
			this->gboxExFlags->ResumeLayout(false);
			this->gboxExFlags->PerformLayout();
			this->tabNandSizeInfo->ResumeLayout(false);
			this->tabNandSizeInfo->PerformLayout();
			this->gboxNandSize->ResumeLayout(false);
			this->gboxNandSize->PerformLayout();
			this->tabRomEditInfo->ResumeLayout(false);
			this->tabRomEditInfo->PerformLayout();
			this->gboxOtherSpec->ResumeLayout(false);
			this->gboxOtherSpec->PerformLayout();
			this->gboxParental->ResumeLayout(false);
			this->gboxParental->PerformLayout();
			this->tabSubmitInfo->ResumeLayout(false);
			this->tabSubmitInfo->PerformLayout();
			this->gboxForeign->ResumeLayout(false);
			this->gboxForeign->PerformLayout();
			this->tabCompanyInfo->ResumeLayout(false);
			this->tabCompanyInfo->PerformLayout();
			this->tabErrorInfo->ResumeLayout(false);
			this->tabErrorInfo->PerformLayout();
			this->gboxErrorTiming->ResumeLayout(false);
			this->gboxErrorTiming->PerformLayout();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridWarn))->EndInit();
			(cli::safe_cast<System::ComponentModel::ISupportInitialize^  >(this->gridError))->EndInit();
			this->ResumeLayout(false);
			this->PerformLayout();

		}
#pragma endregion

	/////////////////////////////////////////////
	// 内部メソッド
	/////////////////////////////////////////////
	private:
		// ----------------------------------------------
		// 初期処理 / 終了処理
		// ----------------------------------------------

		void construct(void);
		void destruct(void);
		void handleArgs(void);	// コマンドライン引数処理

	private:
		// ----------------------------------------------
		// 設定ファイルの読み込み
		// ----------------------------------------------

		void loadInit(void);		// 設定ファイル
		void loadAppendInit(void);	// 追加設定ファイル(内部向け)

	private:
		// ----------------------------------------------
		// ROMファイルのR/W
		// ----------------------------------------------

		// ROMファイルの読み込み (TAD/SRL読み込みをラップ)
		System::Boolean loadRom( System::String ^infile );

		// ROMファイルの書き出し (SRL書き出しをラップ)
		System::Boolean saveRom( System::String ^outname );

		// SRLの読み込み
		System::Boolean loadSrl( System::String ^srlfile );

		// SRLの書き出しと再読み込み
		System::Boolean saveSrl( System::String ^infile, System::String ^outfile );

		// SRLの書き出しのみ @ret 成否
		System::Boolean saveSrlCore( System::String ^infile, System::String ^outfile );

		// tadの読み込み
		System::Boolean loadTad( System::String ^tadfile );

		// 提出ファイル名をゲームコードなどから決定
		System::String^ getSubmitFilePrefix(void)
		{
			System::Byte romver = this->hSrl->RomVersion & 0x0F;		// 下位1桁
			System::Byte subver = System::Decimal::ToByte(this->numSubmitVersion->Value) & 0x0F;

			System::String ^prefix = "T" + this->hSrl->hGameCode + romver.ToString("X") + subver.ToString("X");
			if( this->isPreliminary() == true )
			{
				prefix += "E";
			}
			return prefix;
		}

	private:
		// ----------------------------------------------
		// ミドルウェアリストの書き出し
		// ----------------------------------------------

		// XML形式のリストを作成
		System::Void makeMiddlewareListXml(System::Xml::XmlDocument^ doc);

		// XML形式で書き出し
		System::Boolean saveMiddlewareListXml( System::String ^filename );

		// HTML形式で書き出し(XML->HTML変換)
		System::Boolean saveMiddlewareListHtml( System::String ^filename );

		// XSL埋め込みXML形式で書き出し
		System::Boolean saveMiddlewareListXmlEmbeddedXsl( System::String ^filename );

	private:
		// ----------------------------------------------
		// 一時ファイルの取り扱い
		// ----------------------------------------------

		// 一時保存
		System::Void saveTmp( System::String ^filename );

		// 一時ファイルを読み出す
		void loadTmp( System::String ^filename );

		// 一時保存情報をフォーム情報に変換
		System::Boolean parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::ComboBox ^comb );
		System::Boolean parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::NumericUpDown ^num );
		System::Boolean parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, 
								  cli::array<System::Windows::Forms::RadioButton^>^rbuts, cli::array<System::String ^>^textCands );
		System::Boolean parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::CheckBox ^cbox );
		System::Boolean parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::TextBox ^tbox );
		System::Boolean parseTmp( System::Xml::XmlElement ^root, System::String ^xpath, System::Windows::Forms::DateTimePicker ^date );

	private:
		// ----------------------------------------------
		// フォームの設定変更
		// ----------------------------------------------

		// 非表示項目の表示切替
		void changeVisibleForms( System::Boolean visible )
		{
			this->gboxShared2Size->Visible  = visible;
			this->cboxIsShared2->Visible    = visible;
			this->tboxShared2Size0->Visible = visible;
			this->labShared2Size0->Visible  = visible;
			this->tboxShared2Size1->Visible = visible;
			this->labShared2Size1->Visible  = visible;
			this->tboxShared2Size2->Visible = visible;
			this->labShared2Size2->Visible  = visible;
			this->tboxShared2Size3->Visible = visible;
			this->labShared2Size3->Visible  = visible;
			this->tboxShared2Size4->Visible = visible;
			this->labShared2Size4->Visible  = visible;
			this->tboxShared2Size5->Visible = visible;
			this->labShared2Size5->Visible  = visible;

			this->tboxIsGameCardOn->Visible = visible;
			this->labIsGameCardOn->Visible  = visible;
			this->labIsGameCardOn2->Visible = visible;
			this->labNormalRomOffset->Visible  = visible;
			this->tboxNormalRomOffset->Visible = visible;
			this->labHex3->Visible = visible;
		}
		// 非表示項目の有無でサイズが変わるグループボックス
		void changeVisibleGboxExFlags( System::Boolean visible )
		{
			// 設定ファイルで visible を指定されたら追加設定ファイルがどうであろうと visible
			if( !this->IsVisibleGboxExFlags )
			{
				this->IsVisibleGboxExFlags = visible;
				this->cboxIsNormalJump->Visible = visible;
				if( visible )
				{
					this->gboxExFlags->Size = this->SizeGboxExFlags;
				}
				else
				{
					System::Drawing::Size size = this->SizeGboxExFlags;
					size.Height -= 15;
					this->gboxExFlags->Size = size;
				}
			}
		}

		// 設定/選択可能なフォームをすべて disable にする
		void readOnly( void )
		{
			this->cboxIsUGC->Enabled       = false;
			this->cboxIsPhotoEx->Enabled   = false;

			this->butSetBack->Enabled = false;
			this->combRegion->Enabled = false;
			this->combCERO->Enabled = false;
			this->combESRB->Enabled = false;
			this->combUSK->Enabled = false;
			this->combPEGI->Enabled = false;
			this->combPEGI_PRT->Enabled = false;
			this->combPEGI_BBFC->Enabled = false;
			this->combOFLC->Enabled = false;
		}

		// バージョン情報を取得
		System::String^ getVersion( void )
		{
			System::Reflection::Assembly ^ass = System::Reflection::Assembly::GetEntryAssembly();
			System::Version ^ver =  ass->GetName()->Version;
			return ( ver->Major.ToString() + "." + ver->Minor.ToString() );
		}

		// SRLに登録されないROM仕様のフォーム入力を
		// 新規読み込みのときにはクリアして
		// 再読み込みのときには前の状態に戻す
		void clearOtherForms(void)
		{
			this->cboxIsUGC->Checked     = false;
			this->cboxIsPhotoEx->Checked = false;
			this->IsCheckedUGC     = false;
			this->IsCheckedPhotoEx = false;
		}
		void saveOtherForms(void)
		{
			this->IsCheckedUGC     = this->cboxIsUGC->Checked;
			this->IsCheckedPhotoEx = this->cboxIsPhotoEx->Checked;
		}
		void loadOtherForms(void)
		{
			this->cboxIsUGC->Checked     = this->IsCheckedUGC;
			this->cboxIsPhotoEx->Checked = this->IsCheckedPhotoEx;
		}

		// バックアップメモリの表示をNANDアプリのときに「なし」にする
		void maskBackupMemoryForms(void)
		{
			if( this->hSrl->IsMediaNand )
			{
				this->combBackup->SelectedIndex = this->combBackup->Items->Count - 2;
				this->combBackup->Enabled = false;
			}
			else
			{
				this->combBackup->Enabled = true;
			}
		}

	private:
		// ----------------------------------------------
		// 固定ファイル名の取得
		// ----------------------------------------------

		// バイナリがあるディレクトリ
		System::String^ getBinDir(void)
		{
			System::String ^dir = System::IO::Path::GetDirectoryName( System::Reflection::Assembly::GetEntryAssembly()->Location );
			if( !dir->EndsWith("\\") )
			{
				dir = dir + "\\";
			}
			return dir;
		}
		// リソースファイルがあるディレクトリ
		System::String^ getResDir(void)
		{
			System::String ^dir = System::IO::Directory::GetParent( this->getBinDir() )->Parent->FullName;
			if( !dir->EndsWith("\\") )
			{
				dir = dir + "\\";
			}
			dir = dir + "resource\\";
			return dir;
		}
		// 設定ファイル
		System::String^ getIniFile(void)
		{
			return (this->getResDir() + "ini.xml");
		}
		System::String^ getAppendIniFile(void)
		{
			return (this->getResDir() + "append_ini.xml");
		}
		// 提出確認書テンプレート
		System::String^ getSheetTemplateFile(void)
		{
			return (this->getResDir() + "sheet_templete.xml");
		}
		// ミドルウェアリスト作成用のXSL
		System::String^ getMiddlewareListStyleFile(void)
		{
			return (this->getResDir() + "middleware.xsl");
		}
		System::String^ getMiddlewareListStyleFileEmbedded(void)
		{
			return (this->getResDir() + "middleware_e.xsl");
		}
		// ミドルウェアリスト作成時の一時ファイル
		System::String^ getMiddlewareListTmpFile(void)
		{
			return (this->getBinDir() + "middleware-tmp.xml");
		}
		// tadの読み込みで生成する一時SRLファイル
		System::String^ getSplitTadTmpFile(void)
		{
			return (this->getBinDir() + "tmp.srl");
		}

	private:
		// ----------------------------------------------
		// フォームとSRL内情報を矛盾なく一致させる
		// ----------------------------------------------

		// ROM情報をフォームから取得してSRLクラスのプロパティに反映させる
		// (ROMヘッダへの反映やCRCと署名の再計算をしない)
		void setSrlProperties(void);

		// SRLのROM情報をフォームに反映させる(ファイルが読み込まれていることが前提)
		void setSrlForms(void);

		// ROM情報のうちテキストボックスを更新する(言語切り替えで表示を変えたいので独立させる)
		void setSrlFormsTextBox();

		// フォームの入力をチェックする
		System::Boolean checkSrlForms(void);

		// 事前版かどうか確認する
		bool isPreliminary()
		{
			return this->cboxRemasterVerE->Checked;
		}

	private:
		// ---------------------------------------------------------------------
		// リージョン設定は複雑なので別に切り出す
		// ---------------------------------------------------------------------
		
		// フォーム入力をSRLに反映させる
		void setRegionSrlPropaties(void);

		// SRL情報をフォームに反映させる
		void setRegionForms(void);

	private:
		// ---------------------------------------------------------------------
		// レーティング設定は複雑なので別に切り出す
		// ---------------------------------------------------------------------

		// フォーム入力をSRLに反映させる
		void setRatingSrlProperties(void);

		// SRL情報をフォームに反映させる
		void setRatingForms(void);

		// リージョン情報からペアレンタルコントロールの編集可能団体をマスクする
		void maskRatingForms(void);

		// フォーム入力が正しいか書き込み前チェック
		void checkRatingForms( System::Boolean inRegion, 
							   System::Windows::Forms::ComboBox ^comb, System::String ^msg );

		// クリアする
		void clearRating( System::Windows::Forms::ComboBox ^comb );

		// 編集できるようにする
		void enableRating( System::Windows::Forms::ComboBox ^comb, 
						   System::Windows::Forms::Label    ^lab1, 
						   System::Windows::Forms::Label    ^lab2 );

		// 編集できなくする
		void disableRating( System::Windows::Forms::ComboBox ^comb, 
						    System::Windows::Forms::Label    ^lab1, 
						    System::Windows::Forms::Label    ^lab2 );

		// 全団体を「レーティング表示不要」の設定/解除をする
		void changeUnnecessaryRatingForms( System::Boolean bInitial );

		// 「レーティング表示不要」と表示して編集できなくする
		void unnecessaryRating( System::Windows::Forms::ComboBox ^comb );

		// 「レーティング表示不要」表示を消して通常の設定に戻す
		void necessaryRating( System::Windows::Forms::ComboBox ^comb, System::Boolean bInitial );

		// ----------------------------------------------
		// フォームのチェック
		// ----------------------------------------------

		// テキスト入力がされているかチェック
		System::Boolean checkTextForm( System::String ^formtext, 
									   System::String ^labelJ, System::String ^labelE, System::Boolean affectRom );

		// 数値入力が正常かどうかチェック
		System::Boolean checkNumRange( 
			System::Int32 val, System::Int32 min, System::Int32 max, 
			System::String ^labelJ, System::String ^labelE, System::Boolean affectRom );

		System::Boolean checkNumRange( System::String ^strval, System::Int32 min, System::Int32 max, 
									   System::String ^labelJ, System::String ^labelE, System::Boolean affectRom );
		// コンボボックスをチェック
		System::Boolean checkBoxIndex( System::Windows::Forms::ComboBox ^box, 
			                           System::String ^labelJ, System::String ^labelE, System::Boolean affectRom );

		// -----------------------------------------------------------------
		// 提出情報(SRLに影響しない箇所のみ)とフォーム間のデータのやりとり
		// -----------------------------------------------------------------

		// 提出確認書にフォームを反映
		void setDeliverableProperties(void);

		// 提出確認書に反映するレーティング(1団体)のフォームを返す
		System::String^ setDeliverableRatingOgnProperties( System::Windows::Forms::ComboBox ^box );

		// 提出情報のフォームチェック
		System::Boolean checkDeliverableForms(void);

		// ----------------------------------------------
		// ダイアログ
		// ----------------------------------------------

		void sucMsg( System::String ^msgJ, System::String ^msgE )
		{
			if( this->isJapanese() )
				MessageBox::Show( msgJ, "Information", MessageBoxButtons::OK, MessageBoxIcon::None );
			else
				MessageBox::Show( msgE, "Information", MessageBoxButtons::OK, MessageBoxIcon::None );
		}

		// エラーメッセージを出力
		void errMsg( System::String ^msgJ, System::String ^msgE )
		{
			if( this->isJapanese() )
				MessageBox::Show( msgJ, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error );
			else
				MessageBox::Show( msgE, "Error", MessageBoxButtons::OK, MessageBoxIcon::Error );
		}

	private:
		// ----------------------------------------------
		// 日英両対応
		// ----------------------------------------------

		// 日本語版と英語版でテキストボックスの文字列制限が変わる
		void changeMaxLength( System::Windows::Forms::TextBox ^tbox, System::Int32 maxlen );

		// 日本語版への切り替え
		void changeJapanese(void);

		// 英語版への切り替え
		void changeEnglish(void);

		// 言語リソース切り替え
		void changeLanguage( System::String ^langname );

		// 日本語版かどうか確認する
		bool isJapanese()
		{
			return (this->stripItemJapanese->Checked);
		}
		bool isEnglish()
		{
			return (!this->isJapanese());
		}

		// --------------------------------------------------------
		// エラー情報の登録
		// --------------------------------------------------------

		// 読み込み時エラーの登録
		void setGridError( void );
		void setGridWarn( void );

		// 読み込み時に検出した修正可能エラーに現在の入力を反映
		void overloadGridError( void );
		void overloadGridWarn( void );

		// セルの色を変える
		void colorGridError( RCMrcError ^err );
		void colorGridWarn( RCMrcError ^err );

		// まとめて更新
		void updateGrid(void);

		// ----------------------------------------------
		// エラー処理
		// ----------------------------------------------

		// SRLには関係しない書類上のエラーをチェック
		System::Boolean isValidOnlyDeliverable(void);

		// SRLのバイナリに影響する項目にエラーがあるかチェック
		System::Boolean isValidAffectRom(void);

		// SRLのバイナリに影響する項目の中で修正可能なエラーだけをチェック
		System::Boolean isValidAffectRomModified(void);

	private:
		// ----------------------------------------------
		// ファイル操作ユーティリティ
		// ----------------------------------------------

		// 直前にアクセスしたディレクトリを記憶して次回のデフォルトにする
		System::String ^prevDir;	// 初期値はnullptr

		// ファイルをダイアログで取得
		// @arg [in] 拡張子フィルタ
		// @ret 取得したファイル名 エラーのとき nullptr
		System::String^ openFileDlg( System::String ^filter );

		// セーブするファイルをダイアログで取得
		// @arg [in] 拡張子フィルタ
		// @arg [in] ファイルの拡張子が不正なときに追加するときの正しい拡張子
		// @ret 取得したファイル名 エラーのとき nullptr
		System::String^ saveFileDlg( System::String ^filter, System::String ^extension );

		// セーブするディレクトリをダイアログで取得
		// @ret 取得したディレクトリ名(\\で終わるように調整される) エラーのときnullptr
		System::String^ saveDirDlg( System::String ^msgJ, System::String ^msgE );

		// ファイルが存在するかを調べて上書き確認をする
		bool isOverwriteFile( System::String ^path );

	/////////////////////////////////////////////
	// タイトルバー操作メソッド
	/////////////////////////////////////////////

	private:
		// --------------------------------------------------------
		// 言語切り替え
		// --------------------------------------------------------
		System::Void stripItemEnglish_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->stripItemEnglish->Checked  = true;
			this->stripItemJapanese->Checked = false;
			this->changeEnglish();
			this->updateGrid();
		}
		System::Void stripItemJapanese_Click(System::Object^  sender, System::EventArgs^  e)
		{
			this->stripItemEnglish->Checked  = false;
			this->stripItemJapanese->Checked = true;
			this->changeJapanese();
			this->updateGrid();
		}

	private:
		// --------------------------------------------------------
		// 「ROMファイルを開く」
		// --------------------------------------------------------
		System::Void stripItemOpenRom_Click(System::Object^  sender, System::EventArgs^  e)
		{
			System::String^ filename;

			// ドラッグアンドドロップ以外ではダイアログから入力する
			filename = this->openFileDlg( "rom format (*.srl;*.tad)|*.srl;*.tad|All files (*.*)|*.*" );
			if( filename == nullptr )
			{
				//this->errMsg( "ROMデータファイルのオープンがキャンセルされました。", "Opening the ROM data file is canceled by user." );
				return;
			}

			// 拡張子で tad 読み込みにするかを判定
			if( !this->loadRom( filename ) )
			{
				return;
			}
			this->tboxFile->Text = filename;
			this->clearOtherForms();			// ROMヘッダには反映されない編集情報を更新
			//this->sucMsg( "ROMデータファイルのオープンに成功しました。", "The ROM data file is opened successfully." );
		} //stripItemOpenRom_Click()

	private:
		// --------------------------------------------------------
		// 「SRLのみ作成する」
		// --------------------------------------------------------
		System::Void stripItemMasterRom_Click(System::Object^  sender, System::EventArgs^  e)
		{
			// SRLが読み込まれていないときにはリードさせない
			if( System::String::IsNullOrEmpty( this->tboxFile->Text ) )
			{
				this->errMsg( "ROMデータファイルが読み込まれていませんので、マスターROMの作成ができません。", 
							  "ROM data file has not opened yet. A master ROM data can't be made." );
				return;
			}

			// SRL関連フォーム入力をチェックする
			this->hErrorList->Clear();
			this->hWarnList->Clear();
			if( this->checkSrlForms() == false )
			{
				this->errMsg( "不正な設定があるためマスターROMの作成ができません。",
							  "Setting is illegal. A master ROM data can't be made." );
				return;
			}

			// 出力ファイル名をゲームコードなどから強制的に決める
			System::String ^prefix     = this->getSubmitFilePrefix();
			System::String ^srlfile    = prefix + ".SRL";

			// 注意書き
			this->sucMsg(
				"以下のファイルが作成されます。\n\n"
				+ srlfile + " (マスターROM)\n",

				"Following file for submission will be made. \n\n"
				+ srlfile + " (Master ROM)\n"
			);

			// ダイアログからSRLを保存するディレクトリを取得する
			System::String ^dir = this->saveDirDlg( "保存先フォルダを選択してください。", "Please select a folder for saving the file" );
			if( !dir )
			{
				this->errMsg( "フォルダの選択がキャンセルされましたのでマスターROMは作成されません。", 
							  "A master ROM isn't made, since selecting folder is canceled." );
				return;
			}
			srlfile = dir + srlfile;

			// ファイルが存在するかを調べて上書き確認をする
			if( !this->isOverwriteFile(srlfile) )
			{
				this->errMsg( "ファイルの上書きがキャンセルされましたのでマスターROMは作成されません。", 
							  "Since overwriting a file is canceled, a master ROM isn't made." );
				return;
			}

			try
			{
				if( !this->saveRom( srlfile ) )
				{
					this->errMsg( "マスターROMの作成に失敗しました。",
								  "Making a master ROM failed." );
					return;
				}
				this->sucMsg( "マスターROMの作成が成功しました。", "A master ROM is made successfully." );
				this->tboxFile->Text = srlfile;
			}
			catch( System::Exception ^ex )
			{
				(void)ex;
				this->errMsg( "マスターROMの作成に失敗しました。",
							  "Making a master ROM failed." );
				return;
			}
		} //stripItemMasterRom_Click()

	private:
		// --------------------------------------------------------
		// 「提出データ一式を作成する」
		// --------------------------------------------------------
		System::Void stripItemSheet_Click(System::Object^  sender, System::EventArgs^  e)
		{
			ECDeliverableResult  result;

			// SRLが読み込まれていないときにはリードさせない
			if( System::String::IsNullOrEmpty( this->tboxFile->Text ) )
			{
				this->errMsg( "ROMデータファイルが読み込まれていません。", "ROM file has not opened yet." );
				return;
			}

			// SRLと書類の両方のフォーム入力をチェックする
			this->hErrorList->Clear();
			this->hWarnList->Clear();
			if( this->checkSrlForms() == false )
			{
				this->errMsg( "不正な設定があるため提出データを作成できません。",
							  "Setting is illegal. Submission data can't be made." );
				return;
			}
			if( this->checkDeliverableForms() == false )
			{
				this->errMsg( "入力情報に不足があるため提出データを作成できません。",
							  "Input is not enough. Submission data can't be made." );
				return;
			}

			// 出力ファイル名をゲームコードなどから強制的に決める
			System::String ^prefix     = this->getSubmitFilePrefix();
			System::String ^srlfile    = prefix + ".SRL";
			System::String ^delivfile  = prefix + "_SHEET.XML";
			System::String ^middlefile = prefix + "_MIDDLEWARE.XML";
			System::String ^middlefilePrint = prefix + "_MIDDLEWARE.HTML";

			// 注意書き
			this->sucMsg(
				"以下の提出ファイルが一度に作成されます。\n\n"
				+ srlfile    + " (マスターROM)\n"
				+ delivfile  + " (マスターROM提出確認書)\n"
				+ middlefile + " (ミドルウェア一覧)\n"
				+ middlefilePrint + " (ミドルウェア一覧 印刷用)\n"
				+ "\n",

				"Following files for submission will be made. \n\n"
				+ srlfile    + " (Master ROM)\n"
				+ delivfile  + " (Submission Sheet)\n"
				+ middlefile + " (Middleware List)\n"
				+ middlefilePrint + " (Middleware List For Print)\n"
				+ "\n"
			);

			// ダイアログからSRLを保存するディレクトリを取得する
			System::String ^dir = this->saveDirDlg( "保存先フォルダを選択してください。", "Please select a folder for saving files" );
			if( !dir )
			{
				this->errMsg( "フォルダの選択がキャンセルされましたので提出データ一式は作成されません。", 
							  "A set of submission data can not be made, since selecting folder is canceled." );
				return;
			}
			srlfile    = dir + srlfile;
			delivfile  = dir + delivfile;
			middlefile = dir + middlefile;
			middlefilePrint = dir + middlefilePrint;

			// ファイルが存在するかを調べて上書き確認をする
			if( !this->isOverwriteFile(srlfile)    || !this->isOverwriteFile(delivfile) ||
				!this->isOverwriteFile(middlefile) || !this->isOverwriteFile(middlefilePrint) )
			{
				this->errMsg( "ファイルの上書きがキャンセルされましたので提出データ一式は作成されません。", 
							  "Since overwriting a file is canceled, a set of submission data can not be made." );
				return;
			}
			
			// マスタ提出確認書に必要な情報をフォームから取得して更新
			this->setSrlProperties();	// 先にSrlを更新しておく
			this->setDeliverableProperties();

			// 更新後のSRLを別ファイルに作成
			try
			{
				if( !this->saveRom( srlfile ) )
				{
					this->errMsg( "マスターROMの作成に失敗しました。作成を中止するため一部のファイルは作成されません。",
								  "Making a master ROM failed. Therefore, a part of files can't be made." );
					return;
				}
				this->tboxFile->Text = srlfile;
			}
			catch( System::Exception ^ex )
			{
				(void)ex;
				this->errMsg( "マスターROMの作成に失敗しました。作成を中止するため一部のファイルは作成されません。",
							  "Making a master ROM failed. Therefore, a part of files can't be made." );
				return;
			}
			u16  crc;			// SRL全体のCRCを計算する(書類に記述するため)
			if( !getWholeCRCInFile( srlfile, &crc ) )
			{
				this->errMsg( "CRCの計算に失敗しました。作成を中止するため一部のファイルは作成されません。", 
							  "Calculating CRC is failed. Therefore, a part of files can't be made." );
				return;
			}
			this->tboxWholeCRC->Clear();
			this->tboxWholeCRC->AppendText( "0x" );
			this->tboxWholeCRC->AppendText( crc.ToString("X4") );	// 書き出したSRLを再読み込みするのでCRCを再計算する

			// ミドルウェアのリストを作成
			if( !this->saveMiddlewareListXmlEmbeddedXsl( middlefile ) || !this->saveMiddlewareListHtml( middlefilePrint ) )
			{
				this->errMsg( "ミドルウェアのリストが作成できませんでした。作成を中止するため一部のファイルは作成されません。",
							  "Making a list of middleware failed. Therefore, a part of files can't be made.");
				return;
			}

			// 書類作成
			srlfile = System::IO::Path::GetFileName( srlfile );
			//result = this->hDeliv->write( delivfile, this->hSrl, hcrc, srlfile, !this->isJapanese() );
			result = this->hDeliv->writeSpreadsheet( delivfile, this->getSheetTemplateFile(), this->hSrl, crc, srlfile, !this->isJapanese() );
			if( result != ECDeliverableResult::NOERROR )
			{
				switch( result )
				{
					case ECDeliverableResult::ERROR_FILE_OPEN:
						this->errMsg( "提出確認書のテンプレートが開けなかったため、提出確認書の作成に失敗しました。", 
							          "Since a templete of the submission sheet can't be opened, making the sheet is failed." );
					break;

					case ECDeliverableResult::ERROR_FILE_WRITE:
						this->errMsg( "提出確認書にデータを書き込みできませんでした。同名ファイルがすでに開かれていないかご確認ください。", 
							          "Writing data into a submission sheet failed. Please check that the file has been opened already." );
					break;

					default:
						this->errMsg( "提出確認書の作成に失敗しました。", "Making the submission sheet is failed." );
					break;
				}
				return;
			}
			this->sucMsg( "提出データ一式の作成に成功しました。", "A set of submission data is made successfully." );

		} //stripItemSheet_Click()

	private:
		// --------------------------------------------------------
		// 「提出情報を一時保存する」
		// --------------------------------------------------------
		System::Void stripItemSaveTemp_Click(System::Object^  sender, System::EventArgs^  e)
		{
			System::String ^filename = this->saveFileDlg( "xml format (*.xml)|*.xml", ".xml" );
			if( !filename )
			{
				return;
			}
			this->saveTmp( filename );
		} //stripItemSaveTemp_Click()

	private:
		// --------------------------------------------------------
		// 「一時保存した提出情報を開く」
		// --------------------------------------------------------
		System::Void stripItemLoadTemp_Click(System::Object^  sender, System::EventArgs^  e)
		{
			System::String ^filename = this->openFileDlg( "xml format (*.xml)|*.xml" );
			if( filename == nullptr )
			{
				return;
			}
			this->loadTmp( filename );
		} //stripItemLoadTemp_Click()

	private:
		// --------------------------------------------------------
		// 「ミドルウェアリストのみを作成する(XML形式)」
		// --------------------------------------------------------
		System::Void stripItemMiddlewareXml_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( System::String::IsNullOrEmpty(this->tboxFile->Text) )
			{
				this->errMsg( "ROMデータファイルが読み込まれていません。", "ROM file has not opened yet." );
				return;
			}

			System::String ^filename = this->saveFileDlg( "xml format (*.xml)|*.xml", ".xml" );

			if( !filename || !this->saveMiddlewareListXmlEmbeddedXsl(filename) )
			{
				this->errMsg( "ミドルウェアリストの作成に失敗しました。","Making a middleware list failed." );
			}
		} //stripItemMiddlewareXml_Click()

	private:
		// --------------------------------------------------------
		// 「ミドルウェアリストのみを作成する(HTML形式)」
		// --------------------------------------------------------
		System::Void stripItemMiddlewareHtml_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( System::String::IsNullOrEmpty(this->tboxFile->Text) )
			{
				this->errMsg( "ROMデータファイルがオープンされていません。", "ROM file has not opened yet." );
				return;
			}

			System::String ^filename = this->saveFileDlg( "html format (*.html)|*.html", ".html" );

			if( !filename || !this->saveMiddlewareListHtml(filename) )
			{
				this->errMsg( "ミドルウェアリストの作成に失敗しました。","Making a middleware list failed." );
			}
		} //stripItemMiddlewareHtml_Click


	/////////////////////////////////////////////
	// フォーム操作メソッド
	/////////////////////////////////////////////

	// ドラッグアンドドロップでSRLを読み込む
	private:
		// ドラッグされてまだマウスのボタンが離されていないとき
		System::Void Form1_DragEnter(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
		{
			if( e->Data->GetDataPresent( DataFormats::FileDrop ) )
			{
				e->Effect = DragDropEffects::All;
			}
		}
		// ドラッグされたあとマウスのボタンが離されたとき
		System::Void Form1_DragDrop(System::Object^  sender, System::Windows::Forms::DragEventArgs^  e)
		{
			array<String^> ^files = dynamic_cast< array<String^> ^>(e->Data->GetData( DataFormats::FileDrop ) );
			String ^filename = files[0];

			if( System::IO::File::Exists(filename) == false )
			{
				this->errMsg( "ROMデータファイルが存在しませんので読み込むことができません。", 
							  "The ROM data file is not found. Therefore the file can not be opened." );
				return;
			}
			if( !this->loadRom( filename ) )
			{
				return;
			}
			this->tboxFile->Text = filename;
			this->clearOtherForms();
			//this->sucMsg( "ROMデータファイルのオープンに成功しました。", "The ROM data file is opened successfully." );
		}

	// フォームの状態に連動して他のフォームを有効/無効にする
	private:
		// 担当者2を入力することが選択されたときに入力用のテキストボックスを有効にする
		System::Void cboxIsInputPerson2_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			this->gboxPerson2->Enabled = this->cboxIsInputPerson2->Checked;
			if( this->cboxIsInputPerson2->Checked == true )
			{
				if( (this->tboxCompany1->Text != nullptr) && !(this->tboxCompany1->Text->Equals("")) )
				{
					this->tboxCompany2->Text = gcnew System::String( this->tboxCompany1->Text );	// 会社と部署は担当者1と同じにする
				}
				if( (this->tboxDepart1->Text != nullptr) && !(this->tboxDepart1->Text->Equals("")) )
				{
					this->tboxDepart2->Text  = gcnew System::String( this->tboxDepart1->Text );
				}
			}
			else
			{
				this->tboxCompany2->Clear();
				this->tboxDepart2->Clear();
				this->tboxPerson2->Clear();
				this->tboxFurigana2->Clear();
				this->tboxTel2->Clear();
				this->tboxFax2->Clear();
				this->tboxMail2->Clear();
				this->tboxNTSC2->Clear();
			}
		}
	private:
		// 用途に「その他」が選択されたときに入力用のテキストボックスを有効にする
		System::Void rUsageOther_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			this->tboxUsageOther->Enabled = this->rUsageOther->Checked;
			if( this->rUsageOther->Checked == false )
			{
				this->tboxUsageOther->Clear();
			}
		}
	private:
		System::Void cboxRemasterVerE_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			// 設定不可になったため必要なし
			//this->numRemasterVer->Enabled = !(this->isEngilsh());
			//if( this->isEngilsh() == false )
			//{
			//	this->numRemasterVer->Value = 0;
			//}
		}
	private:
		// バックアップメモリに「その他」が選択されたときに入力用のテキストボックスを有効にする
		System::Void combBackup_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( this->combBackup->SelectedIndex == (this->combBackup->Items->Count - 1) )
			{
				this->tboxBackupOther->Enabled = true;
			}
			else
			{
				this->tboxBackupOther->Enabled = false;
				this->tboxBackupOther->Clear();			// 前にテキストボックスに書かれていた内容を消去
			}
		}
	private:
		// リージョン変更に連動して表示するレーティング団体を変更する
		System::Void combRegion_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			this->maskRatingForms();
			this->changeUnnecessaryRatingForms(false);	// 一度コンボボックスがenableになるので再設定
		}
	private:
		// レーティング表示不要が選択されたときにレーティングを選択不可にする
		System::Void cboxIsUnnecessaryRating_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			this->changeUnnecessaryRatingForms(false);
		}
	private:
		// レーティング情報をSRLを読み込んだ時点の情報に戻す
		System::Void butSetBack_Click(System::Object^  sender, System::EventArgs^  e)
		{
			if( System::String::IsNullOrEmpty( this->tboxFile->Text ) )
				return;

			this->setRegionForms();		// フォームが更新されてもSRLクラスの情報は更新されないので再度読み出すだけ
			this->setRatingForms();
			this->loadOtherForms();		// SRLに登録されていないROM仕様のフォームも戻す
		}
	private:
		// 海外版を発売予定であると選択されたときに海外版の製品コードetc.を入力可能にする
		System::Void cboxReleaseForeign_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			this->tboxProductNameForeign->Enabled   = this->cboxReleaseForeign->Checked;
			this->tboxProductCode1Foreign->Enabled  = this->cboxReleaseForeign->Checked;
			this->tboxProductCode2Foreign1->Enabled = this->cboxReleaseForeign->Checked;
			this->tboxProductCode2Foreign2->Enabled = this->cboxReleaseForeign->Checked;
			this->tboxProductCode2Foreign3->Enabled = this->cboxReleaseForeign->Checked;
			if( this->cboxReleaseForeign->Checked == false )
			{
				this->tboxProductNameForeign->Clear();
				this->tboxProductCode1Foreign->Clear();
				this->tboxProductCode2Foreign1->Clear();
				this->tboxProductCode2Foreign2->Clear();
				this->tboxProductCode2Foreign3->Clear();
			}
		}
	// エラー情報の更新
	private:
		System::Void rErrorReading_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			this->updateGrid();
		}
	private:
		System::Void rErrorCurrent_CheckedChanged(System::Object^  sender, System::EventArgs^  e)
		{
			this->updateGrid();
		}
	// エラー情報タブが選択されるたびにエラー情報を更新
	// (エラータブが選択されたタイミングでのみ更新することができないのでタブが切り替えられるたびに更新する)
	private:
		System::Void tabMain_SelectedIndexChanged(System::Object^  sender, System::EventArgs^  e)
		{
			if( tabMain->SelectedIndex == 5 )
			{
				this->updateGrid();
			}
		}

}; // enf of ref class Form1

} // end of namespace MasterEditorTWL

