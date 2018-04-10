//// upper.cpp : Defines the entry point for the console application.
////
//
//#include "stdafx.h"
//
//#include <boost/array.hpp>
//#include <boost/bind.hpp>
//#include <boost/shared_ptr.hpp>
//#include <boost/asio.hpp>
//
//#include <boost/thread/thread.hpp>
//#include <boost/thread/mutex.hpp>
//#include <boost/lockfree/stack.hpp>
//#include <boost/asio/strand.hpp>
//
//#include "../base/base.h"
//
//using boost::asio::ip::udp;
//
//
//
//class CUpper
//{
//public:
//	CUpper() : socket_(io_context_, udp::endpoint(udp::v4(), 14)), strand_(io_context_)
//	{
//		tar_upper_endpoint_ = udp::endpoint(boost::asio::ip::address_v4::from_string("127.0.0.1"), 13);
//		tar_device_endpoint_ = udp::endpoint(boost::asio::ip::address_v4::from_string("192.168.2.151"), 14);
//	}
//
//
//
//	void run(CUpper* pUpper)
//	{
//		group.create_thread(boost::bind(&CUpper::entry, pUpper));
//	}
//
//	void entry()
//	{
//		start_receive();
//		start_send();
//		io_context_.run();
//	}
//
//	void start_receive()
//	{
//		socket_.async_receive_from(
//			boost::asio::buffer(recv_buffer_), remote_endpoint_,
//			/*boost::bind(&CUpper::handle_receive, this,
//				boost::asio::placeholders::error,
//				boost::asio::placeholders::bytes_transferred)*/
//			boost::asio::bind_executor( strand_, boost::bind(&CUpper::handle_receive, this,
//				boost::asio::placeholders::error,
//				boost::asio::placeholders::bytes_transferred))
//		);
//	}
//
//	void handle_receive(const boost::system::error_code& error,
//		std::size_t /*bytes_transferred*/)
//	{
//		if (recv_buffer_[0] == 'a')
//		{
//			int a = 5;
//		}
//
//		if (!error || error == boost::asio::error::message_size)
//		{
//			branchWorkflow();
//			start_receive();
//		}
//	}
//
//	void branchWorkflow()
//	{
//
//	}
//
//
//	void start_send()
//	{
//		send_buffer_[0] = 'a';
//
//		socket_.async_send_to(boost::asio::buffer(send_buffer_), tar_upper_endpoint_,
//			//boost::bind(&CUpper::handle_send, this)
//			boost::asio::bind_executor(strand_, boost::bind(&CUpper::handle_send, this))
//		);
//	}
//
//	void handle_send()
//	{
//		start_send();
//	}
//
//	boost::asio::io_context io_context_;
//	boost::asio::io_context::strand strand_;
//	udp::socket socket_;
//	udp::endpoint remote_endpoint_;
//	udp::endpoint tar_upper_endpoint_;
//	udp::endpoint tar_device_endpoint_;
//	boost::thread_group group;
//	char send_buffer_[256];
//	char recv_buffer_[256];
//};
//
//int main()
//{
//	CUpper* pUpper = new CUpper();
//	pUpper->run(pUpper);
//	getchar();
//    return 0;
//}
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

class CUpper : public CBase
{
public:
	CUpper(int localport, string targetIP, int targetport) : CBase(localport, targetIP, targetport)
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

