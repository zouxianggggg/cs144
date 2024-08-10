#include "reassembler.hh"
#include <algorithm>

using namespace std;

void Reassembler::insert( uint64_t first_index, string data, bool is_last_substring )
{
 //首先判断几种不处理data的情况
 //1.output的写端关闭了
 //2.first_index > first_unassembled_index+avaliable_capacity
 //3.first_index >= eof_index
 if(output_.writer().is_closed() || (first_index>=first_unassembled_index+output_.writer().available_capacity()) || (first_index>=eof_index))
 {
    return;
 }

 //直接更新eof
 if(is_last_substring)
 {
    eof_index = first_index + data.size();
 }

 //然后是对data进行截取，即first_unassembled_index之前的数据都不要了
 //然后还有一个问题，就是reassembler的internal storage在合并之后只能存不大于output的avaliablecapacity的数据，注意是合并后
 //即前后都要截取一次
 uint64_t wdStartIdx = first_unassembled_index;
 uint64_t wdEndIdx = first_unassembled_index + output_.writer().available_capacity();
 uint64_t curStartIdx = max(wdStartIdx,first_index);
 uint64_t curEndIdx = min(wdEndIdx,first_index+data.size());
 if(curEndIdx <= curStartIdx)
 {
    if (first_unassembled_index >= eof_index) {
            output_.writer().close();
        }
        return;
 }
 //截取data
 string curData = data.substr(curStartIdx-first_index,curEndIdx-curStartIdx);
 interval itv(curStartIdx,curEndIdx,curData);
 //插入set并合并
 //先直接插入set
 buffers.insert(itv);
 //合并暂存到vector中
 vector<interval> merged;

 auto it = buffers.begin();
 interval cur = *it;
 it++;
 while (it!=buffers.end())
 {
    if(it->start_idx <= cur.end_idx)
    {
        cur.end_idx = max(cur.end_idx,it->end_idx);
        if(cur.end_idx <= it->end_idx)
        {
            //拼接的话就是取前面的data的不重叠的部分直接加上后面的，然后前面的不重叠的部分就是两个startidx相减
            cur.data = cur.data.substr(0,it->start_idx - cur.start_idx) + it->data;
        }
    }
    else{
        merged.push_back(cur);
        cur = *it;
    }
    it++;
 }
 merged.push_back(cur);
 //现在塞回set
 buffers.clear();
 for(const auto& m:merged)
 {
    buffers.insert(m);
 }

 //然后就是进行push操作
it =buffers.begin();
 while(it->start_idx == first_unassembled_index)
 {
    output_.writer().push(it->data);
    first_unassembled_index = it->end_idx;
    it = buffers.erase(it);
    if(it == buffers.end())
    {
        break;
    }
 }

 if(first_unassembled_index >= eof_index)
 {
    output_.writer().close();
 }
 


}

uint64_t Reassembler::bytes_pending() const
{
    uint64_t pendcnt = 0;
    for (const auto& interval : buffers) {
        pendcnt += interval.end_idx - interval.start_idx;
    }
    return pendcnt;
}
