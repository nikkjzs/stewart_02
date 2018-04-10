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

#include "../base/base.h"

using boost::asio::ip::udp;
using namespace std;

class CDriver : public CBase
{
public:
	//CDriver(int localport, string targetIP, int targetport) : CBase(localport, targetIP, targetport)
	CDriver() : CBase(), timer_(io_context_)
	{

	}

	void init(int localport, string supTargetIP, int supTargetport,
		string targetIP, int targetport,int misec)
	{
		CBase::init(localport, supTargetIP, supTargetport);
		tar_endpoint_ = udp::endpoint(boost::asio::ip::address_v4::from_string(targetIP), targetport);
		//timer_.expires_from_now(boost::posix_time::millisec(misec));
		misec_ = misec;
	}

	//virtual void init(int localport, string targetIP, int targetport)
	//{
	//	socket_.open(udp::v4());
	//	socket_.bind(udp::endpoint(udp::v4(), localport));
	//	tar_endpoint_ = udp::endpoint(boost::asio::ip::address_v4::from_string(targetIP), targetport);
	//}

	virtual void process_recv_data()
	{
		if (listRecv.empty() == false)
		{
			CMsgIP msgip = listRecv.back();
			char* pBuf = msgip.buf;
			if (pBuf[0] == 'a')
			{
				int i = 5;
			}

			string addr = msgip.endpoint.address().to_string();
			int port = msgip.endpoint.port();
		}


		//do nothing
	}

	virtual void start_send()//realtime
	{
		

		//boost::asio::deadline_timer timer(io_context_, boost::posix_time::millisec(1000));
		timer_.expires_from_now(boost::posix_time::millisec(misec_));
		timer_.async_wait(boost::bind(&CDriver::handle_send, this));

		/*socket_.async_send_to(boost::asio::buffer(send_buffer_), tar_endpoint_,
			boost::asio::bind_executor(strand_, boost::bind(&CBase::handle_send, this))
		);*/
	}

	virtual void handle_send()
	{
		//tmp
		CBase::send_buffer_[0] = 'b';

		socket_.send_to(boost::asio::buffer(CBase::send_buffer_), CBase::tar_endpoint_);
		start_send();
	}
	


	udp::endpoint tar_endpoint_;
	boost::asio::deadline_timer timer_;
	int misec_;
};


int main()
{
	//Sleep(10000);
	CDriver* pDrv = new CDriver();
	string ep = "192.168.2.151";
	string up = "111.111.111.111";
	pDrv->CDriver::init(444,ep,888,
		up, 111, 333);
	
	//pDrv->init(up, 111, 333);
	//pDrv->init();
	pDrv->run(pDrv);
    return 0;
}

