


#include "ingress_sch.h"
//激励包长应从顶层获取，先配置固定256B，稍后再改
ingress_sch::ingress_sch(string name, global_config_c *glb_cfg):sc_module(name)
{

    m_cycle_cnt =0;
    m_cfg = glb_cfg;
    input_fifo.resize(g_m_inter_num);
    input_que.resize(g_m_inter_num);
    for(int i=0; i < g_m_inter_num; i++)
    {
        input_fifo[i] = new sc_fifo_in<TRANS>();
    }
    //sch 
    rr_sch = new RR_SCH(g_m_inter_num);
    //stat
    string debug_file = name + string("_debug.log");
    m_bw_stat =new comm_stat_bw(m_cfg, debug_file,g_m_inter_num);

    SC_METHOD(main_process);
    sensitive << clk.pos();

}
void ingress_sch::main_process()
{
   m_cycle_cnt++;   
   recv_packet_process();
   sch_process();

   //stat
   if((m_cycle_cnt !=0) && (m_cycle_cnt % (m_cfg->stat_period *100) ==0))
   {
       m_bw_stat->print_bw_info(m_cycle_cnt);
   }
}

void ingress_sch::recv_packet_process()
{
    for(int i =0; i < g_m_inter_num; i++)
    {
        while(input_fifo[i]->num_available() >0)
        {
            TRANS input_trans;
            bool flag = input_fifo[i]->nb_read(input_trans);
            assert(flag);
            input_que[i].push_back(input_trans);
        }
    }
}

void ingress_sch::sch_process()
{
    //sch process
    for(int i =0; i < g_m_inter_num; i++)
    {
        if(input_que[i].size() >0)
        {
            rr_sch->set_que_valid(i ,true);
        }
        else
        {
            rr_sch->set_que_valid(i ,false);
        }
    }
    
    //调度
    int rst_que =-1;
    bool rst_flag = rr_sch->get_sch_result(rst_que);
    if(rst_flag ==true)
    {
        TRANS front_trans = input_que[rst_que].front();
        input_que[rst_que].pop_front();
        m_bw_stat->record_bw_info(rst_que, front_trans->valid_len, true);
    }
    
}
