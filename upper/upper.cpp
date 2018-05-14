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
		CustomHead dbgch = *(CustomHead*)msgip.buf;
		if (dbgch.isRequestSucess == true)
		{
			int dbgbreakpoint = 0xcc;
		}
		string drv = "125.125.122.83";
		if (endpoint.address().to_string() == drv)
		{
			recv_process_drv2upper(msgip);
		}
	}

	void recv_process_drv2upper(CMsgIP msgip)
	{
		char* buf = msgip.buf;
		CustomHead inch = *(CustomHead*)msgip.buf;
		DrvToUpper data = *(DrvToUpper*)(buf + sizeof(CustomHead));

		EQU_STATUS stat = (EQU_STATUS)data.equ_stat;

		//////////////////////////////////////////
		//switch (stat)//待修改，不应该自动变状态了
		//{
		//case status0://平台停止，等待开机
		//	up2dr_.upper_cmd = sComd6;
		//	break;
		//case status1:
		//	break;
		//case status2:
		//	up2dr_.upper_cmd = sComd4;
		//	break;
		//case status3:
		//	up2dr_.upper_cmd = sComd0;
		//	break;
		//case status6:
		//	up2dr_.upper_cmd = sComd6;
		//	break;
		//case status7:
		//	up2dr_.upper_cmd = sComd7;
		//	break;
		//case status8:
		//	//donothing
		//	break;
		//case status99:
		//	break;
		//default:
		//	printf("unknown cmd\n");
		//}
	}

	void send_process_upper2drv()
	{
		char buf[BUF_SIZE] = { 0 };

		CustomHead customhead;// = { TYPE_UNDEFINED,DS_UNDEFINED,0, 0 };
		UpperToDrv up2dr = { 0 };

		customhead.type = TYPE_UPGAME;
		customhead.timestamp++;
		up2dr = up2dr_;

		memcpy(CBase::send_buffer_,&customhead,sizeof(customhead));
		char* p = CBase::send_buffer_ + sizeof(customhead);
		memcpy(p, &up2dr, sizeof(up2dr));
	}


	virtual void process_send_data()
	{
		up2dr_.upper_cmd = pInput_->getcmd();

		send_process_upper2drv();
	}

	UpperToDrv up2dr_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, 0, 0};
	DrvToUpper dr2up_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, status99, 0 };

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

