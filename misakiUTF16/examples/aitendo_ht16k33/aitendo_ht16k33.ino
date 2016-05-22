//
// aitendoドットマトリックスデモ by たま吉さん 2016/03/15
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

// テスト用パターン('さ','い','た','ま')
uint8_t msg_saitama[4][8] = {
  {0x08,0x08,0x7E,0x04,0x24,0x40,0x3C,0x00},  // さ
  {0x00,0x88,0x84,0x82,0x82,0x50,0x20,0x00},  // い
  {0x20,0xF0,0x2E,0x40,0x48,0x50,0x8E,0x00},  // た
  {0x08,0x7E,0x08,0x7E,0x08,0x7C,0x7A,0x00}   // ま
};

// テスト用文字列
char msg_str[] = "あいうえお、今日は３月１４日です。かんたんなかんじの表示ができます。";

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

#ifdef MY_DEBUG   
   I2CBusScanTest();
   dump(256) ;
#endif
}

// デモ1 ドット表示チェック
void demo1() {
  ht_clear();
  for (int8_t d = 1; d >= 1; d--) {
    for (int8_t y = 0; y <8; y++) {
      for (int8_t x = 0; x <8;  x++) {
         ht_set_dot(x, y, d);
         ht_update();
         delay(75);
      }
    } 
    delay(500);
  }
  for (int8_t i = 0; i <8; i++) {
    scroll();
    ht_update();
    delay(100);    
  }
}  

// デモ2 パターンを指定座標の表示
void demo2() {
   ht_clear();
   for (uint8_t i = 0; i <4; i++) {
        ht_clear_buffer();
        ht_write_at(msg_saitama[i], 0, 0);
        ht_update();
        delay(1000);  
   }
}

// デモ3 パターンをスクロールイン表示
void demo3() {
   ht_clear();
   for (uint8_t i = 0; i <4; i++) {
        ht_scrollIn(msg_saitama[i], 150);  
   }
}

// デモ4 漢字フォント利用
void demo4() {
  uint16_t wstr[256];
  uint8_t  fnt[8];
  int8_t   len;

  len = Utf8ToUtf16(wstr, msg_str);  // UTF8文字列をUTF16に変換 
  Serial.print("len=");
  Serial.println(len);
  ht_clear();
   for (uint8_t i = 0; i <len; i++) {
       Serial.print("code=");
       Serial.println(wstr[i],HEX); 
       if (getFontDataByUTF16(fnt, wstr[i]) == false) {
         Serial.println("Error"); 
       }  
       ht_scrollIn(fnt, 150);  
   }
}

void loop() {  
   demo4();
   delay(1000);    
   demo1();
   delay(1000); 
   demo2();
   delay(1000); 
   demo3();
   delay(1000); 
}
