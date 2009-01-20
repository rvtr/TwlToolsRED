#include "TextTable.h"

//言語選択タイトル
const Texts g_TextTypes = 
{
	L"Hangeul",	
	L"Japanese",
	L"English" 
};

//テキストテーブル
// 第１要素：ハングル選択時表示
// 第２要素：日本語選択時表示
// 第３要素：英語選択時表示
//
// unidode表示のため、L""で一つの文字列を表現します。
// シフトJISコードで記述可能な場合はそのまま書くことが可能です（'a''あ'など)
// DS固有フォントやハングルなどの特殊な文字については\xからはじまる４桁の16進数で文字コードを記述します。
// 文字コードについては文字順序ファイルds_ipl_korean.xlorを参照ください。

const Texts g_TextTable[] = {
	{ 	
		//"---------------------------------
		//---------------------------------
		//---------------------------------"
		L"--------------------------------\n--------------------------------\n--------------------------------",
		L"--------------------------------\n--------------------------------\n--------------------------------",
		L"--------------------------------\n--------------------------------\n--------------------------------",
	},
	{ 	
		//PictoChat
		L"PictoChat",
		L"PictoChat",
		L"PictoChat",
	},
	{ 	
		//何も差さっていません
		L"\xC544\xBB34\xAC83\xB3C4\x0020\xAF42\xD600\x000A\xC788\xC9C0\x0020\xC54A\xC2B5\xB2C8\xB2E4",
		L"no font",
 		L"There is no DS Card\ninserted"
	},
	{ 	
		//"ああああああああああああああ
		//あ
		//あ"
		L"\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\xC544\x000A\xC544\x000A\xC544",
		L"ああああああああああああああ\nあ\nあ",
		L"ああああああああああああああ\nあ\nあ",
	},
	{ 	
		//GBAカートリッジ
		L"\x0047\x0042\x0041\x0020\xCE74\xD2B8\xB9AC\xC9C0",
		L"GBAカートリッジ",
		L"Start GBA game."
	},
	{ 	
		//何も差さっていません
		L"\xC544\xBB34\xAC83\xB3C4\x0020\xAF42\xD600\x000A\xC788\xC9C0\x0020\xC54A\xC2B5\xB2C8\xB2E4",
		L"no font",
		L"There is no Game Pak\ninserted."
	},
	{ 	
		//DSオプションカートリッジ
		L"\x0044\x0053\x0020\xC635\xC158\x0020\xCE74\xD2B8\xB9AC\xC9C0",
		L"DSオプションカートリッジ",
		L"There is a DS Option\nPak inserted."
	},
	{ 	
		//"------------------------
		//------------------------
		//------------------------"
		L"------------------------\n------------------------\n------------------------\n",
		L"------------------------\n------------------------\n------------------------\n",
		L"------------------------\n------------------------\n------------------------\n",
	},
	{ 	
		//本体設定
		L"\xBCF8\xCCB4\x0020\xC124\xC815",
		L"no font",
		L"Settings",
	},
	{ 	
		//DSダウンロードプレイ
		L"\x0044\x0053\x0020\xB2E4\xC6B4\xB85C\xB4DC\x0020\xD50C\xB808\xC774",
		L"DSダウンロードプレイ",
		L"DS Download Play"
	},
	{ 	
		// もう一度
		L"\xE002\x0020\x0020\xD55C\x0020\xBC88\x0020\xB354",
		L"no font",
		L"\xE002\x0020Try again"
	},
	{ 	
		// とじる
		L"\xE000\x0020\xB2EB\xAE30",
		L"\xE000\x0020とじる",
		L"\xE000\x0020Close"
	},
	{ 	
		// はい
		L"\xE000\x0020\xC608",
		L"\xE000\x0020はい",
		L"\xE000\x0020Yes"
	},
	{ 	
		// いいえ
		L"\xE001\x0020\xC544\xB2C8\xC624",
		L"\xE001\x0020いいえ",
		L"\xE001\x0020No"
	},
	{ 	
		// アラームをセット
		L"\xE002\x0020\x0020\xC54C\xB78C\xC744\x0020\xC124\xC815",
		L"\xE002\x0020アラームをセット",
		L"\xE002\x0020Turn Alarm On"
	},
	{ 	
		// 決定
		L"\xE000\x0020\xACB0\xC815",
		L"no font",
		L"\xE000\x0020Confirm",
	},
	{ 	
		// 入力
		L"\xE000\x0020\xC785\xB825",
		L"no font",
		L"\xE000\x0020Input"
	},
	{ 	
		// 参加
		L"\xE000\x0020\xCC38\xAC00",
		L"no font",
		L"\xE000\x0020Join"
	},
	{ 	
		// OK
		L"\xE000\x0020\x004F\x004B",
		L"\xE000\x0020\x004F\x004B",
		L"\xE000\x0020\x004F\x004B",
	},
	{ 	
		// 選択
		L"\xE000\x0020\xC120\xD0DD",
		L"no font",
		L"\xE000\x0020\x0053\x0065\x006C\x0065\x0063\x0074"
	},
	{ 	
		//－－－－－－－－
		L"－－－－－－－－",
		L"－－－－－－－－",
		L"－－－－－－－－",
	},
	{ 	
		// はい
		L"\xE000\x0020\xC608",
		L"\xE000\x0020はい",
		L"\xE000\x0020Yes"
	},
	{ 	
		// もどる
		L"\xE001\x0020\xB3CC\xC544\xAC00\xAE30",
		L"\xE001\x0020もどる",
		L"\xE001\x0020Go back"
	},
	{ 	
		// キャンセル
		L"\xE001\x0020\xCDE8\xC18C",
		L"\xE001\x0020キャンセル",
		L"\xE001\x0020Cancel"
	},
	{ 	
		// 消去
		L"\xE001\x0020\xC0AD\xC81C",
		L"no font",
		L"\xE001\x0020Erase"
	},
	{ 	
		// いいえ
		L"\xE001\x0020\xC544\xB2C8\xC624",
		L"\xE001\x0020いいえ",
		L"\xE001\x0020No"
	},
	{ 	
		// 終了
		L"\xE001\x0020\xC885\xB8CC",
		L"no font",
		L"\xE001\x0020Quit"
	},
	{ 	
		//－－－－－－－－－
		L"－－－－－－－－－",
		L"－－－－－－－－－",
		L"－－－－－－－－－",
	},
	{ 	
		
		//いいえ
		L"\xE001\x0020\xC544\xB2C8\xC624",
		L"\xE001\x0020いいえ",
		L"\xE001\x0020No"
	},
	{ 	
		
		//はい
		L"\xE000\x0020\xC608",
		L"\xE000\x0020はい",
		L"\xE000\x0020Yes"
	},
	{ 	
		
		//チャットル-ムA
		L"\xCC44\xD305\xBC29\x0020\x0041",
		L"チャットル-ムA",
		L"Chat Room A"
	},
	{ 	
		
		//チャットル-ムB
		L"\xCC44\xD305\xBC29\x0020\x0042",
		L"チャットル-ムB",
		L"Chat Room B"
	},
	{ 	
		
		//チャットル-ムC
		L"\xCC44\xD305\xBC29\x0020\x0043",
		L"チャットル-ムC",
		L"Chat Room C"
	},
	{ 	
		
		//チャットル-ムD
		L"\xCC44\xD305\xBC29\x0020\x0044",
		L"チャットル-ムD",
		L"Chat Room D"
	},
	{ 	
		
		//参加したいルームを選択して下さい。
		L"\xCC38\xAC00\xD558\xACE0\x0020\xC2F6\xC740\x0020\xCC44\xD305\xBC29\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Choose a Chat Room to join."
	},
	{ 	
		
		//"誕生日おめでとう！
		//今日は∩さんの誕生日です。"
		L"\xC0DD\xC77C\x0020\x0020\xCD95\xD558\xD569\xB2C8\xB2E4\x0021\x0020\x0020\xC624\xB298\xC740\x000A\x2229\x0020\xB2D8\xC758\x0020\x0020\xC0DD\xC77C\xC785\xB2C8\xB2E4\x002E",
		L"no font",
		L"Happy Birthday!\nToday is ∩'s Birthday!"
	},
	{ 	
		//"∩さんが
		//ルーム⊿へ移動しました。"
		L"\x2229\x0020\xB2D8\xC774\x000A\xCC44\xD305\xBC29\x22BF\xB85C\x0020\x0020\xC774\xB3D9\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"∩ has moved to\nChat Room ⊿."
	},
	{ 	
		//参加 ⊿  ∩ さん
		L"\xCC38\xAC00\x0020\xE019\x22BF\x0020\x0020\x2229\x0020\xB2D8",
		L"no font",
		L"Now entering ⊿: ∩"
	},
	{ 	
		//退室 ⊿  ∩ さん
		L"\xD1F4\xC2E4\x0020\xE01A\x22BF\x0020\x0020\x2229\x0020\xB2D8",
		L"no font",
		L"Now leaving ⊿: ∩"
	},
	{ 	
		
		//
		L"",
		L"",
		L"",
	},
	{ 	
		//"通信エラーが発生しました。
		//ルーム選択へもどります。"
		L"\xD1B5\xC2E0\x0020\xC5D0\xB7EC\xAC00\x0020\xBC1C\xC0DD\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xCC44\xD305\xBC29\x0020\xC120\xD0DD\xC73C\xB85C\x0020\xB418\xB3CC\xC544\xAC11\xB2C8\xB2E4\x002E",
		L"no font",
		L"Communication error.\nReturn to room selection."
	},
	{ 	
		//"定員オーバーです。
		//ルーム⊿に入れませんでした。"
		L"\xC815\xC6D0\x0020\xCD08\xACFC\xC785\xB2C8\xB2E4\x002E\x000A\xCC44\xD305\xBC29\x0020\x22BF\x0020\xC5D0\x0020\xB4E4\xC5B4\xAC00\xC9C0\x0020\xBABB\x0020\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Chat Room ⊿ is full. You\ncannot enter Chat Room ⊿."
	},
	{ 	
		//"通信エラーが発生しました。
		//一度電源を切って下さい。"
		L"\xD1B5\xC2E0\x0020\xC5D0\xB7EC\xAC00\x0020\xBC1C\xC0DD\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xC804\xC6D0\xC744\x0020\xAEBC\x0020\xC8FC\xC138\xC694\x002E",
		L"no\xfont",
		L"Communication error.\nPlease turn the power off."
	},
	{ 	
		
		//ダウンロードしています。
		L"\xB2E4\xC6B4\xB85C\xB4DC\xD558\xACE0\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"ダウンロードしています。",
		L"Downloading...",
	},
	{ 	
		//ダウンロードをキャンセルしました。
		L"\xB2E4\xC6B4\xB85C\xB4DC\xB97C\x0020\xCDE8\xC18C\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"ダウンロードをキャンセルしました。",
		L"Download has been canceled."
	},
	{ 	
		
		//"定員オーバーです。
		//ダウンロードできませんでした。"
		L"\xC815\xC6D0\x0020\xCD08\xACFC\xC785\xB2C8\xB2E4\x002E\x000A\xB2E4\xC6B4\xB85C\xB4DC\xC5D0\x0020\xC2E4\xD328\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Maximum number of participants has\nbeen reached. Unable to download."
	},
	{ 	
		
		//ダウンロードが完了しました。
		L"\xB2E4\xC6B4\xB85C\xB4DC\xAC00\x0020\xC644\xB8CC\xB418\xC5C8\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The software has been\nsuccessfully downloaded."
	},
	{ 	
		
		//このソフトをダウンロードしますか？
		L"\xC774\x0020\xC18C\xD504\xD2B8\xB97C\x0020\xB2E4\xC6B4\xB85C\xB4DC\xD558\xACA0\xC2B5\xB2C8\xAE4C\x003F",
		L"このソフトをダウンロードしますか？",
		L"Would you like to download\nthis software?"
	},
	{ 	
		//"通信エラーが発生しました。
		//ダウンロードできませんでした。"
		L"\xD1B5\xC2E0\x0020\xC5D0\xB7EC\xAC00\x0020\xBC1C\xC0DD\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xB2E4\xC6B4\xB85C\xB4DC\xC5D0\x0020\xC2E4\xD328\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Communication error.\nDownload not completed."
	},
	{ 	
		//"ダウンロードできる
		//ソフトを探しています。"
		L"\xB2E4\xC6B4\xB85C\xB4DC\x0020\xAC00\xB2A5\xD55C\x000A\xC18C\xD504\xD2B8\xB97C\x0020\xCC3E\xACE0\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Looking for software available\nfor download..."
	},
	{ 	
		
		//「アラーム」を解除しました。
		L"\x300C\xC54C\xB78C\x300D\xC744\x0020\xD574\xC81C\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The alarm has been turned off."
	},
	{ 	
		
		//「アラーム」を解除しますか？
		L"\x300C\xC54C\xB78C\x300D\xC744\x0020\xD574\xC81C\xD558\xACA0\xC2B5\xB2C8\xAE4C\x003F",
		L"no font",
		L"Would you like to turn the\nalarm off?"
	},
	{ 	
		
		//"●●:●●に「アラーム」をセットします。
		//よろしいですか？"
		L"\x25CF\x25CF\x003A\x25CF\x25CF\xC73C\xB85C\x0020\xC54C\xB78C\xC744\x0020\xC124\xC815\xD558\xACA0\xC2B5\xB2C8\xAE4C\xFF1F",
		L"no font",
		L"The alarm has been set for ●●:●●.\nWould you like to turn the alarm on?"
	},
	{ 	
		//振動機能を使いますか？
		L"\xC9C4\xB3D9\x0020\xAE30\xB2A5\xC744\x0020\xC0AC\xC6A9\xD558\xACA0\xC2B5\xB2C8\xAE4C\x003F",
		L"no font",
		L"Would you like the Rumble Featureto accompany the alarm?"
	},
	{ 	
		//"「アラーム」がセットされています。
		//解除するには、画面をタッチするか
		//以外のボタンを押して下さい。
		//（必要に応じて、ボリュームを調整して下さい。）"
		L"\x300C\xC54C\xB78C\x300D\xC774\x0020\xC124\xC815\xB418\xC5B4\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x000A\xD574\xC81C\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xE004\xE005\x0020\xC774\xC678\xC758\x0020\xBC84\xD2BC\xC744\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E\x000A\xFF08\xD544\xC694\xC5D0\x0020\xB530\xB77C\x0020\xBCFC\xB968\xC744\x0020\xC870\xC815\xD574\x0020\xC8FC\xC138\xC694\xFF09",
		L"no font",
		L"The alarm is currently on. Touch the Touch\n"
		L"Screen or press any button to turn it off.\n"
		L" will not turn the alarm off.\n"
		L"(Adjust volume if necessary.)"
	},
	{ 	
		
		//"セットした時刻になりました。
		//解除するには、画面をタッチするか
		//以外のボタンを押して下さい。"
		L"\xC124\xC815\xD55C\x0020\xC2DC\xAC01\xC774\x0020\xB418\xC5C8\xC2B5\xB2C8\xB2E4\x002E\x000A\xD574\xC81C\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xE004\xE005\x0020\xC774\xC678\xC758\x0020\xBC84\xD2BC\xC744\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
L"The alarm has been turned on.\n"
L"To turn the alarm off, touch the Touch\n"
L"Screen or press any button.\n"
L" will not turn the alarm off."
	},
	{ 	
		
		//"セットした時刻から●●:●●たちました。
		//解除するには、画面をタッチするか
		//以外のボタンを押して下さい。"
		L"\xC124\xC815\xD55C\x0020\xC2DC\xAC01\xC73C\xB85C\xBD80\xD130\x0020\x25CF\x25CF\x003A\x25CF\x25CF\x0020\xC9C0\xB0AC\xC2B5\xB2C8\xB2E4\x002E\x000A\xD574\xC81C\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xE004\xE005\x0020\xC774\xC678\xC758\x0020\xBC84\xD2BC\xC744\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"It is ●●:●● past the set alarm time.\n"
		L"To turn the alarm off, touch the Touch\n"
		L"Screen or press any button.\n"
		L" will not turn the alarm off.\n"
	},
	{ 	
		
		//"-------------------------------------
		//-------------------------------------
		//-------------------------------------"
		L"-------------------------------------\n"
		L"-------------------------------------\n"
		L"-------------------------------------",
		
		L"-------------------------------------\n"
		L"-------------------------------------\n"
		L"-------------------------------------",
		
		L"-------------------------------------\n"
		L"-------------------------------------\n"
		L"-------------------------------------"
	},
	{ 	
		
		//現在時刻
		L"\xD604\xC7AC\x0020\xC2DC\xAC01",
		L"no font",
		L"Current Time"
	},
	{ 	
		//0:00:00
		L"0:00:00",
		L"0:00:00",
		L"0:00:00",
	},
	{ 	
		
		//残り時間
		L"\xB0A8\xC740\x0020\xC2DC\xAC04",
		L"no font",
		L"Time Remaining",
	},
	{ 	
		
		//経過時間
		L"\xACBD\xACFC\x0020\xC2DC\xAC04",
		L"no font",
		L"Elapsed Time",
	},
	{ 	
		
		//-----------------
		L"-----------------",
		L"-----------------",
		L"-----------------",
	},
	{ 	
		
		//設定時刻
		L"\xC124\xC815\x0020\xC2DC\xAC01",
		L"no font",
		L"Alarm Setting",
	},
	{ 	
		
		//DSダウンロードプレイ
		L"\x0044\x0053\x0020\xB2E4\xC6B4\xB85C\xB4DC\x0020\xD50C\xB808\xC774",
		L"DSダウンロードプレイ",
		L"DS Download Play",
	},
	{ 	
		
		//オプション
		L"\xC635\xC158",
		L"オプション",
		L"Options",
	},
	{ 	
		
		//起動モード
		L"\xC2E4\xD589\x0020\xBAA8\xB4DC",
		L"no font",
		L"Start-up",
	},
	{ 	
		
		//GBA表示
		L"\x0047\x0042\x0041\x0020\xD45C\xC2DC",
		L"no font",
		L"GBA Mode"
	},
	{ 	
		
		//言語
		L"\xC5B8\xC5B4",
		L"no font",
		L"Language",
	},
	{ 	
		
		//ユーザー
		L"\xC720\xC800",
		L"ユーザー",
		L"User",
	},
	{ 	
		
		//ユーザーネーム
		L"\xC720\xC800\x0020\xB124\xC784",
		L"ユーザーネーム",
		L"User Name",
	},
	{ 	
		
		//ユーザーカラー
		L"\xC720\xC800\x0020\xCEEC\xB7EC",
		L"ユーザーカラー",
		L"Color",
	},
	{ 	
		
		//誕生日
		L"\xC0DD\xC77C",
		L"no font",
		L"Birthday",
	},
	{ 	
		
		//コメント
		L"\xCF54\xBA58\xD2B8",
		L"コメント",
		L"Message",
	},
	{ 	
		
		//タッチスクリーン
		L"\xD130\xCE58\xC2A4\xD06C\xB9B0",
		L"タッチスクリーン",
		L"Touch Screen",
	},
	{ 	
		
		//本体設定
		L"\xBCF8\xCCB4\x0020\xC124\xC815",
		L"no font",
		L"Settings",
	},
	{ 	
		
		//日付＆時刻
		L"\xB0A0\xC9DC\xFF06\xC2DC\xAC01",
		L"no font",
		L"Clock",
	},
	{ 	
		
		//日付
		L"\xB0A0\xC9DC",
		L"no font",
		L"Date",
	},
	{ 	
		
		//時刻
		L"\xC2DC\xAC01",
		L"no font",
		L"Time",
	},
	{ 	
		
		//アラーム
		L"\xC54C\xB78C",
		L"アラーム",
		L"Alarm",
	},
	{ 	
		
		//－－－－－－－－－－－－－－－－－－－
		L"－－－－－－－－－－－－－－－－－－－",
		L"－－－－－－－－－－－－－－－－－－－",
		L"－－－－－－－－－－－－－－－－－－－",
	},
	{ 	
		
		//時
		L"\xC2DC",
		L"no font",
		L"Hour",
	},
	{ 	
		
		//分
		L"\xBD84",
		L"no font",
		L"Minute",
	},
	{ 	
		
		//日
		L"\xC77C",
		L"no font",
		L"Day",
	},
	{ 	
		
		//月
		L"\xC6D4",
		L"no font",
		L"Month",
	},
	{ 	
		
		//日
		L"\xC77C",
		L"no font",
		L"Day",
	},
	{ 	
		
		//月
		L"\xC6D4",
		L"no font",
		L"Month",
	},
	{ 	
		
		//年
		L"\xB144",
		L"no font",
		L"Year",
	},
	{ 	
		
		//時
		L"\xC2DC",
		L"no font",
		L"Hour",
	},
	{ 	
		
		//分
		L"\xBD84",
		L"no font",
		L"Minute",
	},
	{ 	
		
		//"本体の設定をします。
		//設定したいパネルを選択して下さい。"
		L"\xBCF8\xCCB4\x0020\xC124\xC815\xC744\x0020\xD569\xB2C8\xB2E4\x002E\x000A\xC124\xC815\xD558\xACE0\x0020\xC2F6\xC740\x0020\xD328\xB110\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Change system settings here. Select\n"
		L"the setting you'd like to change."
	},
	{ 	
		
		//"---------------------------------
		//---------------------------------"
		L"---------------------------------\n---------------------------------",
		L"---------------------------------\n---------------------------------",
		L"---------------------------------\n---------------------------------",
	},
	{ 	
		
		//ダウンロードするソフトを選択します。
		L"\xB2E4\xC6B4\xB85C\xB4DC\xD560\x0020\xC18C\xD504\xD2B8\xB97C\x0020\xC120\xD0DD\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Download software via DS Download Play."
	},
	{ 	
		
		//オプションを設定します。
		L"\xC635\xC158\xC744\x0020\xC124\xC815\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Change other settings."
	},
	{ 	
		
		//"GBAソフトをプレイする画面を
		//選択して下さい。"
		L"\x0047\x0042\x0041\x0020\xC18C\xD504\xD2B8\xB97C\x0020\xD50C\xB808\xC774\xD560\x0020\xD654\xBA74\xC744\x000A\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select the screen you would like to use\nwhen starting GBA Mode."
	},
	{ 	
		
		//"電源を入れた時の
		//起動方法を選択して下さい。"
		L"\xC804\xC6D0\xC744\x0020\xB123\xC5C8\xC744\x0020\xB54C\xC758\x000A\xC2E4\xD589\x0020\xBC29\xBC95\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Set how you would like your system to\n"
		L"start up when you turn the power on."
	},
	{ 	
		
		//使用する言語を選択して下さい。
		L"\xC0AC\xC6A9\xD560\x0020\xC5B8\xC5B4\xB97C\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select the language to use."
	},
	{ 	
		
		//日付・時刻・アラームを設定します。
		L"\xB0A0\xC9DC\x30FB\xC2DC\xAC01\x30FB\xC54C\xB78C\xC744\x0020\xC124\xC815\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Change date, time, and alarm settings."
	},
	{ 	
		
		//今日の日付に合わせて下さい。
		L"\xC624\xB298\x0020\xB0A0\xC9DC\xB85C\x0020\xB9DE\xCD94\xC5B4\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter today's date."
	},
	{ 	
		
		//現在の時刻に合わせて下さい。
		L"\xD604\xC7AC\x0020\xC2DC\xAC01\xC73C\xB85C\x0020\xB9DE\xCD94\xC5B4\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter the current time."
	},
	{ 	
		
		//"アラームアイコン（）をタッチすると
		//設定した時刻にアラームが鳴ります。"
		L"\xC54C\xB78C\x0020\xC544\xC774\xCF58\xFF08\xE007\xFF09\xC744\x0020\xD130\xCE58\xD558\xBA74\x000A\xC124\xC815\xD55C\x0020\xC2DC\xAC01\xC5D0\x0020\xC54C\xB78C\xC774\x0020\xC6B8\xB9BD\xB2C8\xB2E4\x002E",
		L"no font",
		L"Tap the alarm icon () to have the\nalarm sound at the designated time."
	},
	{ 	
		
		//ユーザー情報を登録します。
		L"\xC720\xC800\x0020\xC815\xBCF4\xB97C\x0020\xB4F1\xB85D\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"Enter user information."
	},
	{ 	
		
		//ニックネームなどを入力して下さい。
		L"\xB2C9\xB124\xC784\x0020\xB4F1\xC744\x0020\xC785\xB825\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter your nickname."
	},
	{ 	
		
		//コメントを入力して下さい。
		L"\xCF54\xBA58\xD2B8\xB97C\x0020\xC785\xB825\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter a personal message."
	},
	{ 	
		
		//好きな色を選択して下さい。
		L"\xC88B\xC544\xD558\xB294\x0020\xC0C9\xC744\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select your favorite color."
	},
	{ 	
		
		//誕生日を入力して下さい。
		L"\xC0DD\xC77C\xC744\x0020\xC785\xB825\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Enter your birthday."
	},
	{ 	
		//"タッチ位置のずれを補正します。
		//タッチペンでをタッチして下さい。"
		L"\xD130\xCE58\x0020\xC704\xCE58\xC758\x0020\xC624\xCC28\xB97C\x0020\xC218\xC815\xD569\xB2C8\xB2E4\x002E\x000A\xD130\xCE58\xD39C\xC73C\xB85C\x0020\x0020\xE014\x0020\xC744\x0020\xD130\xCE58\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"To calibrate the Touch Screen, tap the\n"
		L"center of the  marks with the stylus."
	},
	{ 	
		
		//「下画面」に設定しました。
		L"\x300C\xC544\xB798\x0020\x0020\xD654\xBA74\x300D\xC73C\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Default GBA screen\n"
		L"set to Touch Screen."
	},
	{ 	
		
		//「タッチスクリーン」を補正しました。
		L"\x300C\xD130\xCE58\xC2A4\xD06C\xB9B0\x300D\xC744\x0020\xC218\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The Touch Screen has\n"
		L"been calibrated."
	},
	{ 	
		
		//「上画面」に設定しました。
		L"\x300C\xC704\x0020\xD654\xBA74\x300D\xC73C\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Default GBA screen\n"
		L"set to Top Screen."
	},
	{ 	
		
		//「オートモード」に設定しました。
		L"\x300C\xC624\xD1A0\x0020\xBAA8\xB4DC\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"System start-up set to Auto."
	},
	{ 	
		
		//「マニュアルモード」に設定しました。
		L"\x300C\xB9E4\xB274\xC5BC\x0020\xBAA8\xB4DC\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"System start-up set to Manual."
	},
	{ 	
		
		//「えいご」に設定しました。
		L"\x300C\xC601\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to English."
	},
	{ 	
		
		//「フランスご」に設定しました。
		L"\x300C\xD504\xB791\xC2A4\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to French."
	},
	{ 	
		
		//「ドイツご」に設定しました。
		L"\x300C\xB3C5\xC77C\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to German."
	},
	{ 	
		
		//「イタリアご」に設定しました。
		L"\x300C\xD55C\xAD6D\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to Italian."
	},
	{ 	
		
		//「日本語」に設定しました。
		L"\x300C\xC77C\xBCF8\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"no font"
	},
	{ 	
		
		//「スペインご」に設定しました。
		L"\x300C\xC2A4\xD398\xC778\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Language set to Spanish."
	},
	{ 	
		
		//「日付」を設定しました。
		L"\x300C\xB0A0\xC9DC\x300D\xB97C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Date setting saved."
	},
	{ 	
		
		//「時刻」を設定しました。
		L"\x300C\xC2DC\xAC01\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Time setting saved."
	},
	{ 	
		
		//「アラーム」を設定しました。
		L"\x300C\xC54C\xB78C\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Alarm setting saved."
	},
	{ 	
		
		//「ユーザーネーム」を設定しました。
		L"\x300C\xC720\xC800\x0020\xB124\xC784\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"User Name saved."
	},
	{ 	
		
		//「コメント」を設定しました。
		L"\x300C\xCF54\xBA58\xD2B8\x300D\xB97C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Message saved."
	},
	{ 	
		
		//「ユーザーカラー」を設定しました。
		L"\x300C\xC720\xC800\x0020\xCEEC\xB7EC\x300D\xB97C\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Color setting saved."
	},
	{ 	
		
		//「誕生日」を設定しました。
		L"\x300C\xC0DD\xC77C\x300D\xC744\x0020\xC124\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Birthday setting saved."
	},
	{ 	
		
		//下画面
		L"\xC544\xB798\x0020\x0020\xD654\xBA74",
		L"no font",
		L"Touch\n"
		L"Screen"
	},
	{ 	
		
		//上画面
		L"\xC704\x0020\xD654\xBA74",
		L"no font",
		L"Top\n"
		L"Screen"
	},
	{ 	
		
		//"電源を入れた後
		//DSメニューで何をするか
		//選択できます。"
		L"\xC804\xC6D0\xC744\x0020\xB123\xC740\x0020\xD6C4\x000A\x0044\x0053\x0020\xBA54\xB274\xC5D0\xC11C\x0020\xBB34\xC5C7\xC744\x0020\xD560\xC9C0\x000A\xC120\xD0DD\xD560\x0020\xC218\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"The Main Menu will appear\n"
		L"automatically when you turn\n"
		L"the power on."
	},
	{ 	
		
		// オートモード
		L"\xE02D\x0020\xC624\xD1A0\x0020\xBAA8\xB4DC",
		L"\xE02D\x0020オートモード",
		L"\xE02D\x0020Auto Mode"
	},
	{ 	
		
		//"電源を入れた後
		//じょうきょうに応じて
		//自動でソフトが起動します。"
		L"\xC804\xC6D0\xC744\x0020\xB123\xC740\x0020\xD6C4\x000A\xC0C1\xD669\xC5D0\x0020\xB530\xB77C\xC11C\x000A\xC790\xB3D9\xC73C\xB85C\x0020\x0020\xC18C\xD504\xD2B8\xAC00\x0020\xC2E4\xD589\xB429\xB2C8\xB2E4\x002E",
		L"no font",
		L"If a DS Card or a Game Pak is\n"
		L"inserted when you power up, the\n"
		L"game will begin automatically."
	},
	{ 	
		
		// マニュアルモード
		L"\xE02E\x0020\xB9E4\xB274\xC5BC\x0020\xBAA8\xB4DC",
		L"\xE02E\x0020マニュアルモード",
		L"\xE02E\x0020Manual Mode"
	},
	{ 	
		//English
		L"\x0045\x006E\x0067\x006C\x0069\x0073\x0068",
		L"\x0045\x006E\x0067\x006C\x0069\x0073\x0068",
		L"\x0045\x006E\x0067\x006C\x0069\x0073\x0068",
	},
	{ 	
		//Francais
		L"\x0046\x0072\x0061\x006E\x00E7\x0061\x0069\x0073",
		L"\x0046\x0072\x0061\x006E\x00E7\x0061\x0069\x0073",
		L"\x0046\x0072\x0061\x006E\x00E7\x0061\x0069\x0073",
	},
	{ 	
		//Deutsch
		L"\x0044\x0065\x0075\x0074\x0073\x0063\x0068",
		L"\x0044\x0065\x0075\x0074\x0073\x0063\x0068",
		L"\x0044\x0065\x0075\x0074\x0073\x0063\x0068",
	},
	{ 	
		
		//\ﾕmｭｴﾅ
		L"\xD55C\xAD6D\xC5B4",
		L"\xD55C\xAD6D\xC5B4",
		L"\xD55C\xAD6D\xC5B4",
	},
	{ 	
		//日本語
		L"no font",
		L"no font",
		L"no font",
	},
	{ 	
		//Espanol
		L"\x0045\x0073\x0070\x0061\x00F1\x006F\x006C",
		L"\x0045\x0073\x0070\x0061\x00F1\x006F\x006C",
		L"\x0045\x0073\x0070\x0061\x00F1\x006F\x006C",
	},
	{ 	
		//せんたくカラ-
		L"\xC120\xD0DD\x0020\xCEEC\xB7EC",
		L"せんたくカラ-",
		L"Choose Color"
	},
	{ 	
		//"補正しました。
		//をタッチして確認して下さい。"
		L"\xC218\xC815\xD588\xC2B5\xB2C8\xB2E4\x002E\x000A\xE014\x0020\xC744\x0020\xD130\xCE58\xD574\xC11C\x0020\xD655\xC778\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Calibration complete. Touch the\n"
		L" marks to test the calibration."
	},
	
	{ 	
		
		//ボタンで補正をキャンセルできます。
		L"\xE001\x0020\xBC84\xD2BC\xC73C\xB85C\x0020\xC218\xC815\xC744\x0020\x000A\xCDE8\xC18C\xD560\x0020\xC218\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E",
		L"no font",
		L"Press  to cancel."
	},
	{ 	
		
		//電源を切って終了します。
		L"\xC804\xC6D0\xC744\x0020\xB044\xACE0\x0020\xC885\xB8CC\xD569\xB2C8\xB2E4\x002E",
		L"no font",
		L"The system will now shut down."
	},
	{ 	
		
		//"入力が終了しました。
		//設定を保存して電源を切ります。"
		L"\xC785\xB825\xC774\x0020\xC885\xB8CC\xB418\xC5C8\xC2B5\xB2C8\xB2E4\x002E\x000A\xC124\xC815\xC744\x0020\xC800\xC7A5\xD558\xACE0\x0020\xC804\xC6D0\xC744\x0020\xB055\xB2C8\xB2E4\x002E",
		L"no font",
		L"In order for the new settings\n"
		L"to take effect, you must restart\n"
		L"your system. Shut down now?"
	},
	{ 	
		
		//"初めに本体の設定をしましょう。
		//画面をタッチするか
		//何かボタンを押して下さい。"
		L"\xC6B0\xC120\x0020\xBCF8\xCCB4\x0020\xC124\xC815\xC744\x0020\xD569\xC2DC\xB2E4\x002E\x000A\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xC544\xBB34\x0020\xBC84\xD2BC\xC774\xB098\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Before you begin, you must adjust\n"
		L"system settings. Please touch the\n"
		L"Touch Screen or press any button.\n"
	},
	{ 	
		//"本体の設定を確認して下さい。
		//画面をタッチするか
		//何かボタンを押して下さい。"
		L"\xBCF8\xCCB4\x0020\xC124\xC815\xC744\x0020\xD655\xC778\xD574\x0020\xC8FC\xC138\xC694\x002E\x000A\xD654\xBA74\xC744\x0020\xD130\xCE58\xD558\xAC70\xB098\x000A\xC544\xBB34\x0020\xBC84\xD2BC\xC774\xB098\x0020\xB20C\xB7EC\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Confirm your system settings.\n"
		L"Touch the Touch Screen\n"
		L"or press any button."
	},
	{ 	
		
		//"「日本語」に設定します。
		//よろしいですか？"
		L"\x300C\xD55C\xAD6D\xC5B4\x300D\xB85C\x0020\xC124\xC815\xD558\xACA0\xC2B5\xB2C8\xAE4C\xFF1F",
		L"no font",
		L"This will change your settings to\n"
		L"English. Is this OK?"
	},
	{ 	
		
		//使用する言語を選択して下さい。
		L"\xC0AC\xC6A9\xD560\x0020\xC5B8\xC5B4\xB97C\x0020\xC120\xD0DD\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"Select the language to use."
	},
	{ 	
		
		//設定が消えた可能性があります。をタッチして下さい。
		L"\xC124\xC815\xC774\x0020\xC9C0\xC6CC\xC84C\xC744\x0020\xAC00\xB2A5\xC131\xC774\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x0020\x000A\xE014\x0020\xC744\x0020\xD130\xCE58\xD574\x0020\xC8FC\xC138\xC694\x002E",
		L"no font",
		L"System settings have been lost. Tap ."
	},
	{ 	
		
		//0:00
		L"0:00",
		L"0:00",
		L"0:00",
	},
	{ 	
		
		//MM/DD
		L"MM/DD",
		L"MM/DD",
		L"MM/DD",
	},
	{ 	
		
		//-------------
		L"-------------",
		L"-------------",
		L"-------------",
	},
	{ 	
		
		//金
		L"\xAE08",
		L"no font",
		L"Fr",
	},
	{ 	
		
		//月
		L"\xC6D4",
		L"no font",
		L"Mo"
	},
	{ 	
		
		//土
		L"\xD1A0",
		L"no font",
		L"Sa"
	},
	{ 	
		
		//日
		L"\xC77C",
		L"no font",
		L"Su"
	},
	{ 	
		
		//木
		L"\xBAA9",
		L"no font",
		L"Th"
	},
	{ 	
		//火
		L"\xD654",
		L"no font",
		L"Tu"
	},
	{ 	
		
		//水
		L"\xC218",
		L"no font",
		L"We"
	},
	{ 	
		//YYYY/MM
		L"YYYY/MM",
		L"YYYY/MM",
		L"MM/YYYY",
	},
	{ 	
		//YYYY/MM/DD
		L"YYYY/MM/DD",
		L"YYYY/MM/DD",
		L"MM/DD/YYYY",
	},
	{ 	
		//キャンセル
		L"\xCDE8\xC18C",
		L"キャンセル",
		L"Cancel"
	},
	{ 	
		//決定
		L"\xACB0\xC815",
		L"no font",
		L"Confirm"
	},
	{ 	
		//"DSワイヤレス通信を開始します。
		//よろしいですか？"
		L"\x0044\x0053\x0020\xBB34\xC120\x0020\xD1B5\xC2E0\xC744\x0020\xC2DC\xC791\xD558\xACA0\xC2B5\xB2C8\xAE4C\xFF1F",
		L"no font",
		L"Resume communication?"
	},
	{ 	
		//キャンセル
		L"\xCDE8\xC18C",
		L"キャンセル",
		L"Cancel"
	},
	{ 	
		//決定
		L"\xACB0\xC815",
		L"no font",
		L"Confirm"
	},
	{ 	
		//警告-健康と安全のために
		L"\xACBD\xACE0\x0020\x2013\x0020\xAC74\xAC15\xACFC\x0020\xC548\xC804\xC744\x0020\xC704\xD558\xC5EC",
		L"no font",
		L"WARNING - HEALTH AND SAFETY"
	},
	{ 	
		//"遊ぶまえに取扱説明書の「安全に使用していただく
		//ために…」をお読みください。ここには、あなたの
		//健康と安全のための大切な内容が書かれています。"
		L"\xC0AC\xC6A9\xD558\xAE30\x0020\x0020\xC804\xC5D0\x0020\x0020\xC0AC\xC6A9\xC124\xBA85\xC11C\xC758\x0020\x0020\x300C\xC548\xC804\xC744\x000A\xC704\xD55C\x0020\x0020\xC8FC\xC758\xC0AC\xD56D\x300D\xC744\x0020\x0020\xC77D\xC5B4\x0020\x0020\xC8FC\xC2ED\xC2DC\xC624\x002E\x000A\xC5EC\xAE30\xC5D0\xB294\x0020\x0020\xB2F9\xC2E0\xC758\x0020\x0020\xAC74\xAC15\xACFC\x0020\x0020\xC548\xC804\xC744\x0020\x0020\xC704\xD55C\x000A\xC911\xC694\xD55C\x0020\x0020\xB0B4\xC6A9\xC774\x0020\x0020\xC801\xD600\x0020\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x000A",
		L"no font",
		L"Before playing, read the health\nand safety precautions booklet\nfor important information \nabout your health and safety."
	},
	{ 	
		//"この内容はホームページでも見ることができます。
		//www.nintendo.co.jp/anzen/"
		L"\xC774\x0020\xB0B4\xC6A9\xC740\x0020\xD648\xD398\xC774\xC9C0\xC5D0\xC11C\xB3C4\x0020\xBCFC\x0020\xC218\x0020\xC788\xC2B5\xB2C8\xB2E4\x002E\x000A\x0077\x0077\x0077\x002E\x006E\x0069\x006E\x0074\x0065\x006E\x0064\x006F\x0063\x0061\x0075\x0074\x0069\x006F\x006E\x002E\x0063\x006F\x002E\x006B\x0072",
		L"no font",
		L"\x0054\x004F\x0020\x0047\x0045\x0054\x0020\x0041\x004E\x0020\x0045\x0058\x0054\x0052\x0041\x0020\x0043\x004F\x0050\x0059\x0020\x0046\x004F\x0052\x0020\x0059\x004F\x0055\x0052\x0020\x0052\x0045\x0047\x0049\x004F\x004E\x002C\x0020\x0047\x004F\x0020\x004F\x004E\x004C\x0049\x004E\x0045\x0020\x0041\x0054\x000A\x0077\x0077\x0077\x002E\x006E\x0069\x006E\x0074\x0065\x006E\x0064\x006F\x002E\x0063\x006F\x006D\x002F\x0068\x0065\x0061\x006C\x0074\x0068\x0073\x0061\x0066\x0065\x0074\x0079\x00\x2F"
	},
	{ 	
		//続けるには画面をタッチしてください。
		L"\xACC4\xC18D\xD558\xB824\xBA74\x0020\xD654\xBA74\xC744\x0020\xD130\xCE58\xD574\x0020\xC8FC\xC2ED\xC2DC\xC624\x002E",
		L"no font",
		L"Touch the Touch Screen to continue"
	},

};

const int g_TextTable_Num = (sizeof(g_TextTable)/sizeof(g_TextTable[0]));

