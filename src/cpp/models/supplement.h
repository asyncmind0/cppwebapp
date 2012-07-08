#ifndef _SUPPLEMENT
#define _SUPPLEMENT
#include "../models.h"
#include "../cache.h"

class Supplement
{
public:
    std::string name;
    double dosage;
    uuid_t uuid;
    std::unordered_map<std::string, double> nutrients;
    std::string description;
    std::tm created_datetime;

    Supplement(){
    }
Supplement(std::string name, std::string description):name(name),description(description){
    time_t now = time(NULL);
    created_datetime = *localtime(&now);
    uuid_generate(uuid);
    }

Supplement(std::string name, std::string description, uuid_t u,std::tm created_datetime)
    :name(name), description(description),  created_datetime(created_datetime)
    {
        uuid_copy(uuid, u);
    }
    ~Supplement(){
    }
    friend std::ostream& operator<<(std::ostream& ostr, const Supplement& supplement){
        //ostr << "uuid:" << post.uuid << std::endl;
      ostr << "name:" << supplement.name << std::endl;
      ostr << "description:" << supplement.description << std::endl;
      ostr << "created_datetime:" << asctime(&supplement.created_datetime) << std::endl;
    }
    static int create_table(soci::session &sql){
        try{
            sql << "create table nutrition_supplements (name varchar(200), description varchar(200), uuid varchar(100), createdtime timestamp, dosage numeric, nutrients text);";
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 1;
    }
    static int get_all(soci::session &sql, std::list<Supplement> &supplements){
        try{
            //std::cout << "We have " << get_row_count("blog_posts", sql) << " entries in the blog.\n";
            soci::rowset<Supplement> rs = (sql.prepare << "select * from nutrition_supplements");
            for(auto it:rs)
                supplements.push_back(it);
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 0;
    }

    static int create(soci::session &sql, Supplement& p){
        try{
            int count;
            sql << "insert into nutrition_supplements(uuid, name, description, createdtime, dosage, nutrients) values(:uuid, :name, :description, :createdtime, :dosage, :nutrients)", soci::use(p);
            std::cout << "We have inserted supplement" << std::endl;
        }catch (std::exception const &e){
            std::cerr << "Error: " << e.what() << '\n';
        }catch(...){
            log_ERROR("db error");
        }
        return 0;
    }
    static int update(soci::session &sql, Supplement& s ){
        try{
            sql << "update nutrition_supplements set name = :name , description = :description "
                <<", dosage = :dosage , nutrients = :nutrients where uuid = :uuid", soci::use(s);
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
    template<> struct type_conversion<Supplement>
    {
        typedef values base_type;
        static void from_base(values const & v, indicator /* ind */, Supplement & p)
        {
            p.name = v.get<std::string>("name");
            std::string uu = v.get<std::string>("uuid");
            uuid_parse(uu.c_str(),p.uuid); 
            p.description = v.get<std::string>("description");
            p.created_datetime = v.get<std::tm>("createdtime");
            p.dosage = v.get<double>("dosage");
            std::string str_macronutrients = v.get<std::string>("nutrients");
            json_loads(str_macronutrients,p.nutrients);
            //log_DEBUG(str_macronutrients);

            // p.gender will be set to the default value "unknown"
            // when the column is null:
            //p.created_datetime = v.get<std::tm>("createdtime", "unknown");

            // alternatively, the indicator can be tested directly:
            // if (v.indicator("GENDER") == i_null)
            // {
            //     p.gender = "unknown";
            // }
            // else
            // {
            //     p.gender = v.get<std::string>("GENDER");
            // }
        }
        
        static void to_base(const Supplement & p, values & v, indicator & ind)
        {
            char uuid_str[37];
            uuid_unparse(p.uuid,uuid_str);
            v.set("uuid", std::string(uuid_str));
            v.set("name", p.name);
            v.set("description", p.description);
            v.set("createdtime", p.created_datetime); //, p.created_datetime == std::nullptr ? i_null : i_ok);
            v.set("dosage", p.dosage);
            /*std::ostringstream s;
            s << "{";
            int size = p.nutrients.size();
            int i=0;
            for(auto nutrient : p.nutrients){
                i++;
                s << "{\"" << nutrient.first << "\",\"" << nutrient.second << "\"}";
                if(i!=size){
                    s <<",";
                }
            }
            s << "}";
            v.set("nutrients",s.str());*/
            v.set("nutrients",json_dumps<double>(p.nutrients));

            ind = i_ok;
        }
    };
}
#endif
