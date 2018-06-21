#ifndef __KF_RAND_MODULE_H__
#define __KF_RAND_MODULE_H__

#include "KFInclude.h"

namespace KFrame
{
	class KFRandModule
	{
	public:
		KFRandModule();
		virtual ~KFRandModule();

		// 随机整数32
		virtual uint32 Rand32();

		// 随机浮点数
		double RandFloat();
		
	protected:

		// 初始化
		void Init( uint32 seed );

		// 设置随机数
		void SetRand();
		
	private:
		uint32		_seed[ 256 ], _result[ 256 ], _aa, _bb;
		uint32		_result_count;
	};
}



#endif