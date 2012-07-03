#ifndef _NUTRITION_HANDLER
#define _NUTRITION_HANDLER
#include "../basehandler.h"
#include "../models/supplement.h"
#include  <uuid/uuid.h>
static std::unordered_map<std::string, double> macronutrients = {
    {"Total Fat", 55},
    {"Saturated Fatty Acids", 20},
    {"Cholesterol", 0.3},
    {"Sodium", 2.3},
    {"Potassium", 4.700},
    {"Total Carbohydrate" ,300},
    {"Fiber" ,25},
    {"Protein" ,50}
};

void nurition_handler(request_args &r){
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("base"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
    content_dict->SetFilename("nutrition.html");
    std::list<Supplement> supplements;
    soci::session sql(r.db_pool);
    Supplement::get_all(sql,supplements);
    for(auto it:supplements){
        ctemplate::TemplateDictionary* post_dict = content_dict->AddSectionDictionary("SUPPLEMENTS");
        post_dict->SetValue("NAME", it.name);
        char uuid_str[37];
        uuid_unparse(it.uuid,uuid_str);
        post_dict->SetValue("ID", uuid_str);
    }
    r.conn.reply_http(r.req,render_template("base.html",dict),200,"OK",default_headers);
}
void nurition_edit_handler(request_args &r){
    if(r.args.size()<2){
        r.conn.reply_http(r.req,"Error: no supplement id specified",403,"OK",default_headers);
        return;
    }
    std::vector<m2pp::header> redir_headers = {{"Content-Type","text/html"}};
    int code = 200;
    Supplement s("name","description");
    soci::session sql(r.db_pool);
    std::string uuid;
    uuid = r.args[1].str().substr(1,36);
    if(uuid != "-1/"){
        try{
            uuid_parse(uuid.c_str(),s.uuid);
            log_DEBUG("ID:",uuid);
            sql << "select * from nutrition_supplements where uuid = '" << uuid << "'",soci::into(s);
            if(header_value(r.req.headers,"METHOD") == "POST"){
                std::unordered_map<std::string,std::string> nutrients_form = getFormFields(r.req.body);
                s.name = nutrients_form["name"];
                s.description = nutrients_form["description"];
                s.dosage = atof((const char*)nutrients_form["dosage"].c_str());
                s.nutrients.clear();
                for(auto fields:nutrients_form){
                    std::cout << fields.first << " = " << fields.second << std::endl;
                    auto found = macronutrients.find(fields.first);
                    if(found != macronutrients.end()){
                        s.nutrients[fields.first] = atof((const char*)fields.second.c_str());
                    }
                }
                if(!sql.got_data()){
                    log_DEBUG("Creating ID:",uuid);
                    Supplement::create(sql,s);
                    char uuid_str[37];
                    uuid_unparse(s.uuid,uuid_str);
                    redir_headers.push_back({"Location","/nutrition/edit/"+std::string(uuid_str)+"/" });
                    code = 303;
                }else{
                    sql << "update nutrition_supplements set name = :name , description = :description , dosage = :dosage , nutrients = :nutrients where uuid = '" << uuid << "'", soci::use(s);
                }
            }
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
    }
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("base"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
    content_dict->SetFilename("nutrition_edit.html");
    for(auto nutrient : s.nutrients){
        ctemplate::TemplateDictionary *macronutrients_dict = content_dict->AddSectionDictionary("MACRONUTRIENTS");
        macronutrients_dict->ShowSection("MACRONUTRIENTS");
        log_DEBUG("FETCHED",nutrient.first);
        macronutrients_dict->SetValue("NUTRIENT",nutrient.first);
        std::ostringstream s;
        s << nutrient.second;
        macronutrients_dict->SetValue("DOSAGE",s.str());
    }
    content_dict->SetValue("NAME",s.name);
    content_dict->SetValue("DESCRIPTION",s.description);
    content_dict->SetValue("ID", uuid);
    std::ostringstream str;
    str << s.dosage;
    content_dict->SetValue("DOSAGE",str.str());
    r.conn.reply_http(r.req,render_template("base.html",dict),code,"OK",redir_headers);
}

void macronutrients_list_handler(request_args &r){
    static std::vector<m2pp::header> json_headers = {{"Content-Type","application/json; charset=utf-8"}};
    std::unordered_map<std::string,std::string> args = {{"identifier", "name"},{"label", "name"}};
    std::ostringstream response;
    response <<"{" ;
    for( auto item:args){
        response << "\"" << item.first << "\":\"" << item.second << "\",";
    }
    response << "\"items\":[";
    for(auto item:macronutrients){
        response << "{\"name\":\"" << item.first << "\",\"dosage\":\"" << item.second << "\"},";
    }
    response << "]}" ;
    r.conn.reply_http(r.req,response.str(),200,"OK",json_headers);
}

extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/nutrition"] = &nurition_handler;
    request_handlers_map["/nutrition/edit/(.+)/"] = &nurition_edit_handler;
    request_handlers_map["/nutrition/macronutrients"] = &macronutrients_list_handler;
}

#endif
