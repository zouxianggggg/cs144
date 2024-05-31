#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : buffer(capacity),head(0),tail(0),is_full(false),capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return this->closed;
}

void Writer::push( string data )
{
  uint64_t datalength = data.size();
  uint64_t writesize = std::min(datalength,available_capacity());
  //现在有writesize大小的空间可以写
  //记录一下data写入到哪个字节了
  uint64_t datawriteindex = 0;
  while (writesize)
  {
    //先算一下不需要循环回到数组头的写入部分的大小
    //如果说不需要的回到数组头的话，那写入部分就是writesize，
    //如果需要回的话，那么当前写入的大小就是总大小减去tail的位置
    uint64_t spacetoend = std::min(writesize, capacity_-tail);
    //然后开始对这一部分进行写入
    memcpy(buffer.data()+tail,data.data()+datawriteindex,spacetoend);
    //移动datawriteindex
    datawriteindex += spacetoend;
    tail = (tail+spacetoend)%capacity_;
    writesize -= spacetoend;
    is_full = ((tail == head)&&(tail!=0));
    buffered += spacetoend;
    totalpushed += spacetoend;
  }
  



  // // Your code here.
  // uint64_t datasize = data.size();
  // datasize = std::min( datasize, available_capacity() );
  // buffer.append( data.substr( 0, datasize ) );
  // buffered += datasize;
  // totalpushed += datasize;
}

void Writer::close()
{
  this->closed = true;
  // Your code here.
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
  if(is_full)
    return 0;
  return capacity_ - buffered;
}

uint64_t Writer::bytes_pushed() const
{
  // Your code here.
  return totalpushed;
}

bool Reader::is_finished() const
{
  // Your code here.
  return closed && ( buffered == 0 );
}

uint64_t Reader::bytes_popped() const
{
  // Your code here.
  return totalpoped;
}

string_view Reader::peek() const
{
  // Your code here.
  string peekreturn = "";
  //peekreturn.clear();
  //peekreturn.reserve(buffered); 
  if(head < tail)
  {
    peekreturn.append(buffer.begin()+head,buffer.begin()+tail);
  }
  else
  {
    peekreturn.append(buffer.begin()+head,buffer.end()); 
    peekreturn.append(buffer.begin(),buffer.begin()+tail);
  }
  //string_view ret = peekreturn;
  
  return string_view(&buffer[head],capacity_-head);
}

void Reader::pop( uint64_t len )
{
  //pop的话是不是都不用实际修改数组里面的值，只需要移动head就好了
  uint64_t poplen = std::min(len,buffered);
  while (poplen)
  {
    //先计算不需要回到数组头的移动的head的长度
    uint64_t movetoend = std::min(len,capacity_-head);
    //开始pop
    head = (head+movetoend)%capacity_;
    buffered -= movetoend;
    totalpoped += movetoend;
    poplen -= movetoend;
    is_full = false;
  }
  




/*   // Your code here.
  len = std::min( len, buffered );

  buffer.erase( 0, len );
  buffered -= len;
  totalpoped += len; */
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffered;
}
