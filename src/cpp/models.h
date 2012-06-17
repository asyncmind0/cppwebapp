#ifndef _MODELS
#define _MODELS
#include <iostream>
#include <time.h>
#include <pantheios/pantheios.hpp>
#include <soci/soci.h>
#include <soci/type-conversion.h>
#include <soci/postgresql/soci-postgresql.h>
#include <exception>
#include <list>

const size_t poolSize = 10;
static soci::connection_pool pool(poolSize);
static bool soci_pool_initialized = 0;


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

    
int get_row_count(std::string table_name, soci::session &sql){
    int count;
    sql << "select count(*) from "<< table_name, soci::into(count);
    return count;
}
int get_row_count(std::string table_name){
    soci::session sql(pool);
    return get_row_count(table_name,sql);
}

#endif
