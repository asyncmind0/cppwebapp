#ifndef _HANDLER
#define _HANDLER
#include "../basehandler.h"
#include "../models/post.h"
#include "../models/user.h"
#include  <uuid/uuid.h>
#include <boost/algorithm/string.hpp>

static std::list<std::string> scripts = {};

void login_handler(request_args &r){
    int code=200;
    std::vector<m2pp::header> redir_headers = {{"Content-Type","text/html"}};
    mustache::Context* dict = base_template_variables(new mustache::Context(), scripts);
    mustache::PlustacheTypes::ObjectType login_form_section, message_section;
    soci::session sql(r.db_pool);
    User *user  = new User();
    if(header_value(r.req.headers,"METHOD") == "POST"){
        std::unordered_map<std::string,std::string> login_form = getFormFields(r.req.body);
        for(auto fields:login_form){
            std::cout << fields.first << " = " << fields.second << std::endl;
        }
        std::string username = login_form["userid"];
        log_DEBUG(username);
        sql << "select * from users where username = '" << username << "'",soci::into(*user);
        if(!sql.got_data()){
            message_section["STATUS"] = "Username is not registered.";
        }else{
            redir_headers.push_back({"Location","/nutrition/" });
            code = 303;
        }
    }
    dict->add("LOGIN_FORM", login_form_section);
    dict->add("MESSAGE", message_section);
    
    r.conn.reply_http(r.req,render_template("login",dict),code,"OK",redir_headers);
}

void register_handler(request_args &r){
    mustache::Context* dict = base_template_variables(new mustache::Context(), scripts);
    mustache::PlustacheTypes::ObjectType registration_form_section, message_section;
    if(header_value(r.req.headers,"METHOD") == "POST"){
        std::unordered_map<std::string,std::string> registration_form = getFormFields(r.req.body);
        for(auto fields:registration_form){
            std::cout << fields.first << " = " << fields.second << std::endl;
        }
        time_t now = time(NULL);
        std::tm *timeinfo = localtime(&now);
        std::vector<std::string> names = {"",""};
        boost::split(names, registration_form["name"], boost::is_any_of(" "));
        User u(registration_form["userid"],
               names[0],
               names[1],
             registration_form["password"],
             *timeinfo);
        soci::session sql(r.db_pool);
        int status = User::create(sql,u);
        if(status==1){
            message_section["STATUS"] = "success";
        }else if(status==2){
            message_section["STATUS"] = "User id already exists.";
        }else{
            message_section["STATUS"] = "fail";
        }
    }
    dict->add("REGISTER_FORM",registration_form_section);
    dict->add("MESSAGE",message_section);
    r.conn.reply_http(r.req,render_template("register",dict),200,"OK",default_headers);
}

extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/login"] = &login_handler;
    request_handlers_map["/register"] = &register_handler;
}
#endif
