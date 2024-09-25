#pragma once
#include <jadel.h>

#define EXPAND( x ) x
#define GET_MACRO( _1, _2, _3, _4, NAME, ... ) NAME
#define FOR_EX4( indexType, indexName, min, max ) for (indexType indexName = min; indexName < max; ++indexName)
#define FOR_IN4( indexType, indexName, min, max ) for (indexType indexName = min; indexName <= max; ++indexName)
#define FOR_EX3( indexName, min, max ) FOR_EX4(, indexName, min, max)
#define FOR_IN3( indexName, min, max ) FOR_IN4(, indexName, min, max)

#define FOR_EX( ... ) EXPAND(GET_MACRO(__VA_ARGS__, FOR_EX4, FOR_EX3, FOR_EX2 )(__VA_ARGS__))
#define FOR_IN( ... ) EXPAND(GET_MACRO(__VA_ARGS__, FOR_IN4, FOR_IN3, FOR_EX2 )(__VA_ARGS__))

#define FOR_EX_I3(indexName, min, max) FOR_EX(int, indexName, min, max)
#define FOR_IN_I3(indexName, min, max) FOR_IN(int, indexName, min, max)
#define FOR_EX_I2(indexName, max) FOR_EX(int, indexName, 0, max)
#define FOR_IN_I2(indexName, max) FOR_IN(int, indexName, 0, max)
#define FOR_EX_U3(indexName, min, max) FOR_EX(uint32_t, indexName, min, max)
#define FOR_IN_U3(indexName, min, max) FOR_IN(uint32_t, indexName, min, max)
#define FOR_EX_U2(indexName, max) FOR_EX_U3(indexName, 0, max)
#define FOR_IN_U2(indexName, max) FOR_IN_U3(indexName, 0, max)


#define FOR_EX_I( ... ) EXPAND(GET_MACRO(__VA_ARGS__, UNUSED, FOR_EX_I3, FOR_EX_I2)(__VA_ARGS__))
#define FOR_IN_I( ... ) EXPAND(GET_MACRO(__VA_ARGS__, UNUSED, FOR_IN_I3, FOR_IN_I2)(__VA_ARGS__))
#define FOR_EX_U( ... ) EXPAND(GET_MACRO(__VA_ARGS__, UNUSED, FOR_EX_U3, FOR_EX_U2)(__VA_ARGS__))
#define FOR_IN_U( ... ) EXPAND(GET_MACRO(__VA_ARGS__, UNUSED, FOR_IN_U3, FOR_IN_U2)(__VA_ARGS__))


#define ERRMSG( err, ... ) {jadel::message("[ERROR]"); jadel::message( err, __VA_ARGS__ );}
#ifdef DEBUG
#define DEBUGMSG( msg, ... ) {jadel::message("[DEBUG] "); jadel::message( msg, __VA_ARGS__ );}
#define DEBUGERR( err, ...   ) {jadel::message("[DEBUG ERROR]"); jadel::message( err, __VA_ARGS__ );}
#define DEBUGCALL( func ) {func;}
#else
#define DEBUGMSG( msg, ... )
#define DEBUGERR( err, ... )
#define DEBUGCALL( func )
#endif