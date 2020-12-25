#include <iostream>
#include <string>
#include <boost/dll.hpp>
#include <msgpack.hpp>
#include "../common/msg_codec.h"
#include "../common/error_code.h"

template<typename... Args>
msgpack::sbuffer mock_client_request_buffer(std::string key, Args... args){
  return purecpp::msg_codec::pack_args(std::move(key), std::move(args)...);
}

void test_custom_dll(){
  plugin_resolver resolver("./libcustom.dylib");

  auto hello_buf = mock_client_request_buffer("hello");
  auto plus_buf = mock_client_request_buffer("plus", 2, 3);

  std::string str = resolver.call<std::string>(hello_buf);
  int r = resolver.call<int>(plus_buf);

  auto increment_buf = mock_client_request_buffer("increment", 42);
  int a = resolver.call<int>(increment_buf);
  std::cout<<str<<" "<<a<<" "<<r<<'\n';
}

void test_dummy_dll(){
  plugin_resolver server("./libdummy.dylib");

  auto multiply_buf = mock_client_request_buffer("multiply", 2, 3);
  auto substract_buf = mock_client_request_buffer("substract", 5, 2);

  auto multiply_ret = server.call<int>(multiply_buf);

  int sub_ret = server.call<int>(substract_buf);

  auto echo_buf = mock_client_request_buffer("dummy_t::echo", "hello purecpp");
  std::string echo_str = server.call<std::string>(echo_buf);

  auto add_buf = mock_client_request_buffer("dummy_t::add", 2, 3);
  int add_ret = server.call<int>(add_buf);

  std::cout<<sub_ret<<" "<<multiply_ret<<" "<<echo_str<<" "<<add_ret<<'\n';
}

