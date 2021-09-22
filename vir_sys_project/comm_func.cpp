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
            sch_pos =(tmp_pos +1) % que_num;
            rst_que = tmp_pos;
            return true;
        }
    }
    
    return false;
}



//统计类相关
comm_stat_bw::comm_stat_bw(global_config_c *glb_cfg, string file_name, int que_num)
{
    m_glb_cfg = glb_cfg;
    m_file_name ="run_log/" + file_name;
    m_que_num = que_num;
    m_que_pktlen_stat.resize(m_que_num,0);
    m_que_pktnum_stat.resize(m_que_num,0);
    m_total_pktlen_stat =0;
    m_total_pktnum_stat =0;
    m_stat_period = glb_cfg->stat_period;
    m_fp =  fopen (m_file_name.c_str(), "w+");
    if(m_fp ==NULL)
    {
        system("mkdir -p run_log");
        m_fp =  fopen (m_file_name.c_str(), "w+");
    }
    fclose(m_fp);
}

void comm_stat_bw::record_bw_info(int que_id, int valid_len, int is_eop)
{
    if(!is_eop)
    {
        m_que_pktlen_stat[que_id] +=valid_len;
        m_total_pktlen_stat +=valid_len;
    }
    else
    {
        m_que_pktlen_stat[que_id] += valid_len + g_m_ipg_len;
        m_que_pktnum_stat[que_id] += 1;

        m_total_pktlen_stat += valid_len + g_m_ipg_len;
        m_total_pktnum_stat += 1;
    }
    

}

void comm_stat_bw::print_bw_info(int m_cycle)
{
    m_fp =  fopen (m_file_name.c_str(), "aw+"); 
    if(m_fp == NULL)
    {
        ASSERT(0);
    }
    fprintf(m_fp,    "---------------------cur_time:%8d----------------------------------\n", m_cycle);
    double port_bps;
    double port_pps; 
    double total_bps;
    double total_pps;
    for(int i =0; i < m_que_num; i++)
    {      
        //计算端口级带宽bps和PPS，基于端口计算
        port_bps =(double)m_que_pktlen_stat[i] *8 / m_stat_period ; // Mbps 
        port_pps =(double)m_que_pktnum_stat[i]    / m_stat_period ; // MPPS 
        fprintf(m_fp,"-------cur_portid:%4d    bandwidth:%.2f Mbps(%.2fMPPS)--------\n", i, port_bps, port_pps);
        //清零
        m_que_pktlen_stat[i] =0;
        m_que_pktnum_stat[i] =0;
    }
    //计算total级带宽bps和PPS，基于total计算
    total_bps =(double)m_total_pktlen_stat  *8 / m_stat_period ; // Mbps 
    total_pps =(double)m_total_pktnum_stat     / m_stat_period ; // MPPS 
    m_total_pktlen_stat =0;
    m_total_pktnum_stat =0;
    fprintf(m_fp,"-------total------------,    bandwidth:%.2f Mbps(%.2fMPPS)--------\n\n\n", total_bps, total_pps);


    fclose(m_fp);
}