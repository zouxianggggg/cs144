#include "tcp_sender.hh"
#include "tcp_config.hh"

using namespace std;

uint64_t TCPSender::sequence_numbers_in_flight() const
{
  // Your code here.
  return {};
}

uint64_t TCPSender::consecutive_retransmissions() const
{
  // Your code here.
  return {};
}

void TCPSender::push( const TransmitFunction& transmit )
{
  //不断的从bytestream读取数据并赶紧发送出去，这里不关心超时重传的报文，只处理滑动窗口部分？
  //首先第一步是看滑动窗口的大小，如果滑动窗口大小为0，就当作是1。这样算是一种保活机制？不然发送端不发了那
  //接收端也不会返回新的ack，这样就直接卡住了。
  //同时也是为了处理第一次发送，即握手这部分操作
  //获取当前的window size
  //uint64_t curWindowsSize = windowsize==0?1:windowsize;

  

  (void)transmit;
  
}

TCPSenderMessage TCPSender::make_empty_message() const
{
  // Your code here.

  return {};
}

void TCPSender::receive( const TCPReceiverMessage& msg )
{
  // Your code here.
  (void)msg;
}

void TCPSender::tick( uint64_t ms_since_last_tick, const TransmitFunction& transmit )
{
  // Your code here.
  (void)ms_since_last_tick;
  (void)transmit;
  (void)initial_RTO_ms_;
}
