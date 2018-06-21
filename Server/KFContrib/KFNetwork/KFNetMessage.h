#ifndef __NET_MESSAGE_H__
#define __NET_MESSAGE_H__

#include "KFGuid.h"

#pragma pack( 1 )

namespace KFrame
{
	///////////////////////////////////////////////////////////////////////////////////////////
	class KFNetHead
	{
	public:
		KFGuid _guid;		// guid
		uint32 _length;		// 消息长度
		uint16 _msgid;		// 消息类型
		uint16 _child;		// 子消息个数( 包括自己 )
	};
	
	// 消息基类
	class KFNetMessage : public KFNetHead
	{
	public:
		KFNetMessage();
		~KFNetMessage();

		static KFNetMessage* Create( uint32 length );
		void Release();

		static uint32 HeadLength() { return sizeof( KFNetHead ); }
		//////////////////////////////////////////////////////////////////////////////////////////
		//////////////////////////////////////////////////////////////////////////////////////////

		void CopyData( const int8* data, uint32 length );

		// 复制消息
		void CopyFrom( KFNetMessage* message );
		void CopyFrom( const KFGuid& guid, uint32 msgid, const int8* data, uint32 length );
		///////////////////////////////////////////////////////////////////////////////

	protected:
		// 分配内存
		void MallocData( uint32 length );
		void FreeData();

	public:
		int8* _data;		// 消息数据
	};

	/////////////////////////////////////////////////////////////////////////////
	typedef std::list< KFNetMessage* > MessageList;
}

#pragma pack()
#endif
