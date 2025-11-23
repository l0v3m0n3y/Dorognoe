#ifndef DOROGNOE_H
#define DOROGNOE_H

#include <cpprest/http_client.h>
#include <cpprest/json.h>
#include <pplx/pplx.h>
#include <iostream>
#include <string>
#include <vector>

using namespace web;
using namespace web::http;
using namespace web::http::client;

class Dorognoe {
private:
    std::string api_base = "https://api.dorognoe.ru/api";
    http_client_config client_config;
    
    http_request create_request(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_request request;
        
        if (method == "GET") {
            request.set_method(methods::GET);
        } else if (method == "POST") {
            request.set_method(methods::POST);
        } else if (method == "PUT") {
            request.set_method(methods::PUT);
        } else if (method == "DEL") {
            request.set_method(methods::DEL);
        }
        
        request.set_request_uri(utility::conversions::to_string_t(endpoint));
        
        // Set headers
        request.headers().add(U("Host"), U("api.dorognoe.ru"));
        request.headers().add(U("Content-Type"), U("application/json"));
        request.headers().add(U("User-Agent"), U("Mozilla/5.0 (X11; Linux x86_64; rv:109.0) Gecko/20100101 Firefox/115.0"));
        
        if (!data.empty() && (method == "POST" || method == "PUT")) {
            request.set_body(data);
        }
        
        return request;
    }
    
    std::string build_query_params(const std::map<std::string, std::string>& params) {
        if (params.empty()) return "";
        
        std::string query = "?";
        bool first = true;
        for (const auto& param : params) {
            if (!param.second.empty()) {
                if (!first) query += "&";
                // Кодируем значения параметров
                auto encoded_value = web::uri::encode_data_string(utility::conversions::to_string_t(param.second));
                query += param.first + "=" + utility::conversions::to_utf8string(encoded_value);
                first = false;
            }
        }
        return query;
    }
    
    pplx::task<json::value> make_api_call(const std::string& endpoint, const std::string& method, const std::string& data = "") {
        http_client client(utility::conversions::to_string_t(api_base), client_config);
        auto request = create_request(endpoint, method, data);

        return client.request(request)
            .then([](http_response response) {
                if (response.status_code() == status_codes::OK) {
                    return response.extract_json();
                } else {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("HTTP Error: ") + utility::conversions::to_string_t(std::to_string(response.status_code())));
                    error_obj[U("success")] = json::value::boolean(false);
                    return pplx::task_from_result(error_obj);
                }
            })
            .then([](pplx::task<json::value> previousTask) {
                try {
                    return previousTask.get();
                } catch (const std::exception& e) {
                    json::value error_obj;
                    error_obj[U("error")] = json::value::string(
                        U("Exception: ") + utility::conversions::to_string_t(e.what()));
                    error_obj[U("success")] = json::value::boolean(false);
                    return error_obj;
                }
            });
    }

public:
    Dorognoe(){
        client_config.set_validate_certificates(false);
    }

    pplx::task<json::value> get_program_list(){
        return make_api_call("/program","GET");
    }

    pplx::task<json::value> get_program_by_slug(const std::string& slug,bool scroll=true) {
        std::map<std::string, std::string> params;
        return make_api_call("/program/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(slug))),"GET");
    }

    pplx::task<json::value> get_news_category(){
        return make_api_call("/news-category","GET");
    }

    pplx::task<json::value> get_news_by_slug(const std::string& slug,bool scroll=true) {
        std::map<std::string, std::string> params;
        params["scroll"] = scroll;
        return make_api_call("/news/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(slug))) + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_cities_list(){
        return make_api_call("/cities","GET");
    }

    pplx::task<json::value> podcast_episodes(const std::string& podcast, int limit=0) {
        std::map<std::string, std::string> params;
        if (!podcast.empty()) params["podcast"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(podcast)));
        if (limit>0) params["limit"] = std::to_string(limit);
        return make_api_call("/podcast_episodes" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_news(int limit=0,const std::string& region="") {
        std::map<std::string, std::string> params;
        if (!region.empty()) params["region"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(region)));
        if (limit>0) params["limit"] = std::to_string(limit);
        return make_api_call("/news" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_dj_list(int limit=0,const std::string& region="") {
        std::map<std::string, std::string> params;
        if (!region.empty()) params["region"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(region)));
        if (limit>0) params["limit"] = std::to_string(limit);
        return make_api_call("/djs" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_dj_by_slug(const std::string& slug){
        return make_api_call("/djs/" + utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(slug))),"GET");
    }

    pplx::task<json::value> get_timeline_list(const std::string& weekday,int limit=0) {
        std::map<std::string, std::string> params;
        if (!weekday.empty()) params["weekday"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(weekday)));
        if (limit>0) params["limit"] = std::to_string(limit);
        return make_api_call("/timeline" + build_query_params(params),"GET");
    }

    pplx::task<json::value> get_contact(const std::string& region) {
        std::map<std::string, std::string> params;
        if (!region.empty()) params["region"] = utility::conversions::to_utf8string(web::uri::encode_data_string(utility::conversions::to_string_t(region)));
        return make_api_call("/contact" + build_query_params(params),"GET");
    }
};

#endif
