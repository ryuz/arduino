// ----------------------------------------------------------------------------
//  Serial SLIP(RFC1055) sample for Arduino
//
//                                        Copyright(C) 2017 by Ryuji Fuchikami 
// ----------------------------------------------------------------------------


#include "SerialSlip.h"



void setup() {
  SerialSlip_Start(115200);
}


void loop() {
  static byte rcv_buf[16];
  int  len;
  
  len = SerialSlip_RecvPacket(rcv_buf, 16);
  if ( len >= 0 ) {
    SerialSlip_SendPacket(rcv_buf, len);
  }  
}



// end of file
