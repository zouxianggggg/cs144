#include "tcp_receiver.hh"

using namespace std;

void TCPReceiver::receive( TCPSenderMessage message )
{
  
  //invalid seq
  if(ckeckpoint!=0 && message.seqno == ISN)
  {
    return;
  }
  if(message.RST)
  {
    reassembler_.reader().set_error();
    return;
  }
  
  // Your code here.
  if(!ISN.has_value())
  {
    if(!message.SYN)
      return;
    ISN = message.seqno;
    
  }

  uint64_t abseq = message.seqno.unwrap(ISN.value(),ckeckpoint);
  

  uint64_t indice = abseq!=0?abseq-1:abseq;

  uint64_t lastck = reassembler_.writer().bytes_pushed();

  reassembler_.insert(indice,message.payload,message.FIN);


  ckeckpoint += reassembler_.writer().bytes_pushed() - lastck;

  ckeckpoint += (message.SYN);

  if(reassembler_.writer().is_closed())
  {
    ckeckpoint += 1;
  }
  

  //ckeckpoint += (reassembler_.writer().bytes_pushed() - lastck + message.SYN + message.FIN);
}

TCPReceiverMessage TCPReceiver::send() const
{
  // Your code here.
  TCPReceiverMessage TCPRM;
  if(reassembler_.writer().has_error()) TCPRM.RST = true;
  TCPRM.window_size = (reassembler_.writer().available_capacity())>UINT16_MAX?UINT16_MAX:(reassembler_.writer().available_capacity());
  if(ISN.has_value())
  {
    TCPRM.ackno = Wrap32::wrap(ckeckpoint,ISN.value());
  }
  return TCPRM;
}
