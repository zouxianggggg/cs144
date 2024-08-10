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
 //插入list并合并
if(buffers.empty())
{
    buffers.insert(buffers.begin(),itv);
}
else
{
 //遍历list，找到第一个比他大的节点
 auto it = lower_bound(buffers.begin(),buffers.end(),itv);
 auto prev = it;
 //检查是否是begin
 if(it == buffers.begin()){
    //那就在前面直接插入
    buffers.insert(it,itv);
    prev = std::prev(it);
 }
 else{
    //如果前面还有元素
 
    //先合并左边的
    prev = std::prev(it);
 
    if(prev->end_idx >= itv.start_idx)
    {
        //可以合并
        if(prev->end_idx <= itv.end_idx)
        {
            prev->data = prev->data.substr(0,itv.start_idx - prev->start_idx) + itv.data;
            prev->end_idx = itv.end_idx;
        }
        itv = *prev;
    }
    else{
        //不能合并
        buffers.insert(it,itv);
        prev++;
    }

 }
 //此时prev要么就是合并好的左边节点，要么就是新加入的节点
 //合并右边的
 while(it!=buffers.end() && itv.end_idx>=it->start_idx)
 {
    if(it->end_idx > itv.end_idx)
    {
        //需要合并，因为itv代表的prev是较小的那个
        prev->data = prev->data.substr(0,it->start_idx - itv.start_idx) + it->data;
        prev->end_idx = it->end_idx;
    }
    it = buffers.erase(it);
    itv = *prev;
 }
}



 //然后就是进行push操作
auto it =buffers.begin();
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
