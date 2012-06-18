#ifndef _BASEHANDLER
#define _BASEHANDLER 
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "m2pp.hpp"
#include "form.h"
#include <curl/curl.h>
#include <ctemplate/template.h>

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
#endif
