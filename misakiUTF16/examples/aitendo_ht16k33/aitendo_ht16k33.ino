//
// aitendoドットマトリックスデモ by たま吉さん 2016/03/15
// 2016/07/10,美咲フォントライブラリ用サンプルに特化のための修正 
//  

#include <Wire.h>
#include <misakiUTF16.h> 

// アドレス
#define HT_I2C_ADDRESS 0x70

// コマンド
#define HT_CMS_DATA       0x00
#define HT_CMD_SYSSET     0x20
#define HT_CMD_BLINK      0x80
#define HT_CMD_BRIGHTNESS 0xE0

// System Setup レジスタ設定値
#define HT_SYSSET_OSC_ON  1

// Display setup 設定値
#define HT_BLINK_DISPLAY_ON B00000001
#define HT_BLINK_OFF        B00000000
#define HT_BLINK_2HZ        B00000010
#define HT_BLINK_1HZ        B00000100
#define HT_BLINK_05HZ       B00000110

// 表示パターン用バッファ(R16xC8)
uint8_t buf[8];  

// HT16K33の初期化
void ht_init() {
  Wire.begin();
  ht_system_Seup(HT_I2C_ADDRESS, HT_SYSSET_OSC_ON);  // システムオシレータをONにする
  ht_display_setup(HT_I2C_ADDRESS,HT_BLINK_OFF) ;    // 点滅表示周期の設定
  ht_setBrightness(HT_I2C_ADDRESS,1);                // 明るさの設定
}

// SystemSetup Registerの設定
// (システムオシレータのモード設定)
void ht_system_Seup(uint8_t addr, uint8_t p) {
  Wire.beginTransmission(addr);
  Wire.write(HT_CMD_SYSSET | p);
  Wire.endTransmission(); 
} 

// 明るさの設定
void  ht_setBrightness(uint8_t addr, uint8_t p) {
  if (p > 15) p = 15;
  Wire.beginTransmission(addr);
  Wire.write(HT_CMD_BRIGHTNESS | p);
  Wire.endTransmission(); 
}

// Display setup(点滅周期の設定)
void ht_display_setup(uint8_t addr, uint8_t p) {
  if (p > 3) p = 0;
  Wire.beginTransmission(addr);  
  Wire.write(HT_CMD_BLINK | HT_BLINK_DISPLAY_ON | p); 
  Wire.endTransmission();
}

// 左右ビット入れ替え
uint8_t revRL(uint8_t d) {
  uint8_t rc = 0;
  for (uint8_t i=0; i<8; i++) { 
    rc <<=1;
    rc |= ( d>>i ) & 1;
  }
  return rc;
}

// 表示パターンの送信
void ht_update(void) {
  uint8_t d;
  
  // HT16K33  にデータ転送
  Wire.beginTransmission(HT_I2C_ADDRESS);
  Wire.write(HT_CMS_DATA);

  for (uint8_t i = 0; i<8; i++) {
 
    // モジュールの設置向きに応じて回転・反転の調整
    d = buf[7-i]; // 上下反転
    d = revRL(d); // 左右反転
    
    // データの送信
    Wire.write(d);    
    Wire.write(0);    
  }
  Wire.endTransmission();  
}

// バッファのクリア
void ht_clear_buffer() {
    for (uint8_t i = 0; i <8; i++) 
      buf[i] = 0;
}

// 表示のクリア
void ht_clear() {
  ht_clear_buffer();
  ht_update();
}

// バッファの指定座標のON/OFF
void ht_set_dot(int8_t x, int8_t y, uint8_t d) {
  if ( (x > 7) || (y > 7) || (x < 0) || (y < 0) ) 
     return;     
  if (d)
    buf[y] |= 0x80 >> x;
  else
    buf[y] &= ~(0x80 >>x);  
}

// バッファ上の指定座標に8x8パターンをセット(負座標指定可能)
void ht_write_at(uint8_t* p, int8_t x, int8_t y) {
    int8_t i; 
    if ((x > 7) || (y > 7) || (x < -7) || (y < -7) ) 
        return;
    for (i = 0; i <8; i++) {
        if ((i+y >= 0) && (i+y < 8) ) {
           if (x >= 0)
              buf[i+y] |= p[i]>>x;
           else
              buf[i+y] |= p[i]<<(-x);
        }
    }
}

// バッファ左スクロール
void scroll() {
   for (uint8_t i = 0; i < 8; i++) {
      buf[i]<<=1;  // 各行を左にシフトする
   }
}

// 1文字左スクロール挿入
void ht_scrollIn(uint8_t* p, int wt) {
    int8_t i,j;
    for (j = 0; j < 8; j++) {
        for (i = 0; i < 8; i++) {
            buf[i]<<=1;
        }
        ht_write_at(p, 7-j, 0);
        ht_update();
        delay(wt);
    }
}

void setup() {
   Serial.begin(115200);
   ht_init();
   ht_clear();
}

// デモ4 漢字フォント利用
void demo() {
  uint8_t  fnt[8];
  int8_t   len;
  
  // テスト用文字列
  char msg_str[] = "あいうえお、今日は７月１０日です。かんたんなかんじの表示ができます。";

  char *str = msg_str;
  ht_clear();
  while(*str) {
    if (! (str = getFontData(fnt, str)) )  {
         Serial.println("Error"); 
         break;
    }
    ht_scrollIn(fnt, 150);         
  }
}

void loop() {  
   demo();
   delay(1000); 
}
