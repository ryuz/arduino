// ----------------------------------------------------------------------------
//  Serial SLIP(RFC1055) for Arduino
//
//                                        Copyright(C) 2017 by Ryuji Fuchikami 
// ----------------------------------------------------------------------------



#define SERIAL_SLIP_EOF   EOF   // �f�[�^�Ȃ�
#define SERIAL_SLIP_ESC   -2    // ESC

void SerialSlip_Start(int bps); // ������
void SerialSlip_End(void);      // �I��
int  SerialSlip_GetChar(void);  // 1������M
void SerialSlip_PutChar(int c); // 1�������M
int  SerialSlip_RecvPacket(byte data[], int maxlen);  // �p�P�b�g��M
void SerialSlip_SendPacket(byte data[], int len);     // �p�P�b�g���M



// end of file
