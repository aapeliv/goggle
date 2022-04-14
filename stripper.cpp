#include "stripper.h"
using std::string; 
using std::cout; 

// given a single document extracted from the XML
// assuming that we have everything between <page> and </page>
// we return the flattened version of the text

// title is between <title> and </title>
// text is between <text> and </text>

string find_title(const string s); 
string strip_text(const string s); // eventually modify in-place

int main()
{
    const std::string s = "<title>Anarchism</title>";
    cout << "Title: " << find_title(s) << "\n";

    const string test2 = "<text bytes=\"104699\" xml:space=\"preserve\">{{short description|Political philosophy and movement}}{{other uses}} '''Anarchism''' is a [[political philosophy]] and [[Political movement|movement]] that is sceptical of [[authority]] and rejects all involuntary, coercive forms of [[Social hierarchy|hierarchy]]. Anarchism calls for the abolition of the [[State (polity)|state]],</text>";
    cout << "Text: " << strip_text(test2) << "\n"; 

    return 0; 

}

string find_title(const string s)
{
    std::regex rgx(".*<title>(\\w+)</title>.*");
    std::smatch match;

    if (std::regex_search(s.begin(), s.end(), match, rgx))
        return match[1]; 
    
    return ""; // no title found
}

string strip_text(const string s)
{ 
    string copy; 
    
    // 1. take text between <text ...> and </text> 
    std::regex rgx(".*<text.*>(.*)</text>.*");
    std::smatch match; 

    // if (std::regex_search(s.begin(), s.end(), match, rgx))
    //     copy = match[0]; 
    // else
    //     cout << "No match\n"; 
    copy = std::regex_replace(s, rgx, "$1");

    // 2. remove everything from References onwards (if References exists)
    if (copy.find("== References ==")) 
        copy = copy.substr(0, copy.find("== References =="));
        
    // 3. strip unnecessary formatting
    std::regex rgx2("[{{].*[}}]"); // if between {{ }} delete whole thing
    copy = std::regex_replace(copy, rgx2, "");  
    
    // std::regex rgx3(R"(\[\[([^\[\]\|]+) \| ([^\[\]\|]+)\]\])");
    std::regex case1(R"(\[\[([^\[\]\|]+)\]\])");
    std::regex case2(R"(\[\[([^\[\]\|]+)\|([^\[\]\|]+)\]\])");
    copy = std::regex_replace(copy, case1, "$1");
    copy = std::regex_replace(copy, case2, "$2");

    // // between [[ ]] without a | in the middle -- just remove [[ and ]]
    // std::regex rgx4("\\[\\["); 
    // std::regex rgx5("\\]\\]");
    std::regex rgx6("'''");
    // copy = std::regex_replace(copy, rgx4, ""); 
    // copy = std::regex_replace(copy, rgx5, ""); 
    copy = std::regex_replace(copy, rgx6, ""); 

    return copy;

}