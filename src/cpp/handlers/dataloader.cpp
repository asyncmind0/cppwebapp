#ifndef _DATALOADER
#define _DATALOADER
#include "../basehandler.h"
#include "../models/post.h"
#include "../models/user.h"
#include <fstream>
#include <boost/tuple/tuple.hpp>
#include <csv_iterator.hpp>
#include  <uuid/uuid.h>

using namespace boost::tuples;
typedef boost::tuple<std::string,int,double,double,double,double,int> record;
    

void loaddata_handler(request_args &r){
    //try{
    std::ifstream in("./csv.txt");
    csv::iterator<record> it(in);
    std::ostringstream response;
    // Use the iterator in your algorithms.
    std::for_each(it, csv::iterator<record>(), [](record rec){
            log_DEBUG("ok");
            //response << "ok";
            //response << rec.get<0>();
            });
    r.conn.reply_http(r.req, response.str());
    /*}catch(...){
        log_DEBUG("Error loading data");
        r.conn.reply_http(r.req, "error loadig data");
        }*/
}

extern "C" void init_handler(std::unordered_map<std::string, request_handler> &request_handlers_map){
    request_handlers_map["/loaddata/*"] = &loaddata_handler;
}
#endif
