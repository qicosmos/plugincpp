#include <iostream>
#include <rest_rpc.hpp>
#include <chrono>
#include <fstream>

using namespace rest_rpc;
using namespace rest_rpc::rpc_service;

struct app_client{
    bool connect(const std::string& host, unsigned short port){
      return client_.connect(host, port);
    }

    template<typename R, typename... Args>
    R call(std::string plugin_function_name, Args&&... args) {
      size_t pos = plugin_function_name.find('/');
      auto plugin_name = plugin_function_name.substr(0, pos);
      auto func_name = plugin_function_name.substr(pos+1, plugin_function_name.length()-pos-1);
      auto req_buf = rpc_service::msgpack_codec::pack_args(func_name, std::forward<Args>(args)...);
      std::string req_str(req_buf.data(), req_buf.size());

      auto buf = client_.call<std::string>("plugin_service", plugin_name, req_str);
      return rest_rpc::as<R>(string_view(buf.data(), buf.size()));
    }

    rpc_client client_;
};

void test_plugin() {
  app_client client;
  bool r = client.connect("127.0.0.1", 9000);
  if (!r) {
    return;
  }

  {
    //call service in custom dll
    auto str = client.call<std::string>("custom_plugin/hello");
    auto result = client.call<int>("custom_plugin/plus", 123, 543);
    std::cout << str << " " << result << "\n";
  }

  {
    //call service in dummy dll
    auto result = client.call<int>("dummy_plugin/multiply", 2, 3);
    auto echo_result = client.call<std::string>("dummy_plugin/dummy_t::echo", "echo test");
    std::cout << echo_result <<", multiply result: "<<result << "\n";
  }
}

int main() {
  test_plugin();

  return 0;
}
