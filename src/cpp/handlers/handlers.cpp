#ifndef _HANDLER
#define _HANDLER
#include "../basehandler.h"
#include "../models/post.h"
#include "../models/user.h"
#include  <uuid/uuid.h>


void index_handler(request_args &r){
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("base"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
    content_dict->SetFilename("index.html");
    content_dict->SetValue("FIRST_NAME","Steven");
    content_dict->SetValue("LAST_NAME","Joseph");
    std::list<Post> posts;
    soci::session sql(r.db_pool);
    Post::get_all(sql,posts);
    for(auto it:posts){
        ctemplate::TemplateDictionary* post_dict = content_dict->AddSectionDictionary("POSTS");
        post_dict->SetValue("TITLE", it.title);
        char uuid_str[37];
        uuid_unparse(it.uuid,uuid_str);
        post_dict->SetValue("ID", uuid_str);
    }
    r.conn.reply_http(r.req,render_template("base.html",dict),200,"OK",default_headers);
}

void edit_handler(request_args &r){
    if(r.args.size()<2){
        r.conn.reply_http(r.req,"Error: no post id specified",403,"OK",default_headers);
        return;
    }
    std::vector<m2pp::header> redir_headers = {{"Content-Type","text/html"}};
    int code = 200;
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("base"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
    Post *post  = new Post("","");
    soci::session sql(r.db_pool);
    std::string uuid;
    uuid = r.args[1].str().substr(1,36);
    if(uuid != "-1/"){
        try{
            uuid_parse(uuid.c_str(),post->uuid);
            log_DEBUG("ID:",uuid);
            sql << "select * from blog_posts where uuid = '" << uuid << "'",soci::into(*post);
            if(header_value(r.req.headers,"METHOD") == "POST"){
                std::unordered_map<std::string, std::string> form =  getFormFields(r.req.body);
                post->title = form["title"];
                post->body = form["body"];
                if(!sql.got_data()){
                    log_DEBUG("Creating ID:",uuid);
                    Post::create(sql,*post);
                    char uuid_str[37];
                    uuid_unparse(post->uuid,uuid_str);
                    redir_headers.push_back({"Location","/editpost/"+std::string(uuid_str)+"/" });
                    code = 303;
                }else{
                    sql << "update blog_posts set title = :title , body = :body where uuid = '" << uuid << "'", soci::use(*post);
                }
            }
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
    }
    content_dict->SetFilename("edit.html");
    content_dict->SetValue("TITLE",post->title);
    content_dict->SetValue("BODY",post->body);
    content_dict->SetValue("ID", uuid);
    delete post;
    r.conn.reply_http(r.req,render_template("base.html",dict),code,"OK",redir_headers);
}

void delete_handler(request_args &r){
    static std::vector<m2pp::header> json_headers = {{"Content-Type","application/json; charset=utf-8"}};
    soci::session sql(r.db_pool);
    std::string id,uuid;
    if(r.args.size()<2){
        r.conn.reply_http(r.req,"error",403,"OK",default_headers);
        return;
    }
    id = r.args[1].str();
    uuid = id.substr(1,36);
    try{
        log_DEBUG("ID:",id);
        sql << "delete from blog_posts where uuid = '" << uuid << "'";
    }catch (std::exception const &e){
        log_ERROR("db error:", e);
    }catch(...){
        log_ERROR("unknown db error");
    }

    std::unordered_map<std::string,std::string> resp({{"id",uuid}});
    render_json(r,resp);
}
void register_handler(request_args &r){
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("register"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
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
    r.conn.reply_http(r.req,render_template("register.html",dict),200,"OK",default_headers);
}

void login_handler(request_args &r){
    for(auto fields:getFormFields(r.req.body)){
        std::cout << fields.first << " = " << fields.second << std::endl;
    }
    r.conn.reply_http(r.req, "loggin done");
}

void dbtest_handler(request_args &r){
    time_t now = time(NULL);
    std::tm *timeinfo = localtime(&now);
    uuid_t uuid;
    uuid_generate(uuid);
    Post p("my c++ blog","I must be mental to write webapplications in c++, a genuine sociopath I am.", uuid, *timeinfo);
    soci::session sql(r.db_pool);
    Post::create(sql,p);
    std::list<Post> posts;
    Post::get_all(sql,posts);
    r.conn.reply_http(r.req, "Dbtest done");
}


extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/"] = &index_handler;
    request_handlers_map["/dbtest"] = &dbtest_handler;
    request_handlers_map["/login"] = &login_handler;
    request_handlers_map["/register"] = &register_handler;
    request_handlers_map["/editpost/(.+)/"] = &edit_handler;
    request_handlers_map["/deletepost/(.+)/"] = &delete_handler;
}
#endif
