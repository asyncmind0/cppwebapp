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
#include <template.hpp>
#include <context.hpp>
#include <plustache_types.hpp>
#include <soci/soci.h>
#include <boost/lexical_cast.hpp>
#include <boost/algorithm/string/join.hpp>

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

void include_scripts(mustache::Context *dict,const std::list<std::string> &scripts){
    if(scripts.size()>0){
        mustache::PlustacheTypes::CollectionType c;
        std::string joined = boost::algorithm::join(scripts, "\", \"");
        mustache::PlustacheTypes::ObjectType script;
        script["SCRIPT"] = "\""+joined+"\"";
        c.push_back(script);
        dict->add("SCRIPTS",c);
        log_DEBUG("here");
    }
}

mustache::Context* base_template_variables(mustache::Context *dict,const std::list<std::string> &scripts = {}){
    dict->add("TITLE",  "Nutrient Log - Optimize your nutrition.");
    dict->add("DESCRIPTION", "Nutrient Log - Optimize your nutrition.");
    dict->add("FOOTER", "Aren't these great results?");
    dict->add("STATIC_PREFIX",  "/static/");
    dict->add("DOJOTEXTBOX",  "data-dojo-type=\"dijit.form.TextBox\"" );
    dict->add("DOJOSIMPLETEXTAREA", "data-dojo-type=\"dijit.form.SimpleTextarea\"");
    dict->add("DOJOBUTTON", "data-dojo-type=\"dijit.form.Button\"" );
    include_scripts(dict,scripts);
    return dict;
}

std::string render_template(const std::string templatefile, mustache::Context* dict){
    std::string template_path("src/html/mustache/");
    mustache::template_t t(template_path);
    return t.render(templatefile+".mustache",*dict);
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
