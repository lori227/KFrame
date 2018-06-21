#ifndef __KF_TEST_MODULE_H__
#define __KF_TEST_MODULE_H__

/************************************************************************
//    @Moudle			:    测试模块
//    @Author           :    __凌_痕__
//    @QQ				:    7969936
//    @Mail			    :    lori227@qq.com
//    @Date             :    2017-4-13
************************************************************************/

#include "KFrame.h"
#include "KFTestInterface.h"

namespace KFrame
{
	class KFTestModule : public KFTestInterface
	{
	public:
		KFTestModule();
		~KFTestModule();
		
		// 加载配置
		virtual void InitModule();

		// 逻辑
		virtual void BeforeRun();
		////////////////////////////////////////////////////////////////////////////////
		////////////////////////////////////////////////////////////////////////////////

	protected:
		// 处理测试消息
		void HandleProtoMessage( uint64 id, const char* data, uint32 length );
		void HandleBufferMessage( uint64 id, const char* data, uint32 length );

		// 发送测试消息
		void ProcessClientSendTestMessage( uint64 serverid );
		void ProcessServerSendTestMessage( uint64 id, uint32 type, const std::string& ip, uint32 port );

	};
}



#endif