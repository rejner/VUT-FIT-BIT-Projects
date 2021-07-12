/**
 * @file    main.cpp
 * @author  Michal Rein (xreinm00@vutbr.cz)
 * @brief   ISA Projekt - Discrod bot
 *          This source file contains implementation of ISABot class.
 *          This file is also an entry point of the program.
 * @version 0.2
 * @date    2020-10-17
 */

#include "main.h"

#define DEBUG false

using namespace std;

// Printing errors
void ISABot::error(const char *msg, int type)
{
    perror(msg);
    
    switch (type)
    {
    case ARGUMENTS_ERROR:
        exit(1);
        break;
    
    case CONNECTION_ERROR:
        close_connection();
        exit(2);
        break;
    
    case PARSING_ERROR:
        close_connection();
        exit(3);
        break;
    
    default:
        exit(-1);
        break;
    }
}

// Logging ssl errors
void log_ssl()
{
    int err;
    while ((err = ERR_get_error())) {
        char *str = ERR_error_string(err, 0);
        if (!str)
            return;
        fprintf(stderr, "%s", str);
        fprintf(stderr, "\n");
        fflush(stdout);
    }
}

// ISABot class constructor
// Parse arguments and create instance of MessageProcessor class
ISABot::ISABot(int argc, char **argv){
    parse_args(argc, argv);
    mp = MessageProcessor();
}

// ISABot class destructor
ISABot::~ISABot(){   
}

// Print status
void ISABot::print_status(){
    if (!DEBUG) return;
    cout << "TOKEN: " << bot_token << endl <<
            "VERBOSE: " << (verbose ? "true" : "false") << endl;
}

// Parse program arguments
void ISABot::parse_args(int argc, char** argv){
    
    const char* const short_opts = "hvt:";
    const option long_optps[] = {
            {"help", no_argument, nullptr, 'h'},
            {"verbose", no_argument, nullptr, 'v'}
    };

    while (true){
        const auto opt = getopt_long(argc, argv, short_opts, long_optps, nullptr);

        if (opt == -1){
            break;
        }

        switch (opt) {
            case 't':
                bot_token = string(optarg);
                break;

            case 'v':
                verbose = true;
                break;

            case 'h':
                cout << "Volání programu:" << endl <<
                "./isabot [-h|--help] [-v|--verbose] -t <bot_access_token>" << endl << endl <<
                "-h nebo --help (vypíše tuto hlášku a ukončí program)" << endl <<
                "-v nebo --verbose (začne vypisovat zprávy, na které bot reaguje na stdout ve formátu: <channel> - <username>: <message>)" << endl <<
                "-t <bot_access_token> (autentizační token pro přístup bota na Discrod)" << endl;
                exit(0);
                break;

            default:
                break;
        }
    }

    if (bot_token.empty()){
        error("Je nutné zadat autentizační token pro přístup bota na Discord!", ARGUMENTS_ERROR);
    }

}

// Init socket for connection with Discord servers
void ISABot::init_socket(){

    struct hostent * host = gethostbyname("discord.com");

    if ((host == NULL) || (host->h_addr == NULL))
        error("Chyba při získávání informací DNS!", CONNECTION_ERROR);

    // Prepare server address
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(port);
    memcpy(&serv_addr.sin_addr, host->h_addr, host->h_length);

    // Open socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) 
        error("Chyba při otevírání nového socketu!", CONNECTION_ERROR);
}

// Init SSL libraries and connection
void ISABot::init_ssl(){

    SSL_library_init();
    SSLeay_add_ssl_algorithms();
    SSL_load_error_strings();
    const SSL_METHOD *meth = TLSv1_2_method();
    ctx = SSL_CTX_new (meth);

    ssl = SSL_new (ctx);
    if (!ssl) {
        log_ssl();
        error("Chyba při vytváření nového SSL spojení! (SSL_new)", CONNECTION_ERROR);
    }

    SSL_set_fd(ssl, sockfd);
    int err = SSL_connect(ssl);
    if (err <= 0) {
        log_ssl();
        fflush(stdout);
        error("Chyba Chyba při vytváření nového SSL spojení! (SSL_connect)", CONNECTION_ERROR);
    }
    
}

// Connect to Discord servers
// Initialize socket and establish SSL connection
int ISABot::connect_to_server(){

    // init socket for communication
    init_socket();

    // try to connect
    if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0) {
        error("Chyba při vytváření spojení spojení! (connect)", CONNECTION_ERROR);
    }
        
    // init ssl connection
    init_ssl();

    return 0;
}

// Close connection with Discord servers
int ISABot::close_connection(){


    if (ssl) SSL_shutdown (ssl);  /* send SSL/TLS close_notify */
    if (sockfd) close(sockfd);
    if (ssl) SSL_free (ssl);
    if (ctx) SSL_CTX_free (ctx);
    return 0;
}

// GET guild to which bot has access to
void ISABot::get_guild_id(){

    connect_to_server();
    send_request(GET_GUILDS, "");
    string response = recv_response(false);

    bool header = true;
    bool parsing_class = false;
    bool parsing_key = false, parsing_value = false;
    bool nested = false;
    bool finished_message = false;
    bool inside_string = false;

    string content = "";
    
    string key = "";
    string value = "";

    for (auto it = response.cbegin(); it != response.cend(); ++it){
        
        if (header){
            if (*it == '\r'){
                auto next = it + 2;
                if (*next == '\r'){
                    header = false;
                }
            }
        // copy content into new string
        } else {
            string tmp(1, *it);
            content.append(tmp);
        }
    }

    // remove all endline characters
    std::string::iterator end_pos1 = remove(content.begin(), content.end(), '\r');
    content.erase(end_pos1, content.end());

    for (auto it = content.cbegin(); it != content.cend(); ++it){

        switch (*it)
            {
            case '\n':
                continue;
                break;
            case '{':
                if (!parsing_class) {
                    parsing_class = true;
                } else {
                    nested = true;
                }
                break;
            
            case '}':
                if (nested) {
                    nested = false;
                } else {
                    parsing_class = false;
                    finished_message = false;
                }
                break;
            
            case '"':
                if (key.empty()) {

                    auto next1 = it + 1;
                    auto next2 = it + 2;
                    auto next3 = it + 3;

                    if (*next1 == 'i' && *next2 == 'd' && *next3 == '"' && !nested){
                        parsing_key = true;
                    }

                } else if (parsing_key){
                    auto next1 = it + 1;
                    if (*next1 == ':') it++;
                    parsing_key = false;
                    parsing_value = true;
                } else if (parsing_value){
                    if (!inside_string) {
                        inside_string = true;
                    } else {
                        inside_string = false;
                    }
                }
                break;
            
            case ',':
                if (parsing_value && !inside_string) parsing_value = false;
                if (!key.empty() && !value.empty() && !parsing_key && !parsing_value){
                    
                    if (!finished_message){
                        if (key == "id") guilds.push_front(value);
                    }
                                     
                    key.clear();
                    value.clear();
                }
                break;

            default:

                if (parsing_key) {
                    string tmp(1, *it);
                    key.append(tmp);
                }

                if (parsing_value) {
                    if (!inside_string && *it == ' ') continue; // skip leading value whitespaces
                    string tmp(1, *it);
                    value.append(tmp);
                }
                break;
            }
        }

    close_connection();
}

// GET channels from guild and look for channel with name "isa-bot"
void ISABot::get_channels(){

    connect_to_server();
    send_request(GET_CHANNELS, "");
    string response = recv_response(false);

    bool header = true;
    bool parsing_class = false;
    bool parsing_key = false, parsing_value = false;
    bool nested = false;
    bool finished_message = false;
    bool inside_string = false;

    string content = "";
    
    string key = "";
    string value = "";

    string id = "";
    string name = "";

    for (auto it = response.cbegin(); it != response.cend(); ++it){
        
        if (header){
            if (*it == '\r'){
                auto next = it + 2;
                if (*next == '\r'){
                    header = false;
                }
            }
        // copy content into new string
        } else {
            string tmp(1, *it);
            content.append(tmp);
        }
    }

    // remove all endline characters
    std::string::iterator end_pos1 = remove(content.begin(), content.end(), '\r');
    content.erase(end_pos1, content.end());

    for (auto it = content.cbegin(); it != content.cend(); ++it){

        switch (*it)
            {
            case '\n':
                continue;
                break;
            case '{':
                if (!parsing_class) {
                    parsing_class = true;
                } else {
                    nested = true;
                }
                break;
            
            case '}':
                if (nested) {
                    nested = false;
                } else {
                    parsing_class = false;
                    if (name == "isa-bot"){
                        channel_id = id;
                    }
                    finished_message = false;
                }
                break;
            
            case '"':
                if (key.empty()) {

                    auto next1 = it + 1;
                    auto next2 = it + 2;
                    auto next3 = it + 3;

                    if (*next1 == 'i' && *next2 == 'd' && *next3 == '"' && !nested){
                        parsing_key = true;
                    } else if (*next1 == 'n' && *next2 == 'a' && *next3 == 'm' && !nested){
                        parsing_key = true;
                    }

                } else if (parsing_key){
                    auto next1 = it + 1;
                    if (*next1 == ':') it++;
                    parsing_key = false;
                    parsing_value = true;
                } else if (parsing_value){
                    if (!inside_string) {
                        inside_string = true;
                    } else {
                        inside_string = false;
                    }
                }
                break;
            
            case ',':
                if (parsing_value && !inside_string) parsing_value = false;
                if (!key.empty() && !value.empty() && !parsing_key && !parsing_value){
                    
                    if (!finished_message){
                        if (key == "id") id = value;
                        if (key == "name") name = value;
                    }
                                     
                    key.clear();
                    value.clear();
                }
                break;

            default:

                if (parsing_key) {
                    string tmp(1, *it);
                    key.append(tmp);
                }

                if (parsing_value) {
                    if (!inside_string && *it == ' ') continue; // skip leading value whitespaces
                    string tmp(1, *it);
                    value.append(tmp);
                }
                break;
            }
        }
    
    close_connection();

}

// GET id of the last message on set channel 
void ISABot::get_last_message_id(){

    connect_to_server();
    send_request(GET_LAST_MESSAGE, "");
    string response = recv_response(false);

    bool header = true;
    bool parsing_class = false;
    bool parsing_key = false, parsing_value = false;
    bool nested = false;
    bool finished_message = false;
    bool inside_string = false;

    string content = "";
    
    string key = "";
    string value = "";

    for (auto it = response.cbegin(); it != response.cend(); ++it){
        
        if (header){
            if (*it == '\r'){
                auto next = it + 2;
                if (*next == '\r'){
                    header = false;
                }
            }
        // copy content into new string
        } else {
            string tmp(1, *it);
            content.append(tmp);
        }
    }

    // remove all endline characters
    std::string::iterator end_pos1 = remove(content.begin(), content.end(), '\r');
    content.erase(end_pos1, content.end());

    for (auto it = content.cbegin(); it != content.cend(); ++it){

        switch (*it)
            {
            case '\n':
                continue;
                break;
            case '{':
                if (!parsing_class) {
                    parsing_class = true;
                } else {
                    nested = true;
                }
                break;
            
            case '}':
                if (nested) {
                    nested = false;
                } else {
                    parsing_class = false;
                    finished_message = false;
                }
                break;
            
            case '"':
                if (key.empty()) {

                    auto next1 = it + 1;
                    auto next2 = it + 2;
                    auto next3 = it + 3;

                    if (*next1 == 'l' && *next2 == 'a' && *next3 == 's' && !nested){
                        parsing_key = true;
                    }

                } else if (parsing_key){
                    auto next1 = it + 1;
                    if (*next1 == ':') it++;
                    parsing_key = false;
                    parsing_value = true;
                } else if (parsing_value){
                    if (!inside_string) {
                        inside_string = true;
                    } else {
                        inside_string = false;
                    }
                }
                break;
            
            case ',':
                if (parsing_value && !inside_string) parsing_value = false;
                if (!key.empty() && !value.empty() && !parsing_key && !parsing_value){
                    
                    if (!finished_message){
                        if (key == "last_message_id") last_message = value;
                    }
                                     
                    key.clear();
                    value.clear();
                }
                break;

            default:

                if (parsing_key) {
                    string tmp(1, *it);
                    key.append(tmp);
                }

                if (parsing_value) {
                    if (!inside_string && *it == ' ') continue; // skip leading value whitespaces
                    string tmp(1, *it);
                    value.append(tmp);
                }
                break;
            }
        }

    close_connection();
}

// Send request to Discord servers 
int ISABot::send_request(int variant, string message){

    stringstream stream, after;
    after << "";
    if (!last_message.empty()) after << "?after=" << last_message;

    switch (variant)
    {
    case 0:
        stream << "GET /api/v6/channels/" << channel_id << "/messages" << after.str() << " HTTP/1.1\r\n"
        << "Host: discord.com\r\n"
        << "Authorization: Bot " << bot_token << "\r\n"
        << "Accept: application/json\r\n"
        << "Connection: close\r\n"
        << "\r\n\r\n";
        break;

    case 1:
        stream << "POST /api/v6/channels/" << channel_id << "/messages HTTP/1.1\r\n"
        << "Content-Type: application/json\r\n"
        << "Host: discord.com\r\n"
        << "Authorization: Bot " << bot_token << "\r\n"
        << "Accept: application/json\r\n"
        << "Content-length: " << message.size() << "\r\n"
        << "Connection: close\r\n\r\n"
        << message;
        break;
    
    case 2:
        stream << "GET /api/v6/users/@me/guilds HTTP/1.1\r\n"
        << "Host: discord.com\r\n"
        << "Authorization: Bot " << bot_token << "\r\n"
        << "Accept: application/json\r\n"
        << "Connection: close\r\n"
        << "\r\n\r\n";
        break;
    
    case 3:
        stream << "GET /api/v6/guilds/" << guilds.front() << "/channels HTTP/1.1\r\n"
        << "Host: discord.com\r\n"
        << "Authorization: Bot " << bot_token << "\r\n"
        << "Accept: application/json\r\n"
        << "Connection: close\r\n"
        << "\r\n\r\n";
        break;
    
    case 4:
        stream << "GET /api/v6/channels/" << channel_id << " HTTP/1.1\r\n"
        << "Host: discord.com\r\n"
        << "Authorization: Bot " << bot_token << "\r\n"
        << "Accept: application/json\r\n"
        << "Connection: close\r\n"
        << "\r\n\r\n";
        break;
        
    
    default:
        break;
    }

    string request = stream.str();

    int len = SSL_write(ssl, request.c_str(), strlen(request.c_str()));
    if (len < 0) {
        int err = SSL_get_error(ssl, len);
        switch (err) {
        case SSL_ERROR_WANT_WRITE:
            error("SSL_ERROR_WANT_WRITE", CONNECTION_ERROR);
            break;
        case SSL_ERROR_WANT_READ:
            error("SSL_ERROR_WANT_READ", CONNECTION_ERROR);
            break;
        case SSL_ERROR_ZERO_RETURN:
            error("SSL_ERROR_ZERO_RETURN", CONNECTION_ERROR);
            break;
        case SSL_ERROR_SYSCALL:
            error("SSL_ERROR_SYSCALL", CONNECTION_ERROR);
            break;
        case SSL_ERROR_SSL:
            error("SSL_ERROR_SSL", CONNECTION_ERROR);
            break;
        default:
            error("Něco se pokazilo při odesílání dat pomocí SSL (SSL_write)", CONNECTION_ERROR);
            break;
        }
    }
    
    return 0;
}

// Recieve response from Discord servers
string ISABot::recv_response(bool parse){

    int len = 100;
    char buf[1000000];
    string content;
    do {
        len = SSL_read(ssl, buf, 100);
        buf[len] = 0;
        content.append(string(buf));
        // printf("%s\n",buf);
        //fprintf(fp, "%s",buf);
    } while (len > 0);
    if (len < 0) {
        int err = SSL_get_error(ssl, len);
    if (err == SSL_ERROR_WANT_READ)
            error("Chyba při přijetí odpovědi od serveru! (SSL_read)", CONNECTION_ERROR);
        if (err == SSL_ERROR_WANT_WRITE)
            error("Chyba při přijetí odpovědi od serveru! (SSL_read)", CONNECTION_ERROR);
        if (err == SSL_ERROR_ZERO_RETURN || err == SSL_ERROR_SYSCALL || err == SSL_ERROR_SSL)
            error("Chyba při přijetí odpovědi od serveru! (SSL_read)", CONNECTION_ERROR);
    }


    string status = content.substr(0, 30);
    if (status.find("200 OK") == std::string::npos){
        if (status.find("401 Unauthorized") != std::string::npos){
            error("Server vrátil 401 Unauthorized, zkontrolujte zadaný token nebo pravomoce bota!", CONNECTION_ERROR);
        } else {
            error(status.c_str(), CONNECTION_ERROR);
        }
    }

    if (parse){
        mp.parse_messages(content);
    }

    return content;
}

// Execution loop
int ISABot::run(){

    // fetch guilds (servers) to which bot has access to
    get_guild_id();
    sleep(1);

    // fetch channels on server and find channel with name "isa-bot", set channel_id variable
    get_channels();
    sleep(1);

    // fetch message id of the last message on the channel
    get_last_message_id();
    sleep(1);

    // Main program loop
    while(1){

        // Retrieve messages from channel
        connect_to_server();
        send_request(GET_MESSAGES, "");
        recv_response(true);
        close_connection();
        sleep(1);

        // Message processor has parsed and stored messages into list of objects
        // Interate through messages and send each one as an echo to the server
        for (auto message : mp.messages){

            // Check whether extracted message belongs to bot (such messages won't be echoed), or
            // list of messages is empty (no new messages were extracted in the last itteration of main loop)
            if (!message.bot && !mp.messages.empty()){
                
                std::stringstream echo;
                echo << "{\"content\":\"echo: " << message.username << " - " <<  message.content << "\"}";

                connect_to_server();
                send_request(SEND_MESSAGE, echo.str());
                recv_response(false);
                close_connection();

                if (verbose) cout << "isa-bot - " << message.username << " : " << message.content << endl;
                
                // Get last message id, so in the next itteration of the main loop can bot request only messages
                // that appear after last message that has been parsed in the current itteration
                last_message = message.id;
                sleep(1); // precaution for not overloading servers and getting ban
            }
        }

        mp.clear_messages();

    }


}

/*
   ___________ MAIN _____________
*/
int main(int argc, char **argv){
    
    ISABot bot = ISABot(argc, argv);
    bot.run();

    return 0;

}
