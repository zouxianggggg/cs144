#include "reassembler.hh"

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
    while ((writeIndex<dataLength)&&ByteStreamCap)
    {
     output_.writer().push(std::string(data.at(writeIndex++))); 
     _currentIndex++;
     ByteStreamCap = output_.writer().available_capacity();
    }
    //如果capacity为0，则说明不能再写了，此时也不会将剩下的byte放入buffer中，而是直接丢弃
    //如果是data写完了且正好是last，需要关闭writer
    if((writeIndex == dataLength) && is_last_substring)
    {
      output_.writer().close();
      return;
    }
    //如果不是终结符，并且当前已经插入完毕的话，继续递归的插入
    


    output_.writer().push(data);
    //auto wt = this->writer();
    //wt.push(data);
    _currentIndex += dataLength;
    //如果是laststring，需要关闭writer
    if(is_last_substring)
    {
      output_.writer().close();
      return;
    }
    else
    {
      //这个插完了，还要将map里的data都遍历一遍，看有没有可以插入的
      while(recurseInsert()){};
    }

  }
  else if(first_index > _currentIndex)
  {
    //大于的话，先暂存，然后等待时机push，存的时候，既要考虑index之间的关系，还要考虑当前data的长度是否超过了bytestream的当前可用容量
    std::pair<std::string,bool> currentStr = {data,is_last_substring};
    uint64_t last_index = first_index + data.size();
    //既然是每一个byte一个index，那么还要计算一下他的index的上下界
    bool insertFlag = true;
    uint64_t currentInternalBufferSize = 0;
    for(auto &mapPair : _internalBuffer)
    {
      currentInternalBufferSize += mapPair.second.first.size();
      //当前substring的第一个byte的index
      uint64_t curSubstringStartIndex = mapPair.first;
      uint64_t curSubstringEndIndex = mapPair.second.first.size()+ curSubstringStartIndex;
      //如果当前的data和mapPair的data的index之间有重叠的话，就不能暂存
      bool isoverlap = ((first_index <= curSubstringEndIndex)&&(curSubstringStartIndex <= last_index));
      if(isoverlap)
      {
        insertFlag = false;
      }
    }
    //接下来是检查internalbuffer中所有的data加起来的长度是否超过了bytestream的可用长度
    uint64_t curByteStreamCapacity = this->writer().available_capacity();
    if((currentInternalBufferSize + data.size()) > curByteStreamCapacity)
    {
      insertFlag = false;
    }
    if(insertFlag)
    {
      //都没问题了就放进internalBuffer中去
      _internalBuffer.emplace(first_index,currentStr);
    }
    


  }
  

  (void)first_index;
  (void)data;
  (void)is_last_substring;
}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  uint64_t ret = 0;
  for(auto &map:_internalBuffer)
  {
    ret += map.second.first.size();
  }
  return ret;
}

bool Reassembler::recurseInsert() {
  //在这里进行递归的插入操作

  //首先停止条件，有两种停止条件，_internalBuffer为空或者没有index符合要求的数据
  for(auto &subs: _internalBuffer)
  {
    if(subs.first == _currentIndex)
    {

      //capacity需要大于datalength才能插入




      //开始插入
      uint64_t datalength = subs.second.first.size();
      
      output_.writer().push(subs.second.first);
      _currentIndex += datalength;
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
