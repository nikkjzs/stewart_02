// driver.cpp : Defines the entry point for the console application.
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
			std::cin >> str;
		}
	}

	void run(CInput* pIn)
	{
		group.create_thread(boost::bind(&CInput::loopgetinput, pIn));
		group.join_all();
	}

	std::string str;
	boost::thread_group group;
};


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
				printf("%d\n", dur);
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

class CDriver : public CBase
{
public:
	CDriver() : CBase(), timer_(CDriver::io_context_)
	{

	}

	void init(int suplocalport, string suptargetIP, int suptargetport,
		string targetIP, int targetport, int misec)
	{
		CBase::init(suplocalport, suptargetIP, suptargetport);
		CDriver::tar_endpoint_ = udp::endpoint(boost::asio::ip::address_v4::from_string(targetIP), targetport);
		misec_ = misec;
	}

	void run(CDriver* pDriver,int threadnum)
	{
		start_receive();
		start_send();

		for (int i = 0; i < threadnum; i++)
		{
			group.create_thread(boost::bind(&CDriver::entry, pDriver));
		}

		group.create_thread(boost::bind(&CDriver::start_real_send, pDriver));

		group.join_all();
	}

	//void run_real(CDriver* pDriver)
	//{
	//	start_real_send();

	//	group.create_thread(boost::bind(&CDriver::entry_realsend, pDriver));
	//}

	void entry()
	{
		CBase::io_context_.run();
	}

	//void entry_realsend()
	//{
	//	CDriver::io_context_.run();
	//}

	virtual void process_recv_data(CMsgIP msgip)
	{
		//if (listRecv.empty() == false)
		//{
		//	CMsgIP msgip = listRecv.back();
		//	listRecv.pop_back();

		//	//immediatly pump recv cmd
		//	CBase::start_receive();//any timing, there is only one recv_cmd in the event queue.

		//	workflow_branch(msgip);
		//}

		workflow_branch(msgip);
	}

	void workflow_branch(CMsgIP msgip)
	{
		//char* pBuf = msgip.buf;
		udp::endpoint endpoint = msgip.endpoint;
		string eq = "192.168.2.151";
		string up = "192.168.2.37";
		if (endpoint.address().to_string() == eq)
		{
			recv_process_equ2drv(msgip);

			//if (pBuf[0] == 'a')
			//{
			//	int i = 5;
			//}
		}
		else if (endpoint.address().to_string() == up)
		{
			recv_process_up2drv(msgip);
		}
	}

	void recv_process_equ2drv(CMsgIP msgip)
	{
		char* buf = msgip.buf;
		CustomHead ch = { 0 };//
		EquToDrv data = *(EquToDrv*)(buf + sizeof(CustomHead));

		eq2dr_ = data;

		send_process_drv2up(msgip);

	}

	//obsolete
	void send_process_drv2equ(CMsgIP msgip)
	{
	}

	void recv_process_up2drv(CMsgIP msgip)
	{
		char* buf = msgip.buf;
		CustomHead ch = { 0 };//
		UpperToDrv data = *(UpperToDrv*)(buf + sizeof(CustomHead));

		up2dr_ = data;

		//send_process_drv2up(data);

	}

	void send_process_drv2up(CMsgIP msgip)
	{
		dr2up_.equ_stat = eq2dr_.rComd;

		CustomHead customhead = { 0 };//
		memcpy(CBase::send_buffer_, &customhead, sizeof(customhead));
		char* p = CBase::send_buffer_ + sizeof(customhead);
		memcpy(p, &dr2up_, sizeof(dr2up_));
	}


	virtual void start_real_send()
	{
		bool isfirst = true;
		clock_t cur,lst;

		//CDriver::send_buffer_[0] = 'g';

		while (true)
		{
			CustomHead customhead = { 0 };//
			dr2eq_.sComd = up2dr_.upper_cmd;
			//dr2eq_.sComd = sComd6;

			memcpy(CDriver::send_buffer_, &customhead, sizeof(customhead));
			char* p = CDriver::send_buffer_ + sizeof(customhead);
			memcpy(p, &dr2eq_, sizeof(dr2eq_));


			if (isfirst == true)
			{
				lst = clock();
				isfirst = false;
			}
			else
			{
				cur = clock();
				clock_t dur = cur - lst;
				if (dur >= misec_)
				{
					lst = cur;
				}
				else
				{
					continue;
				}
			}



			socket_.send_to(boost::asio::buffer(CDriver::send_buffer_), CDriver::tar_endpoint_);
			mytimer_.echofunc();
		}
	}

	boost::asio::io_context io_context_;

	udp::endpoint tar_endpoint_;
	boost::asio::deadline_timer timer_;
	char send_buffer_[BUF_SIZE];
	char recv_buffer_[BUF_SIZE];
	int misec_;

	//test
	CMyTime mytimer_;

	//S_CMD send_cmd_ = sComd99;
	//EQU_STATUS equ_status_ = status99;
	UpperToDrv up2dr_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, 0, 0 };
	//DrvToUpper dr2up_ = { 0, status99,{ 0 },{ 0 },{ 0 },{ 0 } };
	DrvToUpper dr2up_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, status99, 0 };
	DrvToEqu dr2eq_ = { 0, sComd99,{ 0 },{ 0 },{ 0 },{ 0 } };
	EquToDrv eq2dr_ = { 0, status99,{ 0 },{ 0 },{ 0 },{ 0 } };
};

using namespace std;

int main()
{
	CInput* in = new CInput();
	in->run(in);

	CDriver* pDrv = new CDriver();
	//string ep = "192.168.2.151";
	string ep = "192.168.2.151";
	string upip = "192.168.2.37";
	int upport = 222;
	//pDrv->init(444,ep,888);
	pDrv->init(444, upip,upport,
		ep,888,5);
	pDrv->run(pDrv,1);
    return 0;
}

