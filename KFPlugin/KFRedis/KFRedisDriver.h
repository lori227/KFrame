#ifndef __KF_REDIS_DRIVER_H__
#define __KF_REDIS_DRIVER_H__

#include "KFrame.h"
#include "KFRedisFormat.h"

namespace KFrame
{
    class KFRedisDriver
    {
    public:
        virtual ~KFRedisDriver() = default;
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 数据库
        template<typename T>
        inline KFResult<uint64>::UniqueType Del( const T& key )
        {
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "del {}", str_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<voidptr>::UniqueType Rename( const T& old_key, const T& new_key )
        {
            auto sql = __FORMAT__( "rename {}", old_key, new_key );
            return WriteVoid( sql );
        }

        template<typename T>
        inline KFResult<voidptr>::UniqueType RenameNx( const T& old_key, const T& new_key )
        {
            auto sql = __FORMAT__( "renamenx {}", old_key, new_key );
            return WriteVoid( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType Exists( const T& key )
        {
            auto sql = __FORMAT__( "exists {}", key );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<std::string>::UniqueType Type( const T& key )
        {
            auto sql = __FORMAT__( "type {}", key );
            return ReadString( sql );
        }

        inline KFResult<voidptr>::UniqueType Select( uint64 index )
        {
            auto sql = __FORMAT__( "select {}", index );
            return WriteVoid( sql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 字段(key, ttl)
        inline KFResult<voidptr>::UniqueType Expire( const std::string& key, uint32 seconds )
        {
            auto sql = __FORMAT__( "expire {} {}", key, seconds );
            return WriteVoid( sql );
        }

        inline KFResult<voidptr>::UniqueType ExpireAt( const std::string& key, uint64 time )
        {
            auto sql = __FORMAT__( "expireat {} {}", key, time );
            return WriteVoid( sql );
        }

        // -1 key not exists or notexpire time
        inline KFResult<uint64>::UniqueType Ttl( const std::string& key )
        {
            auto sql = __FORMAT__( "ttl {}", key );
            return ReadUInt64( sql );
        }

        // remove expire time
        inline KFResult<uint64>::UniqueType Persist( const std::string& key )
        {
            auto sql = __FORMAT__( "persist {}", key );
            return WriteUInt64( sql );
        }

        inline KFResult<voidptr>::UniqueType PExpire( const std::string& key, uint32 milliseconds )
        {
            auto sql = __FORMAT__( "pexpire {} {}", key, milliseconds );
            return WriteVoid( sql );
        }

        inline KFResult<voidptr>::UniqueType PExpireAt( const std::string& key, uint64 milli_time )
        {
            auto sql = __FORMAT__( "pexpireat {} {}", key, milli_time );
            return WriteVoid( sql );
        }

        // -1 key not exists or notexpire time
        inline KFResult<uint64>::UniqueType  PTtl( const std::string& key )
        {
            auto sql = __FORMAT__( "pttl {}", key );
            return ReadUInt64( sql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 字符串(string)
        // set
        template<typename T>
        inline KFResult<voidptr>::UniqueType Set( const std::string& key, const T& value )
        {
            auto sql = __FORMAT__( "set {} {}", key, value );
            return WriteVoid( sql );
        }

        // set if not exists
        template<typename T>
        inline KFResult<voidptr>::UniqueType SetNX( const std::string& key, const T& value )
        {
            auto sql = __FORMAT__( "setnx {} {}", key, value );
            return WriteVoid( sql );
        }

        // set and ttl( second )
        template<typename T>
        inline KFResult<voidptr>::UniqueType SetEX( const std::string& key, const T& value, uint32 seconds )
        {
            auto sql = __FORMAT__( "setex {} {} {}", key, seconds, value );
            return WriteVoid( sql );
        }

        // set and ttl( millisecond )
        template<typename T>
        inline KFResult<voidptr>::UniqueType PSetEX( const std::string& key, const T& value, uint32 milliseconds )
        {
            auto sql = __FORMAT__( "psetex {} {} {}", key, milliseconds, value );
            return WriteVoid( sql );
        }

        // get
        inline KFResult<std::string>::UniqueType Get( const std::string& key )
        {
            auto sql = __FORMAT__( "get {}", key );
            return ReadString( sql );
        }

        inline KFResult<uint64>::UniqueType GetUInt64( const std::string& key )
        {
            auto sql = __FORMAT__( "get {}", key );
            return ReadUInt64( sql );
        }

        // get
        template<typename T>
        inline KFResult<std::string>::UniqueType GetSet( const std::string& key, T& value )
        {
            auto sql = __FORMAT__( "getset {} {}", key, value );
            return WriteString( sql );
        }

        inline KFResult<uint64>::UniqueType GetSetUInt64( const std::string& key, uint64 value )
        {
            auto sql = __FORMAT__( "getset {} {}", key, value );
            return WriteUInt64( sql );
        }

        // value length
        inline KFResult<uint64>::UniqueType StrLen( const std::string& key )
        {
            auto sql = __FORMAT__( "strlen {}", key );
            return ReadUInt64( sql );
        }

        // append value
        template<typename T>
        inline KFResult<voidptr>::UniqueType Append( const std::string& key, const T& value )
        {
            auto sql = __FORMAT__( "append {} {}", key, value );
            return WriteVoid( sql );
        }

        // set range  (return new length, 0=false)
        template<typename T>
        inline KFResult<voidptr>::UniqueType SetRange( const std::string& key, uint32 offset, const T& value )
        {
            auto sql = __FORMAT__( "setrange {} {} {}", key, offset, value );
            return WriteUInt64( sql );
        }

        // get range
        inline KFResult<std::string>::UniqueType GetRange( const std::string& key, uint32 start_offset, uint32 end_offset )
        {
            auto sql = __FORMAT__( "getrange {} {} {}", key, start_offset, end_offset );
            return ReadString( sql );
        }

        // incr
        inline KFResult<uint64>::UniqueType Incr( const std::string& key, int64 value = 1 )
        {
            auto sql = __FORMAT__( "incrby {} {}", key, value );
            return WriteUInt64( sql );
        }

        // decr
        inline KFResult<uint64>::UniqueType Decr( const std::string& key, int64 value = 1 )
        {
            auto sql = __FORMAT__( "decrby {} {}", key, value );
            return WriteUInt64( sql );
        }

        // mset
        inline KFResult<voidptr>::UniqueType MSet( const StringMap& values )
        {
            auto str_value = KFRedisFormat::ToString( values );
            auto sql = __FORMAT__( "mset {}", str_value );
            return WriteVoid( sql );
        }

        // msetnx
        inline KFResult<uint64>::UniqueType MSetNX( const StringMap& values )
        {
            auto str_value = KFRedisFormat::ToString( values );
            auto sql = __FORMAT__( "mset {}", str_value );
            return WriteUInt64( sql );
        }

        // mget
        inline StringMap MGet( const StringVector& keys )
        {
            StringMap values;
            if ( !keys.empty() )
            {
                auto str_value = KFRedisFormat::ToString( keys );
                auto sql = __FORMAT__( "mget {}", str_value );
                auto result = ReadVector( sql );
                for ( auto i = 0u; i < keys.size(); ++i )
                {
                    values[ keys[ i ] ] = result->_value[ i ];
                }
            }

            return values;
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 哈希表(hash)
        template<typename T, typename U>
        inline KFResult<voidptr>::UniqueType HSet( const std::string& key, const T& field, const U& value )
        {
            auto sql = __FORMAT__( "hset {} {} {}", key, field, value );
            return WriteVoid( sql );
        }

        template<typename T>
        inline KFResult<voidptr>::UniqueType HSetNX( const std::string& key, const std::string& field, const T& value )
        {
            auto sql = __FORMAT__( "hsetnx {} {} {}", key, field, value );
            return WriteVoid( sql );
        }

        template<typename T>
        inline KFResult<std::string>::UniqueType HGet( const std::string& key, const T& field )
        {
            auto sql = __FORMAT__( "hget {} {}", key, field );
            return ReadString( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType HGetUInt64( const std::string& key, const T& field )
        {
            auto sql = __FORMAT__( "hget {} {}", key, field );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType HExists( const std::string& key, const T& field )
        {
            auto sql = __FORMAT__( "hexists {} {}", key, field );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType HDel( const std::string& key, const T& fields )
        {
            auto str_value = KFRedisFormat::ToString( fields );
            auto sql = __FORMAT__( "hdel {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType HLen( const std::string& key )
        {
            auto sql = __FORMAT__( "hlen {}", key );
            return ReadUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType HStrLen( const std::string& key, const std::string& field )
        {
            auto sql = __FORMAT__( "hstrlen {} {}", key, field );
            return ReadUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType HIncrby( const std::string& key, const std::string& field, int64 value )
        {
            auto sql = __FORMAT__( "hincrby {} {} {}", key, field, value );
            return WriteUInt64( sql );
        }

        inline KFResult<voidptr>::UniqueType HMSet( const std::string& key, const StringMap& values )
        {
            auto str_value = KFRedisFormat::ToString( values );
            auto sql = __FORMAT__( "hmset {} {}", key, str_value );
            return WriteVoid( sql );
        }

        inline StringMap HMGet( const std::string& key, const StringVector& fields )
        {
            StringMap values;
            if ( !fields.empty() )
            {
                auto str_value = KFRedisFormat::ToString( fields );
                auto sql = __FORMAT__( "hmget {} {}", key, str_value );
                auto result = ReadVector( sql );
                for ( auto i = 0u; i < fields.size(); ++i )
                {
                    values[ fields[ i ] ] = result->_value[ i ];
                }
            }

            return values;
        }

        inline KFResult<StringVector>::UniqueType HKeys( const std::string& key )
        {
            auto sql = __FORMAT__( "hkeys {}", key );
            return ReadVector( sql );
        }

        inline KFResult<StringVector>::UniqueType  HVals( const std::string& key )
        {
            auto sql = __FORMAT__( "hvals {}", key );
            return ReadVector( sql );
        }

        inline KFResult<StringMap>::UniqueType HGetAll( const std::string& key )
        {
            auto sql = __FORMAT__( "hgetall {}", key );
            return ReadMap( sql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 列表(list)
        template<typename T>
        inline KFResult<uint64>::UniqueType LPush( const std::string& key, const T& value )
        {
            auto str_value = KFRedisFormat::ToString( value );
            auto sql = __FORMAT__( "lpush {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType LPushX( const std::string& key, const T& value )
        {
            auto str_value = KFRedisFormat::ToString( value );
            auto sql = __FORMAT__( "lpushx {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType RPush( const std::string& key, const T& value )
        {
            auto str_value = KFRedisFormat::ToString( value );
            auto sql = __FORMAT__( "rpush {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType RPushX( const std::string& key, const T& value )
        {
            auto str_value = KFRedisFormat::ToString( value );
            auto sql = __FORMAT__( "rpushx {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        inline KFResult<std::string>::UniqueType LPop( const std::string& key )
        {
            auto sql = __FORMAT__( "lpop {}", key );
            return ReadString( sql );
        }

        inline KFResult<uint64>::UniqueType LPopUInt64( const std::string& key )
        {
            auto sql = __FORMAT__( "lpop {}", key );
            return ReadUInt64( sql );
        }

        inline KFResult<std::string>::UniqueType RPop( const std::string& key )
        {
            auto sql = __FORMAT__( "rpop {}", key );
            return ReadString( sql );
        }

        inline KFResult<uint64>::UniqueType RPopUInt64( const std::string& key )
        {
            auto sql = __FORMAT__( "rpop {}", key );
            return ReadUInt64( sql );
        }

        inline KFResult<std::string>::UniqueType RPopLPush( const std::string& source_key, const std::string& dest_key )
        {
            auto sql = __FORMAT__( "rpoplpush {} {}", source_key, dest_key );
            return ReadString( sql );
        }

        inline KFResult<uint64>::UniqueType RPopLPushUInt64( const std::string& source_key, const std::string& dest_key )
        {
            auto sql = __FORMAT__( "rpoplpush {} {}", source_key, dest_key );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType LRem( const std::string& key, const T& value, int64 count )
        {
            auto sql = __FORMAT__( "lrem {} {} {}", key, count, value );
            return WriteUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType LLen( const std::string& key )
        {
            auto sql = __FORMAT__( "llen {}", key );
            return ReadUInt64( sql );
        }

        inline KFResult<std::string>::UniqueType LIndex( const std::string& key, int64 index )
        {
            auto sql = __FORMAT__( "lindex {} {}", key, index );
            return ReadString( sql );
        }

        inline KFResult<uint64>::UniqueType LIndexUInt64( const std::string& key, int64 index )
        {
            auto sql = __FORMAT__( "lindex {} {}", key, index );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType LInsertBefore( const std::string& key, const T& find_value, const T& insert_value )
        {
            auto sql = __FORMAT__( "linsert {} before {} {}", key, find_value, insert_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType LInsertAfter( const std::string& key, const T& find_value, const T& insert_value )
        {
            auto sql = __FORMAT__( "linsert {} after {} {}", key, find_value, insert_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<voidptr>::UniqueType LSet( const std::string& key, int64 index, const T& value )
        {
            auto sql = __FORMAT__( "lset {} {} {}", key, index, value );
            return WriteVoid( sql );
        }

        inline KFResult<StringList>::UniqueType LRange( const std::string& key, int64 start_index, int64 end_index )
        {
            auto sql = __FORMAT__( "lrange {} {} {}", key, start_index, end_index );
            return ReadList( sql );
        }

        inline KFResult<voidptr>::UniqueType LTrim( const std::string& key, int64 start_index, int64 end_index )
        {
            auto sql = __FORMAT__( "ltrim {} {} {}", key, start_index, end_index );
            return WriteVoid( sql );
        }

        template<typename T>
        inline KFResult<StringPair>::UniqueType BLPop( const T& key_list, uint64 timeout )
        {
            auto str_value = KFRedisFormat::ToString( key_list );
            auto sql = __FORMAT__( "blpop {} {}", str_value, timeout );
            return ReadPair( sql );
        }

        template<typename T>
        inline KFResult<StringPair>::UniqueType BRPop( const T& key_list, uint64 timeout )
        {
            auto str_value = KFRedisFormat::ToString( key_list );
            auto sql = __FORMAT__( "brpop {} {}", str_value, timeout );
            return ReadPair( sql );
        }

        inline KFResult<std::string>::UniqueType BRPopLPush( const std::string& source_key, const std::string& dest_key, uint64 timeout )
        {
            auto sql = __FORMAT__( "brpoplpush {} {} {}", source_key, dest_key, timeout );
            return ReadString( sql );
        }

        inline KFResult<uint64>::UniqueType BRPopLPushUInt64( const std::string& source_key, const std::string& dest_key, uint64 timeout )
        {
            auto sql = __FORMAT__( "brpoplpush {} {} {}", source_key, dest_key, timeout );
            return ReadUInt64( sql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 集合
        template<typename T>
        inline KFResult<uint64>::UniqueType SAdd( const std::string& key, const T& value )
        {
            auto str_value = KFRedisFormat::ToString( value );
            auto sql = __FORMAT__( "sadd {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType SIsMember( const std::string& key, const T& value )
        {
            auto sql = __FORMAT__( "sismember {} {}", key, value );
            return ReadUInt64( sql );
        }

        inline KFResult<std::string>::UniqueType SPop( const std::string& key )
        {
            auto sql = __FORMAT__( "spop {}", key );
            return WriteString( sql );
        }

        inline KFResult<uint64>::UniqueType SPopUInt64( const std::string& key )
        {
            auto sql = __FORMAT__( "spop {}", key );
            return WriteUInt64( sql );
        }

        inline KFResult<std::string>::UniqueType SRandMember( const std::string& key )
        {
            auto sql = __FORMAT__( "srandmember {}", key );
            return ReadString( sql );
        }

        inline KFResult<uint64>::UniqueType SRandMemberUInt64( const std::string& key )
        {
            auto sql = __FORMAT__( "srandmember {}", key );
            return ReadUInt64( sql );
        }

        inline KFResult<StringVector>::UniqueType SRandMember( const std::string& key, int64 count )
        {
            auto sql = __FORMAT__( "srandmember {} {}", key, count );
            return ReadVector( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType SRem( const std::string& key, const T& value )
        {
            auto str_value = KFRedisFormat::ToString( value );
            auto sql = __FORMAT__( "srem {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType SMove( const std::string& source_key, const T& value, const std::string& dest_key )
        {
            auto sql = __FORMAT__( "smove {} {} {}", source_key, dest_key, value );
            return WriteUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType SCard( const std::string& key )
        {
            auto sql = __FORMAT__( "scard {}", key );
            return ReadUInt64( sql );
        }

        inline KFResult<StringVector>::UniqueType SMembers( const std::string& key )
        {
            auto sql = __FORMAT__( "smembers {}", key );
            return ReadVector( sql );
        }

        // 交集
        template<typename T>
        inline KFResult<StringVector>::UniqueType SInter( const T& key )
        {
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "sinter {}", str_value );
            return ReadVector( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType SInterStore( const T& key, const std::string& dest_key )
        {
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "sinterstore {} {}", dest_key, str_value );
            return WriteUInt64( sql );
        }

        // 并集
        template<typename T>
        inline KFResult<StringVector>::UniqueType SUnion( const T& key )
        {
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "sunion {}", str_value );
            return ReadVector( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType SUnionStore( const T& key, const std::string& dest_key )
        {
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "sunionstore {} {}", dest_key, str_value );
            return WriteUInt64( sql );
        }

        // 差集
        template<typename T>
        inline KFResult<StringVector>::UniqueType SDiff( const T& key )
        {
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "sdiff {}", str_value );
            return ReadVector( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType SDiffStore( const T& key, const std::string& dest_key )
        {
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "sdiffstore {} {}", dest_key, str_value );
            return WriteUInt64( sql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 有序集合
        template<typename T>
        inline KFResult<uint64>::UniqueType ZAdd( const std::string& key, const T& member, uint64 source )
        {
            auto sql = __FORMAT__( "zadd {} {} {}", key, source, member );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType ZAdd( const std::string& key, const T& values )
        {
            auto str_value = KFRedisFormat::ToString( values );
            auto sql = __FORMAT__( "zadd {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType ZScore( const std::string& key, const T& member )
        {
            auto sql = __FORMAT__( "zscore {} {}", key, member );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType ZIncrby( const std::string& key, const T& member, int64 score )
        {
            auto sql = __FORMAT__( "zincrby {} {} {}", key, score, member );
            return WriteUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType ZCard( const std::string& key )
        {
            auto sql = __FORMAT__( "zcard {}", key );
            return ReadUInt64( sql );
        }

        // 返回集合中[minsorce,max_score]的成员个数
        inline KFResult<uint64>::UniqueType ZCount( const std::string& key, uint64 min_score, uint64 max_score )
        {
            auto sql = __FORMAT__( "zcount {} {}", key, min_score, max_score );
            return ReadUInt64( sql );
        }

        // 返回集合中区间的成员( 从小到大 )
        inline KFResult<StringPairList>::UniqueType ZRange( const std::string& key, uint64 start_index = 0, int64 end_index = -1 )
        {
            auto sql = __FORMAT__( "zrange {} {} {} withscores", key, start_index, end_index );
            return ReadPairList( sql );
        }

        // 返回集合中区间的成员( 从大到小 )
        inline KFResult<StringPairList>::UniqueType ZRevRange( const std::string& key, uint64 start_index = 0, int64 end_index = -1 )
        {
            auto sql = __FORMAT__( "zrevrange {} {} {} withscores", key, start_index, end_index );
            return ReadPairList( sql );
        }

        // 返回集合中score在区间的成员( 从小到大 )
        inline KFResult<StringPairList>::UniqueType ZRangeByScore( const std::string& key, uint64 min_score = 0, uint64 max_score = __MAX_UINT64__ )
        {
            auto sql = __FORMAT__( "zrangebyscore {} ({} {} withscores", key, min_score, max_score );
            return ReadPairList( sql );
        }

        // 返回集合中score在区间的成员( 从大到小 )
        inline KFResult<StringPairList>::UniqueType ZRevRangeByScore( const std::string& key, uint64 min_score = 0, uint64 max_score = __MAX_UINT64__ )
        {
            auto sql = __FORMAT__( "zrevrangebyscore {} ({} {} withscores", key, min_score, max_score );
            return ReadPairList( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType ZRank( const std::string& key, const T& member )
        {
            auto sql = __FORMAT__( "zrank {} {}", key, member );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType ZRevRank( const std::string& key, const T& member )
        {
            auto sql = __FORMAT__( "zrevrank {} {}", key, member );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType ZRem( const std::string& key, const T& member )
        {
            auto str_value = KFRedisFormat::ToString( member );
            auto sql = __FORMAT__( "zrem {} {}", key, str_value );
            return WriteUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType ZRemRangeByRank( const std::string& key, uint64 start_index, int64 end_index )
        {
            auto sql = __FORMAT__( "zremrangebyrank {} {}", key, start_index, end_index );
            return WriteUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType ZRemRangeByRank( const std::string& key, uint64 min_score, uint64 max_score )
        {
            auto sql = __FORMAT__( "zremrangebyscore {} {}", key, min_score, max_score );
            return WriteUInt64( sql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 位图
        inline KFResult<uint64>::UniqueType SetBit( const std::string& key, uint32 offset, uint32 value )
        {
            auto sql = __FORMAT__( "setbit {} {}", key, offset, value );
            return WriteUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType GetBit( const std::string& key, uint32 offset )
        {
            auto sql = __FORMAT__( "getbit {} {}", key, offset );
            return ReadUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType BitCount( const std::string& key )
        {
            auto sql = __FORMAT__( "bitcount {}", key );
            return ReadUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType BitCount( const std::string& key, int32 start_offset, int32 end_offset )
        {
            auto sql = __FORMAT__( "bitcount {} {} {}", key, start_offset, end_offset );
            return ReadUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType BitPos( const std::string& key, uint32 value )
        {
            auto sql = __FORMAT__( "bitpos {} {}", key, value );
            return ReadUInt64( sql );
        }

        inline KFResult<uint64>::UniqueType BitPos( const std::string& key, uint32 value, int32 start_offset, int32 end_offset )
        {
            auto sql = __FORMAT__( "bitpos {} {} {} {}", key, value, start_offset, end_offset );
            return ReadUInt64( sql );
        }

        template<typename T>
        inline KFResult<uint64>::UniqueType BitOp( const T& key, uint32 operate, const std::string& dest_key )
        {
            static auto _op_string = []( uint32 operate )->const std::string&
            {
                static std::map<uint32, std::string> _op_map =
                {
                    {KFEnum::And, "and"},
                    {KFEnum::Or, "or"},
                    {KFEnum::Xor, "xor"},
                    {KFEnum::Not, "not"},
                };

                auto iter = _op_map.find( operate );
                if ( iter == _op_map.end() )
                {
                    return _invalid_string;
                }

                return iter->second;
            };

            auto& op_string = _op_string( operate );
            auto str_value = KFRedisFormat::ToString( key );
            auto sql = __FORMAT__( "bitop {} {} {} {}", key, op_string, dest_key, str_value );
            return WriteUInt64( sql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 地理位置
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 写事务
        inline KFResult<voidptr>::UniqueType WriteMulti()
        {
            static std::string _str_multi = "multi";
            return WriteVoid( _str_multi );
        }

        inline KFResult<voidptr>::UniqueType WriteExec()
        {
            static std::string _str_exec = "exec";
            return WriteVoid( _str_exec );
        }

        // 读事务
        inline KFResult<voidptr>::UniqueType ReadMulti()
        {
            static std::string _str_multi = "multi";
            return WriteVoid( _str_multi );
        }

        KFResult<StringList>::UniqueType ReadListExec()
        {
            static std::string _str_exec = "exec";
            return ReadList( _str_exec );
        }

        KFResult<StringVector>::UniqueType ReadVectorExec()
        {
            static std::string _str_exec = "exec";
            return ReadVector( _str_exec );
        }

        KFResult<StringMapList>::UniqueType ReadMapListExec()
        {
            static std::string _str_exec = "exec";
            return ReadMapList( _str_exec );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // 执行数据库
        template<typename... P>
        KFResult<voidptr>::UniqueType ExecuteVoid( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return WriteVoid( sql );
        }

        template<typename... P>
        KFResult<uint64>::UniqueType ExecuteUInt64( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return WriteUInt64( sql );
        }

        template<typename... P>
        KFResult<std::string>::UniqueType ExecuteString( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return WriteVoid( sql );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        template<typename... P>
        KFResult<uint64>::UniqueType QueryUInt64( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ReadUInt64( sql );
        }

        template<typename... P>
        KFResult<std::string>::UniqueType QueryString( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ReadString( sql );
        }

        template<typename... P>
        KFResult<StringMap>::UniqueType QueryMap( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ReadMap( sql );
        }

        template<typename... P>
        KFResult<StringList>::UniqueType QueryList( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ReadList( sql );
        }

        template<typename... P>
        KFResult<StringVector>::UniqueType QueryVector( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ReadVector( sql );
        }

        template<typename... P>
        KFResult<StringMapList>::UniqueType QueryMapList( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ReadMapList( sql );
        }

        template<typename... P>
        KFResult<StringPairList>::UniqueType QueryPairList( const char* fmt, P&& ... args )
        {
            auto sql = __FORMAT__( fmt, std::forward<P>( args )... );
            return ReadPairList( sql );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 写操作
        virtual KFResult<voidptr>::UniqueType WriteVoid( const std::string& sql ) = 0;
        virtual KFResult<uint64>::UniqueType WriteUInt64( const std::string& sql ) = 0;
        virtual KFResult<std::string>::UniqueType WriteString( const std::string& sql ) = 0;

        // 读操作
        virtual KFResult<uint64>::UniqueType ReadUInt64( const std::string& sql ) = 0;
        virtual KFResult<std::string>::UniqueType ReadString( const std::string& sql ) = 0;
        virtual KFResult<StringMap>::UniqueType ReadMap( const std::string& sql ) = 0;
        virtual KFResult<StringPair>::UniqueType ReadPair( const std::string& sql ) = 0;
        virtual KFResult<StringList>::UniqueType ReadList( const std::string& sql ) = 0;
        virtual KFResult<StringVector>::UniqueType ReadVector( const std::string& sql ) = 0;
        virtual KFResult<StringMapList>::UniqueType ReadMapList( const std::string& sql ) = 0;
        virtual KFResult<StringPairList>::UniqueType ReadPairList( const std::string& sql ) = 0;
    };
}

#endif