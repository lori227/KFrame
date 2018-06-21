#ifndef __KF_HTTP_CLIENT_MANAGE_H__
#define __KF_HTTP_CLIENT_MANAGE_H__

/************************************************************************



************************************************************************/
#include "KFrame.h"
#include "KFThread/KFMutex.h"

namespace KFrame
{
	class KFHttpData;
	class KFHttpClientManage
	{
	public:
		KFHttpClientManage();
		~KFHttpClientManage();

		// 初始化
		void Initialize();

		// 执行逻辑
		void RunUpdate();

		// 同步http请求
		std::string SendSTHttp( const std::string& url, const std::string& data );
		std::string SendSTHttps( const std::string& url, const std::string& data );

		// 异步http请求
		void SendMTHttp( const std::string& url, const std::string& data, KFHttpClientFunction& function );
		void SendMTHttps( const std::string& url, const std::string& data, KFHttpClientFunction& function );

		// 执行完成
		void AddFinishHttp( KFHttpData* httpdata );
	protected:
		// 处理执行完成的http请求
		void ExecuteFinishHttp();

		// 请求异步http
		void ExecuteWaitHttp();

	private:
		// 异步等待的列表
		typedef std::list< KFHttpData* > HttpDataList;
		HttpDataList _wait_http_data;

		// 处理完的列表
		KFMutex _finish_mutex;
		HttpDataList _finish_http_data;

		// 正在处理的数量
		uint32 _req_http_data_count;
	};
	
}
#endif