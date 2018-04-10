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
	CDriver(int localport, string targetIP, int targetport) : CBase(localport, targetIP, targetport)
	{

	}

	virtual void process_recv_data()
	{
		//do nothing
	}
};

int main()
{

    return 0;
}

