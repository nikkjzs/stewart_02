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

class CDriver : public CBase
{
public:
	//CDriver(int localport, string targetIP, int targetport) : CBase(localport, targetIP, targetport)
	CDriver() : CBase(), timer_(io_context_)
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
		start_real_send();

		for (int i = 0; i < threadnum; i++)
		{
			group.create_thread(boost::bind(&CDriver::entry, pDriver));
		}
		group.join_all();
	}

	void entry()
	{
		CBase::io_context_.run();
	}

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



	virtual void start_real_send()//realtime
	{
		

		//boost::asio::deadline_timer timer(io_context_, boost::posix_time::millisec(1000));
		timer_.expires_from_now(boost::posix_time::millisec(misec_));
		timer_.async_wait(boost::bind(&CDriver::handle_real_send, this));

		/*socket_.async_send_to(boost::asio::buffer(send_buffer_), tar_endpoint_,
			boost::asio::bind_executor(strand_, boost::bind(&CBase::handle_send, this))
		);*/
	}

	virtual void handle_real_send()
	{
		//tmp
		CDriver::send_buffer_[0] = 'b';

		socket_.send_to(boost::asio::buffer(CDriver::send_buffer_), CDriver::tar_endpoint_);
		start_real_send();
	}



	udp::endpoint tar_endpoint_;
	boost::asio::deadline_timer timer_;
	char send_buffer_[BUF_SIZE];
	char recv_buffer_[BUF_SIZE];
	int misec_;
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
		ep,888,333);
	pDrv->run(pDrv,4);
    return 0;
}

