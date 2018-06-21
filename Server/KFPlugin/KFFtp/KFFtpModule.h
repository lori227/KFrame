#ifndef __KF_FTP_MODULE_H__
#define __KF_FTP_MODULE_H__

/************************************************************************
//    @Module			:    FTP下载模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-2-15
************************************************************************/

#include "KFrame.h"
#include "KFFtpInterface.h"

namespace KFrame
{
	class KFFtpThread;
	class KFFtpModule : public KFFtpInterface
	{
	public:
		KFFtpModule();
		~KFFtpModule();
		
		// 初始化
		virtual void InitModule();

		virtual void BeforeRun();

		virtual void ShutDown();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

	protected:
		
		// 执行
		void Run();

		// 开启下载
		virtual void StartDownload( uint32 ftpid, KFFtpFunction& function );

	private:
		
		// ftp 线程
		KFFtpThread* _kf_ftp_thread;

		// 回调函数
		KFFtpFunction _ftp_function;

	};
}



#endif