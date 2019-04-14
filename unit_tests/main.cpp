#include "../include/Query.hpp"
#include <iostream>

int main() {
    std::string query;
    std::cout << "Enter Query: ";
    std::getline(std::cin, query);

    std::cout << "------------" << std::endl;
        std::cout << query << std::endl;

    auto q = osm::query::Query::Make(query);
    q->Send();
return 0;
}


