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
		//do nothing
	}
};

using namespace std;

int main()
{
	CBase* pEqu = new CEquip();
	string ep = "125.125.122.83";
	pEqu->init(888,ep,444);
	pEqu->run(pEqu);
    return 0;
}

