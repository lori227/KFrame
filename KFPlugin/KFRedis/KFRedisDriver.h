#ifndef __KF_REDIS_DRIVER_H__
#define __KF_REDIS_DRIVER_H__

#include "KFrame.h"
#include "KFRedisFormat.h"

#ifndef __REDIS_KEY_1__
    #define __REDIS_KEY_1__( key ) __FORMAT__( "{}", key )
#endif

#ifndef __REDIS_KEY_2__
    #define __REDIS_KEY_2__( key1, key2 ) __FORMAT__( "{}:{}", key1, key2 )
#endif

#ifndef __REDIS_KEY_3__
    #define __REDIS_KEY_3__( key1, key2, key3 ) __FORMAT__( "{}:{}:{}", key1, key2, key3 )
#endif

#ifndef __REDIS_KEY_4__
    #define __REDIS_KEY_4__( key1, key2, key3, key4 ) __FORMAT__( "{}:{}:{}:{}", key1, key2, key3, key4 )
#endif

namespace KFrame
{
    class KFRedisDriver
    {
    public:
        virtual ~KFRedisDriver() = default;
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 数据库
        template< typename T >
        inline KFResult<uint64>::UniqueType Del( const T& key )
        {
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "del {}", strvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<voidptr>::UniqueType Rename( const T& oldkey, const T& newkey )
        {
            auto strsql = __FORMAT__( "rename {}", oldkey, newkey );
            return WriteVoid( strsql );
        }

        template< typename T >
        inline KFResult<voidptr>::UniqueType RenameNX( const T& oldkey, const T& newkey )
        {
            auto strsql = __FORMAT__( "renamenx {}", oldkey, newkey );
            return WriteVoid( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType Exists( const T& key )
        {
            auto strsql = __FORMAT__( "exists {}", key );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<std::string>::UniqueType Type( const T& key )
        {
            auto strsql = __FORMAT__( "type {}", key );
            return ReadString( strsql );
        }

        inline KFResult<voidptr>::UniqueType Select( uint64 index )
        {
            auto strsql = __FORMAT__( "select {}", index );
            return WriteVoid( strsql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 字段(key, ttl)
        inline KFResult<voidptr>::UniqueType Expire( const std::string& key, uint32 seconds )
        {
            auto strsql = __FORMAT__( "expire {} {}", key, seconds );
            return WriteVoid( strsql );
        }

        inline KFResult<voidptr>::UniqueType ExpireAt( const std::string& key, uint64 time )
        {
            auto strsql = __FORMAT__( "expireat {} {}", key, time );
            return WriteVoid( strsql );
        }

        // -1 key not exists or notexpire time
        inline KFResult<uint64>::UniqueType Ttl( const std::string& key )
        {
            auto strsql = __FORMAT__( "ttl {}", key );
            return ReadUInt64( strsql );
        }

        // remove expire time
        inline KFResult<uint64>::UniqueType Persist( const std::string& key )
        {
            auto strsql = __FORMAT__( "persist {}", key );
            return WriteUInt64( strsql );
        }

        inline KFResult<voidptr>::UniqueType PExpire( const std::string& key, uint32 milliseconds )
        {
            auto strsql = __FORMAT__( "pexpire {} {}", key, milliseconds );
            return WriteVoid( strsql );
        }

        inline KFResult<voidptr>::UniqueType PExpireAt( const std::string& key, uint64 millitime )
        {
            auto strsql = __FORMAT__( "pexpireat {} {}", key, millitime );
            return WriteVoid( strsql );
        }

        // -1 key not exists or notexpire time
        inline KFResult<uint64>::UniqueType  PTtl( const std::string& key )
        {
            auto strsql = __FORMAT__( "pttl {}", key );
            return ReadUInt64( strsql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 字符串(string)
        // set
        template< typename T >
        inline KFResult<voidptr>::UniqueType Set( const std::string& key, const T& value )
        {
            auto strsql = __FORMAT__( "set {} {}", key, value );
            return WriteVoid( strsql );
        }

        // set if not exists
        template< typename T >
        inline KFResult<voidptr>::UniqueType SetNX( const std::string& key, const T& value )
        {
            auto strsql = __FORMAT__( "setnx {} {}", key, value );
            return WriteVoid( strsql );
        }

        // set and ttl( second )
        template< typename T >
        inline KFResult<voidptr>::UniqueType SetEX( const std::string& key, const T& value, uint32 seconds )
        {
            auto strsql = __FORMAT__( "setex {} {} {}", key, seconds, value );
            return WriteVoid( strsql );
        }

        // set and ttl( millisecond )
        template< typename T >
        inline KFResult<voidptr>::UniqueType PSetEX( const std::string& key, const T& value, uint32 milliseconds )
        {
            auto strsql = __FORMAT__( "psetex {} {} {}", key, milliseconds, value );
            return WriteVoid( strsql );
        }

        // get
        inline KFResult<std::string>::UniqueType Get( const std::string& key )
        {
            auto strsql = __FORMAT__( "get {}", key );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType GetUInt64( const std::string& key )
        {
            auto strsql = __FORMAT__( "get {}", key );
            return ReadUInt64( strsql );
        }

        // get
        template< typename T >
        inline KFResult<std::string>::UniqueType GetSet( const std::string& key, T& value )
        {
            auto strsql = __FORMAT__( "getset {} {}", key, value );
            return WriteString( strsql );
        }

        inline KFResult<uint64>::UniqueType GetSetUInt64( const std::string& key, uint64 value )
        {
            auto strsql = __FORMAT__( "getset {} {}", key, value );
            return WriteUInt64( strsql );
        }

        // value length
        inline KFResult<uint64>::UniqueType StrLen( const std::string& key )
        {
            auto strsql = __FORMAT__( "strlen {}", key );
            return ReadUInt64( strsql );
        }

        // append value
        template< typename T >
        inline KFResult<voidptr>::UniqueType Append( const std::string& key, const T& value )
        {
            auto strsql = __FORMAT__( "append {} {}", key, value );
            return WriteVoid( strsql );
        }

        // set range  (return new length, 0=false)
        template< typename T >
        inline KFResult<voidptr>::UniqueType SetRange( const std::string& key, uint32 offset, const T& value )
        {
            auto strsql = __FORMAT__( "setrange {} {} {}", key, offset, value );
            return WriteUInt64( strsql );
        }

        // get range
        inline KFResult<std::string>::UniqueType GetRange( const std::string& key, uint32 startoffset, uint32 endoffset )
        {
            auto strsql = __FORMAT__( "getrange {} {} {}", key, startoffset, endoffset );
            return ReadString( strsql );
        }

        // incr
        inline KFResult<uint64>::UniqueType Incr( const std::string& key, int64 value = 1 )
        {
            auto strsql = __FORMAT__( "incrby {} {}", key, value );
            return WriteUInt64( strsql );
        }

        // decr
        inline KFResult<uint64>::UniqueType Decr( const std::string& key, int64 value = 1 )
        {
            auto strsql = __FORMAT__( "decrby {} {}", key, value );
            return WriteUInt64( strsql );
        }

        // mset
        inline KFResult<voidptr>::UniqueType MSet( const StringMap& values )
        {
            auto strvalue = KFRedisFormat::ToString( values );
            auto strsql = __FORMAT__( "mset {}", strvalue );
            return WriteVoid( strsql );
        }

        // msetnx
        inline KFResult<uint64>::UniqueType MSetNX( const StringMap& values )
        {
            auto strvalue = KFRedisFormat::ToString( values );
            auto strsql = __FORMAT__( "mset {}", strvalue );
            return WriteUInt64( strsql );
        }

        // mget
        inline StringMap MGet( const StringVector& keys )
        {
            StringMap values;
            if ( !keys.empty() )
            {
                auto strvalue = KFRedisFormat::ToString( keys );
                auto strsql = __FORMAT__( "mget {}", strvalue );
                auto result = ReadVector( strsql );
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
        template< typename T, typename U >
        inline KFResult<voidptr>::UniqueType HSet( const std::string& key, const T& field, const U& value )
        {
            auto strsql = __FORMAT__( "hset {} {} {}", key, field, value );
            return WriteVoid( strsql );
        }

        template< typename T >
        inline KFResult<voidptr>::UniqueType HSetNX( const std::string& key, const std::string& field, const T& value )
        {
            auto strsql = __FORMAT__( "hsetnx {} {} {}", key, field, value );
            return WriteVoid( strsql );
        }

        template< typename T >
        inline KFResult<std::string>::UniqueType HGet( const std::string& key, const T& field )
        {
            auto strsql = __FORMAT__( "hget {} {}", key, field );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType HGetUInt64( const std::string& key, const std::string& field )
        {
            auto strsql = __FORMAT__( "hget {} {}", key, field );
            return ReadUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType HExists( const std::string& key, const std::string& field )
        {
            auto strsql = __FORMAT__( "hexists {} {}", key, field );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType HDel( const std::string& key, const T& fields )
        {
            auto strvalue = KFRedisFormat::ToString( fields );
            auto strsql = __FORMAT__( "hdel {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType HLen( const std::string& key )
        {
            auto strsql = __FORMAT__( "hlen {}", key );
            return ReadUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType HStrLen( const std::string& key, const std::string& field )
        {
            auto strsql = __FORMAT__( "hstrlen {} {}", key, field );
            return ReadUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType HIncrby( const std::string& key, const std::string& field, int64 value )
        {
            auto strsql = __FORMAT__( "hincrby {} {} {}", key, field, value );
            return WriteUInt64( strsql );
        }

        inline KFResult<voidptr>::UniqueType HMSet( const std::string& key, const StringMap& values )
        {
            auto strvalue = KFRedisFormat::ToString( values );
            auto strsql = __FORMAT__( "hmset {} {}", key, strvalue );
            return WriteVoid( strsql );
        }

        inline StringMap HMGet( const std::string& key, const StringVector& fields )
        {
            StringMap values;
            if ( !fields.empty() )
            {
                auto strvalue = KFRedisFormat::ToString( fields );
                auto strsql = __FORMAT__( "hmget {} {}", key, strvalue );
                auto result = ReadVector( strsql );
                for ( auto i = 0u; i < fields.size(); ++i )
                {
                    values[ fields[ i ] ] = result->_value[ i ];
                }
            }

            return values;
        }

        inline KFResult<StringVector>::UniqueType HKeys( const std::string& key )
        {
            auto strsql = __FORMAT__( "hkeys {}", key );
            return ReadVector( strsql );
        }

        inline KFResult<StringVector>::UniqueType  HVals( const std::string& key )
        {
            auto strsql = __FORMAT__( "hvals {}", key );
            return ReadVector( strsql );
        }

        inline KFResult<StringMap>::UniqueType HGetAll( const std::string& key )
        {
            auto strsql = __FORMAT__( "hmgetall {}", key );
            return ReadMap( strsql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 列表(list)
        template< typename T >
        inline KFResult<uint64>::UniqueType LPush( const std::string& key, const T& value )
        {
            auto strvalue = KFRedisFormat::ToString( value );
            auto strsql = __FORMAT__( "lpush {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType LPushX( const std::string& key, const T& value )
        {
            auto strvalue = KFRedisFormat::ToString( value );
            auto strsql = __FORMAT__( "lpushx {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType RPush( const std::string& key, const T& value )
        {
            auto strvalue = KFRedisFormat::ToString( value );
            auto strsql = __FORMAT__( "rpush {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType RPushX( const std::string& key, const T& value )
        {
            auto strvalue = KFRedisFormat::ToString( value );
            auto strsql = __FORMAT__( "rpushx {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        inline KFResult<std::string>::UniqueType LPop( const std::string& key )
        {
            auto strsql = __FORMAT__( "lpop {}", key );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType LPopUInt64( const std::string& key )
        {
            auto strsql = __FORMAT__( "lpop {}", key );
            return ReadUInt64( strsql );
        }

        inline KFResult<std::string>::UniqueType RPop( const std::string& key )
        {
            auto strsql = __FORMAT__( "rpop {}", key );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType RPopUInt64( const std::string& key )
        {
            auto strsql = __FORMAT__( "rpop {}", key );
            return ReadUInt64( strsql );
        }

        inline KFResult<std::string>::UniqueType RPopLPush( const std::string& sourcekey, const std::string& destkey )
        {
            auto strsql = __FORMAT__( "rpoplpush {} {}", sourcekey, destkey );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType RPopLPushUInt64( const std::string& sourcekey, const std::string& destkey )
        {
            auto strsql = __FORMAT__( "rpoplpush {} {}", sourcekey, destkey );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType LRem( const std::string& key, const T& value, int64 count )
        {
            auto strsql = __FORMAT__( "lrem {} {} {}", key, count, value );
            return WriteUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType LLen( const std::string& key )
        {
            auto strsql = __FORMAT__( "llen {}", key );
            return ReadUInt64( strsql );
        }

        inline KFResult<std::string>::UniqueType LIndex( const std::string& key, int64 index )
        {
            auto strsql = __FORMAT__( "lindex {} {}", key, index );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType LIndexUInt64( const std::string& key, int64 index )
        {
            auto strsql = __FORMAT__( "lindex {} {}", key, index );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType LInsertBefore( const std::string& key, const T& findvalue, const T& insertvalue )
        {
            auto strsql = __FORMAT__( "linsert {} before {} {}", key, findvalue, insertvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType LInsertAfter( const std::string& key, const T& findvalue, const T& insertvalue )
        {
            auto strsql = __FORMAT__( "linsert {} after {} {}", key, findvalue, insertvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<voidptr>::UniqueType LSet( const std::string& key, int64 index, const T& value )
        {
            auto strsql = __FORMAT__( "lset {} {} {}", key, index, value );
            return WriteVoid( strsql );
        }

        inline KFResult<StringList>::UniqueType LRange( const std::string& key, int64 startindex, int64 endindex )
        {
            auto strsql = __FORMAT__( "lrange {} {} {}", key, startindex, endindex );
            return ReadList( strsql );
        }

        inline KFResult<voidptr>::UniqueType LTrim( const std::string& key, int64 startindex, int64 endindex )
        {
            auto strsql = __FORMAT__( "ltrim {} {} {}", key, startindex, endindex );
            return WriteVoid( strsql );
        }

        template< typename T >
        inline KFResult<StringPair>::UniqueType BLPop( const T& keylist, uint64 timeout )
        {
            auto strvalue = KFRedisFormat::ToString( keylist );
            auto strsql = __FORMAT__( "blpop {} {}", strvalue, timeout );
            return ReadPair( strsql );
        }

        template< typename T >
        inline KFResult<StringPair>::UniqueType BRPop( const T& keylist, uint64 timeout )
        {
            auto strvalue = KFRedisFormat::ToString( keylist );
            auto strsql = __FORMAT__( "brpop {} {}", strvalue, timeout );
            return ReadPair( strsql );
        }

        inline KFResult<std::string>::UniqueType BRPopLPush( const std::string& sourcekey, const std::string& destkey, uint64 timeout )
        {
            auto strsql = __FORMAT__( "brpoplpush {} {} {}", sourcekey, destkey, timeout );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType BRPopLPushUInt64( const std::string& sourcekey, const std::string& destkey, uint64 timeout )
        {
            auto strsql = __FORMAT__( "brpoplpush {} {} {}", sourcekey, destkey, timeout );
            return ReadUInt64( strsql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 集合
        template< typename T >
        inline KFResult<uint64>::UniqueType SAdd( const std::string& key, const T& value )
        {
            auto strvalue = KFRedisFormat::ToString( value );
            auto strsql = __FORMAT__( "sadd {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType SIsMember( const std::string& key, const T& value )
        {
            auto strsql = __FORMAT__( "sismember {} {}", key, value );
            return ReadUInt64( strsql );
        }

        inline KFResult<std::string>::UniqueType SPop( const std::string& key )
        {
            auto strsql = __FORMAT__( "spop {}", key );
            return WriteString( strsql );
        }

        inline KFResult<uint64>::UniqueType SPopUInt64( const std::string& key )
        {
            auto strsql = __FORMAT__( "spop {}", key );
            return WriteUInt64( strsql );
        }

        inline KFResult<std::string>::UniqueType SRandMember( const std::string& key )
        {
            auto strsql = __FORMAT__( "srandmember {}", key );
            return ReadString( strsql );
        }

        inline KFResult<uint64>::UniqueType SRandMemberUInt64( const std::string& key )
        {
            auto strsql = __FORMAT__( "srandmember {}", key );
            return ReadUInt64( strsql );
        }

        inline KFResult<StringVector>::UniqueType SRandMember( const std::string& key, int64 count )
        {
            auto strsql = __FORMAT__( "srandmember {} {}", key, count );
            return ReadVector( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType SRem( const std::string& key, const T& value )
        {
            auto strvalue = KFRedisFormat::ToString( value );
            auto strsql = __FORMAT__( "srem {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType SMove( const std::string& sourcekey, const T& value, const std::string& destkey )
        {
            auto strsql = __FORMAT__( "smove {} {} {}", sourcekey, destkey, value );
            return WriteUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType SCard( const std::string& key )
        {
            auto strsql = __FORMAT__( "scard {}", key );
            return ReadUInt64( strsql );
        }

        inline KFResult<StringVector>::UniqueType SMembers( const std::string& key )
        {
            auto strsql = __FORMAT__( "smembers {}", key );
            return ReadVector( strsql );
        }

        // 交集
        template< typename T >
        inline KFResult<StringVector>::UniqueType SInter( const T& key )
        {
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "sinter {}", strvalue );
            return ReadVector( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType SInterStore( const T& key, const std::string& destkey )
        {
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "sinterstore {} {}", destkey, strvalue );
            return WriteUInt64( strsql );
        }

        // 并集
        template< typename T >
        inline KFResult<StringVector>::UniqueType SUnion( const T& key )
        {
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "sunion {}", strvalue );
            return ReadVector( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType SUnionStore( const T& key, const std::string& destkey )
        {
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "sunionstore {} {}", destkey, strvalue );
            return WriteUInt64( strsql );
        }

        // 差集
        template< typename T >
        inline KFResult<StringVector>::UniqueType SDiff( const T& key )
        {
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "sdiff {}", strvalue );
            return ReadVector( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType SDiffStore( const T& key, const std::string& destkey )
        {
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "sdiffstore {} {}", destkey, strvalue );
            return WriteUInt64( strsql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 有序集合
        template< typename T >
        inline KFResult<uint64>::UniqueType ZAdd( const std::string& key, const T& member, uint64 source )
        {
            auto strsql = __FORMAT__( "zadd {} {} {}", key, source, member );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType ZAdd( const std::string& key, const T& values )
        {
            auto strvalue = KFRedisFormat::ToString( values );
            auto strsql = __FORMAT__( "zadd {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType ZScore( const std::string& key, const T& member )
        {
            auto strsql = __FORMAT__( "zscore {} {}", key, member );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType ZIncrby( const std::string& key, const T& member, int64 score )
        {
            auto strsql = __FORMAT__( "zincrby {} {} {}", key, score, member );
            return WriteUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType ZCard( const std::string& key )
        {
            auto strsql = __FORMAT__( "zcard {}", key );
            return ReadUInt64( strsql );
        }

        // 返回集合中[minsorce,maxscore]的成员个数
        inline KFResult<uint64>::UniqueType ZCount( const std::string& key, uint64 minscore, uint64 maxscore )
        {
            auto strsql = __FORMAT__( "zcount {} {}", key, minscore, maxscore );
            return ReadUInt64( strsql );
        }

        // 返回集合中区间的成员( 从小到大 )
        inline KFResult<StringPairList>::UniqueType ZRange( const std::string& key, uint64 startindex = 0, int64 endindex = -1 )
        {
            auto strsql = __FORMAT__( "zrange {} {} {} withscores", key, startindex, endindex );
            return ReadPairList( strsql );
        }

        // 返回集合中区间的成员( 从大到小 )
        inline KFResult<StringPairList>::UniqueType ZRevRange( const std::string& key, uint64 startindex = 0, int64 endindex = -1 )
        {
            auto strsql = __FORMAT__( "zrevrange {} {} {} withscores", key, startindex, endindex );
            return ReadPairList( strsql );
        }

        // 返回集合中score在区间的成员( 从小到大 )
        inline KFResult<StringPairList>::UniqueType ZRangeByScore( const std::string& key, uint64 minscore = 0, uint64 maxscore = __MAX_UINT64__ )
        {
            auto strsql = __FORMAT__( "zrangebyscore {} ({} {} withscores", key, minscore, maxscore );
            return ReadPairList( strsql );
        }

        // 返回集合中score在区间的成员( 从大到小 )
        inline KFResult<StringPairList>::UniqueType ZRevRangeByScore( const std::string& key, uint64 minscore = 0, uint64 maxscore = __MAX_UINT64__ )
        {
            auto strsql = __FORMAT__( "zrevrangebyscore {} ({} {} withscores", key, minscore, maxscore );
            return ReadPairList( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType ZRank( const std::string& key, const T& member )
        {
            auto strsql = __FORMAT__( "zrank {} {}", key, member );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType ZRevRank( const std::string& key, const T& member )
        {
            auto strsql = __FORMAT__( "zrevrank {} {}", key, member );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType ZRem( const std::string& key, const T& member )
        {
            auto strvalue = KFRedisFormat::ToString( member );
            auto strsql = __FORMAT__( "zrem {} {}", key, strvalue );
            return WriteUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType ZRemRangeByRank( const std::string& key, uint64 startindex, int64 endindex )
        {
            auto strsql = __FORMAT__( "zremrangebyrank {} {}", key, startindex, endindex );
            return WriteUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType ZRemRangeByRank( const std::string& key, uint64 minscore, uint64 maxscore )
        {
            auto strsql = __FORMAT__( "zremrangebyscore {} {}", key, minscore, minscore );
            return WriteUInt64( strsql );
        }
        //////////////////////////////////////////////////////////////////////////////////////////////
        //////////////////////////////////////////////////////////////////////////////////////////////
        // 位图
        inline KFResult<uint64>::UniqueType SetBit( const std::string& key, uint32 offset, uint32 value )
        {
            auto strsql = __FORMAT__( "setbit {} {}", key, offset, value );
            return WriteUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType GetBit( const std::string& key, uint32 offset )
        {
            auto strsql = __FORMAT__( "getbit {} {}", key, offset );
            return ReadUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType BitCount( const std::string& key )
        {
            auto strsql = __FORMAT__( "bitcount {}", key );
            return ReadUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType BitCount( const std::string& key, int32 startoffset, int32 endoffset )
        {
            auto strsql = __FORMAT__( "bitcount {} {} {}", key, startoffset, endoffset );
            return ReadUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType BitPos( const std::string& key, uint32 value )
        {
            auto strsql = __FORMAT__( "bitpos {} {}", key, value );
            return ReadUInt64( strsql );
        }

        inline KFResult<uint64>::UniqueType BitPos( const std::string& key, uint32 value, int32 startoffset, int32 endoffset )
        {
            auto strsql = __FORMAT__( "bitpos {} {} {} {}", key, value, startoffset, endoffset );
            return ReadUInt64( strsql );
        }

        template< typename T >
        inline KFResult<uint64>::UniqueType BitOp( const T& key, uint32 operate, const std::string& destkey )
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

            auto& opstring = _op_string( operate );
            auto strvalue = KFRedisFormat::ToString( key );
            auto strsql = __FORMAT__( "bitop {} {} {} {}", key, opstring, destkey, strvalue );
            return WriteUInt64( strsql );
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

        KFResult< StringList >::UniqueType ReadListExec()
        {
            static std::string _str_exec = "exec";
            return ReadList( _str_exec );
        }

        KFResult< StringVector >::UniqueType ReadVectorExec()
        {
            static std::string _str_exec = "exec";
            return ReadVector( _str_exec );
        }

        KFResult< StringMapList >::UniqueType ReadMapListExec()
        {
            static std::string _str_exec = "exec";
            return ReadMapList( _str_exec );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        // 执行数据库
        template< typename... P >
        KFResult< voidptr >::UniqueType ExecuteVoid( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return WriteVoid( strsql );
        }

        template< typename... P >
        KFResult< uint64 >::UniqueType ExecuteUInt64( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return WriteUInt64( strsql );
        }

        template< typename... P >
        KFResult< std::string >::UniqueType ExecuteString( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return WriteVoid( strsql );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        template< typename... P >
        KFResult< uint64 >::UniqueType QueryUInt64( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ReadUInt64( strsql );
        }

        template< typename... P >
        KFResult< std::string >::UniqueType QueryString( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ReadString( strsql );
        }

        template< typename... P >
        KFResult< StringMap >::UniqueType QueryMap( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ReadMap( strsql );
        }

        template< typename... P >
        KFResult< StringList >::UniqueType QueryList( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ReadList( strsql );
        }

        template< typename... P >
        KFResult< StringVector >::UniqueType QueryVector( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ReadVector( strsql );
        }

        template< typename... P >
        KFResult< StringMapList >::UniqueType QueryMapList( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ReadMapList( strsql );
        }

        template< typename... P >
        KFResult< StringPairList >::UniqueType QueryPairList( const char* myfmt, P&& ... args )
        {
            auto strsql = __FORMAT__( myfmt, std::forward<P>( args )... );
            return ReadPairList( strsql );
        }
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
        /////////////////////////////////////////////////////////////////////////////////////////////////////
    protected:
        // 写操作
        virtual KFResult< voidptr >::UniqueType WriteVoid( const std::string& strsql ) = 0;
        virtual KFResult< uint64 >::UniqueType WriteUInt64( const std::string& strsql ) = 0;
        virtual KFResult< std::string >::UniqueType WriteString( const std::string& strsql ) = 0;

        // 读操作
        virtual KFResult< uint64 >::UniqueType ReadUInt64( const std::string& strsql ) = 0;
        virtual KFResult< std::string >::UniqueType ReadString( const std::string& strsql ) = 0;
        virtual KFResult< StringMap >::UniqueType ReadMap( const std::string& strsql ) = 0;
        virtual KFResult< StringPair >::UniqueType ReadPair( const std::string& strsql ) = 0;
        virtual KFResult< StringList >::UniqueType ReadList( const std::string& strsql ) = 0;
        virtual KFResult< StringVector >::UniqueType ReadVector( const std::string& strsql ) = 0;
        virtual KFResult< StringMapList >::UniqueType ReadMapList( const std::string& strsql ) = 0;
        virtual KFResult< StringPairList >::UniqueType ReadPairList( const std::string& strsql ) = 0;
    };
}

#endif