#include <iostream>
#include <string>
#include <boost/dll.hpp>
#include <msgpack.hpp>
#include "../common/msg_codec.h"
#include "../common/error_code.h"
#include "../plugin/dummy_plugin.cc"

struct plugin_resolver{
  plugin_resolver(const std::string& dll_path) : lib_(dll_path){
    call_in_so_ = boost::dll::import_alias<std::string(const char*, size_t)>(lib_, "call_in_so");
    get_key_in_so_ = boost::dll::import_alias<std::string(void*)>(lib_, "get_key");
  }

  template<typename R>
  R call(const msgpack::sbuffer& buf){
    auto str = call(buf);
    //TODO handle error code and exception
    purecpp::msg_codec codec;
    return codec.result<R>(str.data(), str.size()); //transforma failed will throw exception
  }

  template<typename F>
  std::string get_key(const F& func){
    void* ptr = (void*)&func;
    return get_key_in_so_(ptr);
  }

  std::string call(const msgpack::sbuffer& buf){
    return call(buf.data(), buf.size());
  }

  std::string call(const char* buf, size_t size){
    using namespace purecpp;

    std::string result;
    try {
      result = call_in_so_(buf, size);
    }catch(std::exception& e){
      msg_codec codec;
      result = codec.pack_args_str(purecpp::error_code::FAIL, e.what());
    }

    return result;
  }

private:
  boost::dll::shared_library lib_;
  std::function<std::string(const char*, size_t)> call_in_so_;
  std::function<std::string(void*)> get_key_in_so_;
};

template<typename... Args>
msgpack::sbuffer mock_client_request_buffer(std::string key, Args... args){
  return purecpp::msg_codec::pack_args(std::move(key), std::move(args)...);
}



template<typename T, typename F, typename... Args>
msgpack::sbuffer mock_client_request_buffer1(T& presolver, const F& f, Args... args){
  std::string key = presolver.get_key(f);
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

int multiply(int a, int b);

void test_dummy_dll(){
  plugin_resolver resolver("./libdummy.dylib");

  auto buf1 = mock_client_request_buffer1(resolver, purecpp::multiply, 2, 3);
  int r1 = resolver.call<int>(buf1);
  auto buf2 = mock_client_request_buffer1(resolver, purecpp::substract, 5, 2);
  int r2 = resolver.call<int>(buf2);

//  auto s = server.get_key(purecpp::multiply);
//  auto s1 = server.get_key(purecpp::substract);

  auto multiply_buf = mock_client_request_buffer("multiply", 2, 3);
  auto substract_buf = mock_client_request_buffer("substract", 5, 2);

  auto multiply_ret = resolver.call<int>(multiply_buf);

  int sub_ret = resolver.call<int>(substract_buf);

  auto echo_buf = mock_client_request_buffer("dummy_t::echo", "hello purecpp");
  std::string echo_str = resolver.call<std::string>(echo_buf);

  auto add_buf = mock_client_request_buffer("dummy_t::add", 2, 3);
  int add_ret = resolver.call<int>(add_buf);

  std::cout<<sub_ret<<" "<<multiply_ret<<" "<<echo_str<<" "<<add_ret<<'\n';
}

int main(){
  test_dummy_dll();
}
