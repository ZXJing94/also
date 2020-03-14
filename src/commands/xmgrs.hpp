/* also: Advanced Logic Synthesis and Optimization tool
 * Copyright (C) 2019- Ningbo University, Ningbo, China */

/**
 * @file xmgrs.hpp
 *
 * @brief XMG resubstitution
 *
 * @author Zhufei Chu
 * @since  0.1
 */

#ifndef XMGRS_HPP
#define XMGRS_HPP

#include <mockturtle/mockturtle.hpp>
#include <mockturtle/algorithms/xmg_resub.hpp>
#include <mockturtle/networks/xmg.hpp>

#include "../core/misc.hpp"

namespace alice
{

  class xmgrs_command : public command
  {
    public:
      explicit xmgrs_command( const environment::ptr& env ) : command( env, "Performs XMG resubsitution" )
      {
        add_flag( "-v,--verbose", ps.verbose, "show statistics" );
      }
      
      rules validity_rules() const
      {
        return { has_store_element<xmg_network>( env ) };
      }

    protected:
      void execute()
      {
        /* derive some XMG */
         xmg_network xmg = store<xmg_network>().current();

         xmg_network xmg1, xmg2;
         xmg1 = xmg;
        
         using view_t = depth_view<fanout_view<xmg_network>>;
         fanout_view<xmg_network> fanout_view{xmg};
         view_t resub_view{fanout_view};
         xmg_resubstitution( resub_view, ps, &st );
         //xmg_resubstitution( xmg, ps, &st );
         xmg = cleanup_dangling( xmg );

         xmg2 = xmg;

         std::cout << "[xmgrs] "; 
         also::print_stats( xmg ); 
         
         const auto miter_xmg = *miter<xmg_network>( xmg1, xmg2 ); 
         equivalence_checking_stats eq_st;
         const auto result = equivalence_checking( miter_xmg, {}, &eq_st );
         std::cout << "result: " << *result << std::endl;
         assert( *result );


         store<xmg_network>().extend(); 
         store<xmg_network>().current() = xmg;
      }
    
    private:
      resubstitution_params ps;
      resubstitution_stats st;
  };

  ALICE_ADD_COMMAND( xmgrs, "Rewriting" )

}

#endif
