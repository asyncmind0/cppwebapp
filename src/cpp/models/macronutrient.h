#ifndef _MACRONUTRIENT
#define _MACRONUTRIENT
#include "../models.h"
#include "../cache.h"

class MacroNutrient
{
public:
    std::string name;
    double drv;
    uuid_t uuid;
    std::string description;
    std::tm created_datetime;

    MacroNutrient(){
    }
MacroNutrient(std::string name, std::string description):name(name),description(description){
    time_t now = time(NULL);
    created_datetime = *localtime(&now);
    uuid_generate(uuid);
    }

MacroNutrient(std::string name, std::string description, uuid_t u,std::tm created_datetime)
    :name(name), description(description),  created_datetime(created_datetime)
    {
        uuid_copy(uuid, u);
    }
    ~MacroNutrient(){
    }
    friend std::ostream& operator<<(std::ostream& ostr, const MacroNutrient& macronutrient){
        //ostr << "uuid:" << post.uuid << std::endl;
      ostr << "name:" << macronutrient.name << std::endl;
      ostr << "description:" << macronutrient.description << std::endl;
      ostr << "created_datetime:" << asctime(&macronutrient.created_datetime) << std::endl;
    }
    static int create_table(soci::session &sql){
        try{
            sql << "create table nutrition_macronutrients (name varchar(200), description varchar(200), uuid varchar(100), createdtime timestamp, drv numeric);";
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 1;
    }
    static int get_all(soci::session &sql, std::list<MacroNutrient> &macronutrients){
        try{
            //std::cout << "We have " << get_row_count("blog_posts", sql) << " entries in the blog.\n";
            soci::rowset<MacroNutrient> rs = (sql.prepare << "select * from nutrition_macronutrients");
            for(auto it:rs)
                macronutrients.push_back(it);
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 0;
    }
    static int get_dict(soci::session &sql, std::unordered_map<std::string, double> &macronutrients){
        try{
            //std::cout << "We have " << get_row_count("blog_posts", sql) << " entries in the blog.\n";
            soci::rowset<MacroNutrient> rs = (sql.prepare << "select * from nutrition_macronutrients");
            for(auto it:rs)
                macronutrients[it.name]=it.drv;
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 0;
    }

    static int create(soci::session &sql, MacroNutrient& p){
        try{
            int count;
            sql << "insert into nutrition_macronutrients(uuid, name, description, createdtime, drv) values(:uuid, :name, :description, :createdtime, :drv)", soci::use(p);
            std::cout << "We have inserted macronutrient" << std::endl;
        }catch (std::exception const &e){
            std::cerr << "Error: " << e.what() << '\n';
        }catch(...){
            log_ERROR("db error");
        }
        return 0;
    }
    static int update(soci::session &sql, MacroNutrient& p, std::string& uuid ){
        try{
            sql << "update nutrition_macronutrients set name = :name , description = :description "
                << ", drv = :drv where uuid = :uuid ", soci::use(p);
        }catch (std::exception const &e){
            std::cerr << "Error: " << e.what() << '\n';
        }catch(...){
            log_ERROR("db error");
        }
        return 0;
    }
    /*static void cache_set(Post &post)
    {
        std::vector<char> raw_product(sizeof(Post));
        memcpy(&raw_product[0], &post, sizeof(Post));
            char uuid_str[37];
            uuid_unparse(post.uuid,uuid_str);
        MyCache::singleton().set(uuid_str, raw_product);
    }
    static Post cache_get(const std::string &key)
    {
        std::vector<char> raw_product= MyCache::singleton().get(key);
        Post ret;
        if (! raw_product.empty())
        {
            memcpy(&ret, &raw_product[0], sizeof(Post));
        }
        else
        {
            // retrieve it from the persistent store 
        }
        return ret;
    }*/
};

namespace soci
{
    template<> struct type_conversion<MacroNutrient>
    {
        typedef values base_type;
        static void from_base(values const & v, indicator /* ind */, MacroNutrient & p)
        {
            p.name = v.get<std::string>("name");
            std::string uu = v.get<std::string>("uuid");
            uuid_parse(uu.c_str(),p.uuid); 
            p.description = v.get<std::string>("description");
            p.created_datetime = v.get<std::tm>("createdtime");
            p.drv = v.get<double>("drv");
        }
        
        static void to_base(const MacroNutrient & p, values & v, indicator & ind)
        {
            char uuid_str[37];
            uuid_unparse(p.uuid,uuid_str);
            v.set("uuid", std::string(uuid_str));
            v.set("name", p.name);
            v.set("description", p.description);
            v.set("createdtime", p.created_datetime); //, p.created_datetime == std::nullptr ? i_null : i_ok);
            v.set("drv", p.drv);
            ind = i_ok;
        }
    };
}
#endif
