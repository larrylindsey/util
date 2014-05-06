/**
 * httpclient.cpp adapted from restclient.cpp, written by David Schauenberg
 * https://github.com/mrtazz/restclient-cpp/blob/0df677338a22632b8ebd727f215e86fa233ec9ca/source/restclient.cpp
 */

/*========================
         INCLUDES
  ========================*/
#include "httpclient.h"

#include <cstring>
#include <string>
#include <iostream>
#include <map>
#include <boost/property_tree/json_parser.hpp>
#include <boost/make_shared.hpp>

/** initialize user agent string */
const char* HttpClient::user_agent = "sopnet/0.10";
/** initialize authentication variable */
std::string HttpClient::user_pass =  std::string();
/** Authentication Methods implementation */
void HttpClient::clearAuth(){
  HttpClient::user_pass.clear();
}
void HttpClient::setAuth(const std::string& user,const std::string& password){
  HttpClient::user_pass.clear();
  HttpClient::user_pass += user+":"+password;
}
/**
 * @brief HTTP GET method
 *
 * @param url to query
 *
 * @return response struct
 */
HttpClient::response HttpClient::get(const std::string& url)
{
  /** create return struct */
  HttpClient::response ret;

  // use libcurl
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl)
  {
    /** set basic authentication if present*/
    if(HttpClient::user_pass.length()>0){
      curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
      curl_easy_setopt(curl, CURLOPT_USERPWD, HttpClient::user_pass.c_str());
    }
    /** set user agent */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, HttpClient::user_agent);
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
    /** set the header callback function */
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HttpClient::header_callback);
    /** callback object for headers */
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ret);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}
/**
 * @brief HTTP POST method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP POST body
 *
 * @return response struct
 */
HttpClient::response HttpClient::post(const std::string& url,
                                      const std::string& ctype,
                                      const std::string& data)
{
  /** create return struct */
  HttpClient::response ret;
  /** build content-type header string */
  std::string ctype_header = "Content-Type: " + ctype;

  // use libcurl
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl)
  {
    /** set basic authentication if present*/
    if(HttpClient::user_pass.length()>0){
      curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
      curl_easy_setopt(curl, CURLOPT_USERPWD, HttpClient::user_pass.c_str());
    }
    /** set user agent */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, HttpClient::user_agent);
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** Now specify we want to POST data */
    curl_easy_setopt(curl, CURLOPT_POST, 1L);
    /** set post fields */
    curl_easy_setopt(curl, CURLOPT_POSTFIELDS, data.c_str());
    curl_easy_setopt(curl, CURLOPT_POSTFIELDSIZE, data.size());
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
    /** set the header callback function */
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HttpClient::header_callback);
    /** callback object for headers */
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ret);
    /** set content-type header */
    curl_slist* header = NULL;
    header = curl_slist_append(header, ctype_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}
/**
 * @brief HTTP PUT method
 *
 * @param url to query
 * @param ctype content type as string
 * @param data HTTP PUT body
 *
 * @return response struct
 */
HttpClient::response HttpClient::put(const std::string& url,
                                     const std::string& ctype,
                                     const std::string& data)
{
  /** create return struct */
  HttpClient::response ret;
  /** build content-type header string */
  std::string ctype_header = "Content-Type: " + ctype;

  /** initialize upload object */
  HttpClient::upload_object up_obj;
  up_obj.data = data.c_str();
  up_obj.length = data.size();

  // use libcurl
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl)
  {
    /** set basic authentication if present*/
    if(HttpClient::user_pass.length()>0){
      curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
      curl_easy_setopt(curl, CURLOPT_USERPWD, HttpClient::user_pass.c_str());
    }
    /** set user agent */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, HttpClient::user_agent);
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** Now specify we want to PUT data */
    curl_easy_setopt(curl, CURLOPT_PUT, 1L);
    curl_easy_setopt(curl, CURLOPT_UPLOAD, 1L);
    /** set read callback function */
    curl_easy_setopt(curl, CURLOPT_READFUNCTION, HttpClient::read_callback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_READDATA, &up_obj);
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
    /** set the header callback function */
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HttpClient::header_callback);
    /** callback object for headers */
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ret);
    /** set data size */
    curl_easy_setopt(curl, CURLOPT_INFILESIZE,
                     static_cast<long>(up_obj.length));

    /** set content-type header */
    curl_slist* header = NULL;
    header = curl_slist_append(header, ctype_header.c_str());
    curl_easy_setopt(curl, CURLOPT_HTTPHEADER, header);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}
/**
 * @brief HTTP DELETE method
 *
 * @param url to query
 *
 * @return response struct
 */
HttpClient::response HttpClient::del(const std::string& url)
{
  /** create return struct */
  HttpClient::response ret;

  /** we want HTTP DELETE */
  const char* http_delete = "DELETE";

  // use libcurl
  CURL *curl;
  CURLcode res;

  curl = curl_easy_init();
  if (curl)
  {
    /** set basic authentication if present*/
    if(HttpClient::user_pass.length()>0){
      curl_easy_setopt(curl, CURLOPT_HTTPAUTH, CURLAUTH_BASIC);
      curl_easy_setopt(curl, CURLOPT_USERPWD, HttpClient::user_pass.c_str());
    }
    /** set user agent */
    curl_easy_setopt(curl, CURLOPT_USERAGENT, HttpClient::user_agent);
    /** set query URL */
    curl_easy_setopt(curl, CURLOPT_URL, url.c_str());
    /** set HTTP DELETE METHOD */
    curl_easy_setopt(curl, CURLOPT_CUSTOMREQUEST, http_delete);
    /** set callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEFUNCTION, HttpClient::write_callback);
    /** set data object to pass to callback function */
    curl_easy_setopt(curl, CURLOPT_WRITEDATA, &ret);
    /** set the header callback function */
    curl_easy_setopt(curl, CURLOPT_HEADERFUNCTION, HttpClient::header_callback);
    /** callback object for headers */
    curl_easy_setopt(curl, CURLOPT_HEADERDATA, &ret);
    /** perform the actual query */
    res = curl_easy_perform(curl);
    if (res != 0)
    {
      ret.body = "Failed to query.";
      ret.code = -1;
      return ret;
    }
    long http_code = 0;
    curl_easy_getinfo(curl, CURLINFO_RESPONSE_CODE, &http_code);
    ret.code = static_cast<int>(http_code);

    curl_easy_cleanup(curl);
  }

  return ret;
}

/**
 * @brief write callback function for libcurl
 *
 * @param data returned data of size (size*nmemb)
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to save/work with return data
 *
 * @return (size * nmemb)
 */
size_t HttpClient::write_callback(void *data, size_t size, size_t nmemb,
                            void *userdata)
{
  HttpClient::response* r;
  r = reinterpret_cast<HttpClient::response*>(userdata);
  r->body.append(reinterpret_cast<char*>(data), size*nmemb);

  return (size * nmemb);
}

/**
 * @brief header callback for libcurl
 * 
 * @param data returned (header line) 
 * @param size of data
 * @param nmemb memblock
 * @param userdata pointer to user data object to save headr data
 * @return size * nmemb;
 */
size_t HttpClient::header_callback(void *data, size_t size, size_t nmemb,
                            void *userdata)
{
  HttpClient::response* r;
  r = reinterpret_cast<HttpClient::response*>(userdata);
  std::string header(reinterpret_cast<char*>(data), size*nmemb);
  size_t seperator = header.find_first_of(":");
  if ( std::string::npos == seperator ) { 
    //roll with non seperated headers... 
    trim(header); 
    if ( 0 == header.length() ){ 
	return (size * nmemb); //blank line;
    } 
    r->headers[header] = "present";
  } else {
    std::string key = header.substr(0, seperator);
    trim(key);
    std::string value = header.substr(seperator + 1);
    trim (value);
    r->headers[key] = value;
  }

  return (size * nmemb);
}

/**
 * @brief read callback function for libcurl
 *
 * @param pointer of max size (size*nmemb) to write data to
 * @param size size parameter
 * @param nmemb memblock parameter
 * @param userdata pointer to user data to read data from
 *
 * @return (size * nmemb)
 */
size_t HttpClient::read_callback(void *data, size_t size, size_t nmemb,
                            void *userdata)
{
  /** get upload struct */
  HttpClient::upload_object* u;
  u = reinterpret_cast<HttpClient::upload_object*>(userdata);
  /** set correct sizes */
  size_t curl_size = size * nmemb;
  size_t copy_size = (u->length < curl_size) ? u->length : curl_size;
  /** copy data to buffer */
  memcpy(data, u->data, copy_size);
  /** decrement length and increment data pointer */
  u->length -= copy_size;
  u->data += copy_size;
  /** return copied size */
  return copy_size;
}

boost::shared_ptr<boost::property_tree::ptree>
HttpClient::jsonPtree(const std::string& json)
{
	boost::shared_ptr<boost::property_tree::ptree> pt
		= boost::make_shared<boost::property_tree::ptree>();
	std::stringstream ss;
	ss << json;
	boost::property_tree::read_json(ss, *pt);
	return pt;
}
