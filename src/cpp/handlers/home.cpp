#ifndef _HOMEHANDLER
#define _HOMEHANDLER
#include "../basehandler.h"

static std::list<std::string> scripts = {};
void index_handler(request_args &r){
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("base"), scripts);
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("MAINCONTENT");
    content_dict->SetFilename("landing.html");
    r.conn.reply_http(r.req,render_template("base.html",dict),200,"OK",default_headers);
}
extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/"] = &index_handler;
}
#endif
