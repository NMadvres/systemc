

#ifndef __PE_ENGRESS__
#define __PE_ENGRESS__

#include "comm_def.h"
#include "comm_func.h"
class pe_engress: public sc_module
{
  public: 
    vector<sc_fifo_in<TRANS> *> input_fifo;
    sc_in_clk clk; 
    SC_HAS_PROCESS(pe_engress);
    global_config_c *m_cfg;
    comm_stat_bw            *m_bw_stat;
    
  public:
    pe_engress(string name, global_config_c *glb_cfg);
    void main_process();
    void recv_packet_process();
    void look_pktedit_process();
    void send_sch_process();
  private:
    vector<deque<TRANS> >   input_que;  //存储实体
    vector<deque<TRANS> >   info_que;   //存储描述符信息
    vector<comm_delay_fifo<TRANS> *>   delay_pipe;   //存储描述符信息
    int m_cycle_cnt;
    RR_SCH                  *rr_sch; 
    

};
 

#endif