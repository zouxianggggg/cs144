#include "reassembler.hh"
#include <bits/stdint-uintn.h>
#include <cstdint>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
  // Your code here.
  //首先判断当前的first_index，如果当前first_index正好是current_index+1，那么就可以将其插入到bytestream中
  //如果index对不上的话，那么就暂时先将其放在缓冲区中
  if (first_index == _currentIndex)   //正好是下一个index的情况
  {
    //首先能根据data的长度和bytestream的capacity综合判断，能写多少写多少
    uint64_t writeIndex = 0;
    uint64_t dataLength = data.size();
    uint64_t ByteStreamCap = output_.writer().available_capacity();
    while ((writeIndex < dataLength) && ByteStreamCap)
    {
      std::string singleChar(1, data.at(writeIndex));
      output_.writer().push(singleChar);
      _currentIndex++;
      writeIndex++;
      ByteStreamCap = output_.writer().available_capacity();
    }
    // If capacity is 0, it means we can't write anymore. In this case, we don't put the remaining bytes into the buffer and simply discard them.
    // If data is fully written and it is the last substring, we need to close the writer.
    if ((writeIndex == dataLength) && is_last_substring)
    {
      output_.writer().close();
      return;
    }
    //如果不是终结符，并且当前已经插入完毕的话，继续递归的插入
    while (recurseInsert()) {}
  }
  else if(first_index > _currentIndex)
  {
    //大于的话，先暂存
    //需不需要考虑capacity的大小？需要的，跟上面的直接插入一样，也是暂存datasize和capacity的最小值
    uint64_t datasize = data.size();
    uint64_t ByteStreamCap = output_.writer().available_capacity();
    uint64_t curBufIndex = 0;
    while ((curBufIndex < datasize) && ByteStreamCap)
    {
      //判断当前index如果是data的最后一个byte，而且如果是终结符，那么要将对应的bool值变为true
      if(curBufIndex == datasize - 1){
        if(is_last_substring)
        {
          _internalBuffer.emplace(first_index+curBufIndex,std::make_pair(data.at(curBufIndex),1));
        }
      }
      
      _internalBuffer.emplace(first_index+curBufIndex,std::make_pair(data.at(curBufIndex),0));

    }


    





  }
  else
  {
    //小于的话，直接丢弃
  }
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return _internalBuffer.size();
}

bool Reassembler::recurseInsert() {
  //在这里进行递归的插入操作
  if(output_.writer().available_capacity() == 0){
    return false;
  }


  //首先停止条件，有两种停止条件，_internalBuffer为空或者没有index符合要求的数据
  for(auto &subs: _internalBuffer)
  {
    if(subs.first == _currentIndex)
    {

      //capacity需要大于datalength才能插入
      //开始插入
      
      output_.writer().push(std::string(1,subs.second.first));
      _currentIndex ++;
      //如果是结束符号，需要关闭writer
      if(subs.second.second)
      {
        output_.writer().close();
        return false;
      }
      else{
        _internalBuffer.erase(subs.first);
        return true;
      }
      //插入完并且不算结束之后把当前插入的从buffer中删掉
    }
  }
  return false;
}
