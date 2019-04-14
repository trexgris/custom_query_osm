#pragma once
#include <memory>
#include <string>
namespace osm {
namespace query {
class Query final {
public:
  static constexpr const char * const INTERPRETER_OVERPASS_DATA_URL = "https://www.overpass-api.de/api/interpreter?data=";  
  static std::shared_ptr<Query> Make(std::string out, std::string ql_query_);
  Query(std::string out_file, std::string ql_query_);
  ~Query();
  void Send();
  std::string Encode() const;
private:
  Query() = delete;
  Query(const Query&) = delete;
  Query(Query&&) = delete;
  Query& operator=(const Query&) = delete;
  Query& operator=(Query&&) = delete;

private: 
    struct Impl;
    std::unique_ptr<Impl> pimpl_;        
};    
}
}