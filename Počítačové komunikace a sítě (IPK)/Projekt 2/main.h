/*
* IPK - Project 2, Packet sniffer
* Author: Michal Rein (xreinm00)
* E-mail: xreinm00@stud.fit.vutbr.cz
*/

#include <iostream>
#include <pcap/pcap.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <iomanip>

#define ETHERNET_HEADER_SIZE 14
#define UDP_HEADER_SIZE 8
#define PRINT_LINE_LENGTH 16

/** IP header structure */
struct header_ip {
    u_char  ip_vhl;                 /* version (4 bits) | header length (4 bits) */
    u_char  ip_tos;                 /* type of service (1 byte) */
    u_short ip_len;                 /* total length (2 bytes) */
    u_short ip_id;                  /* identification (2 bytes) */
    u_short ip_off;                 /* fragment offset field (2 bytes) */
    u_char  ip_ttl;                 /* time to live (1 byte) */
    u_char  ip_p;                   /* protocol (1 byte) */
    u_short ip_sum;                 /* checksum (2 bytes) */
    struct  in_addr ip_src,ip_dst;  /* source and dest address (4 bytes each) */
};

/** TCP header structure */
struct header_tcp {
    u_short th_sport;               /* source port (2 bytes) */
    u_short th_dport;               /* destination port (2 bytes) */
    u_int th_seq;                   /* sequence number (4 bytes) */
    u_int th_ack;                   /* acknowledgement number (4 bytes) */
    u_char  th_offset;              /* data offset, rsvd (1 byte) */
    u_char  th_flags;               /* FLAGS (1 byte) */
    u_short th_win;                 /* window (2 bytes) */
    u_short th_sum;                 /* checksum (2 bytes) */
    u_short th_urp;                 /* urgent pointer (2 bytes) */
};

/** UDP header structure */
struct header_udp {
    u_short uh_sport;               /* source port (2 bytes) */
    u_short uh_dport;               /* destination port (2 bytes) */
    u_short uh_len;                 /* length of packet (2 bytes) */
    u_short uh_sum;                 /* checksum (2 bytes) */
};

/**
 * Method used to parse arguments using getopt_long() from <getopt.h>.
 * @param argc Number of arguments given.
 * @param argv  Array of arguments.
 */
void parse_args(int argc, char** argv);

/**
 * Filter expression creator.
 * Check protocol_tcp, protocol_udp and port variables to create filter expression
 * for pcap_setfilter() method.
 */
void create_filter_expr();

/**
 * Protocol checker.
 * Examine protocol from IP header structure and identify protocol.
 * @param protocol Protocol to examine.
 * @return String determining incoming protocol (TCP/UDP/unknown).
 */
std::string check_protocol(u_char ip);

/**
 * Payload printer.
 * Take pointer to certain payload data and print content on STDOUT.
 * Printing format:
 * +----------------------------------------------------------------------------+
 * | Counter |           16 bytes in hex format               | 16 bytes ASCII  |
 * +----------------------------------------------------------------------------+
 * | 0x0000 | 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 00 | ................|
 * +----------------------------------------------------------------------------+
 * @param payload Pointer to start of payload.
 * @param payload_len Calculated length of payload.
 */
void print_payload(const u_char *payload, int payload_len);

/**
 * TCP packet information printer.
 * After determining protocol, this method is called to print information about packet.
 * Printing format:
 * +-------------------------------------------------------------------------------------+
 * |   Time   |    Source address + port   |  Destination address + port  |   Protocol   |
 * +-------------------------------------------------------------------------------------+
 * | 00:00:00 | <src_address> : <src_port> > <dest_address> : <dest_port> |    (TCP)     |
 * +-------------------------------------------------------------------------------------+
 * @param time Formatted time.
 * @param ip IP header for extraction of addresses.
 * @param tcp TCP header for extraction of ports.
 */
void print_TCP_info(std::string time, header_ip ip, header_tcp tcp);

/**
 * UDP packet information printer.
 * After determining protocol, this method is called to print information about packet.
 * Printing format:
 * +-------------------------------------------------------------------------------------+
 * |   Time   |    Source address + port   |  Destination address + port  |   Protocol   |
 * +-------------------------------------------------------------------------------------+
 * | 00:00:00 | <src_address> : <src_port> > <dest_address> : <dest_port> |    (UDP)     |
 * +-------------------------------------------------------------------------------------+
 * @param time Formatted time.
 * @param ip IP header for extraction of addresses.
 * @param tcp UDP header for extraction of ports.
 */
void print_UDP_info(std::string time, header_ip ip, header_udp udp);

/**
 * Packet parser.
 * This method is passed as reference to pcap_loop() method and used as callback for
 * processing packets.
 * @param args User arguments that were passed by pcap_loop() method.
 * @param header Pointer pointing to the packet time stamp and lengths.
 * @param packet  Pointer to the start of captured packet.
 */
void parse_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet);
