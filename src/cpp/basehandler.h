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
#include <boost/lexical_cast.hpp>

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

void include_scripts(ctemplate::TemplateDictionary* dict,const std::list<std::string> &scripts){
        for(auto it:scripts){
            ctemplate::TemplateDictionary* script_dict = dict->AddSectionDictionary("SCRIPTS");
            script_dict->SetValue("SCRIPT", "'"+it+"',");
        }
}
ctemplate::TemplateDictionary* base_template_variables(ctemplate::TemplateDictionary *dict,const std::list<std::string> &scripts = {}){
    ctemplate::Template::SetTemplateRootDirectory("src/html");
    dict->SetGlobalValue("TITLE", "Nutrient Log - Optimize your nutrition.");
    dict->SetGlobalValue("DESCRIPTION", "Nutrient Log - Optimize your nutrition.");
    dict->SetGlobalValue("FOOTER", "Aren't these great results?");
    dict->SetGlobalValue("STATIC_PREFIX", "/static/");
    dict->SetGlobalValue("DOJOTEXTBOX", "data-dojo-type=\"dijit.form.TextBox\"");
    dict->SetGlobalValue("DOJOSIMPLETEXTAREA", "data-dojo-type=\"dijit.form.SimpleTextarea\"");
    dict->SetGlobalValue("DOJOBUTTON", "data-dojo-type=\"dijit.form.Button\"");
    include_scripts(dict,scripts);
    return dict;
}
ctemplate::TemplateDictionary* get_maincontent_dict(ctemplate::TemplateDictionary *dict){
    ctemplate::TemplateDictionary *main_content_dict = dict->AddIncludeDictionary("MAINCONTENT");
    main_content_dict->SetFilename("home.html");
    return main_content_dict;
}

ctemplate::TemplateDictionary* get_content_dict(ctemplate::TemplateDictionary *dict, std::string templatename,const std::list<std::string> &scripts = {}){
    ctemplate::TemplateDictionary *base_dict = base_template_variables(dict,scripts);
    ctemplate::TemplateDictionary *main_content_dict = get_maincontent_dict(base_dict);
    ctemplate::TemplateDictionary *content_dict = main_content_dict->AddIncludeDictionary("CONTENT");
    content_dict->SetFilename(templatename);
    return content_dict;
}

std::string render_template(const std::string templatefile, ctemplate::TemplateDictionary* dict){
    std::string output;
    bool error_free = ctemplate::ExpandTemplate(templatefile, ctemplate::STRIP_WHITESPACE, dict, &output);
    //log_DEBUG("Template rendered:",templatefile,":",pantheios::boolean(error_free));
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
