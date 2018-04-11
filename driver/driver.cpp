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
	CDriver() : CBase()
	{

	}

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
		}


		//do nothing
	}
};

using namespace std;

int main()
{
	Sleep(10000);
	CBase* pDrv = new CDriver();
	string ep = "192.168.2.151";
	pDrv->init(444,ep,888);
	pDrv->run(pDrv);
    return 0;
}

