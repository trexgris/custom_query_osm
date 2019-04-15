#include "../include/Query.hpp"
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
#include <iostream>
#include <fstream>

namespace osm {
namespace query {
struct Query::Impl final {
   // static constexpr const char * const OUTPUT_FILE = ".osm";
    static constexpr const char * const INTERPRETER_OVERPASS_DATA_URL = "https://www.overpass-api.de/api/interpreter?data=";
    Impl(std::string out_file, std::string ql_query);
    virtual ~Impl();
    std::string Encode() const;
    void Send(WriteToFile opt);   
private:
    inline bool is_sub_delimiter(const char c) const;
    inline bool is_gen_delimiter(const char c) const;
    inline bool is_unreserved(const char c) const;

    Impl() = delete;    
    Impl(const Impl&) = delete;
    Impl(Impl&&) = delete;
    Impl& operator=(const Impl&) = delete;
    Impl& operator=(Impl&&) = delete;
public:
    std::string out_file_;
    std::string ql_query_;
    std::string response_;   
};

std::shared_ptr<Query> Query::Make(std::string out_file, std::string ql_query_) {
    return std::make_shared<Query>(std::move(out_file),std::move(ql_query_));
}

Query::Impl::Impl(std::string out_file, std::string ql_query) : out_file_(std::move(out_file)), ql_query_(std::move(ql_query)) {
    std::string query_head = std::string(INTERPRETER_OVERPASS_DATA_URL);
    ql_query_ = query_head + ql_query_;
}

Query::Impl::~Impl() {}

Query::Query(std::string out_file, std::string ql_query_)
: pimpl_(new Impl(std::move(out_file), std::move(ql_query_))) {}

Query::~Query() {}

std::string Query::GetLastResponse() const {
    if(!pimpl_) throw std::runtime_error("Pimpl is null.");
    return pimpl_->response_;
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

void Query::Send(WriteToFile opt) {
    if(!pimpl_) throw std::runtime_error("Pimpl is null.");
    pimpl_->Send(opt);
}

void Query::Impl::Send(WriteToFile opt) {
    curlpp::Cleanup cleaner;
    curlpp::Easy request;
    std::ostringstream ostr_response;
    // overpass QL
    std::string query = Encode();

	request.setOpt(new curlpp::options::Url(query));
	request.setOpt(new curlpp::options::Verbose(true));
    request.setOpt(new curlpp::options::WriteStream(&ostr_response));
	request.perform(); //blocks
    response_ = ostr_response.str();
    if(opt == WriteToFile::YES) {
        std::ofstream output_file;
        output_file.open(out_file_, std::ios::out);
        output_file <<response_;
    }
}

}
}
