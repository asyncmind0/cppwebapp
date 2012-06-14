#include <zmq.hpp>
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <unordered_map>
#include "m2pp.hpp"
#include <regex>
#include <future>
#include <curl/curl.h>
#include <log4cxx/logger.h>
#include "log4cxx/basicconfigurator.h"

static const std::string templatepath = "src/html/";
typedef void (*request_handler)(m2pp::request& req, m2pp::connection& conn);

#include "models.h"
#include "handlers.h"

int main(int argc, char *argv[])
{
  log4cxx::BasicConfigurator::configure();
  std::string sender_id = "82209006-86FF-4982-B5EA-D1E29E55D481";
  std::unordered_map<std::string, request_handler> request_handlers;
  request_handlers["/"] = &index_handler;
  request_handlers["/dbtest"] = &dbtest_handler;
  request_handlers["/login"] = &login_handler;

  if(curl_global_init(CURL_GLOBAL_ALL)!=0){
    std::cerr << "Curl init failed" << std::endl;
  }

  if (argc != 3) {
    std::cerr << "Usage: " << argv[0] << " <from> <to>\n"
      "\tExample: " << argv[0] << " 'tcp://127.0.0.1:8988'"
      " 'tcp://127.0.0.1:8989'" << std::endl;
    return 1;
  }

  m2pp::connection conn(sender_id, argv[1], argv[2]);

  std::cout << "== starting server ==" << std::endl;
  int handled = 0;
  while (1) {
    m2pp::request req = conn.recv();

    if (req.disconnect) {
      std::cout << "== disconnect ==" << std::endl;
      continue;
    }

    std::ostringstream response;
    response << "<pre>" << std::endl;
    response << "SENDER: " << req.sender << std::endl;
    response << "IDENT: " << req.conn_id << std::endl;
    response << "PATH: " << req.path << std::endl;
    response << "BODY: " << req.body << std::endl;
    for (std::vector<m2pp::header>::iterator it=req.headers.begin();it!=req.headers.end();it++) {
      response << "HEADER: " << it->first << ": " << it->second << std::endl;
    }
    response << "</pre>" << std::endl;
    std::cout << response.str();
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
      std::cout << "No handler found:"<< req.path << std::endl;
      conn.reply_http(req, std::string("No handler found"));
    }
                  
  }

  return 0;
}

