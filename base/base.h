#pragma once


// upper.cpp : Defines the entry point for the console application.
//
#include <boost/array.hpp>
#include <boost/bind.hpp>
#include <boost/shared_ptr.hpp>
#include <boost/asio.hpp>

#include <boost/thread/thread.hpp>
#include <boost/thread/mutex.hpp>
#include <boost/lockfree/stack.hpp>
#include <boost/asio/strand.hpp>

#include <boost/container/list.hpp>

using boost::asio::ip::udp;
using namespace std;

#define BUF_SIZE 256

class CBase
{
public:
	typedef struct _CMsgIP
	{
		udp::endpoint endpoint;
		char buf[BUF_SIZE];
	}CMsgIP;

public:
	//CBase(int localport, string targetIP, int targetport) : socket_(io_context_, udp::endpoint(udp::v4(), localport)), strand_(io_context_)
	CBase() : socket_(io_context_), strand_(io_context_)
	{
		//tar_endpoint_ = udp::endpoint(boost::asio::ip::address_v4::from_string(targetIP), targetport);
	}

	void init(int localport, string targetIP, int targetport)
	{
		socket_.open(udp::v4());
		socket_.bind(udp::endpoint(udp::v4(), localport));
		tar_endpoint_ = udp::endpoint(boost::asio::ip::address_v4::from_string(targetIP), targetport);
	}

	void run(CBase* pBase,int threadnum)
	{
		start_receive();
		start_send();

		for (int i = 0; i < threadnum; i++)
		{
			group.create_thread(boost::bind(&CBase::entry, pBase));
		}
		group.join_all();
	}

	void entry()
	{
		io_context_.run();
	}

	void start_receive()
	{
		socket_.async_receive_from(
			boost::asio::buffer(recv_buffer_), remote_endpoint_,
			boost::asio::bind_executor(strand_, boost::bind(&CBase::handle_receive, this,
				boost::asio::placeholders::error,
				boost::asio::placeholders::bytes_transferred))
		);
	}

	void handle_receive(const boost::system::error_code& error,
		std::size_t /*bytes_transferred*/)
	{
		CMsgIP lMsgIP;
		lMsgIP.endpoint = remote_endpoint_;
		memcpy(lMsgIP.buf, recv_buffer_, BUF_SIZE);

		//sync cache received data
		listRecv.push_back(lMsgIP);

		//pump a unprocessed message
		strand_.post(boost::bind(&CBase::process_recv_data,this));

		if (!error || error == boost::asio::error::message_size)
		{
			start_receive();
		}
	}

	void start_send()
	{
		//tmp
		send_buffer_[0] = 'a';

		socket_.async_send_to(boost::asio::buffer(send_buffer_), tar_endpoint_,
			boost::asio::bind_executor(strand_, boost::bind(&CBase::handle_send, this))
		);
	}

	void handle_send()
	{

		start_send();
	}

	virtual void process_recv_data() = 0;

	boost::asio::io_context io_context_;
	boost::asio::io_context::strand strand_;
	boost::thread_group group;
	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	udp::endpoint tar_endpoint_;
	char send_buffer_[BUF_SIZE];
	char recv_buffer_[BUF_SIZE];

	boost::container::list<CMsgIP> listRecv;
};
