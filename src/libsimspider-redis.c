/*
 * simspider-redis
 * author	: calvin
 * email	: calvinwilliams.c@gmail.com
 *
 * Licensed under the LGPL v2.1, see the file LICENSE in base directory.
 */

#include "libsimspider-redis.h"

char	__SIMSPIDER_REDIS_VERSION_1_0_0[] = "1.0.0" ;
char	*__SIMSPIDER_REDIS_VERSION = __SIMSPIDER_REDIS_VERSION_1_0_0 ;

#define SIMSPIDER_REQUESTQUEUE_REDISKEY		"SIMSPIDER_REQUESTQUEUE_REDISKEY"
#define SIMSPIDER_DONEQUEUE_REDISKEY		"SIMSPIDER_DONEQUEUE_REDISKEY"

static funcResetRequestQueueProc ResetRequestQueueProc_REDIS ;
int ResetRequestQueueProc_REDIS( struct SimSpiderEnv *penv )
{
	redisContext	*conn = NULL ;
	redisReply	*reply = NULL ;
	char		command[ 1024 + 1 ] ;
	
	conn = (redisContext*)GetRequestQueueHandler(penv) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "conn invalid" );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	memset( command , 0x00 , sizeof(command) );
	snprintf( command , sizeof(command) , "FLUSHDB" );
	reply = redisCommand( conn , command ) ;
	if( reply && reply->type == REDIS_REPLY_STATUS && strcasecmp( reply->str , "OK" ) == 0 )
	{
		DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
		freeReplyObject( reply );
	}
	else
	{
		ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s][%s]" , command , conn->errstr , reply?reply->str:"" );
		freeReplyObject( reply );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	return 0;
}

static funcResizeRequestQueueProc ResizeRequestQueueProc_REDIS ;
int ResizeRequestQueueProc_REDIS( struct SimSpiderEnv *penv , long new_size )
{
	return 0;
}

static funcPushRequestQueueUnitProc PushRequestQueueUnitProc_REDIS ;
int PushRequestQueueUnitProc_REDIS( struct SimSpiderEnv *penv , char url[SIMSPIDER_MAXLEN_URL+1] )
{
	redisContext	*conn = NULL ;
	redisReply	*reply = NULL ;
	char		command[ 1024 + 1 ] ;
	
	conn = (redisContext*)GetRequestQueueHandler(penv) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "conn invalid" );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	memset( command , 0x00 , sizeof(command) );
	snprintf( command , sizeof(command) , "LPUSH "SIMSPIDER_REQUESTQUEUE_REDISKEY" %s" , url );
	reply = redisCommand( conn , command ) ;
	if( reply && reply->type == REDIS_REPLY_INTEGER )
	{
		DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
		freeReplyObject( reply );
	}
	else
	{
		ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s]" , command , conn->errstr , reply?reply->str:"" );
		freeReplyObject( reply );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	return 0;
}

static funcPopupRequestQueueUnitProc PopupRequestQueueUnitProc_REDIS ;
int PopupRequestQueueUnitProc_REDIS( struct SimSpiderEnv *penv , char url[SIMSPIDER_MAXLEN_URL+1] )
{
	redisContext	*conn = NULL ;
	redisReply	*reply = NULL ;
	char		command[ 1024 + 1 ] ;
	
	conn = (redisContext*)GetRequestQueueHandler(penv) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "conn invalid" );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	memset( command , 0x00 , sizeof(command) );
	snprintf( command , sizeof(command) , "RPOP "SIMSPIDER_REQUESTQUEUE_REDISKEY );
	reply = redisCommand( conn , command ) ;
	if( reply && reply->type == REDIS_REPLY_STRING )
	{
		DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
		strncpy( url , reply->str , SIMSPIDER_MAXLEN_URL-1 );
		freeReplyObject( reply );
	}
	else if( reply && reply->type == REDIS_REPLY_NIL )
	{
		DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok , but no data" , command );
		return SIMSPIDER_INFO_NO_TASK_IN_REQUEST_QUEUE;
	}
	else
	{
		ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s]" , command , conn->errstr , reply?reply->str:"" );
		freeReplyObject( reply );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	return 0;
}

static funcResetDoneQueueProc ResetDoneQueueProc_REDIS ;
int ResetDoneQueueProc_REDIS( struct SimSpiderEnv *penv )
{
	redisContext	*conn = NULL ;
	redisReply	*reply = NULL ;
	char		command[ 1024 + 1 ] ;
	
	conn = (redisContext*)GetRequestQueueHandler(penv) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "conn invalid" );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	memset( command , 0x00 , sizeof(command) );
	snprintf( command , sizeof(command) , "FLUSHDB" );
	reply = redisCommand( conn , command ) ;
	if( reply && reply->type == REDIS_REPLY_STATUS && strcasecmp( reply->str , "OK" ) == 0 )
	{
		DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
		freeReplyObject( reply );
	}
	else
	{
		ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s]" , command , conn->errstr , reply?reply->str:"" );
		freeReplyObject( reply );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	return 0;
}

static funcResizeDoneQueueProc ResizeDoneQueueProc_REDIS ;
int ResizeDoneQueueProc_REDIS( struct SimSpiderEnv *penv , long new_size )
{
	return 0;
}

static funcQueryDoneQueueUnitProc QueryDoneQueueUnitProc_REDIS ;
int QueryDoneQueueUnitProc_REDIS( struct SimSpiderEnv *penv , char url[SIMSPIDER_MAXLEN_URL+1] , struct DoneQueueUnit *pdqu , int SizeOfDoneQueueUnit )
{
	redisContext	*conn = NULL ;
	redisReply	*reply = NULL ;
	char		command[ 1024 + 1 ] ;
	
	conn = (redisContext*)GetRequestQueueHandler(penv) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "conn invalid" );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	memset( command , 0x00 , sizeof(command) );
	snprintf( command , sizeof(command) , "hmget "SIMSPIDER_DONEQUEUE_REDISKEY":%s referer_url url recursive_depth retry_count status" , url );
	reply = redisCommand( conn , command ) ;
	if( reply && reply->type == REDIS_REPLY_ARRAY )
	{
		DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
		if( reply->element[0]->type == REDIS_REPLY_NIL )
		{
			return SIMSPIDER_INFO_NO_TASK_IN_DONE_QUEUE;
		}
		if( pdqu )
		{
			if( reply->element[0]->type == REDIS_REPLY_STRING )
			{
				strncpy( GetDoneQueueUnitRefererUrl(pdqu) , reply->element[0]->str , SIMSPIDER_MAXLEN_URL-1 );
			}
			if( reply->element[1]->type == REDIS_REPLY_STRING )
			{
				strncpy( GetDoneQueueUnitUrl(pdqu) , reply->element[1]->str , SIMSPIDER_MAXLEN_URL-1 );
			}
			if( reply->element[2]->type == REDIS_REPLY_STRING )
			{
				SetDoneQueueUnitRecursiveDepth( pdqu , atol(reply->element[2]->str) );
			}
			if( reply->element[3]->type == REDIS_REPLY_STRING )
			{
				SetDoneQueueUnitRetryCount( pdqu , atol(reply->element[3]->str) );
			}
			if( reply->element[4]->type == REDIS_REPLY_STRING )
			{
				SetDoneQueueUnitStatus( pdqu , atoi(reply->element[4]->str) );
			}
			
			if( GetDoneQueueUnitRefererUrl(pdqu)[0] == '\0' || GetDoneQueueUnitUrl(pdqu)[0] == '\0' )
			{
				ErrorLog( __FILE__ , __LINE__ , "data invalid:" );
				ErrorLog( __FILE__ , __LINE__ , "    referer_url[%s]" , GetDoneQueueUnitRefererUrl(pdqu) );
				ErrorLog( __FILE__ , __LINE__ , "            url[%s]" , GetDoneQueueUnitUrl(pdqu) );
				ErrorLog( __FILE__ , __LINE__ , "recursive_depth[%ld]" , GetDoneQueueUnitRecursiveDepth(pdqu) );
				ErrorLog( __FILE__ , __LINE__ , "    retry_count[%ld]" , GetDoneQueueUnitRetryCount(pdqu) );
				ErrorLog( __FILE__ , __LINE__ , "         status[%d]" , GetDoneQueueUnitStatus(pdqu) );
				return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
			}
		}
		freeReplyObject( reply );
	}
	else
	{
		ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s]" , command , conn->errstr , reply?reply->str:"" );
		freeReplyObject( reply );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	return 0;
}

static funcAddDoneQueueUnitProc AddDoneQueueUnitProc_REDIS ;
int AddDoneQueueUnitProc_REDIS( struct SimSpiderEnv *penv , char *referer_url , char *url , long recursive_depth , int SizeOfDoneQueueUnit )
{
	redisContext	*conn = NULL ;
	redisReply	*reply = NULL ;
	char		command[ 1024 + 1 ] ;
	
	conn = (redisContext*)GetRequestQueueHandler(penv) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "conn invalid" );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	memset( command , 0x00 , sizeof(command) );
	snprintf( command , sizeof(command) , "hsetnx "SIMSPIDER_DONEQUEUE_REDISKEY":%s url %s" , url , url );
	reply = redisCommand( conn , command ) ;
	if( reply && reply->type == REDIS_REPLY_INTEGER )
	{
		if( reply->integer == 1 )
		{
			DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
			freeReplyObject( reply );
			
			memset( command , 0x00 , sizeof(command) );
			snprintf( command , sizeof(command) , "hmset "SIMSPIDER_DONEQUEUE_REDISKEY":%s referer_url %s url %s recursive_depth %ld retry_count 0 status 0" , url , referer_url[0]?referer_url:url , url , recursive_depth );
			reply = redisCommand( conn , command ) ;
			if( reply && reply->type == REDIS_REPLY_STATUS && strcasecmp( reply->str , "OK" ) == 0 )
			{
				DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
				freeReplyObject( reply );
			}
			else
			{
				ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s]" , command , conn->errstr , reply?reply->str:"" );
				freeReplyObject( reply );
				return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
			}
			
			return SIMSPIDER_INFO_ADD_TASK_IN_DONE_QUEUE;
		}
	}
	else
	{
		ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s]" , command , conn->errstr , reply?reply->str:"" );
		freeReplyObject( reply );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	return 0;
}

static funcUpdateDoneQueueUnitProc UpdateDoneQueueUnitProc_REDIS ;
int UpdateDoneQueueUnitProc_REDIS( struct SimSpiderEnv *penv , struct DoneQueueUnit *pdqu , int SizeOfDoneQueueUnit )
{
	redisContext	*conn = NULL ;
	redisReply	*reply = NULL ;
	char		command[ 1024 + 1 ] ;
	
	conn = (redisContext*)GetRequestQueueHandler(penv) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "conn invalid" );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	memset( command , 0x00 , sizeof(command) );
	snprintf( command , sizeof(command) , "hmset "SIMSPIDER_DONEQUEUE_REDISKEY":%s referer_url %s url %s recursive_depth %ld retry_count %ld status %d" , GetDoneQueueUnitUrl(pdqu) , GetDoneQueueUnitRefererUrl(pdqu)[0]?GetDoneQueueUnitRefererUrl(pdqu):GetDoneQueueUnitUrl(pdqu) , GetDoneQueueUnitUrl(pdqu) , GetDoneQueueUnitRecursiveDepth(pdqu) , GetDoneQueueUnitRetryCount(pdqu) , GetDoneQueueUnitStatus(pdqu) );
	reply = redisCommand( conn , command ) ;
	if( reply && reply->type == REDIS_REPLY_STATUS && strcasecmp( reply->str , "OK" ) == 0 )
	{
		DebugLog( __FILE__ , __LINE__ , "redisCommand[%s] ok" , command );
		freeReplyObject( reply );
	}
	else
	{
		ErrorLog( __FILE__ , __LINE__ , "redisCommand[%s] failed[%s]" , command , conn->errstr , reply?reply->str:"" );
		freeReplyObject( reply );
		return SIMSPIDER_ERROR_FUNCPROC_INTERRUPT;
	}
	
	return 0;
}

int BindSimspiderRedisQueueHandler( struct SimSpiderEnv *penv , char *redis_ip , long redis_port )
{
	redisContext	*conn = NULL ;
	
	conn = redisConnect( redis_ip , redis_port ) ;
	if( conn == NULL )
	{
		ErrorLog( __FILE__ , __LINE__ , "redisConnect[%s:%d] failed" , redis_ip , redis_port );
		return -1;
	}
	else
	{
		InfoLog( __FILE__ , __LINE__ , "redisConnect[%s:%d] ok" , redis_ip , redis_port );
	}
	
	SetRequestQueueHandler( penv , (void*)conn );
	
	SetResetRequestQueueProc( penv , & ResetRequestQueueProc_REDIS );
	SetResizeRequestQueueProc( penv , & ResizeRequestQueueProc_REDIS );
	SetPushRequestQueueUnitProc( penv , & PushRequestQueueUnitProc_REDIS );
	SetPopupRequestQueueUnitProc( penv , & PopupRequestQueueUnitProc_REDIS );
	
	SetDoneQueueHandler( penv , (void*)conn );
	
	SetResetDoneQueueProc( penv , & ResetDoneQueueProc_REDIS );
	SetResizeDoneQueueProc( penv , & ResizeDoneQueueProc_REDIS );
	SetQueryDoneQueueUnitProc( penv , & QueryDoneQueueUnitProc_REDIS );
	SetAddDoneQueueUnitProc( penv , & AddDoneQueueUnitProc_REDIS );
	SetUpdateDoneQueueUnitProc( penv , & UpdateDoneQueueUnitProc_REDIS );
	
	return 0;
}

void UnbindSimspiderRedisQueueHandler( struct SimSpiderEnv *penv )
{
	redisContext	*conn = NULL ;
	
	conn = GetRequestQueueHandler( penv );
	if( conn == NULL )
		return;
	
	redisFree( conn );
	InfoLog( __FILE__ , __LINE__ , "redisFree ok" );
	
	SetRequestQueueHandler( penv , NULL );
	
	SetResetRequestQueueProc( penv , NULL );
	SetResizeRequestQueueProc( penv , NULL );
	SetPushRequestQueueUnitProc( penv , NULL );
	SetPopupRequestQueueUnitProc( penv , NULL );
	
	SetDoneQueueHandler( penv , NULL );
	
	SetResetDoneQueueProc( penv , NULL );
	SetResizeDoneQueueProc( penv , NULL );
	SetQueryDoneQueueUnitProc( penv , NULL );
	SetAddDoneQueueUnitProc( penv , NULL );
	SetUpdateDoneQueueUnitProc( penv , NULL );
	
	return;
}
