#ifndef _NUTRITION_HANDLER
#define _NUTRITION_HANDLER
#include "../basehandler.h"
#include "../models/supplement.h"
#include "../models/macronutrient.h"
#include  <uuid/uuid.h>

static std::list<std::string> scripts = {"app/nutrition"};

/*
  static std::unordered_map<std::string, double> macronutrients = {
  {"Total Fat", 55},
  {"Saturated Fatty Acids", 20},
  {"Cholesterol", 0.3},
  {"Sodium", 2.3},
  {"Potassium", 4.700},
  {"Total Carbohydrate" ,300},
  {"Fiber" ,25},
  {"Protein" ,50},
  {"Clarinol" ,3.2},
  {"L-Carnitine" ,2},
  {"L-Tartrate" ,0.5}
  };
*/

void nurition_handler(request_args &r){
    mustache::Context* dict = base_template_variables(new mustache::Context(), scripts);
    std::list<Supplement> supplements;
    soci::session sql(r.db_pool);
    Supplement::get_all(sql,supplements);
    mustache::PlustacheTypes::CollectionType c;
    for(auto it:supplements){
        mustache::PlustacheTypes::ObjectType post_dict;
        post_dict["NAME"] =  it.name;
        char uuid_str[37];
        uuid_unparse(it.uuid,uuid_str);
        post_dict["ID"] = uuid_str;
        c.push_back(post_dict);
    }
    dict->add("SUPPLEMENTS",c);
    r.conn.reply_http(r.req,render_template("nutrition",dict),200,"OK",default_headers);
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
                std::unordered_map<std::string, double> macronutrients ;
                MacroNutrient::get_dict(sql,macronutrients);
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
                    Supplement::update(sql,s);
                }
            }
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
    }
    mustache::Context* dict = base_template_variables(new mustache::Context(), scripts);
    mustache::PlustacheTypes::CollectionType macronutrients_section;
    for(auto nutrient : s.nutrients){
        mustache::PlustacheTypes::ObjectType macronutrients_dict;
        macronutrients_dict["NUTRIENT"] = nutrient.first;
        macronutrients_dict["DOSAGE"] = toDoubleStr(nutrient.second);
        macronutrients_section.push_back(macronutrients_dict);
    }
    std::unordered_map<std::string, double> macronutrients ;
    MacroNutrient::get_dict(sql,macronutrients);
    mustache::PlustacheTypes::CollectionType drvmap_section;
    for(auto nutrient : macronutrients){
        mustache::PlustacheTypes::ObjectType drvmap;
        drvmap["NUTRIENT"] = nutrient.first;
        drvmap["DRV"] = toDoubleStr(nutrient.second);
        drvmap_section.push_back(drvmap);
    }
    dict->add("NAME",s.name);
    dict->add("DESCRIPTION",s.description);
    dict->add("ID", uuid);
    dict->add("DOSAGE",toDoubleStr(s.dosage));
    dict->add("DRVMAP", drvmap_section);
    dict->add("MACRONUTRIENTS",macronutrients_section);
    r.conn.reply_http(r.req,render_template("nutrition_edit",dict),code,"OK",redir_headers);

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

    std::unordered_map<std::string, double> macronutrients ;
    soci::session sql(r.db_pool);
    MacroNutrient::get_dict(sql,macronutrients);
    for(auto item:macronutrients){
        response << "{\"name\":\"" << item.first << "\",\"dosage\":\"" << item.second << "\"},";
    }
    response << "]}" ;
    r.conn.reply_http(r.req,response.str(),200,"OK",json_headers);
}

void macronutrient_handler(request_args &r){
    mustache::Context* dict = base_template_variables(new mustache::Context(), scripts);
    mustache::PlustacheTypes::CollectionType macronutrients_section;
    std::list<MacroNutrient> macronutrients;
    soci::session sql(r.db_pool);
    MacroNutrient::get_all(sql,macronutrients);
    for(auto it:macronutrients){
        mustache::PlustacheTypes::ObjectType post_dict;
        post_dict["NAME"] = it.name;
        char uuid_str[37];
        uuid_unparse(it.uuid,uuid_str);
        post_dict["ID"] = uuid_str;
        std::ostringstream s;
        s << it.drv;
        post_dict["DRV"] = s.str();
    }
    dict->add("MACRONUTRIENTS", macronutrients_section);
    r.conn.reply_http(r.req,render_template("macronutrients",dict),200,"OK",default_headers);
}
void macronutrient_edit_handler(request_args &r){
    if(r.args.size()<2){
        r.conn.reply_http(r.req,"Error: no supplement id specified",403,"OK",default_headers);
        return;
    }
    std::vector<m2pp::header> redir_headers = {{"Content-Type","text/html"}};
    int code = 200;
    MacroNutrient s("name","description");
    soci::session sql(r.db_pool);
    std::string uuid;
    uuid = r.args[1].str().substr(1,36);
    if(uuid != "-1/"){
        try{
            uuid_parse(uuid.c_str(),s.uuid);
            log_DEBUG("ID:",uuid);
            sql << "select * from nutrition_macronutrients where uuid = '" << uuid << "'",soci::into(s);
            if(header_value(r.req.headers,"METHOD") == "POST"){
                std::unordered_map<std::string,std::string> nutrients_form = getFormFields(r.req.body);
                s.name = nutrients_form["name"];
                s.description = nutrients_form["description"];
                s.drv = atof((const char*)nutrients_form["dosage"].c_str());
                if(!sql.got_data()){
                    log_DEBUG("Creating ID:",uuid);
                    MacroNutrient::create(sql,s);
                    char uuid_str[37];
                    uuid_unparse(s.uuid,uuid_str);
                    redir_headers.push_back({"Location","/nutrition/macronutrient/edit/"+std::string(uuid_str)+"/" });
                    code = 303;
                }else{
                    MacroNutrient::update(sql,s,uuid);
                }
            }
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
    }
    mustache::Context* dict = base_template_variables(new mustache::Context(), scripts);
    dict->add("NAME",s.name);
    dict->add("DESCRIPTION",s.description);
    dict->add("ID", uuid);
    std::ostringstream str;
    str << s.drv;
    dict->add("DRV",str.str());
    r.conn.reply_http(r.req,render_template("macronutrient_edit",dict),code,"OK",redir_headers);
}

extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/nutrition/edit/(.+)/*"] = &nurition_edit_handler;
    request_handlers_map["/nutrition/*"] = &nurition_handler;
    request_handlers_map["/nutrition/macronutrients/*"] = &macronutrients_list_handler;
    request_handlers_map["/nutrition/macronutrient/*"] = &macronutrient_handler;
    request_handlers_map["/nutrition/macronutrient/edit/(.+)/*"] = &macronutrient_edit_handler;
}

#endif
