#ifndef __KF_BUFFER_H__
#define __KF_BUFFER_H__

#include "KFMap.h"
#include "KFInclude.h"

namespace KFrame
{
	enum EConstDefine
	{
		Buff_1M = 1024 * 1024,		// buff大小
		Buff_10M = 10 * Buff_1M,
		Buff_20M = 20 * Buff_1M,
		Buff_40M = 40 * Buff_1M,
		Buff_100M = 100 * Buff_1M,
		Buff_200M = 200 * Buff_1M,
	};

	///////////////////////////////////////////////////////////////////////////////////////////
	///////////////////////////////////////////////////////////////////////////////////////////
	template< class T >
	__MT_CLASS__( KFBuffer )
	{
	public:
		KFBuffer()
		{
			_length = 0;
			_buffer = nullptr;
		}

		~KFBuffer()
		{
			delete[] _buffer;
			_length = 0;
		}

	public:
		T* _buffer;
		uint32 _length;
	};

	//////////////////////////////////////////////////////////////////////////////////////////
	class KFThreadBuffer
	{
	public:
		KFThreadBuffer();
		~KFThreadBuffer();

		// int8
		int8* GetInt8( uint32 length, const char* function, uint32 line );
		
		// uint8
		uint8* GetUInt8( uint32 length, const char* function, uint32 line );

	private:
		// buff列表
		typedef std::pair< uint32, uint32 > BuffKey;
		KFMap< BuffKey, const BuffKey&, KFBuffer< int8 >, KFMutex > _kf_int8;
		KFMap< BuffKey, const BuffKey&, KFBuffer< uint8 >, KFMutex > _kf_uint8;
	};
}

#endif
