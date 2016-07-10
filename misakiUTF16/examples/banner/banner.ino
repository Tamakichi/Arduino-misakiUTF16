//
// 美咲フォントライブラリサンプルプログラム by たま吉さん
//  2016/03/15 作成
//  2016/07/10 UTF-8文字列から直接フォントデータを取得するように修正
//

#include <misakiUTF16.h>

//
// フォントパターンをコンソールに表示する
// 引数
//  pUTF8 表示する文字列
//  fore  ドットに使用するキャラクタ
//  back  空白に使用するキャラクタ
//
// ※半角文字は全角文字に置き換えを行う
//
void banner(char * pUTF8, char* fore, char* back) {
  int n=0;
  byte buf[20][8];  //160x8ドットのバナー表示パターン

  // バナー用パターン作成
  while(*pUTF8)
    pUTF8 = getFontData(&buf[n++][0], pUTF8);  // フォントデータの取得
    
  // バナー表示
  for (byte i=0; i < 8; i++) {
    for (byte j=0; j < n; j++) 
        for (byte k=0; k<8;k++)
          Serial.print(bitRead(buf[j][i],7-k) ? fore: back);
    Serial.println();    
  }
  Serial.println();
}

void setup() {
    Serial.begin(115200);
    banner("abcdef","$"," ");
    banner("misakiUTF16ﾗｲﾌﾞﾗﾘ","$"," ");    
    banner("あいうえお","##","  ");
    banner("埼玉☆さいたま","[]","  ");
    banner("みさきフォント","@"," ");    
}

void loop() {
}



