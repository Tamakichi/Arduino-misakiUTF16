// 
// misakiUTF16.h 美咲フォントドライバ ヘッダーファイル v1.1B by たま吉さん 2016/03/15
// 内部フラッシュメモリバージョン
//
// 2016/03/16 全角小文字英数字の不具合対応
// 2016/07/05 getFontData()関数の追加
//

#ifndef misakiUTF16_h
#define misakiUTF16_h

#include <avr/pgmspace.h>
#include <arduino.h>

#define FTABLESIZE     1710      // フォントテーブルデータサイズ
#define FONT_LEN       8         // 1フォントのバイト数

extern PROGMEM const uint8_t fdata[];						// フォントデータ
extern PROGMEM const uint16_t ftable [];					// インデックステーブル

int findcode(uint16_t  ucode) ;								// フォントコード検索
boolean getFontDataByUTF16(byte* fontdata, uint16_t utf16) ;// UTF16に対応する美咲フォントデータ8バイトを取得
uint16_t hkana2kana(uint16_t ucode);						// 半角カナを全角に変換
uint16_t utf16_HantoZen(uint16_t utf16); 					// UTF16半角コードをUTF16全角コードに変換
byte charUFT8toUTF16(char *pUTF8, uint16_t *pUTF16);		// UTF8文字(1～3バイ)をUTF16に変換
byte Utf8ToUtf16(uint16_t* pUTF16, char *pUTF8);			// UTF8文字列をUTF16文字列に変換

char* getFontData(byte* fontdata,char *pUTF8);              // 指定したUTF8文字列の先頭のフォントデータの取得

#endif