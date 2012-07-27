#ifndef _HANDLER
#define _HANDLER
#include "../basehandler.h"
#include "../models/post.h"
#include "../models/user.h"
#include  <uuid/uuid.h>

static std::list<std::string> scripts = {};

void login_handler(request_args &r){
    int code=200;
    std::vector<m2pp::header> redir_headers = {{"Content-Type","text/html"}};
    ctemplate::TemplateDictionary* dict =base_template_variables(new ctemplate::TemplateDictionary("login"), scripts);
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("MAINCONTENT");
    content_dict->SetFilename("login.html");
    ctemplate::TemplateDictionary* login_form_section = content_dict->AddSectionDictionary("LOGIN_FORM");
    content_dict->ShowSection("LOGIN_FORM");
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
            content_dict->SetValueAndShowSection("STATUS", "Username is not registered.", "MESSAGE");
        }else{
            redir_headers.push_back({"Location","/nutrition/" });
            code = 303;
        }
    }
    
    r.conn.reply_http(r.req,render_template("base.html",dict),code,"OK",redir_headers);
}

void register_handler(request_args &r){
    ctemplate::TemplateDictionary* dict =base_template_variables(new ctemplate::TemplateDictionary("register"), scripts);
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("MAINCONTENT");
    content_dict->SetFilename("register.html");
    if(header_value(r.req.headers,"METHOD") == "POST"){
        std::unordered_map<std::string,std::string> registration_form = getFormFields(r.req.body);
        for(auto fields:registration_form){
            std::cout << fields.first << " = " << fields.second << std::endl;
        }
        time_t now = time(NULL);
        std::tm *timeinfo = localtime(&now);
        User u(registration_form["userid"],
             registration_form["firstname"],
             registration_form["lastname"],
             registration_form["password"],
             *timeinfo);
        soci::session sql(r.db_pool);
        int status = User::create(sql,u);
        if(status==1){
            content_dict->SetValueAndShowSection("REGISTER_STATUS", "success", "REGISTER_MESSAGE");
        }else if(status==2){
            content_dict->SetValueAndShowSection("REGISTER_STATUS", "User id already exists.", "REGISTER_MESSAGE");
            content_dict->ShowSection("REGISTER_FORM");
        }else{
            content_dict->SetValueAndShowSection("REGISTER_STATUS", "fail", "REGISTER_MESSAGE");
            content_dict->ShowSection("REGISTER_FORM");
        }
    }else{
        content_dict->ShowSection("REGISTER_FORM");
    }
    r.conn.reply_http(r.req,render_template("base.html",dict),200,"OK",default_headers);
}

extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/login"] = &login_handler;
    request_handlers_map["/register"] = &register_handler;
}
#endif
