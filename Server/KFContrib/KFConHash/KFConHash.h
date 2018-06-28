#ifndef __KF_CONHASH_H__
#define __KF_CONHASH_H__

#include "KFMap.h"
#include "KFHashData.hpp"
#include "KFHashFunction.hpp"

namespace KFrame
{
	/////////////////////////////////////////////////////////////////////////////////////
	/////////////////////////////////////////////////////////////////////////////////////
	// 一致性hash算法
	__ST_CLASS__( KFConHash )
	{
	public:
		KFConHash();
		~KFConHash();
		
		// 添加Hash节点
		// 添加Hash节点
		void AddHashNode( const std::string& name, uint32 id, uint32 virtualcount = 100 );

		// 删除Hash节点
		bool RemoveHashNode( uint32 id );

		// 查抄Hash节点
		uint32 FindHashNode( uint64 objectid, bool cache = false );
		uint32 FindHashNode( const std::string& data, bool cache = false );
		uint32 FindHashNode( const std::string& data, uint64 objectid, bool cache = false );

		// 获得所有节点
		void GetAllHashNode( std::list< uint32 >& nodes );

		// 是否存在节点
		bool HaveHashNode( uint32 id );

		// 获得节点数量
		uint32 GetHashNodeCount();

	protected:
		// 添加虚拟节点
		void AddVirtualNode( KFHashNode* node, uint32 virtualcount );

		// 查找已经选择的节点
		uint32 FindSelectNode( const std::string& data );

	private:
		// Hash 生产函数
		MD5HashFunction _hash_function;

		// 节点列表
		KFMap< uint32, uint32, KFHashNode > _node_list;

		// 虚拟节点列表
		KFMap< uint32, uint32, KFVirtualNode > _virtual_list;

		// 已经选择的节点
		std::map< std::string, uint32 > _select_list;
	};
}

#endif
