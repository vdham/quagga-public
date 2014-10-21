/*
 * mpls_msg.h
 *
 *  Created on: Sep 26, 2013
 *      Author: Vikram
 */

#ifndef MPLS_MSG_H_
#define MPLS_MSG_H_

#include "queue.h"

#include <netinet/in.h>


typedef enum nhlfe_operation_enum {
  PUSH = 0,
  POP = 1,
  SWAP = 2
} nhlfe_operation_enum_t;

typedef enum msg_type_enum {
  NHLFE = 0,
  FTN = 1
} msg_type_enum_t;


typedef enum table_operation_enum {
  ADD_LSP = 0,
  REMOVE_LSP = 1,
} table_operation_enum_t;


typedef enum ip_version_enum {
  IPv4 = 4,
  IPv6 = 6,
} ip_version_enum_t;


typedef union ip4_or_6_address{
    struct in_addr ipv4;
    struct in6_addr ipv6;
  } ip4_or_6_address_t;


/*
 * Next Hop Label Forwarding Entry (NHLFE)
 * Used from FPM protocol version 1
 */
typedef struct nhlfe_msg_t_
{

  /*
   * The IP version (4 or 6) that the next hop field contains
   * values defined in ip_version_enum_t
   */

  uint8_t ip_version;

  /*
   * The next hop IP address. This
   * is used to look up the interface number the packet is
   * going out
   */

  ip4_or_6_address_t next_hop_ip;

  /*
   * What operation we are doing on the flow table (add, delete, in the future
   * perhaps modify), values defined in table_operation_enum_t
   */

  uint8_t table_operation;

  /*
   * Specifies if this is a push, pop or swap label operation.
   * Values defined in nhlfe_operation_enum_t
   */
  uint8_t nhlfe_operation;

  /*
   * In network byte order
   */

  uint32_t in_label;

  /*
   * In network byte order
   */

  uint32_t out_label;

} __attribute__((packed)) nhlfe_msg_t;




typedef struct ftn_msg_t_
{
  /*
   * The IP version (4 or 6) that we are matching on, and IP version of next hop
   * Values defined in ip_version_enum_t
   */

  uint8_t ip_version;

  uint8_t mask;

  /*
   * What operation we are doing on the flow table (add, delete, in the future
   * perhaps modify), values defined in table_operation_enum_t
   */

  uint8_t table_operation;

  ip4_or_6_address_t match_network_ip;

  /*
   * In network byte order
   */

  uint32_t out_label;

  /*
   * The next hop IP address. This
   * is used to look up the interface number the packet is
   * going out
   */

  ip4_or_6_address_t next_hop_ip;


} __attribute__((packed)) ftn_msg_t;

typedef struct mpls_msg_t_
{
	void *data;

	int len;

	int type;

	/*
	   * Flags, see below.
	   */
	  u_int32_t flags;

	  /*
	   * Linkage to put dest on the FPM processing queue.
	   */
	  TAILQ_ENTRY(mpls_msg_t_) fpm_q_entries;
} mpls_msg_t;



#endif /* MPLS_MSG_H_ */
