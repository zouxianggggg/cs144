#include "reassembler.hh"
#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
 //首先判断几种不处理data的情况
 //1.output的写端关闭了
 //2.first_index > first_unassembled_index+avaliable_capacity
 //3.first_index >= eof_index
 if(output_.writer().is_closed() || (first_index>first_unassembled_index+output_.writer().available_capacity()))
 {

 }
}

uint64_t Reassembler::bytes_pending() const
{
 
}
