#ifndef _BASEHANDLER
#define _BASEHANDLER 
#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "m2pp.hpp"
#include "MPFDParser-0.1.1/Parser.h"
#include "MPFDParser-0.1.1/Exception.h"
#include "MPFDParser-0.1.1/Field.h"
#include <curl/curl.h>
#include <ctemplate/template.h>
#include <pantheios/pantheios.hpp>
#include <pantheios/inserters.hpp>  // Pantheios inserter classes


static std::vector<m2pp::header> default_headers = {{"Content-Type","text/html"},};

ctemplate::TemplateDictionary* base_template_variables(ctemplate::TemplateDictionary *dict){
    ctemplate::Template::SetTemplateRootDirectory("src/html");
    dict->SetValue("TITLE", "C++ web dev");
    dict->SetValue("DESCRIPTION", "Steven's first site written using c++11.");
    dict->SetValue("FOOTER", "Aren't these great results?");
    dict->SetValue("STATIC_PREFIX", "/static/");
    return dict;
}
    

template <typename OutIt>
OutIt split(const std::string &text, char sep, OutIt out)
{
    size_t start = 0, end=0;
    while((end = text.find(sep, start)) != std::string::npos)
    {
        *out++ = text.substr(start, end - start);
        start = end + 1;
    }
    *out++ = text.substr(start);
    return out;
}
std::unordered_map<std::string, std::string> getFormFields(std::string& form_data){
    CURL *curl = curl_easy_init( );
    std::unordered_map<std::string, std::string> form_fields;
    std::vector<std::string> pairs;
    split(form_data, '&', std::back_inserter(pairs));
    for(auto a:pairs){
        std::vector<std::string> vals;
        split(a, '=', std::back_inserter(vals));
        if(vals.size() == 2){
            char *unencoded_value =  curl_easy_unescape(curl , vals[1].c_str() , 0 , nullptr); 
            char *unencoded_key =  curl_easy_unescape(curl , vals[0].c_str() , 0 , nullptr); 
            form_fields[unencoded_key] = std::string(unencoded_value);
            curl_free(unencoded_key);
            curl_free(unencoded_value);
        }
    }
    return form_fields;
}
std::map<std::string,MPFD::Field *> getFormFields(const char* form_data, const std::string content_type){
    try {
        std::cout << "init parser" <<std::endl;
        MPFD::Parser* POSTParser = new MPFD::Parser();
        //POSTParser->SetTempDirForFileUpload("/tmp");
        std::cout << "init parser 2" <<std::endl;
        //POSTParser->SetMaxCollectedDataLength(20*1024);

        std::cout << "init parser 3" <<std::endl;
        POSTParser->SetContentType(content_type);/* Here you know the
                                                    Content-type: string. And you pass it.*/
        std::cout << "AcceptSomeData" <<std::endl;
        POSTParser->AcceptSomeData(form_data, std::strlen(form_data));
        std::map<std::string,MPFD::Field *> fields =  POSTParser->GetFieldsMap();
        std::cout << "Have " << fields.size() << " fields\n\r";
        return fields;
    } catch (MPFD::Exception e) {
        // Parsing input error
        std::cout << "Exception:"<<e.GetError() << std::endl;
        std::map<std::string,MPFD::Field *> fields;
        return fields;
    }
}

std::string render_template(const std::string templatefile, ctemplate::TemplateDictionary* dict){
    std::string output;
    bool error_free = ctemplate::ExpandTemplate(templatefile, ctemplate::STRIP_WHITESPACE, dict, &output);
    pantheios::log_DEBUG("Template rendered:",templatefile,":",pantheios::boolean(error_free));
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
