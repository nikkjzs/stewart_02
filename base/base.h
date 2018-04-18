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


typedef struct
{
	float x;
	float y;
	float z;
	float pitch;
	float yaw;
	float roll;
	float reserve1;
	float reserve2;
	float reserve3;
	float timeStamp;
	int upper_cmd;
	int equ_stat;
	bool reset;
}UpperToDrv;



typedef struct
{
	unsigned char sMark;     //��־λ��Ĭ��Ϊ55��   
	unsigned char sComd;     //��λ������ 
	unsigned char sRepa[2];  //��Ĭ��Ϊ0�� 
	float sAtti[6]; //������̬�������£��ã�X��Y��Z�� 
	float sVelo[6]; //��Ĭ��ֵΪ0�� 
	float sAcce[3]; //���ٶȣ�Ĭ��ֵΪ0�� 
}DrvToEqu;


typedef struct
{
	unsigned char rMark;
	unsigned char rComd;
	unsigned char rPara[2];
	float rAtti[6];
	float rRese[6];
	float rMoto[6];
}EquToDrv,DrvToUpper;


typedef struct
{
	int timestamp;
}CustomHead;


enum EQU_STATUS
{
	status0 = 0,        //ƽֹ̨ͣ���ȴ�����
	status1 = 1,        //ƽ̨���ڻ���λ����ʱ����Ӧ��λ������ 
	status2 = 2,        //ƽ̨����λ 
	status3 = 3,        //ƽ̨�ڹ���̬�������������˶����� 
	status6 = 6,        //ƽ̨���ڿ���
	status7 = 7,        //ƽ̨���ڹػ�
	status8 = 8,        //ƽ̨����
	status99 = 99,      //custom default value
};


enum S_CMD
{
	sComd0 = 0,			//���������˶�����
	sComd2 = 2,			//����ƽ̨�ص�����λ
	sComd4 = 4,			//����Э�飺ƽ̨�л�������̬
	sComd6 = 6,			//�������ƽ̨�ɵ�λ����������λ
	sComd7 = 7,			//�ػ����ƽ̨������λ�ص���λ
	sComd8 = 8,			//����
	sComd99 = 99,
};


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
	CBase() : socket_(io_context_), strand_(io_context_)
	{
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
		//listRecv.push_back(lMsgIP);

		//pump a unprocessed message
		//strand_.post(boost::bind(&CBase::process_recv_data,this));

		if (!error || error == boost::asio::error::message_size)
		{
			start_receive();
		}


		process_recv_data(lMsgIP);
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

	//virtual void process_recv_data(CMsgIP lMsgIP) = 0;
	virtual void process_recv_data(CMsgIP lMsgIP)
	{}

	boost::asio::io_context io_context_;
	boost::asio::io_context::strand strand_;
	boost::thread_group group;
	udp::socket socket_;
	udp::endpoint remote_endpoint_;
	udp::endpoint tar_endpoint_;
	char send_buffer_[BUF_SIZE];
	char recv_buffer_[BUF_SIZE];

	//boost::container::list<CMsgIP> listRecv;
};
