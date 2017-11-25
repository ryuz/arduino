

#define SERIAL_SLIP_EOF   EOF   // データなし
#define SERIAL_SLIP_ESC   -2    // ESC

void SerialSlip_Start(int bps); // 初期化
void SerialSlip_End(void);      // 終了
int  SerialSlip_GetChar(void);  // 1文字受信
void SerialSlip_PutChar(int c); // 1文字送信
int  SerialSlip_RecvPacket(byte data[], int maxlen); // パケット受信
void SerialSlip_SendPacket(byte data[], int len);   // パケット送信



boolean SerialSlip_blRecvEsc;		// ESC受信中フラグ
int     SerialSlip_iRecvPacketLen;

// 初期化
void SerialSlip_Start(int bps)
{
  // inisialize
  SerialSlip_blRecvEsc = false;
  Serial.begin(bps);

  // send ESC
  Serial.write(0xc0);
}

// 終了
void SerialSlip_End(void)
{
  Serial.end();
}


// 1文字受信
int SerialSlip_GetChar(void)
{
  int c;
  c = Serial.read();
  if ( c == EOF ) {
    return SERIAL_SLIP_EOF;
  }
  
  if ( c == 0xc0 ) {
    SerialSlip_blRecvEsc = false;
    return SERIAL_SLIP_ESC;
  }
  
  if ( c == 0xdb ) {
    SerialSlip_blRecvEsc = true;
    return SERIAL_SLIP_EOF;
  }
  
  if ( SerialSlip_blRecvEsc ) {
    SerialSlip_blRecvEsc = false;
    if ( c == 0xdc ) {
      return 0xc0;
    }
    if ( c == 0xdd ) {
      return 0xdb;
    }
    return SERIAL_SLIP_EOF;
  }
  
  return c;
}

// 1文字送信
void SerialSlip_PutChar(int c)
{
    if ( c == SERIAL_SLIP_ESC ) {
      Serial.write(0xc0);      
    }
    else if ( c == 0xc0 ) {
      Serial.write(0xdb);
      Serial.write(0xdc);
    }
    else if ( c == 0xdb ) {
      Serial.write(0xdb);
      Serial.write(0xdd);
    }
    else {
      Serial.write(c);
    }
}

// パケット受信
int SerialSlip_RecvPacket(byte data[], int maxlen)
{
  int c;
  c = SerialSlip_GetChar();
  if ( c == SERIAL_SLIP_EOF ) {
    return -1;
  }
  
  if ( c == SERIAL_SLIP_ESC ) {
    int len = SerialSlip_iRecvPacketLen;
    SerialSlip_iRecvPacketLen = 0;
    return len;
  }

  if ( SerialSlip_iRecvPacketLen < maxlen ) {
    data[SerialSlip_iRecvPacketLen++] = c;
  }
  
  return -1;  
}

// パケット送信
void SerialSlip_SendPacket(byte data[], int len)
{
  int i;
  for ( i = 0; i < len; i++ ) {
    SerialSlip_PutChar(data[i]);
  }
  SerialSlip_PutChar(SERIAL_SLIP_ESC);
}

// end of file
