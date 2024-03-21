// 
// 美咲フォントドライバ v1.1B by たま吉さん 2016/03/14
// 内部フラッシュメモリバージョン
//
// 2016/03/16 全角小文字英数字の不具合対応
// 2016/07/05 getFontData()関数の追加
// 2016/07/10 getFontTableAddress()関数の追加
// 2016/08/07 getFontData()に半角=>全角変換指定を追加
// 2016/08/19 charUFT8toUTF16()の引数を変更
// 2016/09/30 半角カナ全角変換テーブルをフラッシュメモリ配置に変更
// 2016/12/15 findcode()の不具合対応(flg_stopの初期値を-1から0に訂正)
// 2016/12/18 getFontDataByUTF16()で未登録フォント指定時に豆腐(□:0x25a1)を返すように修正
// 2019/01/30 _hkremap[]の非AVR系対応
// 2019/01/30 Utf8ToUtf16()の戻り値の型をbyteからint16_tに変更
// 2019/07/16 ファイル名misakiUTF16FontData.inc misakiUTF16FontData.hに変更
// 2019/07/16 1フォントを7バイトに圧縮
// 2022/11/24 utf16_HantoZen()の半角スペース対応漏れの不具合対応
// 2024/03/15 isBasicLatin(),isLatinSupple(),LatenS2Zen(),isZenkaku()の追加

#include <Arduino.h>
#ifdef __AVR__
#include <avr/pgmspace.h>
#endif  // __AVR__
#include "misakiUTF16.h"
#include "misakiUTF16FontData.h"

// 半角カナ全角変換テーブル
#ifdef __AVR__
PROGMEM static const uint8_t _hkremap[] = {
#else  // __AVR__
static const uint8_t _hkremap[] = {
#endif  // __AVR__
  0x02,0x0c,0x0d,0x01,0xfb,0xf2,0xa1,0xa3,
  0xa5,0xa7,0xa9,0xe3,0xe5,0xe7,0xc3,0xfc,
  0xa2,0xa4,0xa6,0xa8,0xaa,0xab,0xad,0xaf,
  0xb1,0xb3,0xb5,0xb7,0xb9,0xbb,0xbd,0xbf,
  0xc1,0xc4,0xc6,0xc8,0xca,0xcb,0xcc,0xcd,
  0xce,0xcf,0xd2,0xd5,0xd8,0xdb,0xde,0xdf,
  0xe0,0xe1,0xe2,0xe4,0xe6,0xe8,0xe9,0xea,
  0xeb,0xec,0xed,0xef,0xf3,0x9b,0x9c,
};

// nバイト読込
// rcvdata: 読込データ格納アドレス
// n      : 読込みデータ数
// 戻り値  : 読み込んだバイト数
//
byte Sequential_read(unsigned long address, byte* rcvdata, byte n)  {
  for (int i = 0; i < n ; i++) 
    rcvdata[i] = pgm_read_byte(address + fdata + i);
 return n;
}

// フォントコード検索
// (コードでROM上のテーブルを参照し、フォントコードを取得する)
// ucode(in) UTF-16 コード
// 戻り値    該当フォントがある場合 フォントコード(0-FTABLESIZE)
//           該当フォントが無い場合 -1

int findcode(uint16_t ucode)  {
 int  t_p = 0;            //　検索範囲上限
 int  e_p = FTABLESIZE-1; //  検索範囲下限
 int  pos;
 uint16_t  d = 0;
 int flg_stop = 0;
 
 while(true) {
    pos = t_p + ((e_p - t_p+1)>>1);
    d = pgm_read_word (ftable+pos);
   if (d == ucode) {
     // 等しい
     flg_stop = 1;
     break;
   } else if (ucode > d) {
     // 大きい
     t_p = pos + 1;
     if (t_p > e_p) {
       break;
     }
   } else {
     // 小さい
    e_p = pos -1;
    if (e_p < t_p) 
      break;
   }
 } 

 if (!flg_stop) {
    return -1;
 }
 return pos;    
}

// 基本ラテン文字判定
//    指定した文字コードが0x20～0x7eの場合true、そうでない場合falseを返す
//    引数 ucode: UTF-16 コード
boolean isBasicLatin(uint16_t ucode) {
    return (ucode >=0x20) && (ucode <= 0x7e);
}

// ラテン1補助判定
//    指定した文字コードが0x20～0x7eの場合true、そうでない場合falseを返す
//    引数 ucode: UTF-16 コード
boolean isLatinSupple(uint16_t ucode) {
    return (ucode >=0xa1) && (ucode <= 0xff);
}

// 半角カナコード判定
//    引数 ucode: UTF-16 コード
boolean isHkana(uint16_t ucode) {
  return (ucode >=0xFF61) && (ucode <= 0xFF9F);
}

// 半角カナ全角変換
//    引数 ucode: UTF-16 コード
uint16_t hkana2kana(uint16_t ucode) {
  if (isHkana(ucode))
      return pgm_read_byte(_hkremap + ucode - 0xFF61) + 0x3000;
  return ucode;
}

// 半角ラテン1補助全角文字変換(美咲フォント用)  
//    指定した半角ラテン1補助文字に対応する全角文字コードを返す
//    引数 ucode: UTF-16 コード
//    戻り値： 全角カタカナコード（半角ラテン1補助文字コードでない場合はそのまま返す）
//
//    ※本関数は利用しているフォントに依存する('¢', '£', '¥', '¦', '¬', '¯')
//
uint16_t LatenS2Zen(uint16_t ucode) {
  uint16_t c = 0;
  switch (ucode) {
    case 0xa2: c = 0xffe0; break; // '¢'
    case 0xa3: c = 0xffe1; break; // '£'
    case 0xa5: c = 0xffe5; break; // '¥'
    case 0xa6: c = 0xffe4; break; // '¦' 
    case 0xac: c = 0xffe2; break; // '¬'
    case 0xaf: c = 0xffe3; break; // '¯'
    default:   c = ucode;  break;
  }
  return c;
}

//
// UTF16に対応する美咲フォントデータ8バイトを取得する
//   data(out): フォントデータ格納アドレス
//   utf16(in): UTF16コード
//   戻り値: true 正常終了１, false 異常終了
//
boolean getFontDataByUTF16(byte* fontdata, uint16_t utf16) {
  int16_t code;
  boolean rc = false;

  if ( 0 > (code  = findcode(utf16))) { 
    // 該当するフォントが存在しない
    code = findcode(FONT_TOFU);  // add by Tamakichi,2016/12/18
    rc = false;  
  }
  
  if ( FONT_LEN  == Sequential_read((code)*7, fontdata, (byte)FONT_LEN) ) {
     rc =  true;
     fontdata[7]=0; // 8バイト目に0をセット
  }
  return rc;
}

//
// UTF16半角コードをUTF16全角コードに変換する
// (変換できない場合は元のコードを返す)
//   utf16(in): UTF16文字コード
//   戻り値: 変換コード
uint16_t utf16_HantoZen(uint16_t utf16) {
    if (utf16 == 0x20) {               // 全角スペース文字
      return 0x3000;
    } else if(isBasicLatin(utf16)) {   // 基本ラテン文字
      return utf16 - 0x20 + 0xff00;
    } else if (isLatinSupple(utf16)) { // ラテン1補助
      return LatenS2Zen(utf16);
    } else if(isHkana(utf16)) {        // 半角カタカナ
      return hkana2kana(utf16);
    }
    return utf16;
}

/// 全角・半角判定
///
///  引数   ucode UTF-16 コード
///  戻り値: 半角幅 False、全角幅 True
boolean isZenkaku(uint16_t ucode) {
    if (isBasicLatin(ucode))                  // 基本ラテン文字
        return false;
    else if (isLatinSupple(ucode))            // ラテン1補助
        return (LatenS2Zen(ucode) == ucode) ? false : true;
    else if (isHkana(ucode))                  // 半角カタカナ
        return false;
    else if (ucode < 0x20)                    // C0
        return false;        
    else if ((ucode>=0x7f) && (ucode<=0xa0))  // C1
        return false;
    return true;                              // その他
}

//
// UTF8文字(1～3バイト)をUTF16に変換する
// pUTF16(out): UTF16文字列格納アドレス
// pUTF8(in):   UTF8文字列格納アドレス
// 戻り値: 変換処理したUTF8文字バイト数

byte charUFT8toUTF16(uint16_t *pUTF16, char *pUTF8) { 
 byte bytes[3]; 
 uint16_t unicode16; 
 
 bytes[0] = *pUTF8++; 
 if( bytes[0] < 0x80 ) { 
   *pUTF16 = bytes[0]; 
   return(1); 
 } 
 bytes[1] = *pUTF8++; 
 if( bytes[0] >= 0xC0 && bytes[0] < 0xE0 )  { 
   unicode16 = 0x1f&bytes[0]; 
   *pUTF16 = (unicode16<<6)+(0x3f&bytes[1]); 
   return(2); 
 } 
 
 bytes[2] = *pUTF8++; 
 if( bytes[0] >= 0xE0 && bytes[0] < 0xF0 ) { 
   unicode16 = 0x0f&bytes[0]; 
   unicode16 = (unicode16<<6)+(0x3f&bytes[1]); 
   *pUTF16 = (unicode16<<6)+(0x3f&bytes[2]); 
   return(3); 
 } else 
 return(0); 
} 

// UTF8文字列をUTF16文字列に変換する
// pUTF16(out): UFT16文字列
// pUTF8(in):   UTF8文字列
// 戻り値: UFT16文字長さ (変換失敗時は-1を返す)
// 
int16_t Utf8ToUtf16(uint16_t* pUTF16, char *pUTF8) {
  int len = 0;
  int n;

  while (*pUTF8) {
    n = charUFT8toUTF16(pUTF16, pUTF8);
    if (n == 0) 
      return -1;
    
    pUTF8 += n;
    len++;
    pUTF16++;
  }
  return len; 
}

// 指定したUTF8文字列の先頭のフォントデータの取得
//   data(out): フォントデータ格納アドレス
//   utf8(in) : UTF8文字列
//   h3z(in)  : true :半角を全角に変換する false: 変換しない 
//   戻り値   : 次の文字列位置、取得失敗の場合NULLを返す
//
char* getFontData(byte* fontdata,char *pUTF8, bool h2z) {
  uint16_t utf16;
  uint8_t  n;

  if (pUTF8 == NULL)
    return NULL;
  if (*pUTF8 == 0) 
    return NULL;   

  n = charUFT8toUTF16(&utf16, pUTF8);
  if (n == 0)
    return NULL;  
  if (h2z) {
    utf16 = utf16_HantoZen(utf16);
  }
  if (false == getFontDataByUTF16(fontdata, utf16) ) 
    return NULL;
  return (pUTF8+n);
}

// フォントデータテーブル先頭アドレス取得
// 戻り値 フォントデータテーブル先頭アドレス(PROGMEM上)
const uint8_t* getFontTableAddress() {
	return fdata;
}
