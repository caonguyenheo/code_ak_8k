

#ifndef _AEC_INTERFACE_H_
#define _AEC_INTERFACE_H_

//#include "anyka_types.h"
//#include "anyka_types_internal.h"
#define  ECHO_LIB_VERSION_STRING (signed char *)"Echo Version V1.1.09 debug"


// chip binding
typedef enum 
{
    ECHO_CHIP_UNKNOW = 0,
    ECHO_CHIP_AK10XXC = 1,
    ECHO_CHIP_AK37XXC = 2,
    ECHO_CHIP_AK39XX = 3,
    ECHO_CHIP_AK10XXT = 4,
    ECHO_CHIP_AK39XXE = 5,
    ECHO_CHIP_AK37XXC_RTOS = 6,
    ECHO_CHIP_AK39XXEV2 = 7,
    ECHO_CHIP_AK10XXD = 8,
    ECHO_CHIP_AK39XXEV2_RTOS = 9
}ECHO_CHIP_ID;

typedef void (*AEC_CALLBACK_FUN_MALLOC)(unsigned long size); 
typedef void  (*AEC_CALLBACK_FUN_FREE)(void * mem);
typedef signed long   (*AEC_CALLBACK_FUN_PRINTF)(const signed char format, ...);
typedef void  (*AEC_CALLBACK_FUN_RTC_DELAY) (unsigned long ulTicks);
typedef void  (*AEC_CALLBACK_FUN_NOTIFY) (unsigned long event);

typedef struct
{
    AEC_CALLBACK_FUN_MALLOC			Malloc;
    AEC_CALLBACK_FUN_FREE			Free;
    AEC_CALLBACK_FUN_PRINTF			printf;
    AEC_CALLBACK_FUN_RTC_DELAY		delay;
    AEC_CALLBACK_FUN_NOTIFY         notify;
}T_AEC_CB_FUNS;

typedef enum
{
    AEC_TYPE_UNKNOWN ,
    AEC_TYPE_1
}T_AEC_TYPE;

typedef struct {
    unsigned long   m_aecEna;
    unsigned long   m_PreprocessEna;
    unsigned long   m_framelen;  //NN
    unsigned long   m_tail;      //TAIL 
    unsigned long   m_agcEna;
    unsigned long   m_agcLevel;  // 1~32767, 0: use default
    unsigned long   m_maxGain;   // agc's max_gain
}T_AEC_PARAMS;

typedef struct
{
    const signed char * strVersion; // must be ECHO_LIB_VERSION_STRING
    ECHO_CHIP_ID chip;
    unsigned long	m_Type;				//扩展用，标明AEC算法类别，是T_AEC_TYPE的枚举之一
    unsigned long	m_SampleRate;		//sample rate, sample per second
    unsigned short	m_Channels;			//channel number
    unsigned short	m_BitsPerSample;	//bits per sample  
    
    union {
        T_AEC_PARAMS m_aec;
    }m_Private;
}T_AEC_IN_INFO;

typedef struct
{
    void *buf_near;  //near, mic
    unsigned long  len_near;
    void *buf_far;  //far, speaker
    unsigned long  len_far;
    void *buf_out; // output of aec
    unsigned long  len_out;
}T_AEC_BUF;

typedef struct
{
    T_AEC_CB_FUNS	cb_fun;
    T_AEC_IN_INFO	m_info;
}T_AEC_INPUT;

typedef enum
{
    AEC_REQ_UNKNOWN ,
    AEC_REQ_GET_SAMPLING_RATE, //get sample rate, data: int
    AEC_REQ_GET_FILTER_SIZE, // get filter length, data: int
    AEC_REQ_GET_FILTER, // get filter weight, data: 
    AEC_REQ_GET_PARAMS, 
    AEC_REQ_SET_PARAMS, 
}T_AEC_REQ_TYPE;

typedef struct
{
    unsigned long m_req_type;  // 要做什么事情，例如是否设置采样率
    unsigned char  *addr;       // 要设置或获取的参数地址
    unsigned long addrLen;     // addr指向的地址空间的字节数
}T_AEC_REQUEST;

void *AECLib_Open(T_AEC_INPUT *p_aec_input);
signed long AECLib_Close(void *p_aec);
signed long AECLib_Control(void *p_aec, T_AEC_BUF *p_aec_buf);
signed long AECLib_Request(void *p_aec, T_AEC_REQUEST *p_aec_req);

#endif
