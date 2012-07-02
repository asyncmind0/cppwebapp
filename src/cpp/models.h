#ifndef _MODELS
#define _MODELS
#include <iostream>
#include <time.h>
#include <soci/soci.h>
#include <soci/type-conversion.h>
#include <soci/postgresql/soci-postgresql.h>
#include <exception>
#include <list>
#include  <uuid/uuid.h>
#include "utils.h"

const size_t poolSize = 10;
static bool soci_pool_initialized = 0;


soci::connection_pool* init_soci(){
    if(soci_pool_initialized==1)return 0 ;
    soci::connection_pool *pool = new soci::connection_pool(poolSize);
    for (size_t i = 0; i != poolSize; ++i)
    {
        soci::session & sql = pool->at(i);
        sql.open("postgresql://dbname=cppblog");
    }
    soci_pool_initialized=1;
    return pool;
}

    
int get_row_count(std::string table_name, soci::session &sql){
    int count;
    sql << "select count(*) from "<< table_name, soci::into(count);
    return count;
}
int get_row_count(soci::connection_pool &pool, std::string table_name){
    soci::session sql(pool);
    return get_row_count(table_name,sql);
}

#endif
