


#include "SerialSlip.h"


// �R���X�g���N�^
SerialSlip::SerialSlip()
{
	m_hCom = INVALID_HANDLE_VALUE;
}


// �f�X�g���N�^
SerialSlip::~SerialSlip()
{
	Close();
}


// �J��
bool SerialSlip::Open(int comPort, long bps)
{
	// �J���Ă���Ε���
	Close();

	// �w��|�[�g���J��
	TCHAR	szPortName[16];
	_stprintf_s<16>(szPortName, _T("\\\\.\\COM%d"), comPort);
	m_hCom = ::CreateFile(szPortName, GENERIC_READ | GENERIC_WRITE, 0, NULL, OPEN_EXISTING, 0, NULL);
	if (m_hCom == INVALID_HANDLE_VALUE) {
		return false;
	}

	// �{�[���[�g�ݒ�
	DCB	dcb;
	::GetCommState(m_hCom, &dcb);
	dcb.BaudRate = bps;
	::SetCommState(m_hCom, &dcb);

	// �^�C���A�E�g�ݒ�
	COMMTIMEOUTS	ct;
	::GetCommTimeouts(m_hCom, &ct);
	ct.WriteTotalTimeoutConstant   = 0;
	ct.WriteTotalTimeoutMultiplier = 0;
	ct.ReadIntervalTimeout         = 1;
	ct.ReadTotalTimeoutMultiplier  = 1;
	ct.ReadTotalTimeoutConstant    = 1;
	::SetCommTimeouts(m_hCom, &ct);
	
	// ESC�𑗐M
	SendEsc();

	// �X���b�h�쐬
	m_stop = false;
	m_thread = std::make_unique<std::thread>(&SerialSlip::ThreadProc, this);

	return true;
}


// ����
void SerialSlip::Close(void)
{
	if (m_hCom != INVALID_HANDLE_VALUE) {
		// �X���b�h��~
		m_stop = true;
		m_thread->join();
		m_thread.reset();

		// �|�[�g�����
		::CloseHandle(m_hCom);
		m_hCom = INVALID_HANDLE_VALUE;
	}
}


// ESC�̑��M
void SerialSlip::SendEsc(void)
{
	unsigned char c = 0xc0;
	DWORD	dwSize;
	::WriteFile(m_hCom, &c, 1, &dwSize, NULL);
}

// �f�[�^�̑��M
void SerialSlip::SendByte(unsigned char c)
{
	unsigned char	buf[2];
	DWORD			dwSize;

	if (c == 0xc0) {
		buf[0] = 0xdb;
		buf[1] = 0xdc;
		::WriteFile(m_hCom, buf, 2, &dwSize, NULL);
	}
	else if (c == 0xdb) {
		buf[0] = 0xdb;
		buf[1] = 0xdd;
		::WriteFile(m_hCom, buf, 2, &dwSize, NULL);
	}
	else {
		buf[0] = c;
		::WriteFile(m_hCom, buf, 1, &dwSize, NULL);
	}
}

// �p�P�b�g�̑��M
int SerialSlip::SendPacket(const std::vector<unsigned char> data)
{
	if (!IsOpend()) { return -1; }

	for ( auto& c : data ) {
		SendByte(c);
	}
	SendEsc();

	return (int)data.size();
}


// �f�[�^�̓����m�F
bool SerialSlip::IsPacketArrived(void)
{
	std::lock_guard<std::mutex> guard(m_mtxQue);
	return !m_queRecv.empty();
}


// �f�[�^��M(�|�[�����O)
std::vector<unsigned char>	SerialSlip::PolPacket(void)
{
	std::lock_guard<std::mutex> guard(m_mtxQue);
	std::vector<unsigned char> packet;

	if ( !m_queRecv.empty() ) {
		packet = m_queRecv.front();
		m_queRecv.pop();
	}

	return packet;
}



// �f�[�^��҂�
std::vector<unsigned char>	SerialSlip::RecvPacket(void)
{
	std::unique_lock<std::mutex> lock(m_mtxCv);

	for ( ; ; )	{
		auto packet = PolPacket();
		if (!packet.empty()) {
			return packet;
		}

		m_cv.wait_for(lock, std::chrono::microseconds(10), [this] { return (IsPacketArrived() || m_stop); });
		if (m_stop) {
			return packet;
		}
	}
}



// �X���b�h����
void SerialSlip::ThreadProc(void)
{
	std::vector<unsigned char>	buf;
	unsigned char				c;
	bool						escFlag = false;
	DWORD						dwSize;

	while (!m_stop) {
		if (::ReadFile(m_hCom, &c, 1, &dwSize, NULL)) {
			if (dwSize != 1) { continue; }

			switch (c) {
			case 0xc0:
				// ESC��M���Ƀf�[�^������΃p�P�b�g�G���h�Ƃ݂Ȃ�
				if (buf.size() > 0) {
					{
						std::lock_guard<std::mutex> guard(m_mtxQue);
						m_queRecv.push(buf);
					}
					m_cv.notify_one();
					buf.clear();
				}
				escFlag = false;
				break;

			case 0xdb:
				escFlag = true;
				break;

			default:
				if (escFlag) {
					switch (c) {
					case 0xdc:	buf.push_back(0xc0);	break;
					case 0xdd:	buf.push_back(0xdb);	break;
					}
					escFlag = false;
				}
				else {
					buf.push_back(c);
				}
			}
		}
	}
}

// end of file
