/**
 * @file    message.h
 * @author  Michal Rein (xreinm00@vutbr.cz)
 * @brief   ISA Projekt - Discrod bot
 *          This header file contains declaration of MessageProcessor class and Message structure.
 * @version 0.1
 * @date    2020-10-11
 */

#include <iostream>
#include <list>
#include <string.h>
#include <algorithm>

using namespace std;

/**
 * @brief Message data structure.
 */
struct Message {
    string id;          // id (snowflake) of message
    string username;    // author's username
    string content;     // content of message
    bool bot;           // author is a bot (flag)
};

/**
 * @brief Class MessageProcessor.
 *        Instance of this class is being used by ISABot instance
 *        for parsing responses on bot's GET requests to retrieve 
 *        messages on specified channel
 */
class MessageProcessor {

    public:
        
        list<Message> messages = list<Message>(); // list for storing extracted messages

        /**
         * @brief Method for parsing response from Discrod server.
         *        Whole response is recieved, end of header is found and it's
         *        body is parsed into Message objects, which are stored into list 'messages'.
         * 
         * @param response entire server's response in a string data structure
         */
        void parse_messages(string response);

        /**
         * @brief Clear list of messages.
         *        This method is being used by ISABot class instance to reset
         *        MessageProcessor instance and prepare for parsing of new messages
         *        from new GET request. 
         */
        void clear_messages();

        /**
         * @brief Print all objects currently stored in the list of messages.
         *        Format:
         * 
         *        Id:       <message->id>
         *        Username: <message->username> 
         *        Content:  <message->content>
         *        Bot:      <message->bot> 
         */
        void print_messages();

};