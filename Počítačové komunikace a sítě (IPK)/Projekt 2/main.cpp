/*
* IPK - Project 2, Packet sniffer
* Author: Michal Rein (xreinm00)
* E-mail: xreinm00@stud.fit.vutbr.cz
*/

#include "main.h"

using namespace std;

int number_of_packets = 1;
int received_args = 0;
string interface = "";
string port;
bool protocol_tcp, protocol_udp;
string filter_expr = "";


void parse_args(int argc, char** argv){

    const char* const short_opts = "hi:p:tun:";
    const option long_optps[] = {
            {"help", no_argument, nullptr, 'h'},
            {"tcp", no_argument, nullptr, 't'},
            {"udp", no_argument, nullptr, 'u'}
    };

    while (true){
        const auto opt = getopt_long(argc, argv, short_opts, long_optps, nullptr);

        if (opt == -1){
            break;
        }

        switch (opt) {
            case 'i':
                if (optarg[0] == '-'){
                    interface = "";
                } else {
                    interface = string(optarg);
                }
                received_args += 2;
                break;

            case 'p':
                port = string(optarg);
                received_args += 2;
                break;

            case 't':
                protocol_tcp = true;
                received_args += 1;
                break;

            case 'u':
                protocol_udp = true;
                received_args += 1;
                break;

            case 'n':
                number_of_packets = strtol(optarg, NULL, 10);
                received_args += 2;
                break;

            case 'h':
                cout << "Volání programu:" << endl <<
                "./ipk-sniffer -i rozhraní [-p ­­port] [--tcp|-t] [--udp|-u] [-n num]" << endl << endl <<
                "-h nebo --help (vypíše tuto hlášku a ukončí program)" << endl <<
                "-i <jméno_rozhraní> (rozhraní, na kterém se bude poslouchat. Nebude-li tento parametr uveden, vypíše se seznam aktivních rozhraní)" << endl <<
                "-p <číslo_portu> (bude filtrování paketů na daném rozhraní podle portu; nebude-li tento parametr uveden, uvažují se všechny porty)" << endl <<
                "-t nebo --tcp (bude zobrazovat pouze tcp pakety)" << endl <<
                "-u nebo --udp (bude zobrazovat pouze udp pakety)" << endl <<
                "(Pokud nebude -tcp ani -udp specifikováno, uvažují se TCP a UDP pakety zároveň)" << endl <<
                "-n <počet_paketů> (určuje počet paketů, které se mají zobrazit; pokud není uvedeno, uvažujte zobrazení pouze 1 paket)" << endl;
                exit(0);
                break;

            default:
                cerr << "An unknown argument detected, skipping..." << endl;
                break;
        }
    }
}

void create_filter_expr(){

    if (protocol_tcp && !protocol_udp){
        filter_expr.append("tcp ");
    } else if (protocol_udp && !protocol_tcp){
        filter_expr.append("udp ");
    }

    if (!port.empty()){
        filter_expr.append("port " + port + " ");
    }
}

string check_protocol(u_char protocol){

    switch(protocol){
        case IPPROTO_TCP:
            return "TCP";
        case IPPROTO_UDP:
            return "UDP";
        default:
            return "unknown";
    }
}

void print_payload(const u_char *payload, int payload_len){

    int remaining_bytes = payload_len;
    int current_line_len;
    int offset = 0;

    const u_char *payload_hex = payload, *payload_ascii = payload;

    bool end = false;
    while(not end){

        // print counter
        cout << "0x" << setw(5) << setfill('0') << hex << offset << "  ";

        // print 16 (by default) bytes of payload in hex format
        current_line_len = PRINT_LINE_LENGTH % remaining_bytes;
        for (int i = 0; i < current_line_len; i++){
            cout << setw(2) << hex << int(*payload_hex) << " ";
            payload_hex++;
        }

        cout << "  ";

        //print 16 (by default) bytes of payload in ascii format (if possible)
        for (int j = 0; j < current_line_len; j++){
            if (isprint(*payload_ascii)){
                cout << *payload_ascii;
            } else {
                cout << ".";
            }
            payload_ascii++;
        }

        // print end line
        cout << endl;

        // calculate remaining bytes and offset
        remaining_bytes = remaining_bytes - current_line_len;
        offset = offset + PRINT_LINE_LENGTH;

        // if remaining bytes are less than standard line length (16 bytes),
        // print the rest of the payload separately
        if (remaining_bytes <= PRINT_LINE_LENGTH){

            // print counter
            cout << "0x" << setw(5) << setfill('0') << hex << offset << "  ";

            // print rest of the payload in hex
            int k = 0;
            for (; k < remaining_bytes; k++){
                cout << setw(2) << hex << int(*payload_hex) << " ";
                payload_hex++;
            }

            // if rest of the payload had lesser length than standard line (16 bytes)
            if (k < PRINT_LINE_LENGTH){

                // print rest of the missing bytes as white space to fill the missing bytes
                for (; k < PRINT_LINE_LENGTH; k++){
                    cout << "   ";
                }
            }

            // hex | char separator
            cout << "  ";

            // print rest of the printable chars
            for (int l = 0; l < remaining_bytes; l++){
                if (isprint(*payload_ascii)){
                    cout << *payload_ascii;
                } else {
                    cout << ".";
                }
                payload_ascii++;
            }
            cout << endl;

            // end cycle
            end = true;
        }
    }
}

void print_TCP_info(string time, header_ip ip, header_tcp tcp){

    string ip_addr = inet_ntoa(ip.ip_src);
    struct hostent *src_name, *dst_name;

    cout << time << " ";

    if ((src_name = gethostbyaddr((const void *)&ip.ip_src, sizeof(ip.ip_src), AF_INET)) == NULL){
        cout << dec << inet_ntoa(ip.ip_src);
    } else {
        cout << src_name->h_name;
    }

    //cout << inet_ntoa(ip.ip_src);

    cout << " : " << dec << ntohs(tcp.th_sport) << " > ";

    if ((dst_name = gethostbyaddr((const void *)&ip.ip_dst, sizeof(ip.ip_dst), AF_INET)) == NULL){
        cout << inet_ntoa(ip.ip_dst);
    } else {
        cout << dst_name->h_name;
    }

    //cout << inet_ntoa(ip.ip_dst);

    cout << " : " << dec << ntohs(tcp.th_dport) << " (TCP)" <<endl;
}

void print_UDP_info(string time, header_ip ip, header_udp udp){

    string ip_addr = inet_ntoa(ip.ip_src);
    struct hostent *src_name, *dst_name;

    cout << time << " ";

    if ((src_name = gethostbyaddr((const void *)&ip.ip_src, sizeof(ip.ip_src), AF_INET)) == NULL){
        cout << dec << inet_ntoa(ip.ip_src);
    } else {
        cout << src_name->h_name;
    }

    cout << " : " << dec << ntohs(udp.uh_sport) << " > ";

    if ((dst_name = gethostbyaddr((const void *)&ip.ip_dst, sizeof(ip.ip_dst), AF_INET)) == NULL){
        cout << inet_ntoa(ip.ip_dst);
    } else {
        cout << dst_name->h_name;
    }

    cout << " : " << dec << ntohs(udp.uh_dport) << " (UDP)" << endl;
}

void parse_packet(u_char *args, const struct pcap_pkthdr *header, const u_char *packet){

    time_t now = time(0);
    char time[10] = "";
    strftime(time, 10, "%H:%M:%S", localtime(&now));

    static int packet_num = 1; // packet counter

    const struct header_ip *ip; // ip header
    const struct header_tcp *tcp; // tcp header
    const struct header_udp *udp; // udp header
    const u_char *payload; // packet payload

    int ip_header_size, tcp_header_size, payload_size;

    ip = (struct header_ip*)(packet + ETHERNET_HEADER_SIZE); // ethernet header should be always 14

    /*                 ip_vhl
     * | version (4 bit) | length (4 bit) * 4 |
     *       0 1 0 0           X X X X
     * - we need only length part, this can be done by application of and(&) operator
     *   with value of 15 (00001111), version gets annulled
     * */
    ip_header_size = (ip->ip_vhl & 0x0f) * 4;


    /* TCP protocol processing */
    if (check_protocol(ip->ip_p) == "TCP"){
        tcp = (struct header_tcp*)(packet + ETHERNET_HEADER_SIZE + ip_header_size);
        tcp_header_size = ((tcp->th_offset & 0xf0) >> 4) * 4; // reserved and FLAGS must be annulled to get offset

        print_TCP_info(time, *ip, *tcp);

        /* Get payload */
        payload = (u_char *)(packet + ETHERNET_HEADER_SIZE + ip_header_size + tcp_header_size);
        payload_size = ntohs(ip->ip_len) - (ip_header_size + tcp_header_size);  // ip_len must be converted from network byte order

        if (payload_size > 0){
            print_payload(payload, payload_size);
        }

    }

    /* UDP protocol processing */
    if (check_protocol(ip->ip_p) == "UDP"){
        udp = (struct header_udp*)(packet + ETHERNET_HEADER_SIZE + ip_header_size);

        print_UDP_info(time, *ip, *udp);

        /* Get payload */
        payload = (u_char *)(packet + ETHERNET_HEADER_SIZE + ip_header_size + UDP_HEADER_SIZE);
        payload_size = ntohs(ip->ip_len) - (ip_header_size + UDP_HEADER_SIZE);  // ip_len must be converted from network byte order

        if (payload_size > 0){
            print_payload(payload, payload_size);
        }
    }

    cout << endl;
}

/* ----- MAIN ----- */
int main(int argc, char **argv) {

    char error_msg[PCAP_ERRBUF_SIZE];
    pcap_t *handle;
    bpf_program fp;
    bpf_u_int32 mask;
    bpf_u_int32 net;
    //struct pcap_pkthdr header;
    //const u_char *packet;

    /* Parse arguments */
    parse_args(argc, argv);

    if (interface == ""){
        cout << "No device specified, you can use of the following:" << endl;
        cout << pcap_lookupdev(error_msg) << endl;
        exit(0);
    }

    if (received_args != argc - 1){
        cerr << "Illegal argument(s) given..." << endl;
        exit(1);
    }

    if (pcap_lookupnet(interface.c_str(), &net, &mask, error_msg) == -1){
        cerr << "An error has occurred during device netmask scan: " << error_msg << endl;
        net = 0;
        mask = 0;
    }

    /* Print info */
    cout << "Device: " << interface << endl;
    cout << "Number of packets: " << number_of_packets << endl;
    cout << "Filter expression: " << filter_expr << endl << endl << endl;

    /* Open capture device */
    handle = pcap_create(interface.c_str(), error_msg);
    if (pcap_activate(handle) != 0){
        cerr << "Device " << interface.c_str() << " couldn't be activated.." << pcap_geterr(handle) <<endl;
        pcap_close(handle);
        exit(EXIT_FAILURE);
    }


    if (handle == NULL){
        cerr << "Device " << interface << " couldn't be opened..." << endl;
        exit(EXIT_FAILURE);
    }

    /* Create filter expression */
    create_filter_expr();

    /* Compile filter */
    if (pcap_compile(handle, &fp, filter_expr.c_str(), 0, net) == -1){
        cerr << "Failed to compile filter: " << filter_expr << ": " << pcap_geterr(handle) << endl;
        exit(EXIT_FAILURE);
    }

    /* Apply filter */
    if (pcap_setfilter(handle, &fp) == -1){
        cerr << "Failed to apply filter: " << filter_expr << ": " << pcap_geterr(handle) << endl;
        exit(EXIT_FAILURE);
    }

    /* Start capturing loop */
    pcap_loop(handle, number_of_packets, parse_packet, nullptr);

    pcap_freecode(&fp);
    pcap_close(handle);

    cout << "Sniffing complete..." << endl;

    return 0;
}
