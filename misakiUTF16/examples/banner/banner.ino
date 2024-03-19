//
// 美咲フォントライブラリサンプルプログラム by たま吉さん
//  2016/03/15 作成
//  2016/07/10 UTF-8文字列から直接フォントデータを取得するように修正
//  2024/03/18 半角文字の4ドット幅を考慮した表示に修正
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
void banner(char* pUTF8, char* fore, char* back) {
  byte buf[8*20];    // 160x8ドットのバナー表示パターン
  uint8_t font[8];   // フォントデータ
  uint8_t width;     // フォント幅
  
  // バナー用パターン作成
  uint8_t n=0;
  while(*pUTF8) {
    uint16_t ucode;  // utf16文字コード

    pUTF8 += charUFT8toUTF16(&ucode, pUTF8);  // utf8 1文字分をutf16に変換
    width = isZenkaku(ucode) ? 8: 4;          // フォント幅の取得
    getFontDataByUTF16(font, ucode);          // フォントデータの取得

    // 表示用バッファにデータの格納
    for (uint8_t w = 0; w < width; w++) {
      uint8_t data = 0;
      for (uint8_t row=0; row<8; row++)
          if (font[row] & (0x80>>w))
              data |= (0x80>>row);
      buf[n++] = data;
    }
  }

  // 表示用バッファの表示
  for (uint8_t row=0; row < 8; row++) {
    for(uint8_t i=0; i < n; i++) {
        Serial.print(buf[i] & (0x80>>row) ? fore : back);
    }
    Serial.println();
  }
}

void setup() {
    Serial.begin(115200);
    banner("埼玉☆さいたま ｻｲﾀﾏ!", "##", "  ");
    banner("misakiUTF16ﾗｲﾌﾞﾗﾘ","%%", "  ");
    banner("あいうえお", "##", "  ");
    banner("みさきフォント", "@", " ");
}

void loop() {
}
