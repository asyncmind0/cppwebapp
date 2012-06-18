#ifndef _POSTS
#define _POSTS
#include "../models.h"
#include "../cache.h"

class Post
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
    static int create_table(){
        try{
            soci::session sql(pool);
            sql << "create table blog_posts (title varchar(200), body varchar(500), uuid varchar(100), createdtime timestamp);";
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 1;
    }
    static int get_all(std::list<Post> &posts){
        try{
            soci::session sql(pool);
            std::cout << "We have " << get_row_count("blog_posts", sql) << " entries in the blog.\n";
            soci::rowset<Post> rs = (sql.prepare << "select * from blog_posts");
            for(auto it:rs)
                posts.push_back(it);
        }catch (std::exception const &e){
            log_ERROR("db error:", e);
        }catch(...){
            log_ERROR("unknown db error");
        }
        return 0;
    }

    static int create(Post& p){
        try{
            soci::session sql(pool);
            int count;
            sql << "insert into blog_posts(uuid, title, body, createdtime) values(:uuid, :title, :body, :createdtime)", soci::use(p);
            std::cout << "We have inserted post" << std::endl;
        }catch (std::exception const &e){
            std::cerr << "Error: " << e.what() << '\n';
        }catch(...){
            log_ERROR("db error");
        }
        return 0;
        return 0;
    }
    static void cache_set(Post &post)
    {
        std::vector<char> raw_product(sizeof(Post));
        memcpy(&raw_product[0], &post, sizeof(Post));
        MyCache::singleton().set(post.uuid, raw_product);
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
            /* retrieve it from the persistent store */
        }
        return ret;
    }
};

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
#endif
