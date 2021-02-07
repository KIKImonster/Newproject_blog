#include"db.hpp"
#include"httplib.h"

using namespace httplib;
blog_system::TableBlog* table_blog;
blog_system::TableTag* table_tag;
//业务处理模块，这里处理函数内部是需要对数据库进行操作的，因此我们有必要在之前创建好数据库的操作句柄
void InsertBlog(const Request& req, Response& rsp)
{
	Json::Reader reader;
	Json::Value blog;
	Json::FastWriter writer;
	Json::Value errmsg;
	bool ret = reader.parse(req.body, blog);
	if (ret == false)
	{
		std::cout << "InsertBlog parse blog Json failed!" << std::endl;
		rsp.status = 400;
		errmsg["ok"] = false;
		errmsg["reason"] = "parse blog jion failed!";
		rsp.set_content(writer.write(errmsg), "application/json");
		return;
	}
	ret = table_blog->Insert(blog);
	if (ret == false)
	{
		printf("InsertBlog insert into datadbase failed!");
		rsp.status = 500;
		return;
	}
	rsp.status = 200;
	return;
}
void DeleteBlog(const Request& req, Response& rsp)
{
	int blog_id = std::stoi(req.matches[1]);//这里blog//\d+是正则表达式，入req成员match匹配
	bool ret = table_blog->Delete(blog_id);
	if(ret == false)
	{
		std::cout << "DeleteBlog delete from database failed!\n" << std::endl;
		rsp.status = 500;
		return;
	}
	rsp.status = 200;
	return;
}
void UpdatBlog(const Request& req, Response& rsp)
{
	int blog_id = std::stoi(req.matches[1]);
	Json::Value blog;
	Json::Reader reader;
	bool ret = reader.parse(req.body, blog);
	if (ret == false)
	{
		printf("blog parse json failed!");
		rep.status = 400;
		return;
	}
	blog["id"] = blog_id;
	ret = table_blog->Update(blog);
	if (ret == false)
	{
		printf("UpdateBlog update database failed!");
		rsp.status = 500;
		return;
	}
	rsp.status = 200;
	return;
}
void GetAllBlog(const Request& req, Response& rsp)
{
	Json::value blogs;
	bool ret = table_blog->GetAll(&blogs);
	if (ret == false)
	{
		printf("GetALLBlog select all from database failed!");
		rsp.status = 500;
		return;
	}
	Json::FastWriter writer;
	rsp.set_content(writer.writer(blogs), "application/json");
	rsp.status = 200;
	return;
}
void GetOneBlog(const Request& req, Response& rsp)
{
        int blog_id = std::stoi(req.matches[1]);
	Json::value blog;
        blog["id"] = blog_id;
	bool ret = table_blog->GetOne(&blogs);
	if (ret == false)
	{
		printf("GetOneBlog select one from database failed!");
		rsp.status = 500;
		return;
	}
	Json::FastWriter writer;
	rsp.set_content(writer.write(blog), "application/json");
	rsp.status = 200;
	return;
}
void InsertTag(const Request& req, Response& rsp)
{
	Json::Value tag;
	Json::Reader reader;
	bool ret = reader.parse(req.body, tag);
	if (ret == false)
	{
		printf("InsertTag parse tag json failed!\n");
		rsp.status = 400;
		return;
	}
	ret = table_tag->Insert(tag);
	if (ret == false)
	{
		printf("InsertTag insert tag into database failed!");
		rsp.status = 500;
		return;
	}
	rsp.status = 200;
	return;
}
void DeleteTag(const Request& req, Response& rsp)
{
	int tag_id = std::stoi(req.matches[1]);
	bool ret = table_tag->Delete(tag_id);
	if (ret == false)
	{
		printf("DeleteTag delete from dataase failed!");
		rsp.status = 500;
		return;
	}
	rsp.status = 200;
	return;
}
void UpdateTag(const Request& req, Response& rsp)
{
	int tag_id = std::stoi(req.matches[1]);
	Json::Value tag;
	Json::Reader reader;
	bool ret = reader.parse(rsp.body, tag);
	if (ret == false)
	{
		printf("UpdateTag parse tag failed!");
		rsp.status = 400;
		return;
	}
	tag["id"] = tag_id;
	ret = table_tag->Update(tag);//注意tag更新时需要where条件，这里需要提前获得，见上一行
	if (ret == false)
	{
		printf("UpdateTag update tag failed!");
		rsp.status = 500;
		return;
	}
	rsp.status = 200;
	return;
}
void GetAllTag(const Request& req, Response& rsp)
{
	Json::Valude tags;
	bool ret = table_tag->GetAll(&tags);
	if (ret == false)
	{
		printf("GetAllTag getall from database failed!");
		rsp.status = 500;
		return;
	}
	Json::FastWriter writer;
	rsp.set_content(writer.write(tags), "application/blog");
	rsp.status = 200;
	return;
}
void GetOneTag(const Request& req, Response& rsp)
{
	Json::Value tag;
        int tag_id = std::stoi(req.matches[1]);
        tag["id"] = tag_id;
	bool ret = table_tag->Getone(&tag);
	if (ret == false)
	{
		printf("GetOneTag gteone from databse failed!");
		rsp.status = 500;
		return;
	}
	Json::FastWriter writer;
	rsp.set_content(writer.write(tag), "application/json");
	rsp.status = 200;
	return;
}


int main()
{
	MYSQL* mysql = blog_system::MysqlInit();
	table_blog = new blog_system::TableBlog(mysql);
	table_tag = new blog_system::TableTag(mysql);
	Server server;//实例化一个Server对象，----map表（请求，处理函数的指针）
	server.set_base_dir("./www");//设置相对根目录
	//博客的增删改查
	server.Post("/blog", InsterBlog);
	//server.Delete("/blog//\d+", DeleteBlog)
	server.Delete(R"(/blog/(\d+)", DeleteBlog);//正则表达式，取出一个整形数据R(）代表去除（）内的转义，正则的（）代表取出数据；
	server.put(R"(/blog/(\d+)", UpdateBlog);

	server.Get("/blog", GetAllBlog);
	server.Get(R"(/blog/(\d+)",GetOneBlog);
	//tag的增删改查
	server.Post("/tag", InsertTag);
	server.Delete(R"(/tag/(\d+)", DeleteTag);
	server.Put(R"(/tag/(\d+)", UpdateTag);
	server.Get("/tag", GetAllTag);//获取所有博客
	server.Get(R"(/tag/(\d+)", GetOneTag);//获取一个博客

	sever.listen("0,0,0,0", 9000);

	return 0;
}


