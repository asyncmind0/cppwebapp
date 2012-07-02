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
    int code = 200;
    Supplement s("name","description");
    if(header_value(r.req.headers,"METHOD") == "POST"){
        std::unordered_map<std::string,std::string> nutrients_form = getFormFields(r.req.body);
        s.name = nutrients_form["name"];
        s.description = nutrients_form["description"];
        s.dosage = atof((const char*)nutrients_form["dosage"].c_str());
        for(auto fields:nutrients_form){
            std::cout << fields.first << " = " << fields.second << std::endl;
            auto found = macronutrients.find(fields.first);
            if(found != macronutrients.end()){
                s.nutrients[fields.first] = atof((const char*)fields.second.c_str());
            }
        }
        soci::session sql(r.db_pool);
        Supplement::create(sql,s);
    }
    ctemplate::TemplateDictionary* dict = base_template_variables(new ctemplate::TemplateDictionary("base"));
    ctemplate::TemplateDictionary *content_dict = dict->AddIncludeDictionary("CONTENT");
    content_dict->SetFilename("nutrition.html");
    for(auto nutrient : s.nutrients){
        ctemplate::TemplateDictionary *macronutrients_dict = content_dict->AddSectionDictionary("MACRONUTRIENTS");
        macronutrients_dict->ShowSection("MACRONUTRIENTS");
        macronutrients_dict->SetValue("NUTRIENT",nutrient.first);
        std::ostringstream s;
        s << nutrient.second;
        macronutrients_dict->SetValue("DOSAGE",s.str());

    }
    content_dict->SetValue("STATIC_PREFIX","/static/");
    content_dict->SetValue("NAME","OneStopExtreme");
    content_dict->SetValue("DESCRIPTION","Powder");
    content_dict->SetValue("ID", "id");
    content_dict->SetValue("DOSAGE", "400");
    content_dict->SetValue("DOSAGE_SIZE", "400");
    r.conn.reply_http(r.req,render_template("base.html",dict),code,"OK",default_headers);
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
