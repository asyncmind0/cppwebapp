#ifndef _UTILS
#define _UTILS
#include <string>
#include <json/json.h>
#include "logging.h"
template <typename OutIt> OutIt split(const std::string &text, char sep, OutIt out){
    size_t start = 0, end=0;
    while((end = text.find(sep, start)) != std::string::npos)
    {
        *out++ = text.substr(start, end - start);
        start = end + 1;
    }
    *out++ = text.substr(start);
    return out;
}
void json_loads(const std::string& jsondoc, std::unordered_map<std::string,double> &hdrs) {
	json_object * jobj = json_tokener_parse(jsondoc.c_str());

        //log_DEBUG(jsondoc);
	if (jobj && json_object_is_type(jobj, json_type_object)) {
		json_object_object_foreach(jobj, key, value) {
			if (key && value && json_object_is_type(value, json_type_string)) {
                            log_DEBUG(key);
                            hdrs[std::string(key)] = json_object_get_double(value);
			}
		}
	}

	json_object_put(jobj); // free json object
}
template <typename T>
void json_loads(const std::string& jsondoc, std::unordered_map<std::string,T> &hdrs) {
	json_object * jobj = json_tokener_parse(jsondoc.c_str());
        std::cout<< "JSON_LOADS"<<std::endl;
        //log_DEBUG(jsondoc);

	if (jobj && json_object_is_type(jobj, json_type_object)) {
		json_object_object_foreach(jobj, key, value) {
			if (key && value && json_object_is_type(value, json_type_string)) {
                            std::ostringstream s;
                            s <<json_object_get_string(value);
                            hdrs[std::string(key)] = s.str();
			}
		}
	}

	json_object_put(jobj); // free json object
}
template <typename T>
std::string json_dumps(const std::unordered_map<std::string,T> &obj){
    json_object* my_object = json_object_new_object();
    for(auto a:obj){
        std::ostringstream s;
        s<< a.second;
        json_object_object_add(my_object, a.first.c_str(), json_object_new_string((const char *)s.str().c_str()));
    }
    std::ostringstream s;
    s << json_object_to_json_string(my_object);
    return s.str();
}
#endif
