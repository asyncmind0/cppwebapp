#ifndef _BASEHANDLER
#define _BASEHANDLER 
#include "logging.h"
#include <zmq.hpp>
#include <sstream>
#include <fstream>
#include <m2pp.hpp>
#include <regex>
#include <future>
#include <curl/curl.h>
#include <dlfcn.h>
#include <unordered_map>
#include <cstring>
#include <string>
#include <iostream>
#include <map>
#include <list>
#include "form.h"
#include <ctemplate/template.h>
#include <soci/soci.h>

typedef struct _request_args_ {
    soci::connection_pool &db_pool;
    m2pp::request &req;
    m2pp::connection &conn;
    //std::match_results<std::string::const_iterator> &args;
    std::smatch &args;
_request_args_(m2pp::request &req, m2pp::connection &conn, soci::connection_pool &pool,std::smatch &args)
: db_pool(pool),req(req),conn(conn),args(args){
}
    
} request_args;

typedef void (*request_handler)(request_args &req_args);
typedef int (*handler_initializer)(std::unordered_map<std::string, request_handler> &request_handlers_map);
static std::vector<m2pp::header> default_headers = {{"Content-Type","text/html"},};


ctemplate::TemplateDictionary* base_template_variables(ctemplate::TemplateDictionary *dict){
    ctemplate::Template::SetTemplateRootDirectory("src/html");
    dict->SetValue("TITLE", "C++ web dev");
    dict->SetValue("DESCRIPTION", "Steven's first site written using c++11.");
    dict->SetValue("FOOTER", "Aren't these great results?");
    dict->SetValue("STATIC_PREFIX", "/static/");
    return dict;
}


std::string render_template(const std::string templatefile, ctemplate::TemplateDictionary* dict){
    std::string output;
    bool error_free = ctemplate::ExpandTemplate(templatefile, ctemplate::STRIP_WHITESPACE, dict, &output);
    log_DEBUG("Template rendered:",templatefile,":",pantheios::boolean(error_free));
    return output;
}

const std::string header_value(std::vector<m2pp::header> headers, std::string key){
    for(auto h:headers){
        if(h.first == key){
            return h.second;
        }
    }
    return "";
}

template <typename T>
void render_dojo_json(request_args& r, std::unordered_map<std::string,std::string> headers,
                      std::unordered_map<std::string,T> &items){
    static std::vector<m2pp::header> json_headers = {{"Content-Type","application/json; charset=utf-8"}};
    std::ostringstream response;
    response <<"{" ;
    for( auto item:headers){
        response << "\"" << item.first << "\":\"" << item.second << "\",";
    }
    response << "\"items\":[";
    for(auto item:items){
        response << "{\"" << item.first << "\":\"" << item.second << "\"},";
    }
    response << "]}" ;
    r.conn.reply_http(r.req,response.str(),200,"OK",json_headers);
}
template <typename T>
void render_json(request_args& r, std::unordered_map<std::string,T> &map){
    static std::vector<m2pp::header> json_headers = {{"Content-Type","application/json; charset=utf-8"}};
    std::ostringstream response;
    response <<"{" ;
    for( auto item:map){
        response << "\"" << item.first << "\":\"" << item.second << "\",";
    }
    response << "}" ;
    r.conn.reply_http(r.req,response.str(),200,"OK",json_headers);
}
    
    
#endif
