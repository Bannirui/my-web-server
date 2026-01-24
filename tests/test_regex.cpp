//
// Created by dingrui on 1/24/26.
//

#include <iostream>
#include <string>
#include <regex>

int main()
{
    std::string pattern = "^/([a-zA-Z0-9]*([.].*)?)";
    std::regex  r(pattern);
    std::smatch match;

    std::string uri1 = "/index.html";
    std::regex_search(uri1, match, r);
    std::cout << uri1 << " size=" << match.size() << " " << match[1] << " " << match[2] << std::endl;

    std::string uri2 = "/";
    std::regex_search(uri2, match, r);
    std::cout << uri2 << " size=" << match.size() << " " << match[1] << " " << match[2] << std::endl;

    std::string uri3 = "/a.php";
    std::regex_search(uri3, match, r);
    std::cout << uri3 << " size=" << match.size() << " " << match[1] << " " << match[2] << std::endl;

    std::string uri4 = "";
    std::regex_search(uri4, match, r);
    std::cout << uri4 << " size=" << match.size() << " " << match[1] << " " << match[2] << std::endl;

    std::string uri5 = "a";
    std::regex_search(uri5, match, r);
    std::cout << uri5 << " size=" << match.size() << " " << match[1] << " " << match[2] << std::endl;
    return 0;
}