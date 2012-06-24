#ifndef _USER
#define _USER
#include "../models.h"

class User
{
public:
    std::string id;
    std::string firstname;
    std::string lastname;
    std::string password;
    std::tm created_datetime;

    User(){
    }

User(std::string id, std::string firstname, std::string lastname, std::string password, std::tm created_datetime)
    :id(id), firstname(firstname), lastname(lastname), password(password), created_datetime(created_datetime)
    {
    }
    ~User(){
    }
    friend std::ostream& operator<<(std::ostream& ostr, const User& user){
      ostr << "id:" << user.id << std::endl;
      ostr << "firstname:" << user.firstname << std::endl;
      ostr << "lastname:" << user.lastname << std::endl;
      ostr << "password:" << user.password << std::endl;
      ostr << "created_datetime:" << asctime(&user.created_datetime) << std::endl;
    }
    static int create_table(soci::session &sql){
        try{
            sql << "create table blog_users (id varchar(10) NOT NULL PRIMARY KEY, firstname varchar(100), lastname varchar(100), password varchar(8), createdtime timestamp);";
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 1;
    }
    static int get_all(soci::session &sql, std::list<User> &users){
        try{
            std::cout << "We have " << get_row_count("blog_users", sql) << " entries in the blog.\n";
            soci::rowset<User> rs = (sql.prepare << "select * from blog_users");
            for(auto it:rs)
                users.push_back(it);
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 0;
    }

    static int create(soci::session &sql, User& p){
        try{
            int count;
            sql << "insert into blog_users(id, firstname, lastname, password, createdtime) values(:id, :firstname, :lastname, :password, :createdtime)", soci::use(p);
            log_DEBUG("We have inserted user");
            return 1;
        }catch (std::exception const &e){
            std::string what = e.what();
            std::cerr << "Error: " << what << " (" << typeid(e).name() << ")\n";
            if(what.find("unique") != std::string::npos)
                return 2;
        }catch(...){
            log_ERROR("db error");
        }
        return 0;
    }
};

namespace soci
{
    template<> struct type_conversion<User>
    {
        typedef values base_type;
        static void from_base(values const & v, indicator /* ind */, User & p)
        {
            p.id = v.get<std::string>("id");
            p.firstname = v.get<std::string>("firstname");
            p.lastname = v.get<std::string>("lastname");
            p.password = v.get<std::string>("password");
            p.created_datetime = v.get<std::tm>("createdtime");

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
        
        static void to_base(const User & p, values & v, indicator & ind)
        {
            v.set("id", p.id);
            v.set("firstname", p.firstname);
            v.set("lastname", p.lastname);
            v.set("password", p.password);
            v.set("createdtime", p.created_datetime); //, p.created_datetime == std::nullptr ? i_null : i_ok);
            ind = i_ok;
        }
    };
}
#endif
