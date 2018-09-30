#include "../include/Process.h"
#include <cstdio>
#include <iostream>
#include "SimpleHttpRequest.hpp"

using namespace std;
using namespace request;

const string HOSTNAME = "www.google.com";
const string PORT = "80";
const string PATH = "/";
const string PROTOCOL = "http:";


int main(){


  try {
    string url = argc > 1
               ? argv[1]
               : PROTOCOL + "//" + HOSTNAME + ":" + PORT + PATH;

    SimpleHttpRequest request;
    request.timeout = 5000;
    request.get(url)
    .on("error", [](Error&& err){
      cerr << err.name << endl << err.message << endl;
      throw std::runtime_error(err.message);
    }).on("response", [](Response&& res){
      cout << res.str();
    }).end();
  } catch(const std::exception &e) {
    cerr << "exception catched : " << e.what() << endl ;
  }



	return 0;
}
