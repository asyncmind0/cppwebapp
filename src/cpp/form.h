#ifndef _FORMS
#define _FORMS
#include <string>
#include <cstring>
#include <map>
#include <string>
#include "utils.h"
#ifdef MPFD
#include "MPFDParser-0.1.1/Parser.h"
#include "MPFDParser-0.1.1/Exception.h"
#include "MPFDParser-0.1.1/Field.h"

std::map<std::string,MPFD::Field *> getFormFields(const char* form_data, const std::string content_type){
    try {
        std::cout << "init parser" <<std::endl;
        MPFD::Parser* POSTParser = new MPFD::Parser();
        //POSTParser->SetTempDirForFileUpload("/tmp");
        std::cout << "init parser 2" <<std::endl;
        //POSTParser->SetMaxCollectedDataLength(20*1024);

        std::cout << "init parser 3" <<std::endl;
        POSTParser->SetContentType(content_type);/* Here you know the
                                                    Content-type: string. And you pass it.*/
        std::cout << "AcceptSomeData" <<std::endl;
        POSTParser->AcceptSomeData(form_data, std::strlen(form_data));
        std::map<std::string,MPFD::Field *> fields =  POSTParser->GetFieldsMap();
        std::cout << "Have " << fields.size() << " fields\n\r";
        return fields;
    } catch (MPFD::Exception e) {
        // Parsing input error
        std::cout << "Exception:"<<e.GetError() << std::endl;
        std::map<std::string,MPFD::Field *> fields;
        return fields;
    }
}

#endif

std::unordered_map<std::string, std::string> getFormFields(std::string& form_data){
    CURL *curl = curl_easy_init( );
    std::unordered_map<std::string, std::string> form_fields;
    std::vector<std::string> pairs;
    std::replace( form_data.begin(), form_data.end(), '+', ' ');
    split(form_data, '&', std::back_inserter(pairs));
    for(auto a:pairs){
        std::vector<std::string> vals;
        split(a, '=', std::back_inserter(vals));
        if(vals.size() == 2){
            char *unencoded_value =  curl_easy_unescape(curl , vals[1].c_str() , 0 , nullptr); 
            char *unencoded_key =  curl_easy_unescape(curl , vals[0].c_str() , 0 , nullptr); 
            form_fields[unencoded_key] = std::string(unencoded_value);
            curl_free(unencoded_key);
            curl_free(unencoded_value);
        }
    }
    return form_fields;
}

//    std::map<std::string,MPFD::Field *> getFormFields(const char* form_data, const std::string content_type){
//try {
//POSTParser = new MPFD::Parser();
//POSTParser->SetTempDirForFileUpload("/tmp");
//POSTParser->SetMaxCollectedDataLength(20*1024);
//
//POSTParser->SetContentType(/* Here you know the Content-type:
//                              string. And you pass it.*/);
//
//const int ReadBufferSize = 4 * 1024;
//
//char input[ReadBufferSize];
//
//do {
//// Imagine that you redirected std::cin to accept POST data.
//std::cin.read(input, ReadBufferSize);
//int read = std::cin.gcount();
//if (read) {
//POSTParser->AcceptSomeData(input, read);
//}
//
//} while (!std::cin.eof());
//
//// Now see what we have:
//std::map<std::string,MPFD::Field *> fields=p.GetFieldsMap();
//
//std::cout << "Have " << fields.size() << " fields\n\r";
//
//std::map<std::string,MPFD::Field *>::iterator it;
//for (it=fields.begin();it!=fields.end();it++) {
//if (fields[it->first]->GetType()==MPFD::Field::TextType) {
//std::cout<<"Got text field: ["<<it->first<<"], value: ["<< fields[it->first]->GetTextTypeContent() <<"]\n";
//} else {
//std::cout<<"Got file field: ["<<it->first<<"] Filename:["<<fields[it->first]->GetFileName()<<"] \n";
//}
//}
//} catch (MPFD::Exception e) {
//// Parsing input error
//FinishConnectionProcessing();
//return false;
//}
//}
#endif
