#include <iostream>
#include <string>
#include <boost/dll.hpp>
#include <msgpack.hpp>
#include "../common/msg_codec.h"
#include "../common/error_code.h"

struct my_server{
    my_server(const std::string& dll_path) : lib_(dll_path){
      call_in_so_ = boost::dll::import_alias<std::string(const char*, size_t)>(lib_, "call_in_so");
    }

    template<typename R>
    R call(const msgpack::sbuffer& buf){
      auto str = call(buf);
      //TODO handle error code and exception
      purecpp::msg_codec codec;
      return codec.result<R>(str.data(), str.size()); //transforma failed will throw exception
    }

private:
    std::string call(const msgpack::sbuffer& buf){
      using namespace purecpp;

      std::string result;
      try {
        result = call_in_so_(buf.data(), buf.size());
      }catch(std::exception& e){
        msg_codec codec;
        result = codec.pack_args_str(error_code::FAIL, e.what());
      }

      return result;
    }

    boost::dll::shared_library lib_;
    std::function<std::string(const char*, size_t)> call_in_so_;
};


template<typename... Args>
msgpack::sbuffer mock_client_request_buffer(std::string key, Args... args){
  return purecpp::msg_codec::pack_args(std::move(key), std::move(args)...);
}

void test_custom_dll(){
  my_server server("./libcustom.dylib");

  auto hello_buf = mock_client_request_buffer("hello");
  auto increment_buf = mock_client_request_buffer("increment", 42);
  auto plus_buf = mock_client_request_buffer("plus", 2, 3);

  std::string str = server.call<std::string>(hello_buf);
  int a = server.call<int>(increment_buf);
  int b = server.call<int>(plus_buf);
  std::cout<<str<<" "<<a<<" "<<b<<'\n';
}

void test_dummy_dll(){
  my_server server("./libdummy.dylib");

  auto multiply_buf = mock_client_request_buffer("multiply", 2, 3);
  auto substract_buf = mock_client_request_buffer("substract", 5, 2);

  auto multiply_ret = server.call<int>(multiply_buf);

  int sub_ret = server.call<int>(substract_buf);

  auto echo_buf = mock_client_request_buffer("&dummy_t::echo", "hello purecpp");
  std::string echo_str = server.call<std::string>(echo_buf);

  auto add_buf = mock_client_request_buffer("&dummy_t::add", 2, 3);
  int add_ret = server.call<int>(add_buf);

  std::cout<<sub_ret<<" "<<multiply_ret<<" "<<echo_str<<" "<<add_ret<<'\n';
}

int main(){
  test_dummy_dll();
  test_custom_dll();

  return 0;
}