#include "basehandler.h"
#include "models/post.h"
#include "models/user.h"
#include "models/supplement.h"
#include "models/macronutrient.h"
#include "cache.h"

static const std::string templatepath = "src/html/";
static std::unordered_map<std::string,void*> handler_lib_map;
static std::list<std::string> handler_libs = {
    "libhandlers.so",
    "libnutrition.so",
    "libhome.so",
    "libdataloader.so",
    "libauth.so"
};

int reload_handler(std::list<std::string> handlerlibs,std::unordered_map<std::string, request_handler> &request_handlers){
    //http://stackoverflow.com/questions/496664/c-dynamic-shared-library-on-linux

    pantheios::uninit();
    for(std::string name:handlerlibs){
        auto found = handler_lib_map.find(name);
        if(found != handler_lib_map.end()){
            try{
                std::cout << RED << "Found previous handlers " << found->first << ":" << typeid(found->second).name() << ENDCOLOR <<std::endl;
                dlclose(found->second);
            }catch(...){
                std::cout << RED << "Failed to unload cleanly" ENDCOLOR << std::endl;
            }
        }
        std::cout << RED << "loading new handlers" ENDCOLOR << std::endl;
        void* handle = dlopen(name.c_str(), RTLD_LAZY);
        if (!handle)
        {
            fprintf(stderr, LIGHTRED "Loading handlers: %s\n" ENDCOLOR, dlerror());
            exit(1);
        }
        handler_lib_map[name] = handle;
        char *error;
        handler_initializer handler =  (handler_initializer)dlsym(handle,"init_handler");
        (*handler)(request_handlers);
    }
    pantheios::init();
}
int main(int argc, char *argv[])
{
    try
    {
        log_DEBUG("Entering main(", pantheios::args(argc, argv, pantheios::args::arg0FileOnly), ")");


        std::string sender_id = "82209006-86FF-4982-B5EA-D1E29E55D481";
        std::unordered_map<std::string, request_handler> request_handlers;
        reload_handler(handler_libs, request_handlers);

        if (argc != 3) {
            std::cerr << "Usage: " << argv[0] << " <from> <to>\n"
                "\tExample: " << argv[0] << " 'tcp://127.0.0.1:8988'"
                " 'tcp://127.0.0.1:8989'" << std::endl;
            return 1;
        }

        if(curl_global_init(CURL_GLOBAL_ALL)!=0){
            log_CRITICAL("Curl init failed");
            return 1;
        }

        soci::connection_pool *pool = init_soci("postgresql://dbname=cppblog");
        soci::connection_pool *pool_sqlite = init_soci("sqlite3://dbname=cppblog");
        log_INFORMATIONAL("== starting db connection pool ==");
        if(pool == nullptr){
            log_CRITICAL("== starting db connection pool failed ==");
            return 1;
        }
        if(pool_sqlite == nullptr){
            log_CRITICAL("== starting sqlite_db connection pool failed ==");
            return 1;
        }

        soci::session sql(*pool);
        Post::create_table(sql);
        User::create_table(sql);
        Supplement::create_table(sql);
        MacroNutrient::create_table(sql);

        m2pp::connection conn(sender_id, argv[1], argv[2]);
        log_INFORMATIONAL("== starting server ==");
        int handled = 0;
        while (1) {
            m2pp::request req = conn.recv();
            if (req.disconnect) {
                log_INFORMATIONAL("== disconnect ==");
                continue;
            }
            /*
              log_DEBUG( "<pre>");
            log_DEBUG( "SENDER: ", req.sender );
            log_DEBUG( "IDENT: " , req.conn_id );
            log_DEBUG( "PATH: " , req.path);
            log_DEBUG( "BODY: " , req.body );
            for( auto header:req.headers){
                log_DEBUG("HEADER: ", header.first, ": ", header.second);
            }
            log_DEBUG( "</pre>");
            */
            //log_DEBUG( req.path);
            handled = 0;
            if(req.path == "/reload_libs"){
                reload_handler(handler_libs, request_handlers);
                log_WARNING("Reloaded handlers");
                conn.reply_http(req, std::string("Reloaded handlers."));
                continue;
            }
            for(auto handler: request_handlers){
                std::regex rx(handler.first);
                std::smatch result;
                if(std::regex_match(req.path,result,rx)){
                    log_DEBUG("Matched:",handler.first);
                    request_args r (req,conn,*pool,result);
                    auto result = std::async(std::launch::async,
                                             *handler.second,std::ref(r));
                    handled = 1;
                    break;
                }
            }
            if(handled==0){
                log_WARNING("No handler found:", req.path);
                conn.reply_http(req, std::string("No handler found"));
            }

        }
        log_INFORMATIONAL("Done serving:");
        return 0;
    }
    catch(std::bad_alloc&){
        log_ALERT("out of memory");
    }
    catch(std::exception& x){
        log_CRITICAL("Exception: ", x);
    }
    catch(...){
        log_EMERGENCY( "Unknown error");
    }
    return 2;
}

