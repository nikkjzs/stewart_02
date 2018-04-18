// equipment.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/asio/strand.hpp>


#include <boost/timer/timer.hpp>  
#include <time.h>

#include "../base/base.h"

using boost::asio::ip::udp;


class CMyTime
{
public:
	CMyTime()
	{
	}

	bool IsTimeout(clock_t timeout)
	{
		if (isfirst_ == true)
		{
			lst_ = clock();
			isfirst_ = false;
			return true;
		}
		else
		{
			clock_t cur = clock();
			clock_t dur = cur - lst_;
			if (dur >= timeout)
			{
				lst_ = cur;
				printf("%d\n",dur);
				return true;
			}
			else
			{
				return false;
			}
		}
	}

	void echofunc()
	{
		if (isfirst_ == true)
		{
			lst_ = clock();
			isfirst_ = false;
		}
		else
		{
			clock_t cur = clock();
			clock_t dur = cur - lst_;
			lst_ = cur;
			printf("%d\n", dur);
		}
	}

	bool isfirst_ = true;
	clock_t lst_;
};

class CEquip : public CBase
{
public:
	CEquip() : CBase()
	{

	}

	virtual void process_recv_data(CMsgIP msgip)
	{
		//if (listRecv.empty() == false)
		//{
		//	CMsgIP msgip = listRecv.back();
		//	listRecv.pop_back();
		//	char* pBuf = msgip.buf;
		//	if (pBuf[0] == 'a')
		//	{
		//		int i = 5;
		//	}

		//	if (pBuf[0] == 'g')
		//	{
		//		mytimer_.echofunc();
		//	}

		//}
		//do nothing
		workflow_branch(msgip);
	}

	void workflow_branch(CMsgIP msgip)
	{
		//char* pBuf = msgip.buf;
		udp::endpoint endpoint = msgip.endpoint;
		string drv = "125.125.122.83";
		if (endpoint.address().to_string() == drv)
		{
			recv_process_drv2equ(msgip);
		}
	}

	void recv_process_drv2equ(CMsgIP msgip)
	{
		char* buf = msgip.buf;
		CustomHead ch = { 0 };//
		DrvToEqu data = *(DrvToEqu*)(buf + sizeof(CustomHead));

		S_CMD cmd = (S_CMD)data.sComd;
		EQU_STATUS outStat = (EQU_STATUS)eq2dr_.rComd;

		switch ((EQU_STATUS)eq2dr_.rComd)
		{
			case status0://平台停止，等待开机
				if (cmd == sComd6)
					outStat = status6;
				break;
			case status1://平台正在回中位，此时不响应上位机命令 
				outStat = status2;
				break;
			case status2://平台在中位 
				if (cmd == sComd4)
					outStat = status3;
				else if (cmd == sComd7)
					outStat = status7;
				break;
			case status3://平台在工作态，可正常接受运动参数 
				if (cmd == sComd7)
					outStat = status7;
				else if (cmd == sComd2)
					outStat = status1;
				break;
			case status6://平台正在开机
				outStat = status2;
				break;
			case status7://平台正在关机
				outStat = status0;
				break;
			case status8:
				//donothing
				break;
			case status99:
				break;
			default:
				break;
		}

		printf("%d", outStat);

		eq2dr_.rComd = outStat;

		send_process_equ2drv();

		start_send();
	}

	void send_process_equ2drv()
	{
		char buf[BUF_SIZE] = { 0 };

		//todo
		CustomHead customhead = { 0 };//
		EquToDrv eq2dr = { 0 };

		//eq2dr.rComd = equ_status_;
		eq2dr = eq2dr_;

		memcpy(CBase::send_buffer_, &customhead, sizeof(customhead));
		char* p = CBase::send_buffer_ + sizeof(customhead);
		memcpy(p,&eq2dr,sizeof(eq2dr));
	}

	//S_CMD recv_cmd_ = sComd99;
	//EQU_STATUS equ_status_ = status0;
	DrvToEqu dr2eq_ = { 0, sComd99, { 0 },{ 0 },{ 0 },{ 0 } };
	EquToDrv eq2dr_ = { 0, status99,{ 0 },{ 0 },{ 0 },{ 0 } };
	CMyTime mytimer_;
};

using namespace std;

int main()
{
	CBase* pEqu = new CEquip();
	string dr = "125.125.122.83";
	pEqu->init(888,dr,444);
	pEqu->run(pEqu,4);
    return 0;
}

