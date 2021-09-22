


#include "pe_engress.h"
#if 1
pe_engress::pe_engress(string name, global_config_c *glb_cfg):sc_module(name)
{

    m_cycle_cnt =0;
    m_cfg = glb_cfg;
    input_fifo.resize(g_m_inter_num);
    input_que.resize(g_m_inter_num);
    info_que.resize(g_m_inter_num);
    delay_pipe.resize(g_m_inter_num);
    for(int i=0; i < g_m_inter_num; i++)
    {
        input_fifo[i] = new sc_fifo_in<TRANS>();
        delay_pipe[i] = new comm_delay_fifo<TRANS>(name, 15);
        delay_pipe[i]->clk(clk);
    }
    //sch 
    rr_sch = new RR_SCH(g_m_inter_num);
    //stat
    string debug_file = name + string("_debug.log");
    m_bw_stat =new comm_stat_bw(m_cfg, debug_file,g_m_inter_num);

    //随机数种子
    srand((unsigned)time(NULL));

    SC_METHOD(main_process);
    sensitive << clk.pos();

}
void pe_engress::main_process()
{
   m_cycle_cnt++;   
   recv_packet_process();
   look_pktedit_process();
   send_sch_process();

   //stat
   if((m_cycle_cnt !=0) && (m_cycle_cnt % (m_cfg->stat_period *100) ==0))
   {
       m_bw_stat->print_bw_info(m_cycle_cnt);
   }
}

void pe_engress::recv_packet_process()
{
    for(int i =0; i < g_m_inter_num; i++)
    {
        while(input_fifo[i]->num_available() >0)
        {
            TRANS input_trans;
            bool flag = input_fifo[i]->nb_read(input_trans);
            assert(flag);
            input_que[i].push_back(input_trans);  //报文实体写入input que
            delay_pipe[i]->notify(input_trans);   //描述符信息存入delay pipe
        }
    }
}

//模拟描述符查表延时
 void pe_engress::look_pktedit_process()
 {
    for(int i =0; i < g_m_inter_num; i++)
    {
        TRANS info_trans;
        while(delay_pipe[i]->get_ready_info(info_trans))
        {
            info_que[i].push_back(info_trans);  
        }
    }

 }

//队列非空&&描述符查表返回后，可以参与调度，调度后的报文编辑后
//调度部分待添加流控响应
void pe_engress::send_sch_process()
{
    //sch process
    for(int i =0; i < g_m_inter_num; i++)
    {
        if((input_que[i].size() >0)&&(info_que[i].size() >0))
        {
            rr_sch->set_que_valid(i ,true);
        }
        else
        {
            rr_sch->set_que_valid(i ,false);
        }
    }
    
    //调度，做报文编辑后发出
    int rst_que =-1;
    bool rst_flag = rr_sch->get_sch_result(rst_que);
    if(rst_flag ==true)
    {
        TRANS front_trans = input_que[rst_que].front();
        input_que[rst_que].pop_front();
        info_que[rst_que].pop_front();
        
        //报文编辑后发出,编辑范围-20~20Byte
        int mody_len = -rand()%41+20;
        front_trans->valid_len += mody_len;
        m_bw_stat->record_bw_info(rst_que, front_trans->valid_len, true);
    }
    
}

#endif