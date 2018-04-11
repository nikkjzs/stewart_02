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


#include "../base/base.h"

using boost::asio::ip::udp;

class CEquip : public CBase
{
public:
	//CEquip(int localport, string targetIP, int targetport) : CBase(localport, targetIP, targetport)
	CEquip() : CBase()
	{

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

			if (pBuf[0] == 'b')
			{
				//boost::timer::auto_cpu_timer
				boost::timer::auto_cpu_timer
					timer("%w clock time, %t totle program time(%p%)\n");
				int i = 5;
			}

		}
		//do nothing
	}
};

using namespace std;

int main()
{
	CBase* pEqu = new CEquip();
	string dr = "125.125.122.83";
	pEqu->init(888,dr,444);
	pEqu->run(pEqu);
    return 0;
}

