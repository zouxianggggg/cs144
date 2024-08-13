#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{

  // invalid seq
  if ( ckeckpoint != 0 && message.seqno == ISN ) {
    return;
  }
  if ( message.RST ) {
    reassembler_.reader().set_error();
    return;
  }

  // 初始化ISN
  if ( !ISN.has_value() ) {
    if ( !message.SYN )
      return;
    ISN = message.seqno;
  }

  uint64_t abseq = message.seqno.unwrap( ISN.value(), ckeckpoint );

  // 为什么要在abseq==0时取0值，占位符罢了，只是为了处理这第一包数据没有payload但有FIN，我们能正常关闭TCP
  // 有payload的话，那么payload的第一个字节也是0，不影响。为了能在payload为空时调用insert
  uint64_t indice = abseq != 0 ? abseq - 1 : abseq;

  // 为了更新checkpoint，因为接收到N长度的数据但不一定能push进N长度的
  uint64_t lastck = reassembler_.writer().bytes_pushed();

  reassembler_.insert( indice, message.payload, message.FIN );

  // 根据push进的data更新checkpoint即当前的abseq,用is_closed判断是否有FIN
  ckeckpoint += reassembler_.writer().bytes_pushed() - lastck + message.SYN + reassembler_.writer().is_closed();
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage TCPRM;
  if ( reassembler_.writer().has_error() )
    TCPRM.RST = true;
  // 为什么window size是available_capacity而不是available_capacity-pending_bytes？
  // 存在assembler里的数据不算，只有push进去的才算
  TCPRM.window_size = ( reassembler_.writer().available_capacity() ) > UINT16_MAX
                        ? UINT16_MAX
                        : ( reassembler_.writer().available_capacity() );
  if ( ISN.has_value() ) {
    TCPRM.ackno = Wrap32::wrap( ckeckpoint, ISN.value() );
  }
  return TCPRM;
}
