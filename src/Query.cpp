#include "../include/Query.hpp"
#include "../include/writer/Writer.hpp"
#include <curlpp/cURLpp.hpp>
#include <curlpp/Easy.hpp>
#include <curlpp/Options.hpp>
#include <curlpp/Exception.hpp>
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <cerrno>
#include <cctype>
#include <iomanip>
#include <sstream>
#include <string>

namespace osm {
namespace query {
struct Query::Impl final {
    static constexpr const char * const OUTPUT_FILE = "osm_response.osm";
    static constexpr const char * const INTERPRETER_OVERPASS_DATA_URL = "https://www.overpass-api.de/api/interpreter?data=";
    Impl(std::string ql_query);
    virtual ~Impl();

    inline bool is_sub_delimiter(const char c) const;
    inline bool is_gen_delimiter(const char c) const;
    inline bool is_unreserved(const char c) const;

    std::string Encode() const;
    void Send();
private:
    Impl() = delete;    
    Impl(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) = delete;
private:
    std::string ql_query_;   
};

std::shared_ptr<Query> Query::Make(std::string ql_query_) {
    return std::make_shared<Query>(std::move(ql_query_));
}

Query::Impl::Impl(std::string ql_query) : ql_query_(std::move(ql_query)) {
    std::string query_head = std::string(INTERPRETER_OVERPASS_DATA_URL);
    ql_query_ = query_head + ql_query_;
}

Query::Impl::~Impl() {}

Query::Query(std::string ql_query_)
: pimpl_(new Impl(std::move(ql_query_))) {}

Query::~Query() {}

std::string Query::Encode() const {
    if(!pimpl_) throw std::runtime_error("Pimpl is null.");
    return pimpl_->Encode();
}


 bool Query::Impl::is_sub_delimiter(const char c) const {
switch (c)
    {
        case '!':
        case '$':
        case '&':
        case '\'':
        case '(':
        case ')':
        case '*':
        case '+':
        case ',':
        case ';':
        case '=': return true;
        default: return false;
    }
 }

 bool Query::Impl::is_gen_delimiter(const char c) const {
switch (c)
    {
        case ':':
        case '/':
        case '?':
        case '[':
        case ']':
        case '@':  
        case '#': return true;
        default: return false;
    }
 }


inline bool Query::Impl::is_unreserved(const char c) const {
if(isalnum(c))
    return true;
switch (c)
    {
        case '.':
        case '-':
        case '_':  
        case '~': return true;
        default: return false;
    }
}

std::string Query::Impl::Encode() const {
    std::ostringstream escaped;
    escaped.fill('0');
    escaped << std::hex;

    for (auto && c : ql_query_) {

        // Keep alphanumeric and other accepted characters intact
        if (is_unreserved(c) || is_gen_delimiter(c) || is_sub_delimiter(c)) {
            escaped << c;
            continue;
        }
        escaped << std::uppercase;
        escaped << '%' << std::setw(2) << static_cast<int>(static_cast<unsigned char> (c));
        escaped << std::nouppercase;
    }
    return escaped.str();
}

void Query::Send() {
    if(!pimpl_) throw std::runtime_error("Pimpl is null.");
    pimpl_->Send();
}

void Query::Impl::Send() {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;
    // overpass QL
    std::string query = Encode();
    curlpp::options::WriteFunctionCurlFunction callback(writer::Callback); // c style, can improve
    FILE *file = stdout;
    if(OUTPUT_FILE != nullptr) {
        file = fopen(OUTPUT_FILE, "wb");
        if(file == nullptr) {
            throw std::runtime_error("File is null");
        }
    }
    curlpp::OptionTrait<void *, CURLOPT_WRITEDATA> data(file);        
    request.setOpt(callback);
    request.setOpt(data);
    std::cout << query << std::endl;
    
	request.setOpt(new curlpp::options::Url(query));
	request.setOpt(new curlpp::options::Verbose(true));
	request.perform();
}

}
}
