/**
 * @file    message.cpp
 * @author  Michal Rein (xreinm00@vutbr.cz)
 * @brief   ISA Projekt - Discrod bot
 *          This source file contains implementation of MessageProcessor class.
 * @version 0.1
 * @date    2020-10-11
 */

#include "message.h"

// Method for parsing response from Discrod server
void MessageProcessor::parse_messages(string response){

    bool header = true;
    bool parsing_key = false, parsing_value = false;
    int  nested = 0;
    bool finished_message = false;
    bool inside_string = false;

    string content = "";
    
    string key = "";
    string value = "";

    Message msg;

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
                    nested += 1;
                break;
            
            case '}':
                if (nested == 1) {
                    messages.push_front(msg);
                    finished_message = false;
                }
                nested -= 1;
                break;
            
            case '"':
                if (key.empty()) {

                    auto next1 = it + 1;
                    auto next2 = it + 2;
                    auto next3 = it + 3;

                    if (*next1 == 'i' && *next2 == 'd' && *next3 == '"' && nested == 1){
                        parsing_key = true;
                    } else if (*next1 == 'c' && *next2 == 'o' && *next3 == 'n' && nested == 1){
                        parsing_key = true;
                    } else if (*next1 == 'u' && *next2 == 's' && *next3 == 'e'){
                        parsing_key = true;
                    } else if (*next1 == 'a' && *next2 == 'v' && *next3 == 'a'){
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
                        if (key == "id") msg.id = value;
                        if (key == "content") msg.content = value;
                        if (key == "username") {
                            msg.username = value;
                            if (value.find("bot") != string::npos){
                                msg.bot = true;
                            } else {
                                msg.bot = false;
                            }
                        }
                        if (key == "avatar") finished_message = true;
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

}

// Clear list of extracted messages
void MessageProcessor::clear_messages(){
    messages.clear();
}

// Print extracted messages
void MessageProcessor::print_messages(){
    if (messages.empty()) cout << "No messages!" << endl;
    for (auto it = messages.begin(); it != messages.end(); ++it){
        std::cout << endl << "Id: "       << it->id << endl
                          << "Username: " << it->username << endl
                          << "Content: "  << it->content << endl
                          << "Bot: "      << it->bot << endl << endl;
    }
}