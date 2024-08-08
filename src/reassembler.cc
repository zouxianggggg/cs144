#include "reassembler.hh"
#include <bits/stdint-uintn.h>
#include <cstdint>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{

  //首先判断还能不能存了，不能直接return啊
  if(output_.writer().available_capacity()==0)
  {
    return;
  }


  //先进行对First_UnassembledIndex的截取，即小于他的肯定没用了，然后再进行capacity的判断

  //如果说他得first_index大于First_UnassembledIndex+capacity，那么也不能存，直接return
  //首先判断经过重组之后data能不能全部push进bytestream，如果不能第一步是先进行截取。
  //截取还需要考虑first_index和First_UnassembledIndex
  uint64_t stIndex = 0;
  if(First_UnassembledIndex >= first_index)
  {
    //说明
    stIndex = First_UnassembledIndex-first_index;
  }
  //先截取吧
  //这里截取index可能会超过当前的字符串

  uint64_t len = data.size();
  if(len<stIndex) return;
  len = min(len-stIndex,First_UnassembledIndex+output_.writer().available_capacity()-first_index);
  //len = min(len,data.size()-stIndex);
  string curdata = data.substr(stIndex,len);

  if(is_last_substring && (len+stIndex == data.size()))
  {
    ending_index = first_index + len;
    ending_flag = true;
  }

  //第二步进行合并操作,应该进行前后的合并，因为前后都是合并好的，只需要考虑当前的data就好了
  {
    //首先考虑和前面的合并
    //当前的data的索引
    
    uint64_t curdataIndex = first_index + stIndex;
    //找到第一个小于当前索引的元素
    auto leftside = hashmap.lower_bound(curdataIndex);
    //标记合并左边之后当前data是否不需要了
    bool isVanished = false;
    if(leftside == hashmap.begin())
    {
      //说明没有元素小于当前元素，这边就不用合并了
      //continue;
    }
    else{
      --leftside;//获取到第一个小于当前索引的元素
      //开始合并左边的
    //第一种情况，左边和当前的中间还有空隙，就没法合并了
    if((leftside->first + leftside->second.size())<curdataIndex)
    {
        //直接放弃
        //continue;
    }
    else if(leftside->first + leftside->second.size()==curdataIndex)
    {
        //能直接拼
        leftside->second.append(curdata);
        //把
        isVanished = true;
    }
    else{
        uint64_t leftsideendindex = leftside->first + leftside->second.size();
        uint64_t overlaplength = leftsideendindex - curdataIndex;
        if(overlaplength < len)
        {
          leftside->second.append(curdata.substr(overlaplength));
        }
        isVanished = true;
    }
    }
    //左边合并完了，开始合并右边，右边所有的，不会只有一个
    auto rightside = hashmap.lower_bound(curdataIndex);
    if(rightside == hashmap.end())
    {
      //右边没有了，直接插或者五十发生
      if(!isVanished)
      {
        hashmap.emplace(std::make_pair(curdataIndex,curdata));
      }
    }
    else{
      if(!isVanished)
      {
        //左边没有合并，合并当前和右边的
        if(curdataIndex + curdata.size() < rightside->first)
        {
          hashmap.emplace(std::make_pair(curdataIndex,curdata));
        }
        else if(curdataIndex + curdata.size() == rightside->first)
        {
          curdata.append(rightside->second);

          hashmap.emplace(std::make_pair(curdataIndex,curdata));

          hashmap.erase(rightside);
        }
        else{
          //现在的情况是当前data覆盖了若干个map里已有的数据，需要吧
          size_t overlaplength = (curdataIndex + curdata.size()) - rightside->first;
          while (overlaplength >= rightside->second.size())
          {
            /* code */
            hashmap.erase(rightside);
            rightside = hashmap.lower_bound(curdataIndex);
            if(rightside!=hashmap.end())
            {
                  overlaplength = ((curdataIndex + curdata.size()) > rightside->first)?((curdataIndex + curdata.size()) - rightside->first):0;
            }
            else{
              overlaplength = 0;
              break;
            }
            
          }
          if(overlaplength > 0)
          {
              curdata.append(rightside->second.substr(overlaplength)); 
              hashmap.erase(rightside);
          }
          // hashmap.erase(rightside);
          hashmap.emplace(std::make_pair(curdataIndex,curdata));
        }
      }
      else{
        //左边合并了，合并左边和右边的
        if((leftside->first + leftside->second.size())>=rightside->first)
        {
            uint64_t overlaplength = (leftside->first + leftside->second.size())-rightside->first;
            if(overlaplength < rightside->second.size())
            {
              leftside->second.append(rightside->second.substr(overlaplength));
            }
              hashmap.erase(rightside);
            
        }
      }
    }
    
  }


  //合并完了之后就可以进行插入操作了，也是直接lowerbond就完事了
  while (true)
  {
    //找到下标正好是待排序的索引的data
    auto curyuansu = hashmap.lower_bound(First_UnassembledIndex);
    //没找到就说明没有，还要等待，直接return吧
    if(curyuansu == hashmap.end())
    {
      break;
    }
    if(curyuansu->first == First_UnassembledIndex)
    {
      //找到了，就push
      uint64_t curyuansulen = curyuansu->second.size();
      output_.writer().push(curyuansu->second);
      First_UnassembledIndex += curyuansulen;
      hashmap.erase(curyuansu);
    }
    else{
      break;
    }
    
  }

  if(ending_flag)
  {
    if(First_UnassembledIndex >= ending_index)
    {
      output_.writer().close();
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
  for(const auto& c:hashmap)
  {
    ret += c.second.size();
  } 
  return ret;
}

