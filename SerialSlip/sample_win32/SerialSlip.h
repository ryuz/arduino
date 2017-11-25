

#pragma once


#include <Windows.h>
#include <tchar.h>
#include <thread>
#include <mutex>
#include <memory>
#include <vector>
#include <queue>


class SerialSlip
{
public:
	SerialSlip();
	~SerialSlip();

	bool	Open(int comPort, long bps);
	void	Close(void);
	bool	IsOpend(void) { return (m_hCom != INVALID_HANDLE_VALUE); }

	int							SendPacket(const std::vector<unsigned char> data);
	bool						IsPacketArrived(void);
	std::vector<unsigned char>	PolPacket(void);
	std::vector<unsigned char>	RecvPacket();

protected:
	void	SendEsc(void);
	void	SendByte(unsigned char c);
	void	ThreadProc(void);

	HANDLE										m_hCom;
	std::unique_ptr<std::thread>				m_thread;
	volatile bool								m_stop;
	std::mutex									m_mtxQue;
	std::mutex									m_mtxCv;
	std::condition_variable						m_cv;
	std::queue< std::vector<unsigned char> >	m_queRecv;
};

