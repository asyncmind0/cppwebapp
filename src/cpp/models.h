#include <libpq-fe.h>
#include <iostream>
#include <time.h>
#include <pantheios/pantheios.hpp>
#include <soci/soci.h>
#include <soci/type-conversion.h>
#include <soci/postgresql/soci-postgresql.h>
#include <exception>
#include <list>
#include "cjango/Variant.h"

class Post : public cjango::Variant
{
public:
    std::string title;
    std::string body;
    std::string uuid;
    std::tm created_datetime;

    Post(){
    }
Post(std::string title, std::string body, std::string uuid,std::tm created_datetime)
    :title(title), body(body), uuid(uuid), created_datetime(created_datetime)
    {
    }
    ~Post(){
    }
    friend std::ostream& operator<<(std::ostream& ostr, const Post& post){
      ostr << "uuid:" << post.uuid << std::endl;
      ostr << "title:" << post.title << std::endl;
      ostr << "body:" << post.body << std::endl;
      ostr << "created_datetime:" << asctime(&post.created_datetime) << std::endl;
    }
};
const size_t poolSize = 10;
static soci::connection_pool pool(poolSize);
static bool soci_pool_initialized = 0;

namespace soci
{
    template<> struct type_conversion<Post>
    {
        typedef values base_type;
        static void from_base(values const & v, indicator /* ind */, Post & p)
        {
            p.title = v.get<std::string>("title");
            p.uuid = v.get<std::string>("uuid");
            p.body = v.get<std::string>("body");
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
        
        static void to_base(const Post & p, values & v, indicator & ind)
        {
            v.set("uuid", p.uuid);
            v.set("title", p.title);
            v.set("body", p.body);
            v.set("createdtime", p.created_datetime); //, p.created_datetime == std::nullptr ? i_null : i_ok);
            ind = i_ok;
        }
    };
}

int init_soci(){
    if(soci_pool_initialized==1)return 0 ;
    for (size_t i = 0; i != poolSize; ++i)
    {
        soci::session & sql = pool.at(i);
        sql.open("postgresql://dbname=cppblog");
    }
    soci_pool_initialized=1;
    return 1;
}

int create_tables(){
    try{
      soci::session sql(pool);
      sql << "create table blog_posts (title varchar(200), body varchar(500), uuid varchar(100), createdtime timestamp);";
    }catch (std::exception const &e){
        pantheios::log_ERROR("db error:", e);
    }catch(...){
        pantheios::log_ERROR("unknown db error");
    }
    return 1;
}

int getPosts(std::list<Post> &posts){
    try{
        int count;
        soci::session sql(pool);
        sql << "select count(*) from blog_posts", soci::into(count);
        std::cout << "We have " << count << " entries in the blog.\n";
        soci::rowset<Post> rs = (sql.prepare << "select * from blog_posts");
        for(auto it:rs)
          {
            std::cout << it << std::endl;
            posts.push_back(it);
          }
    }catch (std::exception const &e){
        pantheios::log_ERROR("db error:", e);
    }catch(...){
        pantheios::log_ERROR("unknown db error");
    }
    return 0;
}

int createPost(Post& p){
    try{
        soci::session sql(pool);
        int count;
        sql << "insert into blog_posts(uuid, title, body, createdtime) values(:uuid, :title, :body, :createdtime)", soci::use(p);
        std::cout << "We have inserted post" << std::endl;
    }catch (std::exception const &e){
        std::cerr << "Error: " << e.what() << '\n';
    }catch(...){
        pantheios::log_ERROR("db error");
    }
    return 0;
    return 0;
}


