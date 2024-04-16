#include "byte_stream.hh"

using namespace std;

ByteStream::ByteStream( uint64_t capacity ) : capacity_( capacity ) {}

bool Writer::is_closed() const
{
  // Your code here.
  return this->closed;
}

void Writer::push( string data )
{
  // Your code here.
  uint64_t datasize = data.size();
  datasize = std::min( datasize, available_capacity() );
  buffer.append( data.substr( 0, datasize ) );
  buffered += datasize;
  totalpushed += datasize;
}

void Writer::close()
{
  this->closed = true;
  // Your code here.
}

uint64_t Writer::available_capacity() const
{
  // Your code here.
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

  return buffer;
}

void Reader::pop( uint64_t len )
{
  // Your code here.
  len = std::min( len, buffered );

  buffer.erase( 0, len );
  buffered -= len;
  totalpoped += len;
}

uint64_t Reader::bytes_buffered() const
{
  // Your code here.
  return buffered;
}
