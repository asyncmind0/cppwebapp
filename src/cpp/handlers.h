#include <cstring>
#include <string>
#include <iostream>
#include <sstream>
#include <fstream>
#include <map>
#include "m2pp.hpp"
#include "cjango/Parser.h"
#include "cjango/TemplateNode.h"
#include "cjango/Context.h"
#include "cjango/VariantUtils.h"
#include "MPFDParser-0.1.1/Parser.h"
#include "MPFDParser-0.1.1/Exception.h"
#include "MPFDParser-0.1.1/Field.h"
#include <curl/curl.h>

static std::vector<m2pp::header> default_headers = {{"Content-Type","text/html"},};

void base_context(cjango::Context& context){
  context.set("title", std::string("Steven's first site written using c++11."));
  context.set("description", std::string("Steven's first site written using c++11."));
  context.set("static_prefix", std::string("/static/"));
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
  
void index_handler(m2pp::request& req, m2pp::connection& conn ){
  std::fstream inputStream((templatepath+"/index.html").c_str(), std::fstream::in);
  cjango::Parser parser(&inputStream);
  cjango::TemplateNode* root = parser.parse();
  cjango::Context context;
  base_context(context);
  context.set("first_name", std::string("Steven"));
  context.set("last_name", std::string("Joseph"));
  std::ostringstream outputStream;
  root->render(&context, &outputStream);
  //std::cout << outputStream.str() <<std::endl;
  delete root;
  conn.reply_http(req,outputStream.str(),200,"OK",default_headers);
}

void login_handler(m2pp::request& req, m2pp::connection& conn ){
    for(auto fields:getFormFields(req.body)){
            std::cout << fields.first << " = " << fields.second << std::endl;
    }
        
    conn.reply_http(req, "loggin done");
}

void dbtest_handler(m2pp::request& req, m2pp::connection& conn ){
    //test_connection();
  time_t now = time(NULL);
  Post p("my c++ blog","I must be mental to write webapplications in c++, a genuine sociopath I am.","23232312sadad",now);
  createPost(p);
  conn.reply_http(req, "Dbtest done");
}

