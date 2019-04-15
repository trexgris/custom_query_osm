#include "../include/Query.hpp"
#include <iostream>

int main() {
    std::string outfile, query;
    std::cout << "Enter output file name (will be of type xml): " << std::endl;
    std::getline(std::cin, outfile);
    std::cout << "Enter Query: ";
    std::getline(std::cin, query);
    auto q = osm::query::Query::Make(std::move(outfile), std::move(query));
    q->Send(osm::query::Query::WriteToFile::YES);
return 0;
}


