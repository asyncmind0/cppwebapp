#include <zmq.hpp>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include <m2pp.hpp>
#include <regex>
#include <future>
#include <curl/curl.h>


static const std::string templatepath = "src/html/";
typedef void (*request_handler)(m2pp::request& req, m2pp::connection& conn);
#define __DEBUG__ 1
#include "logging.h"
#include "models/post.h"
#include "models/user.h"
#include "handlers.h"
#include "cache.h"

int main(int argc, char *argv[])
{
    try
    {
        log_DEBUG("Entering main(", pantheios::args(argc, argv, pantheios::args::arg0FileOnly), ")");


        std::string sender_id = "82209006-86FF-4982-B5EA-D1E29E55D481";
        std::unordered_map<std::string, request_handler> request_handlers;
        request_handlers["/"] = &index_handler;
        request_handlers["/dbtest"] = &dbtest_handler;
        request_handlers["/login"] = &login_handler;
        request_handlers["/register"] = &register_handler;

        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <from> <to>\n"
                "\tExample: " << argv[0] << " 'tcp://127.0.0.1:8988'"
                " 'tcp://127.0.0.1:8989'" << std::endl;
            return 1;
        }

        if(curl_global_init(CURL_GLOBAL_ALL)!=0){
            log_CRITICAL("Curl init failed");
            return 1;
        }

        log_INFORMATIONAL("== starting db connection pool ==");
        if(!init_soci()){
            log_CRITICAL("== starting db connection pool failed ==");
            return 1;
        }

        Post::create_table();
        User::create_table();

        m2pp::connection conn(sender_id, argv[1], argv[2]);
        log_INFORMATIONAL("== starting server ==");
        int handled = 0;
        while (1) {
            m2pp::request req = conn.recv();
            if (req.disconnect) {
                log_INFORMATIONAL("== disconnect ==");
                continue;
            }

            std::ostringstream response;
            log_DEBUG( "<pre>");
            log_DEBUG( "SENDER: ", req.sender );
            log_DEBUG( "IDENT: " , req.conn_id );
            log_DEBUG( "PATH: " , req.path);
            log_DEBUG( "BODY: " , req.body );
            for( auto header:req.headers){
                log_DEBUG("HEADER: ", header.first, ": ", header.second);
            }
            log_DEBUG( "</pre>");
            handled = 0;
            for(auto handler: request_handlers){
                std::regex rx(handler.first);
                if(regex_match(req.path,rx) == 1){
                    auto result = std::async(std::launch::async,
                                             *handler.second,std::ref(req), std::ref(conn));
                    handled = 1;
                    break;
                }
            }
            if(handled==0){
                log_WARNING("No handler found:", req.path);
                conn.reply_http(req, std::string("No handler found"));
            }

        }
        log_INFORMATIONAL("Done serving:");
        return 0;
    }
    catch(std::bad_alloc&){
        log_ALERT("out of memory");
    }
    catch(std::exception& x){
        log_CRITICAL("Exception: ", x);
    }
    catch(...){
        log_EMERGENCY( "Unknown error");
    }
    return 2;
}

