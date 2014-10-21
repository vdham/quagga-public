#include <zebra.h>

#include "if.h"
#include "zebra/mpls_lib.h"
#include "zebra/mpls_msg.h"
#include "zebra/zebra_fpm.h"

int mpls_kernel_enable_interface (struct interface *ifp) { return 0; };
int mpls_kernel_disable_interface (struct interface *ifp) { return 0; };

static int
mpls_fpm_ilm (int cmd, unsigned flags, u_int32_t in_label,
		 struct mpls_lsp *lsp)
{
	  int nhlfe_op = 0;

	  /* MPLS LSP */
	  if (lsp)
	    {
        /* MPLS label operation */
	    if (in_label != MPLS_IMPLICIT_NULL)
		{
	      zlog_debug("SWAP in_label %d remote_label %d", in_label, lsp->remote_label);
		  nhlfe_op = SWAP;
		  sendNHLFEUpdate(cmd, nhlfe_op, in_label, lsp->remote_label,
		  		lsp->addr.s_addr);
		}
	      else
		{
	      zlog_debug("POP in_label %d", in_label);
		  nhlfe_op = POP;
		  sendNHLFEUpdate(cmd, nhlfe_op, in_label, 0,
				  		lsp->addr.s_addr);
		}

	    }
	  /* "No Label" case */
	  else
	    {//revisit this vdham
	     /* struct rtattr *mpls;

	      if (IS_ZEBRA_DEBUG_KERNEL)
		zlog_debug ("mpls_kernel_ilm(): no label");

	      mpls = addattr_nest (&req.n, sizeof (req), RTA_MPLS);
	      u_int8_t pop = POP_ALL;
	      addattr_l (&req.n, sizeof (req), MPLSA_POP, &pop, sizeof (pop));
	      addattr_nest_end (&req.n, mpls);*/

		//  zlog_debug("POP in_label %d", in_label);

  		  //nhlfe_op = POP;

  		  //sendNHLFEUpdate(cmd, nhlfe_op, in_label, 0,
		  				  	//	0);

		  if (in_label == MPLS_IMPLICIT_NULL)
		  {
			  zlog_debug("POP in_label %d", in_label);
			  nhlfe_op = POP;
			  struct in_addr next_hop;
     		  sendNHLFEUpdate(cmd, nhlfe_op, in_label, 0,
	  				next_hop);
		  }
	    }
}

int mpls_kernel_install_ilm (u_int32_t in_label, struct mpls_lsp *lsp)
{
	return mpls_fpm_ilm (ADD_LSP, 0, in_label, lsp);
}

int mpls_kernel_uninstall_ilm (u_int32_t in_label, struct mpls_lsp *lsp)
{
	 return mpls_fpm_ilm (REMOVE_LSP, 0, in_label, lsp);
}

void mpls_kernel_init (void) {}
void mpls_kernel_exit (void) {}

void sendNHLFEUpdate(int cmd, int nhlfe_op, int in_label, int out_label,
		struct in_addr nexthop_ip)
{
  nhlfe_msg_t *nhlfe_msg = malloc (sizeof (nhlfe_msg_t));
  memset (nhlfe_msg, sizeof(nhlfe_msg_t), 0);
  zlog_debug("Installing type %d for label %d", nhlfe_op, in_label);
  nhlfe_msg->ip_version = IPv4;
  nhlfe_msg->table_operation = cmd;
  nhlfe_msg->nhlfe_operation = nhlfe_op;
  nhlfe_msg->in_label  = htonl(in_label);
  nhlfe_msg->out_label = htonl(out_label);
  if (nhlfe_op == SWAP)
	  nhlfe_msg->next_hop_ip.ipv4 = nexthop_ip;
  zfpm_trigger_nhlfe_update (nhlfe_msg, sizeof (nhlfe_msg_t), "mpls sendNHLFEUpdate");
}

void sendFTN(int cmd, struct in_addr match_ip, int mask, int out_label, struct in_addr nexthop_ip)
{
	ftn_msg_t *ftn_msg = malloc (sizeof(ftn_msg_t));
	zlog_debug("Installing FTN cmd %d for FEC %s out label %d", cmd, inet_ntoa (match_ip), out_label);
	ftn_msg->out_label = htonl(out_label);
    ftn_msg->ip_version = IPv4;
    ftn_msg->next_hop_ip.ipv4 = nexthop_ip;
    ftn_msg->match_network_ip.ipv4 = match_ip;
    ftn_msg->mask = mask;
    ftn_msg->table_operation = cmd;
    zfpm_trigger_ftn_update (ftn_msg, sizeof (ftn_msg_t), "mpls sendFTN");
}

void mpls_kernel_install_ftn  (struct prefix *p, u_int32_t label, struct in_addr nexthop_ip)
{
   struct in_addr match_ip;
   memcpy (&(match_ip.s_addr), &(p->u.prefix), sizeof (struct in_addr));
   sendFTN (ADD_LSP, match_ip, p->prefixlen, label, nexthop_ip);
}


