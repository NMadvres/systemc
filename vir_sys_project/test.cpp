

#include "stdio.h"
#include <iostream>
#include "packet_gen.h"
#include "ingress_sch.h"
#include "pe_engress.h"
#include "systemc.h"
#include <memory>
#include "comm_def.h"
#include "comm_func.h"
using namespace std;


int sc_main(int argc, char *argv[])
{
   sc_clock clk("clk",10,SC_NS);  //10ns 对应100MHZ 
   vector< sc_fifo<TRANS> * >ingress_fifo;  
   vector< sc_fifo<TRANS> * >egress_fifo;  
   ingress_fifo.resize(g_m_inter_num);
   egress_fifo.resize(g_m_inter_num);
   for(int i=0; i < g_m_inter_num; i++)
   {
      ingress_fifo[i] =new sc_fifo<TRANS>();
      egress_fifo[i] =new sc_fifo<TRANS>();
   }

   //init module
   global_config_c *glb_cfg = new global_config_c();
   packet_gen_module pkt_gen_mod("packet_gen", glb_cfg);
   pkt_gen_mod.clk(clk);

   ingress_sch ing_sch_mod("recv_sch", glb_cfg);
   ing_sch_mod.clk(clk);

   pe_engress  pe_engr_mod("pe_engress", glb_cfg);
   pe_engr_mod.clk(clk);
   
   //blind
   for(int i =0; i < g_m_inter_num; i++)
   {
      pkt_gen_mod.output[i]->bind(*ingress_fifo[i]);
      ing_sch_mod.input_fifo[i]->bind(*ingress_fifo[i]);
      
      ing_sch_mod.output_fifo[i]->bind(*egress_fifo[i]);
      pe_engr_mod.input_fifo[i]->bind(*egress_fifo[i]);
      //(*pkt_gen_mod.output[i])(*tmp_fifo[i]);
      //(ing_sch_mod.input_fifo[i])(tmp_fifo[i]);
   }
   //char

   sc_start(100,SC_MS);

   return 0;
}