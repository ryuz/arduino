// ----------------------------------------------------------------------------
//  Serial SLIP(RFC1055) for Arduino
//
//                                        Copyright(C) 2017 by Ryuji Fuchikami 
// ----------------------------------------------------------------------------

#ifndef __RYUZ__SERIALSLIP_H__
#define __RYUZ__SERIALSLIP_H__


#define SERIAL_SLIP_EOF   EOF   // データなし
#define SERIAL_SLIP_ESC   -2    // ESC

void SerialSlip_Start(long bps); // 初期化
void SerialSlip_End(void);      // 終了
int  SerialSlip_GetChar(void);  // 1文字受信
void SerialSlip_PutChar(int c); // 1文字送信
int  SerialSlip_RecvPacket(unsigned char data[], int maxlen);  // パケット受信
void SerialSlip_SendPacket(const unsigned char data[], int len);     // パケット送信


#endif // __RYUZ__SERIALSLIP_H__

// end of file

