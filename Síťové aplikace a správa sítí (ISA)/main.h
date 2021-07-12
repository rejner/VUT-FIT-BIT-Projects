/**
 * @file    main.h
 * @author  Michal Rein (xreinm00@vutbr.cz)
 * @brief   ISA Projekt - Discrod bot 
 *          This header file contains declaration of ISABot class.
 * @version 0.2
 * @date    2020-10-17
 */

#include <iostream>
#include <unistd.h>
#include <string.h>
#include <netdb.h>
#include <arpa/inet.h>
#include <getopt.h>
#include <iomanip>
#include <sys/socket.h>
#include <netinet/in.h>
#include <openssl/ssl.h>
#include <openssl/err.h>
#include "message.h"

#define MAX_DATA_LEN 4096

enum {
    GET_MESSAGES,
    SEND_MESSAGE,
    GET_GUILDS,
    GET_CHANNELS,
    GET_LAST_MESSAGE
};

enum {
    ARGUMENTS_ERROR,
    PARSING_ERROR,
    CONNECTION_ERROR

};

using namespace std;

/**
 * @brief ISABot class
 * This class represents Discord bot and encapsulates all of its functionalities
 */
class ISABot{

    public:

        /**
         * @brief Construct a new ISABot object
         * 
         * @param argc number of program arguments
         * @param argv program arguments
         */
        ISABot(int argc, char **argv);
        ~ISABot();

        /**
         * @brief Run bot, starts execution loop
         * @return int return code
         */
        int run();

        /**
         * @brief Prints bot's status
         */
        void print_status();

    private:

        string bot_token = "";                    // bot access token
        int port = 443;                           // standard HTTPS protocol port
        bool verbose = false;                     // flag verbose enables bot's output to stdout

        int sockfd;                               // file descriptor of the opened socket 
        SSL *ssl;                                 // ssl structure of an SSL session
        SSL_CTX *ctx;                             // context
        char buffer[MAX_DATA_LEN];                // buffer for storing recieved data
        struct sockaddr_in serv_addr;             // server address

        MessageProcessor mp;                      // instance of MessageProcessor object

        list<string> guilds = list<string>();     // list of all guilds to which bot has access to
        string channel_id = "";                   // id of channel with label "isa-bot"
        string last_message = "";                 // last message id that has been parsed already

        /**
         * @brief Initialize socket and establish SSL connection with
         *        Discord servers.
         * 
         * @return int return code
         */
        int connect_to_server();

        /**
         * @brief Send https request to Discord servers.
         *        There are 2 variants of requests:
         *        GET  - Retrieve messages from specified channel
         *        POST - Send a message to specified channel
         * 
         * @param variant specifies whether GET or POST method should be used (GET_MESSAGES or SEND_MESSAGE)
         * @param message string representing a body of request (only matter for POST method)  
         * @return int return code
         */
        int send_request(int variant, string message);

        /**
         * @brief Recieve response from Discord servers.
         *        This method should be called right after send_request method.
         * 
         * @param parse if set true, body of response will be parsed with an instance of MessageParser
         * @return int return code
         */
        string recv_response(bool parse);

        /**
         * @brief Close connection with Discord servers.
         *        Socket and SSL will be closed.
         * 
         * @return int return code
         */
        int close_connection();

        /**
         * @brief Parse arguments.
         * 
         * @param argc number of program arguments
         * @param argv program arguments
         */
        void parse_args(int argc, char **argv);

        /**
         * @brief Init socket for connection with Discord servers.
         */
        void init_socket();

        /**
         * @brief  Init SSL libraries and connection.
         */
        void init_ssl();

        /**
         * @brief Get the guild id object from Discrod server.
         * 
         */
        void get_guild_id();

        /**
         * @brief Get the channels object from Discrod server and look for isa-bot room.
         * 
         */
        void get_channels();

        /**
         * @brief Get the last message id object from Discord server.
         * 
         */
        void get_last_message_id();

        /**
         * @brief Method for error printing and handling
         * 
         * @param msg Message to be printed
         */
        void error(const char *msg, int type);


};
