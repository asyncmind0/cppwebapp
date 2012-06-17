#ifndef _HANDLER
#define _HANDLER
#include "basehandler.h"
#include "models/post.h"
#include "models/user.h"

void index_handler(m2pp::request& req, m2pp::connection& conn ){
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("base"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
    content_dict->SetFilename("index.html");
    content_dict->SetValue("FIRST_NAME","Steven");
    content_dict->SetValue("LAST_NAME","Steven");
    std::list<Post> posts;
    Post::get_all(posts);
    for(auto it:posts){
        //std::cout << "handler" << it << std::endl;
        ctemplate::TemplateDictionary* post_dict = content_dict->AddSectionDictionary("POSTS");
        post_dict->SetValue("TITLE", it.title);
    }
    conn.reply_http(req,render_template("base.html",dict),200,"OK",default_headers);
}

void register_handler(m2pp::request& req, m2pp::connection& conn ){
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("register"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
    content_dict->SetFilename("register.html");
    if(header_value(req.headers,"METHOD") == "POST"){
        std::unordered_map<std::string,std::string> registration_form = getFormFields(req.body);
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
        int status = User::create(u);
        if(status==1){
            dict->SetValueAndShowSection("REGISTER_STATUS", "success", "REGISTER_MESSAGE");
        }else if(status==2){
            dict->SetValueAndShowSection("REGISTER_STATUS", "User id already exists.", "REGISTER_MESSAGE");
            dict->ShowSection("REGISTER_FORM");
        }else{
            dict->SetValueAndShowSection("REGISTER_STATUS", "fail", "REGISTER_MESSAGE");
            dict->ShowSection("REGISTER_FORM");
        }
    }else{
        dict->ShowSection("REGISTER_FORM");
    }
    conn.reply_http(req,render_template("register.html",dict),200,"OK",default_headers);
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
    std::tm *timeinfo = localtime(&now);
    Post p("my c++ blog","I must be mental to write webapplications in c++, a genuine sociopath I am.","23232312sadad",*timeinfo);
    Post::create(p);
    std::list<Post> posts;
    Post::get_all(posts);
    conn.reply_http(req, "Dbtest done");
}
#endif
