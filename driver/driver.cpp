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

#include "../base/base.h"

//class CMyTimer
//{
//public:
//	bool IsTimeOut(boost::timer::nanosecond_type timeout)
//	{
//		if (isfirst_ == true)
//		{
//			isfirst_ = false;
//			timer_.start();
//			return false;
//		}
//
//		boost::timer::cpu_times elap = timer_.elapsed();
//		if (elap.wall / 1000000 > timeout)
//		{
//			timer_.start();
//			printf("%d\n", elap.wall / 1000000);
//			return true;
//		}
//		
//		printf("%d false\n", elap.wall / 1000000);
//		return false;
//	}
//
//	bool test()
//	{
//		if (isfirst_ == true)
//		{
//			isfirst_ = false;
//			timer_.start();
//			return false;
//		}
//
//		boost::timer::cpu_times elap = timer_.elapsed();
//		//if (elap.wall / 1000000 > timeout)
//		{
//			//printf("%d\n", elap.wall / 1000000);
//			if (elap.wall / 1000000 > 10)
//			{
//				int lllag = 6;
//			}
//
//			timer_.start();
//			return true;
//		}
//	}
//
//
//	bool isfirst_ = true;
//	boost::timer::cpu_timer timer_;
//	//boost::timer::nanosecond_type duration;
//};


using boost::asio::ip::udp;

class CMyTime
{
public:
	CMyTime()//clock_t timeout)
	{
		//timeout_ = timeout;
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
	//clock_t timeout_;
	clock_t lst_;
};

class CDriver : public CBase
{
public:
	//CDriver(int localport, string targetIP, int targetport) : CBase(localport, targetIP, targetport)
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

	virtual void process_recv_data()
	{
		if (listRecv.empty() == false)
		{
			CMsgIP msgip = listRecv.back();
			listRecv.pop_back();
			char* pBuf = msgip.buf;
			if (pBuf[0] == 'a')
			{
				int i = 5;
			}
		}

	}


	virtual void start_real_send()
	{
		bool isfirst = true;
		clock_t cur,lst;

		CDriver::send_buffer_[0] = 'g';

		while (true)
		{
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


	//virtual void start_real_send()//realtime
	//{
	//	

	//	//boost::asio::deadline_timer timer(io_context_, boost::posix_time::millisec(1000));
	//	timer_.expires_from_now(boost::posix_time::millisec(misec_));
	//	timer_.async_wait(boost::bind(&CDriver::handle_real_send, this));

	//	/*socket_.async_send_to(boost::asio::buffer(send_buffer_), tar_endpoint_,
	//		boost::asio::bind_executor(strand_, boost::bind(&CBase::handle_send, this))
	//	);*/
	//}

	//virtual void handle_real_send()
	//{
	//	//tmp
	//	CDriver::send_buffer_[0] = 'b';

	//	while (1)
	//	{
	//		//Sleep(1);
	//		socket_.send_to(boost::asio::buffer(CDriver::send_buffer_), CDriver::tar_endpoint_);

	//		//test

	//		mytimer_.test();
	//	}

	//	socket_.send_to(boost::asio::buffer(CDriver::send_buffer_), CDriver::tar_endpoint_);

	//	//test

	//	mytimer_.test();

	//	/*if (mytimer_.IsTimeOut(400) == true)
	//	{
	//		int kk = 66;
	//	}*/
	//	

	//	start_real_send();
	//}


	boost::asio::io_context io_context_;

	udp::endpoint tar_endpoint_;
	boost::asio::deadline_timer timer_;
	char send_buffer_[BUF_SIZE];
	char recv_buffer_[BUF_SIZE];
	int misec_;



	//test
	CMyTime mytimer_;
};

using namespace std;

int main()
{
	CDriver* pDrv = new CDriver();
	string ep = "192.168.2.151";
	string upip = "192.168.2.151";
	int upport = 888;
	//pDrv->init(444,ep,888);
	pDrv->init(444, upip,upport,
		ep,888,5);
	pDrv->run(pDrv,5);
    return 0;
}

