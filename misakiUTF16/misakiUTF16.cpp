// 
// 美咲フォントドライバ v1.1B by たま吉さん 2016/03/14
// 内部フラッシュメモリバージョン
//
// 2016/03/16 全角小文字英数字の不具合対応
// 2016/07/05 getFontData()関数の追加
//


#include <avr/pgmspace.h>
#include <arduino.h>
#include "misakiUTF16.h"
#include "misakiUTF16FontData.inc"

// 半角カナ全角変換テーブル
static uint8_t _hkremap [] = {
   0x02,0x0C,0x0D,0x01,0xFB,0xF2,0xA1,0xA3,0xA5,0xA7,0xA9,0xE3,0xE5,0xE7,0xC3,0xFD,
   0xA2,0xA4,0xA6,0xA8,0xAA,0xAB,0xAD,0xAF,0xB1,0xB3,0xB5,0xB7,0xB9,0xBB,0xBD,0xBF,
   0xC1,0xC4,0xC6,0xC8,0xCA,0xCB,0xCC,0xCD,0xCE,0xCF,0xD2,0xD5,0xD8,0xDB,0xDE,0xDF,
   0xE0,0xE1,0xE2,0xE4,0xE6,0xE8,0xE9,0xEA,0xEB,0xEC,0xED,0xEF,0xF3,0x9B,0x9C
};

// nバイト読込
// rcvdata: 読込データ格納アドレス
// n      : 読込みデータ数
// 戻り値  : 読み込んだバイト数
//
byte Sequential_read(unsigned long address, byte* rcvdata, byte n)  {
  for (int i=0; i<n;i++) 
    rcvdata[i] = pgm_read_byte(address+fdata+i);
 return n;
}

// フォントコード検索
// (コードでROM上のテーブルを参照し、フォントコードを取得する)
// ucode(in) UTF-16 コード
// 戻り値    該当フォントがある場合 フォントコード(0-FTABLESIZE)
//           該当フォントが無い場合 -1

int findcode(uint16_t  ucode)  {
 int  t_p = 0;            //　検索範囲上限
 int  e_p = FTABLESIZE-1; //  検索範囲下限
 int  pos;
 uint16_t  d = 0;
 int flg_stop = -1;
 
 while(true) {
    pos = t_p + (e_p - t_p+1)/2;
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

// 半角カナコード判定
uint8_t isHkana(uint16_t ucode) {
if (ucode >=0xFF61 && ucode <= 0xFF9F)
   return 1;
else 
  return 0;  
}

// 半角カナ全角変換
uint16_t hkana2kana(uint16_t ucode) {
if (isHkana(ucode))
   return _hkremap[ucode-0xFF61] + 0x3000;
return ucode;
}

//
// UTF16に対応する美咲フォントデータ8バイトを取得する
//   data(out): フォントデータ格納アドレス
//   utf16(in): UTF16コード
//   戻り値: true 正常終了１, false 異常終了
//
boolean getFontDataByUTF16(byte* fontdata, uint16_t utf16) {
  uint16_t code;
  unsigned long addr;
  byte n;
 
  //utf16 = hkana2kana(utf16);	// 半角カナは全角カナを利用する
	
  if ( 0 > (code  = findcode(utf16))) { 
    // 該当するフォントが存在しない
    return false;
  }
  
  addr = code;
  addr<<=3;
  n =  Sequential_read(addr, fontdata, (byte)FONT_LEN);
  if (n!=8) {
    return false;
  }
  return true;
}



//
// UTF16半角コードをUTF16全角コードに変換する
// (変換できない場合は元のコードを返す)
//   utf16(in): UTF16文字コード
//   戻り値: 変換コード
uint16_t utf16_HantoZen(uint16_t utf16) {
  
  utf16 = hkana2kana(utf16);

  if (utf16 > 0xff || utf16 < 0x21 ) {
    return utf16;
  }

	
  switch(utf16) {
    case 0x005C:
    case 0x00A2:
    case 0x00A3:
    case 0x00A7:
    case 0x00A8:
    case 0x00AC:
    case 0x00B0:
    case 0x00B1:
    case 0x00B4:
    case 0x00B6:
    case 0x00D7:
    case 0x00F7: return utf16;
    case 0x00A5: return 0xFFE5;

    case 0x0021: return 0xFF01;
    case 0x0022: return 0x201D; 
    case 0x0023: return 0xFF03;
    case 0x0024: return 0xFF04;
    case 0x0025: return 0xFF05;
    case 0x0026: return 0xFF06;
    case 0x0027: return 0x2019;
    case 0x0028: return 0xFF08;
    case 0x0029: return 0xFF09;
    case 0x002A: return 0xFF0A;
    case 0x002B: return 0xFF0B;
    case 0x002C: return 0xFF0C;
    case 0x002D: return 0x2212;
    case 0x002E: return 0xFF0E;    
  }  
   return  utf16 - 0x2F +  0xFF0F;     
}

//
// UTF8文字(1～3バイト)をUTF16に変換する
// pUTF8(in):   UTF8文字列格納アドレス
// pUTF16(out): UTF16文字列格納アドレス
// 戻り値: 変換処理したUTF8文字バイト数

byte charUFT8toUTF16(char *pUTF8, uint16_t *pUTF16) { 
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
byte Utf8ToUtf16(uint16_t* pUTF16, char *pUTF8) {
  int len = 0;
  int n;
  uint16_t wstr;

  while (*pUTF8) {
    n = charUFT8toUTF16(pUTF8, pUTF16);
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
//   戻り値   : 次の文字列位置、取得失敗の場合NULLを返す
//
char* getFontData(byte* fontdata,char *pUTF8) {
  uint16_t utf16;
  uint8_t  n;
  if (pUTF8 == NULL)
    return NULL;
  if (*pUTF8 == 0) 
    return NULL;   
  n = charUFT8toUTF16(pUTF8, &utf16);
  if (n == 0)
    return NULL;  
  if (false == getFontDataByUTF16(fontdata, utf16) ) 
    return NULL;
  return (pUTF8+n);
}
