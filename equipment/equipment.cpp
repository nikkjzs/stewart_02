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
	//clock_t timeout_;
	clock_t lst_;
};

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

			if (pBuf[0] == 'g')
			{
				mytimer_.echofunc();
				//boost::timer::auto_cpu_timer
				/*boost::timer::auto_cpu_timer
					timer("%w clock time, %t totle program time(%p%)\n");
				int i = 5;*/
			}

		}
		//do nothing
	}

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

