#include "comm_func.h"
#include "string.h"
#include <iostream>
comm_shape_func::comm_shape_func(int shape_value, int tmp_cbs_value, int add_value, int fill_period)
{
    
    cbs_value = tmp_cbs_value;
    token_value = add_value;  //初始令牌
    fill_period = fill_period;
}

 void comm_shape_func::add_token(int add_value)
 {
     token_value +=add_value;
     if(token_value >=cbs_value)
     {
         token_value = cbs_value;
     }
 }
void comm_shape_func::sub_token(int sub_value)
{
    token_value -=sub_value;
}

bool comm_shape_func::shape_status(int packet_len)
{
    if(token_value >=packet_len)
    {
        sub_token(packet_len);
        return true;
    }
    else
    {
        return false;
    }
  
}

RR_SCH::RR_SCH(int tmp_que_num)
{
    que_num = tmp_que_num;
    que_status.resize(que_num,0);
    sch_pos = 0;
}

void RR_SCH::set_que_valid(int que_id, bool valid_flag)
{
    if(que_id >= que_num)
    {
        cout << "error que_id" << que_id <<endl;
    }
    else
    {
        que_status[que_id] = valid_flag;
    }
}

bool  RR_SCH::get_sch_result(int &rst_que)
{
    int tmp_pos = sch_pos;
    for (int i=0; i< que_num; i++)
    {
        tmp_pos = (sch_pos +i) % que_num;
        if(que_status[tmp_pos] == 1)
        {
            sch_pos =tmp_pos;
            rst_que = tmp_pos;
            return true;
        }
    }
    
    return false;
}


template <class T>
comm_delay_fifo<T>::comm_delay_fifo(string name, sc_time t1):sc_module(name)
{
    m_cycle_cnt =0;
    SC_METHOD(main_process);
    sensitive << clk.pos();
}

template <class T>
void comm_delay_fifo<T>::notify(T &)
{
    
}

template <class T>
bool comm_delay_fifo<T>::get_ready_info(T &)
{
    return true;
}

template <class T>
void comm_delay_fifo<T>::main_process()
{
    m_cycle_cnt ++;
}