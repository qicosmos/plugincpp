#include <iostream>
#include <string>
#include <boost/dll.hpp>
#include <msgpack.hpp>
#include "../common/msg_codec.h"
#include "../common/error_code.h"
#include <cinatra.hpp>
#include <rest_rpc.hpp>

using namespace rest_rpc;
using namespace rpc_service;
using namespace cinatra;

struct plugin_resolver{
    plugin_resolver(const std::string& dll_path) : lib_(dll_path){
      call_in_so_ = boost::dll::import_alias<std::string(const char*, size_t)>(lib_, "call_in_so");
    }

    template<typename R>
    R call(const msgpack::sbuffer& buf){
      auto str = call(buf);
      //TODO handle error code and exception
      purecpp::msg_codec codec;
      return codec.result<R>(str.data(), str.size()); //transforma failed will throw exception
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
};

std::unordered_map<std::string, std::shared_ptr<plugin_resolver>> g_plugin_map;

void start_rpc_server(){
  rpc_server server(9000, std::thread::hardware_concurrency());

  server.register_handler("plugin_service", [](rpc_conn conn, std::string plugin_name, std::string service_buf){
      auto it = g_plugin_map.find(plugin_name);
      if(it==g_plugin_map.end()){
        return std::string();
      }

      return it->second->call(service_buf.data(), service_buf.size());
  });

  server.run();
}

void start_http_server(){
  int max_thread_num = std::thread::hardware_concurrency();
  http_server server(max_thread_num);
  server.set_multipart_begin([](request& req, std::string& name){
    name = req.get_multipart_field_name("filename");
  });
  server.listen("0.0.0.0", "80");

  server.set_http_handler<POST>("/add_plugin", [](request& req, response& res) {
      auto& files = req.get_upload_files();
      for (auto& file : files) {
        std::string plugin_name = std::string(req.get_query_value("plugin_name"));
        g_plugin_map.emplace(plugin_name, std::make_unique<plugin_resolver>(file.get_file_path()));
      }

      res.set_status_and_content(status_type::ok, "add plugin successful");
  });

  server.set_http_handler<POST>("/remove_plugin", [](request& req, response& res) {
      std::string plugin_name = std::string(req.get_query_value("plugin_name"));
      auto it = g_plugin_map.find(plugin_name);
      g_plugin_map.erase(it);

      res.set_status_and_content(status_type::ok, "remove plugin successful");
  });

  server.run();
}

int main(){
  std::thread rpc_thd(start_rpc_server);
  std::thread http_thd(start_http_server);

  rpc_thd.join();
  http_thd.join();

  return 0;
}