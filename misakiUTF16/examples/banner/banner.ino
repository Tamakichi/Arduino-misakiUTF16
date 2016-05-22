//
// 美咲フォントライブラリサンプルプログラム by たま吉さん
//  2016/03/15

#include <misakiUTF16.h>

// ビットパターン表示
// d: 8ビットパターンデータ
//
void bitdisp(uint8_t d, char*fore , char* back) {
  for (byte i=0; i<8;i++) {
    if (d & 0x80>>i) 
      Serial.print(fore);
    else
      Serial.print(back);
  }
}

// フォントデータの表示
// c(in) : フォントコード(UTF16コード)
//
void fontdisp(uint16_t c) {
  byte buf[8]; 
  
  getFontDataByUTF16(buf, c);  // フォントデータの取得   
  Serial.println((unsigned short)c, HEX);  // UTF16コード表示

  for (byte i = 0; i < 8; i++) {
    bitdisp(buf[i],"##","  ");
    Serial.println("");
  }  
  Serial.println("");

} 

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
  int n;
  uint16_t pUTF16[40];
  byte buf[20][8];  //160x8ドットのバナー表示パターン
  n = Utf8ToUtf16(pUTF16, pUTF8);  // UTF8からUTF16に変換する

  // バナー用パターン作成
  for (byte i=0; i < n; i++) {
    getFontDataByUTF16(&buf[i][0], utf16_HantoZen(pUTF16[i]));  // フォントデータの取得    
  }
  
  // バナー表示
  for (byte i=0; i < 8; i++) {
    for (byte j=0; j < n; j++)
        bitdisp(buf[j][i],fore, back);
    Serial.println("");    
  }
   Serial.println("");
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



