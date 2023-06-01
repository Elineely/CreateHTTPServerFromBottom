#include <iostream>

#include "./Config.hpp"
#include "./PathFinder.hpp"
#include "./Request.hpp"
#include "./utils.hpp"

using namespace std;

void print_location(t_location c)
{
  cout << "language : " << c.language << endl;
  cout << "root : " << c.root << endl;
  cout << "auto_index : " << c.auto_index << endl;
  cout << "index : " << c.index << endl;
  cout << "ourcgi_pass : " << c.ourcgi_pass << endl;
  cout << "ourcgi_index : " << c.ourcgi_index << endl;
  cout << "uploaded_path : " << c.uploaded_path << endl;
  cout << "accepted_method : " << c.accepted_method << endl;
}

void print_basics(Response& c)
{
  cout << "accepted : " << c.accepted_method << endl;
  cout << "auto : " << c.auto_index << endl;
  cout << "file_exist : " << c.file_exist << endl;
  cout << "file : " << c.file_name << endl;
  cout << "path_exist : " << c.path_exist << endl;
  cout << "path : " << c.file_path << endl;
  cout << "cgi_flag : " << c.cgi_flag << endl;
  cout << "cgi_path : " << c.cgi_bin_path << endl;
  cout << "save_path : " << c.uploaded_path << endl;
}

void fill_location(t_location& a, std::string b, std::string c, std::string d,
                   std::string e, std::string f, std::string g, std::string h,
                   std::string i)
{
  a.language = b;
  a.root = c;
  a.auto_index = d;
  a.index = e;
  a.ourcgi_pass = f;
  a.ourcgi_index = g;
  a.uploaded_path = h;
  a.accepted_method = i;
}

pair<std::string, t_location> make_p_l(std::string key, t_location& b)
{
  return make_pair<std::string, t_location>(key, b);
}

void make_server(t_server& a, t_location& b, t_location& c, t_location& d)
{
  // a.server_name = "localhost";
  // a.listen = "10005";
  // a.index = "";
  // a.root = "./data";
  // a.max_header_size = "";
  // a.max_body_size = "10M";
  fill_location(b, "", "./a", "", "a", "", "", "", "GET");
  fill_location(c, "", "", "", "", "./binary/cgi/file/location", "./cgi/d.py",
                "", "GET,POST,DELETE");
  fill_location(d, "", "./a", "on", "b", "", "", "./dd", "POST,DELETE");
  a.locations.insert(make_p_l("/", b));
  a.locations.insert(make_p_l(".py", c));
  a.locations.insert(make_p_l("/admin", d));
}

int main(void)
{
  Config a("./server.conf", "./server_content.conf");
  a.showServerConf();
  // t_server a;
  // t_location ab;
  // t_location ac;
  // t_location ad;
  // make_server(a, ab, ac, ad);
  Request b;
  Response c;
  // b.uri = "/";
  // b.uri = "/basics/index.py";
  // b.uri = "/admin";
  // b.uri = "/bdmin";
  // b.uri = "/admin/a/b/c/d";
  // b.uri = "/admin/b"; // file은 컨피그내 index b 사용
  b.uri = "/admin/b/a";  // file은 uri 내 a 사용
  cout << "start!" << endl;
  cout << "location  : '" << b.uri << "'" << endl;
  // cout << "✅ listen ✅" << endl;
  // cout << a.get_m_server_conf()[0].listen[0] << endl;

  // print_location( a.get_m_server_conf()[0].locations.begin()->second);
  PathFinder finder(b, a.get_m_server_conf()[0], c);
  // std::cout <<  c.body.size() << std::endl;
  // PathFinder finder(b, a, c);
  // std::cout << "BYE" << std::endl;

  std::cout << std::endl;
  // std::cout <<  c.response_message.size() << std::endl;
  print_basics(c);
  return (0);
  // cout << (a.get_m_server_conf())[0].locations.begin()->first << endl;
}