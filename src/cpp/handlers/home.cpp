#ifndef _HOMEHANDLER
#define _HOMEHANDLER
#include "../basehandler.h"

static std::list<std::string> scripts = {};
void index_handler(request_args &r){
    mustache::Context* dict = base_template_variables(new mustache::Context(), scripts);
    r.conn.reply_http(r.req,render_template("landing",dict),200,"OK",default_headers);
}
extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/"] = &index_handler;
}
#endif
