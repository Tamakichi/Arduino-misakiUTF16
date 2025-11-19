//
// 美咲フォントライブラリサンプルプログラム by たま吉さん
// 2016/03/15 作成
// 2016/07/10 UTF-8文字列から直接フォントデータを取得するように修正
// 2024/03/18 半角文字の4ドット幅を考慮した表示に修正
// 2025/11/06 符号なし1バイトデータの型をuint8_tに統一
// 2025/11/08 エラー時の処理の追加
// 2025/11/19 ネイティブUSBシリアル対応
//

#include <misakiUTF16.h>
#include <string.h>

// フォントパターンをコンソールに表示する
// 引数
//  pUTF8 表示する文字列
//  fore  ドットに使用するキャラクタ列（できれば同幅の文字列）
//  back  空白に使用するキャラクタ列（できれば fore と同じ長さ）
//
void banner(const char* pUTF8, const char* fore, const char* back) {
  const size_t BUF_COLS = 8 * 20; // 160 列分（必要なら増やす）
  uint8_t buf[BUF_COLS];          // 160x8ドットのバナー表示パターン
  uint8_t font[8];                // フォントデータ（1文字分）
  size_t n = 0;                   // バッファ使用列数

  // バナー用パターン作成
  while (*pUTF8) {
    uint16_t ucode;                               // utf16文字コード
    size_t len = charUTF8toUTF16(&ucode, pUTF8);  // utf8 1文字分をutf16に変換
    if (len == 0) {
        Serial.println();
        Serial.println("未サポート文字が使用されています");
        break;
    }

    pUTF8 += len;
    uint8_t width = isZenkaku(ucode) ? 8 : 4;     // フォント幅の取得

    // バッファオーバーフロー防止
    if (n + width > BUF_COLS)
        // 十分な領域がない -> ここでは切り捨ててループを抜ける
        break;  

    // フォントデータの取得
    getFontDataByUTF16(font, ucode);

    // 表示用バッファにデータの格納（列単位に変換）
    for (uint8_t w = 0; w < width; w++) {
        uint8_t data = 0;
        for (uint8_t row = 0; row < 8; row++) {
            if (font[row] & (0x80 >> w))
                data |= (0x80 >> row);
        }
      buf[n++] = data;
    }
  }

  // 表示用バッファの表示
  for (uint8_t row = 0; row < 8; row++) {
    for (size_t i = 0; i < n; i++) {
      // 注意: fore と back の長さが異なると見た目が崩れます
      Serial.print((buf[i] & (0x80 >> row)) ? fore : back);
    }
    Serial.println();
  }
}

void setup() {
  Serial.begin(115200);
  while (!Serial) { delay(10); }

  // 使用例（fore/back は同幅のものを指定することを推奨）
  banner("埼玉☆さいたま ｻｲﾀﾏ!", "##", "  ");
  banner("misakiUTF16ﾗｲﾌﾞﾗﾘ", "%%", "  ");
  banner("あいうえお", "##", "  ");
  banner("みさきフォント", "@", " ");
}

void loop() {
}