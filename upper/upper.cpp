#include "stdafx.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/asio/strand.hpp>

#include <string>
#include <iostream>

#include "../base/base.h"


using namespace std;

class CInput
{
public:
	void loopgetinput()
	{
		while (true)
		{
			std::cout << "Please input cmd:" << std::endl;
			std::cin >> str;
		}
	}

	void run(CInput* pIn)
	{
		group.create_thread(boost::bind(&CInput::loopgetinput, pIn));
		group.join_all();
	}

	int getcmd()
	{
		int ret = std::stoi(str);
		return ret;
	}

	std::string str;
	boost::thread_group group;
};


using boost::asio::ip::udp;

class CUpper : public CBase
{
public:
	CUpper() : CBase()
	{
		pInput_ = new CInput();
		pInput_->run(pInput_);
	}
	

	virtual void process_recv_data(CMsgIP msgip)
	{
		udp::endpoint endpoint = msgip.endpoint;
		string drv = "125.125.122.83";
		if (endpoint.address().to_string() == drv)
		{
			recv_process_drv2upper(msgip);

			//if (pBuf[0] == 'a')
			//{
			//	int i = 5;
			//}
		}
	}

	void recv_process_drv2upper(CMsgIP msgip)
	{
		char* buf = msgip.buf;
		CustomHead ch = { 0 };//
		DrvToUpper data = *(DrvToUpper*)(buf + sizeof(CustomHead));

		EQU_STATUS stat = (EQU_STATUS)data.equ_stat;


		//send_cmd_ = sComd99;

		switch (stat)
		{
		case status0://平台停止，等待开机
			up2dr_.upper_cmd = sComd6;
			break;
		case status1:
			break;
		case status2:
			up2dr_.upper_cmd = sComd4;
			break;
		case status3:
			up2dr_.upper_cmd = sComd0;
			break;
		case status6:
			up2dr_.upper_cmd = sComd6;
			break;
		case status7:
			up2dr_.upper_cmd = sComd7;
			break;
		case status8:
			//donothing
			break;
		case status99:
			break;
		default:
			printf("unknown cmd\n");
		}

		//test
		up2dr_.upper_cmd = pInput_->getcmd();

		send_process_upper2drv();

		start_send();
	}

	void send_process_upper2drv()
	{
		char buf[BUF_SIZE] = { 0 };

		//todo
		CustomHead customhead = { 0 };
		UpperToDrv up2dr = { 0 };

		up2dr = up2dr_;

		//if (equ_status_ != 3)
		{
			memcpy(CBase::send_buffer_,&customhead,sizeof(customhead));
			char* p = CBase::send_buffer_ + sizeof(customhead);
			memcpy(p, &up2dr, sizeof(up2dr));
		}
		//CBase::send_buffer_
	}

	//S_CMD send_cmd_ = sComd99;
	//EQU_STATUS equ_status_ = status99;
	UpperToDrv up2dr_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, 0, 0};
	DrvToUpper dr2up_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, status99, 0 };
	//DrvToUpper dr2up_ = { 0, status99, { 0 },{ 0 },{ 0 },{ 0 } };

	CInput*	pInput_;
};


int main()
{
	CBase* pUp = new CUpper();
	string dr = "125.125.122.83";
	pUp->init(222, dr, 444);
	pUp->run(pUp, 1);
    return 0;
}

