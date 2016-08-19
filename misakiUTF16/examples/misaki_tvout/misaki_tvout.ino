//
// TVout with 美咲フォント  
// 2016/07/10 たま吉さん 
// 2016/08/19 ライブラリの仕様変更対応
//
 
#include <misakiUTF16.h>
#include <TVout.h>

TVout TV;

// 指定位置に１文字表示
void mputc(uint8_t x, uint8_t y, uint16_t code) {
  int16_t pos; // フォントデータテーブルコード
  if ( (pos = findcode(code)) < 0) 
    return;
  TV.bitmap(x,y, getFontTableAddress()+pos*8 ,0,8,8); 
}

// 指定位置に文字列表示
void mprint(uint8_t x, uint8_t y, char* str) {
  uint16_t utf16;
  int8_t   len;
  
  while(1) {
    len = charUFT8toUTF16(&utf16, str); // 先頭文字のutf16コードの取得
    if (!len) 
      break;  // コードエラー   
    mputc(x, y, utf16); // 1文字表示
    x+=8;
    if (x >= 120) {
      x = 0; y+=8;
    }
    str+=len;
    if (!*str)
      break; // 文末に達した
  }
}

void setup() {
  TV.begin(NTSC, 120, 96);
  mprint(0,0,"【みさきフォント表示テスト】");
  mprint(15,14,"今日は３月１６日です。");
  mprint(5,30,"ねこにコ・ン・バ・ン・ワ");  
  mprint(0,45,"ＡｒｄｕｉｎｏでＴＶ出力です。");  
  mprint(0,60,"ＴＶＯＵＴライブラリ使用です。");
  mprint(0,75,"そこそこ読めますね！");  
}

void loop() {
}
