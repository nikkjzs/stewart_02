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




using boost::asio::ip::udp;

//boost::mutex clientmutex;

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
				//printf("%d\n", dur);
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
			//printf("%d\n", dur);
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
		//start_send();//must be connect 

		for (int i = 0; i < threadnum; i++)
		{
			group.create_thread(boost::bind(&CDriver::entry, pDriver));
		}

		group.create_thread(boost::bind(&CDriver::start_real_send, pDriver));

		group.join_all();
	}


	void entry()
	{
		CBase::io_context_.run();
	}

	virtual void process_recv_data(CMsgIP msgip)
	{
		workflow_branch(msgip);
	}
	
	void workflow_branch(CMsgIP msgip)
	{
		char* pBuf = msgip.buf;
		CustomHead ch = *(CustomHead*)pBuf;

		string eq = "192.168.2.151";
		//string up = "192.168.2.37";
		string echostr = msgip.endpoint.address().to_string();
		udp::endpoint endpoint = msgip.endpoint;

		if (endpoint.address().to_string() == eq)
		{
			recv_process_equ2drv(msgip);
		}
		else if (ch.type == TYPE_UPGAME)
		{
			recv_process_up2drv(msgip);
		}
		else if (ch.type == TYPE_UPCTRL)
		{
			recv_process_up2drv(msgip);
		}
	}


	//drv从设备收
	void recv_process_equ2drv(CMsgIP msgip)
	{
		char* buf = msgip.buf;
		CustomHead ch;// = { TYPE_UNDEFINED, DS_UNDEFINED,0,0 };//
		EquToDrv data = *(EquToDrv*)(buf + sizeof(CustomHead));

		equstatmutex_.lock();
		eq2dr_ = data;
		equstatmutex_.unlock();
	}

	//obsolete
	void send_process_drv2equ(CMsgIP msgip)
	{
	}

	//drv从上位收
	void recv_process_up2drv(CMsgIP msgip)
	{
		char* buf = msgip.buf;
		CustomHead ch;// = { TYPE_UNDEFINED,DS_UNDEFINED,0, 0 };//
		ch = *(CustomHead*)buf;

		//game or controller
		if (ch.type == TYPE_UPGAME)
		{
			int dbg = 1;
		}
		else if (ch.type == TYPE_UPCTRL)
		{
			int dbg = 1;
		}
		else
		{
			int dbg = 1;
		}


		if (ch.timestamp <= lasttimestamp_)
		{
			return;
		}

		UpperToDrv data = *(UpperToDrv*)(buf + sizeof(CustomHead));
		
		//save up's data
		outputmutex_.lock();
		//cmd filter
		bool iscmdsucessed = cmdfilter(data);
		if (iscmdsucessed == true)
		{
			up2dr_ = data;
		}
		outputmutex_.unlock();

		//cache buf,push to clients vector
		CMsgIP* pRequest = new CMsgIP;
		memset(pRequest->buf,0,sizeof(pRequest->buf));
		CustomHead* pCH = new CustomHead;
		memset(pCH, 0, sizeof(*pCH));
		pCH->isRequestSucess = iscmdsucessed;

		if (ch.type == TYPE_UPCTRL)
		{
			send_process_drv2upctrl(*pRequest, msgip, *pCH);
		}
		else if (ch.type == TYPE_UPGAME)
		{
			send_process_drv2upgame(*pRequest, msgip, *pCH);
		}

		//package
		string dbgstring = pRequest->endpoint.address().to_string();

		//pump send to asigned endpoint
		CDriver::start_send(pRequest, pCH);
	}

	///////////////////////////////
	//把命令是否失败传回up去
	bool cmdfilter(UpperToDrv data)
	{
		int magicnum = data.upper_cmd;
		if (magicnum != sComd0 && magicnum != sComd2 && magicnum != sComd4
			&& magicnum != sComd6 && magicnum != sComd7 && magicnum != sComd8
			&& magicnum != sComd99)
		{
			return false;
		}

		S_CMD cmd = (S_CMD)magicnum;

		EQU_STATUS equstat = (EQU_STATUS)eq2dr_.rComd;

		bool isRequestSucess_Local = false;
		switch (equstat)
		{
			case status0:
				if (cmd == sComd6)
				{
					isRequestSucess_Local = true;
				}
				break;
			case status1:
				//isRequestSucess_Local = false;
				break;
			case status2:
				if (cmd == sComd4 || cmd == sComd7)
				{
					isRequestSucess_Local = true;
				}
				break;
			case status3:
				if (cmd == sComd0 || cmd == sComd2 || cmd == sComd6 || cmd == sComd7)
				{
					isRequestSucess_Local = true;
				}
				break;
			case status6:
				//isRequestSucess_Local = false;
				break;
			case status7:
				//isRequestSucess_Local = false;
				break;

			case status8:
				::MessageBox(NULL,_T("err"), _T("err"),NULL);
				//isRequestSucess_Local = false;
				break;
			case status99:
				//::MessageBox(NULL, _T("err"), _T("err"), NULL);
				//isRequestSucess_Local = false;
				break;
			default:
				::MessageBox(NULL, _T("err"), _T("err"), NULL);
				//isRequestSucess_Local = false;
				break;
		}

		return isRequestSucess_Local;
	}


	//朝上位control发
	void send_process_drv2upctrl(CMsgIP& outmsgip, CMsgIP inmsgip, CustomHead incustomhead)
	{
		send_process_drv2up_impl(outmsgip, inmsgip, incustomhead);
	}

	//朝上位game发
	void send_process_drv2upgame(CMsgIP& outmsgip, CMsgIP inmsgip, CustomHead incustomhead)
	{
		send_process_drv2up_impl(outmsgip, inmsgip, incustomhead);
	}

	void send_process_drv2up_impl(CMsgIP& outmsgip, CMsgIP inmsgip, CustomHead incustomhead)
	{
		boost::mutex::scoped_lock scopedlock(outputmutex_);

		dr2up_.equ_stat = eq2dr_.rComd;

		CustomHead customhead = incustomhead;// = { TYPE_DRV,DS_UNDEFINED,0, 0 };//
		customhead.type = TYPE_DRV;
		//memcpy(CBase::send_buffer_, &customhead, sizeof(customhead));
		//char* p = CBase::send_buffer_ + sizeof(customhead);
		//memcpy(p, &dr2up_, sizeof(dr2up_));

		//outputmsgip 添加
		outmsgip.endpoint = inmsgip.endpoint;

		memcpy(outmsgip.buf, &customhead, sizeof(customhead));
		char* pother = outmsgip.buf + sizeof(customhead);
		memcpy(pother, &dr2up_, sizeof(dr2up_));
	}

	//drv实时朝设备发
	virtual void start_real_send()
	{
		bool isfirst = true;
		clock_t cur,lst;

		while (true)
		{
			//up2dr_ critical area
			outputmutex_.lock();
			CustomHead customhead;// = { TYPE_UNDEFINED,DS_UNDEFINED,0, 0 };//
			dr2eq_.sComd = up2dr_.upper_cmd;
			outputmutex_.unlock();

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

	/*void start_send(CMsgIP* p)
	{
		socket_.async_send_to(boost::asio::buffer(p->buf), p->endpoint,
			boost::asio::bind_executor(strand_, boost::bind(&CDriver::handle_send, this, p))
		);
	}

	void handle_send(CMsgIP* p)
	{
		delete p;
	}*/

	void start_send(CMsgIP* pMsgIP, CustomHead* pCH)
	{
		socket_.async_send_to(boost::asio::buffer(pMsgIP->buf), pMsgIP->endpoint,
			boost::asio::bind_executor(strand_, boost::bind(&CDriver::handle_send, this, pMsgIP, pCH))
		);
	}

	void handle_send(CMsgIP* p, CustomHead* pCH)
	{
		delete p;
		delete pCH;
	}

	boost::asio::io_context io_context_;

	udp::endpoint tar_endpoint_;
	boost::asio::deadline_timer timer_;
	char send_buffer_[BUF_SIZE];
	char recv_buffer_[BUF_SIZE];
	int misec_;

	udp::endpoint GameEndpoint_;

	boost::mutex outputmutex_;
	boost::mutex equstatmutex_;

	int lasttimestamp_;
	//test
	CMyTime mytimer_;

	UpperToDrv up2dr_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, 0, 0 };
	DrvToUpper dr2up_ = { 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, sComd99, status99, 0 };
	DrvToEqu dr2eq_ = { 0, sComd99,{ 0 },{ 0 },{ 0 },{ 0 } };
	EquToDrv eq2dr_ = { 0, status99,{ 0 },{ 0 },{ 0 },{ 0 } };

	
	Driver_Stat drstat_ = DS_ACCEPTIABLE;
};

using namespace std;

int main()
{


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

