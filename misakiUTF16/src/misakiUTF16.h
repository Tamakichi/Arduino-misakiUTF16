// 
// misakiUTF16.h 美咲フォントドライバ ヘッダーファイル v1.1B by たま吉さん 2016/03/15
// 内部フラッシュメモリバージョン
//
// 2016/03/16 全角小文字英数字の不具合対応
// 2016/07/05 getFontData()関数の追加
// 2016/07/10 getFontTableAddress()関数の追加, fdata[],ftable[]の公開禁止
// 2016/08/07 getFontData()に半角=>全角変換指定を追加
// 2016/08/19 charUFT8toUTF16()の引数を変更
// 2019/01/30 Utf8ToUtf16()の戻り値の型をbyteからint16_tに変更
// 2019/07/16 FTABLESIZEを実配列から算出に変更
// 2024/03/15 isBasicLatin(),isLatinSupple(),LatenS2Zen(),isZenkaku()の追加
//

#ifndef misakiUTF16_h
#define misakiUTF16_h

#include <Arduino.h>

#define FTABLESIZE     (sizeof ftable / sizeof ftable[0])     // フォントテーブルデータサイズ
#define FONT_LEN       7                                      // 1フォントのバイト数
#define FONT_TOFU      0x25a1                                 // 豆腐"□"コード

int findcode(uint16_t  ucode) ;                               // フォントコード検索
boolean getFontDataByUTF16(byte* fontdata, uint16_t utf16) ;  // UTF16に対応する美咲フォントデータ8バイトを取得
uint16_t hkana2kana(uint16_t utf16);                          // 半角カナを全角に変換
uint16_t utf16_HantoZen(uint16_t utf16);                      // UTF16半角コードをUTF16全角コードに変換
byte charUFT8toUTF16(uint16_t *pUTF16, char *pUTF8);          // UTF8文字(1～3バイ)をUTF16に変換
int16_t Utf8ToUtf16(uint16_t* pUTF16, char *pUTF8);           // UTF8文字列をUTF16文字列に変換

char* getFontData(byte* fontdata,char *pUTF8,bool h2z=false); // フォントデータ取得
const uint8_t*  getFontTableAddress();                        // フォントデータテーブル先頭アドレス取得

boolean isBasicLatin(uint16_t ucode);                         // 基本ラテン文字判定
boolean isLatinSupple(uint16_t ucode);                        // ラテン1補助判定
uint16_t LatenS2Zen(uint16_t ucode);                          // 半角ラテン1補助全角文字変換
boolean isZenkaku(uint16_t ucode);                            // 全角・半角判定
#endif
