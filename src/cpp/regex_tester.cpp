#include <string>
#include <iostream>
#include <regex>
using namespace std;

int main()
{
    //This should match any real number
    //regex number("((\\+|-)?[[:digit:]]+)(\\.(([[:digit:]]+)?))?((e|E)((\\+|-)?)[[:digit:]]+)?");
    //This should match any word
    //regex word("[[:alpha:]]+");
    regex url1("/nutrition/");
    regex url2("/nutrition/edit/(.+)/");
    string input,clean_words,clean_numbers, clean_url;
    //Replace with an empty string
    const string format="#";
    getline(cin,input);
    //Split the input string in numbers and words
    //clean_numbers=regex_replace(input,number,format,regex_constants::format_default);
    //clean_words=regex_replace(input,word,format,regex_constants::format_default);
    clean_url=regex_replace(input,url1,format,regex_constants::format_default);
    std::smatch result;
    if(std::regex_match(input,result,url1)){
        cout <<"Matched url1";
    }
    if(std::regex_match(input,result,url2)){
        cout <<"Matched url2";
    }
    //Print the results
    //cout<<clean_words<<endl;
    //cout<<clean_numbers<<endl;
    cout<<clean_url<<endl;
    return(0);
}
