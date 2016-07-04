// 
// misakiUTF16.h ����t�H���g�h���C�o �w�b�_�[�t�@�C�� v1.1B by ���܋g���� 2016/03/15
// �����t���b�V���������o�[�W����
//
// 2016/03/16 �S�p�������p�����̕s��Ή�
// 2016/07/05 getFontData()�֐��̒ǉ�
//

#ifndef misakiUTF16_h
#define misakiUTF16_h

#include <avr/pgmspace.h>
#include <arduino.h>

#define FTABLESIZE     1710      // �t�H���g�e�[�u���f�[�^�T�C�Y
#define FONT_LEN       8         // 1�t�H���g�̃o�C�g��

extern PROGMEM const uint8_t fdata[];						// �t�H���g�f�[�^
extern PROGMEM const uint16_t ftable [];					// �C���f�b�N�X�e�[�u��

int findcode(uint16_t  ucode) ;								// �t�H���g�R�[�h����
boolean getFontDataByUTF16(byte* fontdata, uint16_t utf16) ;// UTF16�ɑΉ��������t�H���g�f�[�^8�o�C�g���擾
uint16_t hkana2kana(uint16_t ucode);						// ���p�J�i��S�p�ɕϊ�
uint16_t utf16_HantoZen(uint16_t utf16); 					// UTF16���p�R�[�h��UTF16�S�p�R�[�h�ɕϊ�
byte charUFT8toUTF16(char *pUTF8, uint16_t *pUTF16);		// UTF8����(1�`3�o�C)��UTF16�ɕϊ�
byte Utf8ToUtf16(uint16_t* pUTF16, char *pUTF8);			// UTF8�������UTF16������ɕϊ�

char* getFontData(byte* fontdata,char *pUTF8);              // �w�肵��UTF8������̐擪�̃t�H���g�f�[�^�̎擾

#endif