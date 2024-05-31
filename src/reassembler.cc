#include "reassembler.hh"
#include <bits/stdint-uintn.h>
#include <cstdint>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{

  //不管大于小于还是等于，都要进行处理：
  //1. first_index正好是当前index的情况下，这样就是正常状态，可以进行正常的插入
  //2. first_index>index的情况下，暂存，根据capacity来
  //3. first_index<index的情况下，也要看看后面有没有可以暂存的部分
  //那就直接遍历好咯
  uint64_t curIdx = first_index;
  
  for (uint64_t i = 0; i < data.length(); i++,curIdx++)
  {

    if(output_.writer().available_capacity()==0)
    {
      return;
    }
    if(curIdx == _currentIndex)
    {
      //等于的情况，顺着插入
      std::string singleChar(1, data.at(i));
      output_.writer().push(singleChar);
      
      _currentIndex++;
      if((i==data.size()-1)&&is_last_substring)
      {
        output_.writer().close();
        return;
      }
    }
    else if(curIdx < _currentIndex)
    {
      ;
    }
    else{
      //考虑暂存
      //如果interbuffer里已经有了相同数据并且index不一样，那么后面的都不要了
      if(_internalBuffer.size()>=output_.writer().available_capacity()-1)
      {
        return;
      }
      /*
     for(auto c:_internalBuffer)
     {
      if(c.second.first == data[i])
      {
        if(c.first != curIdx)
        {
          return;
        }
      }
     }*/
     if((i == data.size() - 1)&&is_last_substring){
        
        _internalBuffer.emplace(curIdx,std::make_pair(data.at(i),1));
        
      }
      else{
        _internalBuffer.emplace(curIdx,std::make_pair(data.at(i),0));
      } 
    }
  }
  if(((curIdx==first_index))&&is_last_substring)
  {
    output_.writer().close();
    return; 
  }
  recurseInsert();
  



  #if 0
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
    //




    //大于的话，先暂存
    //小于的话，也要暂存，因为可能他很长，后面的可以插入之类的
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
      else{
          _internalBuffer.emplace(first_index+curBufIndex,std::make_pair(data.at(curBufIndex),0));
      }      
      curBufIndex++;
      ByteStreamCap = output_.writer().available_capacity();

    }
  }
  #endif
  
  (void)first_index;
  (void)data;
  (void)is_last_substring;

}

uint64_t Reassembler::bytes_pending() const
{
  // Your code here.
  return _internalBuffer.size();
}

void Reassembler::recurseInsert() {
  //在这里进行递归的插入操作
  //根据index从小到大遍历，看看有能否插入的byte，插入完之后对已插入的byte进行标记
  for(auto it = _internalBuffer.begin();it!=_internalBuffer.end();)
  {
    if(it->first<_currentIndex)
    {
      //删除！
      it = _internalBuffer.erase(it);
    }
    else if(it->first == _currentIndex)
    {
      if(output_.writer().available_capacity() == 0)
      {
        //没空间力
        return;
      }
      else{
        //插入
        output_.writer().push(std::string(1,it->second.first));
        _currentIndex++;
        //判断是不是结束符
        if(it->second.second)
        {
          it = _internalBuffer.erase(it);
          output_.writer().close();
          return;
        }
        else
        {
          it = _internalBuffer.erase(it);
        }
      }
    }
    else
    {
      return;
    }
  }
}
