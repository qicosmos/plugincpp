#include <iostream>
#include <string>
#include <boost/dll.hpp>
#include <msgpack.hpp>
#include "../common/msgpack_codec.h"

struct my_server{
    my_server(const std::string& dll_path) : lib_(dll_path){
      call_in_so_ = boost::dll::import_alias<std::string(const char*, size_t)>(lib_, "call_in_so");
    }

    std::string call(const msgpack::sbuffer& buf){
      std::string result;
      try {
        result = call_in_so_(buf.data(), buf.size());
      }catch(std::exception& e){
        std::cout<<e.what()<<'\n';
      }

      return result;
    }

private:
    boost::dll::shared_library lib_;
    std::function<std::string(const char*, size_t)> call_in_so_;
};


template<typename... Args>
msgpack::sbuffer mock_client_request_buffer(std::string key, Args... args){
  return purecpp::msgpack_codec::pack_args(std::move(key), std::move(args)...);
}

int main(){
  my_server server("/Users/yu/code/plugincpp/cmake-build-debug/libcustom.dylib");

  auto hello_buf = mock_client_request_buffer("hello");
  auto increment_buf = mock_client_request_buffer("increment", 42);
  auto plus_buf = mock_client_request_buffer("plus", 2, 3);

  server.call(hello_buf);
  server.call(increment_buf);
  server.call(plus_buf);

  return 0;
}