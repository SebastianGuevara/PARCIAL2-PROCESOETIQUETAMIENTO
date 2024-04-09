/**
 * Head of code common to all C targets
 **/

#include "beremiz.h"
#include <string.h>
/*
 * Prototypes of functions provided by generated C softPLC
 **/
void config_run__(unsigned long tick);
void config_init__(void);

/*
 * Prototypes of functions provided by generated target C code
 * */
long long AtomicCompareExchange64(long long*, long long, long long);
void __init_debug(void);
void __cleanup_debug(void);
/*void __retrieve_debug(void);*/
void __publish_debug(void);

/*
 *  Variables used by generated C softPLC and plugins
 **/
IEC_TIME __CURRENT_TIME;
IEC_BOOL __DEBUG = 0;
unsigned long __tick = 0;
char *PLC_ID = NULL;

/*
 *  Variable generated by C softPLC and plugins
 **/
extern unsigned long greatest_tick_count__;

/* Help to quit cleanly when init fail at a certain level */
static int init_level = 0;

/*
 * Prototypes of functions exported by plugins
 **/


/*
 * Retrieve input variables, run PLC and publish output variables
 **/
void __run(void)
{
    __tick++;
    if (greatest_tick_count__)
        __tick %= greatest_tick_count__;

    

    /*__retrieve_debug();*/

    config_run__(__tick);

    __publish_debug();

    

}

/*
 * Initialize variables according to PLC's default values,
 * and then init plugins with that values
 **/
int __init(int argc,char **argv)
{
    int res = 0;
    init_level = 0;
    
    /* Effective tick time with 1ms default value */
    if(!common_ticktime__)
        common_ticktime__ = 1000000;

    config_init__();
    __init_debug();
    
    return res;
}
/*
 * Calls plugin cleanup proc.
 **/
void __cleanup(void)
{
    
    __cleanup_debug();
}

void PLC_GetTime(IEC_TIME *CURRENT_TIME);
void PLC_SetTimer(unsigned long long next, unsigned long long period);



IEC_BOOL var0;
IEC_BOOL *__IX0_0 = &var0;
IEC_BOOL var1;
IEC_BOOL *__IX0_1 = &var1;
IEC_BOOL var2;
IEC_BOOL *__IX0_2 = &var2;
IEC_BOOL var3;
IEC_BOOL *__QX0_0 = &var3;
IEC_BOOL var4;
IEC_BOOL *__QX0_1 = &var4;
IEC_BOOL var5;
IEC_BOOL *__QX0_2 = &var5;
IEC_BOOL var6;
IEC_BOOL *__QX0_3 = &var6;
/**
 * Win32 specific code
 **/

#include <stdio.h>
#include <sys/timeb.h>
#include <time.h>
#include <windows.h>
#include <locale.h>


long AtomicCompareExchange(long* atomicvar, long compared, long exchange)
{
    return InterlockedCompareExchange(atomicvar, exchange, compared);
}
CRITICAL_SECTION Atomic64CS; 
long long AtomicCompareExchange64(long long* atomicvar, long long compared, long long exchange)
{
    long long res;
    EnterCriticalSection(&Atomic64CS);
    res=*atomicvar;
    if(*atomicvar == compared){
        *atomicvar = exchange;
    }
    LeaveCriticalSection(&Atomic64CS);
    return res;
}

struct timeb timetmp;
void PLC_GetTime(IEC_TIME *CURRENT_TIME)
{
	ftime(&timetmp);

	(*CURRENT_TIME).tv_sec = timetmp.time;
	(*CURRENT_TIME).tv_nsec = timetmp.millitm * 1000000;
}

HANDLE PLC_timer = NULL;
void PLC_SetTimer(unsigned long long next, unsigned long long period)
{
	LARGE_INTEGER liDueTime;
	/* arg 2 of SetWaitableTimer take 100 ns interval*/
	liDueTime.QuadPart =  next / (-100);

	if (!SetWaitableTimer(PLC_timer, &liDueTime, period<1000000?1:period/1000000, NULL, NULL, 0))
    {
        printf("SetWaitableTimer failed (%d)\n", GetLastError());
    }
}

int PLC_shutdown;

int ForceSaveRetainReq(void) {
    return PLC_shutdown;
}

/* Variable used to stop plcloop thread */
void PlcLoop()
{
    PLC_shutdown = 0;
    while(!PLC_shutdown) {
        if (WaitForSingleObject(PLC_timer, INFINITE) != WAIT_OBJECT_0){
            PLC_shutdown = 1;
            break;
        }
        PLC_GetTime(&__CURRENT_TIME);
        __run();
    }
}

HANDLE PLC_thread;
HANDLE debug_sem;
HANDLE debug_wait_sem;
HANDLE python_sem;
HANDLE python_wait_sem;

#define maxval(a,b) ((a>b)?a:b)
int startPLC(int argc,char **argv)
{
	unsigned long thread_id = 0;
    BOOL tmp;

    debug_sem = CreateSemaphore(
                            NULL,           // default security attributes
                            1,  			// initial count
                            1,  			// maximum count
                            NULL);          // unnamed semaphore
    if (debug_sem == NULL)
    {
        printf("startPLC CreateSemaphore debug_sem error: %d\n", GetLastError());
        return 1;
    }

    debug_wait_sem = CreateSemaphore(
                            NULL,           // default security attributes
                            0,  			// initial count
                            1,  			// maximum count
                            NULL);          // unnamed semaphore

    if (debug_wait_sem == NULL)
    {
        printf("startPLC CreateSemaphore debug_wait_sem error: %d\n", GetLastError());
        return 1;
    }

    python_sem = CreateSemaphore(
                            NULL,           // default security attributes
                            1,  			// initial count
                            1,  			// maximum count
                            NULL);          // unnamed semaphore

    if (python_sem == NULL)
    {
        printf("startPLC CreateSemaphore python_sem error: %d\n", GetLastError());
        return 1;
    }
    python_wait_sem = CreateSemaphore(
                            NULL,           // default security attributes
                            0,  			// initial count
                            1,  			// maximum count
                            NULL);          // unnamed semaphore


    if (python_wait_sem == NULL)
    {
        printf("startPLC CreateSemaphore python_wait_sem error: %d\n", GetLastError());
        return 1;
    }


    /* Create a waitable timer */
    timeBeginPeriod(1);
    PLC_timer = CreateWaitableTimer(NULL, FALSE, "WaitableTimer");
    if(NULL == PLC_timer)
    {
        printf("CreateWaitableTimer failed (%d)\n", GetLastError());
        return 1;
    }
    if( __init(argc,argv) == 0 )
    {
        PLC_SetTimer(common_ticktime__,common_ticktime__);
        PLC_thread = CreateThread(NULL, 0, (LPTHREAD_START_ROUTINE)PlcLoop, NULL, 0, &thread_id);
    }
    else{
        return 1;
    }
    return 0;
}
static unsigned long __debug_tick;

int TryEnterDebugSection(void)
{
	//printf("TryEnterDebugSection\n");
    if(WaitForSingleObject(debug_sem, 0) == WAIT_OBJECT_0){
        /* Only enter if debug active */
        if(__DEBUG){
            return 1;
        }
        ReleaseSemaphore(debug_sem, 1, NULL);
    }
    return 0;
}

void LeaveDebugSection(void)
{
	ReleaseSemaphore(debug_sem, 1, NULL);
    //printf("LeaveDebugSection\n");
}

int stopPLC()
{
 	
    PLC_shutdown = 1;
    // force last wakeup of PLC thread
    SetWaitableTimer(PLC_timer, 0, 0, NULL, NULL, 0);
    // wait end of PLC thread
    WaitForSingleObject(PLC_thread, INFINITE);

    __cleanup();

    CloseHandle(PLC_timer);
    CloseHandle(debug_wait_sem);
    CloseHandle(debug_sem);
    CloseHandle(python_wait_sem);
    CloseHandle(python_sem);
    CloseHandle(PLC_thread);
}

/* from plc_debugger.c */
int WaitDebugData(unsigned long *tick)
{
	DWORD res;
	res = WaitForSingleObject(debug_wait_sem, INFINITE);
    *tick = __debug_tick;
    /* Wait signal from PLC thread */
	return res != WAIT_OBJECT_0;
}

/* Called by PLC thread when debug_publish finished
 * This is supposed to unlock debugger thread in WaitDebugData*/
void InitiateDebugTransfer()
{
    /* remember tick */
    __debug_tick = __tick;
    /* signal debugger thread it can read data */
    ReleaseSemaphore(debug_wait_sem, 1, NULL);
}

int suspendDebug(int disable)
{
    /* Prevent PLC to enter debug code */
    WaitForSingleObject(debug_sem, INFINITE);
    __DEBUG = !disable;
    if(disable)
        ReleaseSemaphore(debug_sem, 1, NULL);
    return 0;
}

void resumeDebug()
{
	__DEBUG = 1;
    /* Let PLC enter debug code */
	ReleaseSemaphore(debug_sem, 1, NULL);
}

/* from plc_python.c */
int WaitPythonCommands(void)
{
    /* Wait signal from PLC thread */
	return WaitForSingleObject(python_wait_sem, INFINITE);
}

/* Called by PLC thread on each new python command*/
void UnBlockPythonCommands(void)
{
    /* signal debugger thread it can read data */
	ReleaseSemaphore(python_wait_sem, 1, NULL);
}

int TryLockPython(void)
{
	return WaitForSingleObject(python_sem, 0) == WAIT_OBJECT_0;
}

void UnLockPython(void)
{
	ReleaseSemaphore(python_sem, 1, NULL);
}

void LockPython(void)
{
	WaitForSingleObject(python_sem, INFINITE);
}

static void __attribute__((constructor))
beremiz_dll_init(void)
{
    InitializeCriticalSection(&Atomic64CS);

}

static void __attribute__((destructor))
beremiz_dll_destroy(void)
{
    DeleteCriticalSection(&Atomic64CS);
}

struct RT_to_nRT_signal_s {
    HANDLE sem;
};

typedef struct RT_to_nRT_signal_s RT_to_nRT_signal_t;

#define _LogAndReturnNull(text) \
    {\
    	char mstr[256] = text " for ";\
        strncat(mstr, name, 255);\
        LogMessage(LOG_CRITICAL, mstr, strlen(mstr));\
        return NULL;\
    }

void *create_RT_to_nRT_signal(char* name){
    RT_to_nRT_signal_t *sig = (RT_to_nRT_signal_t*)malloc(sizeof(RT_to_nRT_signal_t));

    if(!sig) 
    	_LogAndReturnNull("Failed allocating memory for RT_to_nRT signal");

    sig->sem = CreateSemaphore(
                            NULL,           // default security attributes
                            1,  			// initial count
                            1,  			// maximum count
                            NULL);          // unnamed semaphore

    if(sig->sem == NULL)
    {
    	char mstr[256];
        snprintf(mstr, 255, "startPLC CreateSemaphore %s error: %d\n", name, GetLastError());
        LogMessage(LOG_CRITICAL, mstr, strlen(mstr));
        return NULL;
    }

    return (void*)sig;
}

void delete_RT_to_nRT_signal(void* handle){
    RT_to_nRT_signal_t *sig = (RT_to_nRT_signal_t*)handle;

    CloseHandle(python_sem);

    free(sig);
}

int wait_RT_to_nRT_signal(void* handle){
    int ret;
    RT_to_nRT_signal_t *sig = (RT_to_nRT_signal_t*)handle;
	return WaitForSingleObject(sig->sem, INFINITE);
}

int unblock_RT_to_nRT_signal(void* handle){
    RT_to_nRT_signal_t *sig = (RT_to_nRT_signal_t*)handle;
	return ReleaseSemaphore(sig->sem, 1, NULL);
}

void nRT_reschedule(void){
    SwitchToThread();
}


/*
  This file is part of Beremiz, a Integrated Development Environment for
  programming IEC 61131-3 automates supporting plcopen standard and CanFestival.

  See COPYING.runtime

  Copyright (C) 2018: Sergey Surkov <surkov.sv@summatechnology.ru>
  Copyright (C) 2018: Andrey Skvortsov <andrej.skvortzov@gmail.com>

*/

#ifndef HAVE_RETAIN
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include "iec_types.h"

int GetRetainSize(void);

/* Retain buffer.  */
FILE *retain_buffer;
const char rb_file[]      = "retain_buffer_file";
const char rb_file_bckp[] = "retain_buffer_file.bak";


/* Retain header struct.  */
struct retain_info_t {
	uint32_t retain_size;
	uint32_t hash_size;
	uint8_t* hash;
	uint32_t header_offset;
	uint32_t header_crc;
};

/* Init retain info structure.  */
struct retain_info_t retain_info;

/* CRC lookup table and initial state.  */
static const uint32_t crc32_table[256] = {
	0x00000000, 0x77073096, 0xEE0E612C, 0x990951BA, 0x076DC419, 0x706AF48F, 0xE963A535, 0x9E6495A3,
	0x0EDB8832, 0x79DCB8A4, 0xE0D5E91E, 0x97D2D988, 0x09B64C2B, 0x7EB17CBD, 0xE7B82D07, 0x90BF1D91,
	0x1DB71064, 0x6AB020F2, 0xF3B97148, 0x84BE41DE, 0x1ADAD47D, 0x6DDDE4EB, 0xF4D4B551, 0x83D385C7,
	0x136C9856, 0x646BA8C0, 0xFD62F97A, 0x8A65C9EC, 0x14015C4F, 0x63066CD9, 0xFA0F3D63, 0x8D080DF5,
	0x3B6E20C8, 0x4C69105E, 0xD56041E4, 0xA2677172, 0x3C03E4D1, 0x4B04D447, 0xD20D85FD, 0xA50AB56B,
	0x35B5A8FA, 0x42B2986C, 0xDBBBC9D6, 0xACBCF940, 0x32D86CE3, 0x45DF5C75, 0xDCD60DCF, 0xABD13D59,
	0x26D930AC, 0x51DE003A, 0xC8D75180, 0xBFD06116, 0x21B4F4B5, 0x56B3C423, 0xCFBA9599, 0xB8BDA50F,
	0x2802B89E, 0x5F058808, 0xC60CD9B2, 0xB10BE924, 0x2F6F7C87, 0x58684C11, 0xC1611DAB, 0xB6662D3D,
	0x76DC4190, 0x01DB7106, 0x98D220BC, 0xEFD5102A, 0x71B18589, 0x06B6B51F, 0x9FBFE4A5, 0xE8B8D433,
	0x7807C9A2, 0x0F00F934, 0x9609A88E, 0xE10E9818, 0x7F6A0DBB, 0x086D3D2D, 0x91646C97, 0xE6635C01,
	0x6B6B51F4, 0x1C6C6162, 0x856530D8, 0xF262004E, 0x6C0695ED, 0x1B01A57B, 0x8208F4C1, 0xF50FC457,
	0x65B0D9C6, 0x12B7E950, 0x8BBEB8EA, 0xFCB9887C, 0x62DD1DDF, 0x15DA2D49, 0x8CD37CF3, 0xFBD44C65,
	0x4DB26158, 0x3AB551CE, 0xA3BC0074, 0xD4BB30E2, 0x4ADFA541, 0x3DD895D7, 0xA4D1C46D, 0xD3D6F4FB,
	0x4369E96A, 0x346ED9FC, 0xAD678846, 0xDA60B8D0, 0x44042D73, 0x33031DE5, 0xAA0A4C5F, 0xDD0D7CC9,
	0x5005713C, 0x270241AA, 0xBE0B1010, 0xC90C2086, 0x5768B525, 0x206F85B3, 0xB966D409, 0xCE61E49F,
	0x5EDEF90E, 0x29D9C998, 0xB0D09822, 0xC7D7A8B4, 0x59B33D17, 0x2EB40D81, 0xB7BD5C3B, 0xC0BA6CAD,
	0xEDB88320, 0x9ABFB3B6, 0x03B6E20C, 0x74B1D29A, 0xEAD54739, 0x9DD277AF, 0x04DB2615, 0x73DC1683,
	0xE3630B12, 0x94643B84, 0x0D6D6A3E, 0x7A6A5AA8, 0xE40ECF0B, 0x9309FF9D, 0x0A00AE27, 0x7D079EB1,
	0xF00F9344, 0x8708A3D2, 0x1E01F268, 0x6906C2FE, 0xF762575D, 0x806567CB, 0x196C3671, 0x6E6B06E7,
	0xFED41B76, 0x89D32BE0, 0x10DA7A5A, 0x67DD4ACC, 0xF9B9DF6F, 0x8EBEEFF9, 0x17B7BE43, 0x60B08ED5,
	0xD6D6A3E8, 0xA1D1937E, 0x38D8C2C4, 0x4FDFF252, 0xD1BB67F1, 0xA6BC5767, 0x3FB506DD, 0x48B2364B,
	0xD80D2BDA, 0xAF0A1B4C, 0x36034AF6, 0x41047A60, 0xDF60EFC3, 0xA867DF55, 0x316E8EEF, 0x4669BE79,
	0xCB61B38C, 0xBC66831A, 0x256FD2A0, 0x5268E236, 0xCC0C7795, 0xBB0B4703, 0x220216B9, 0x5505262F,
	0xC5BA3BBE, 0xB2BD0B28, 0x2BB45A92, 0x5CB36A04, 0xC2D7FFA7, 0xB5D0CF31, 0x2CD99E8B, 0x5BDEAE1D,
	0x9B64C2B0, 0xEC63F226, 0x756AA39C, 0x026D930A, 0x9C0906A9, 0xEB0E363F, 0x72076785, 0x05005713,
	0x95BF4A82, 0xE2B87A14, 0x7BB12BAE, 0x0CB61B38, 0x92D28E9B, 0xE5D5BE0D, 0x7CDCEFB7, 0x0BDBDF21,
	0x86D3D2D4, 0xF1D4E242, 0x68DDB3F8, 0x1FDA836E, 0x81BE16CD, 0xF6B9265B, 0x6FB077E1, 0x18B74777,
	0x88085AE6, 0xFF0F6A70, 0x66063BCA, 0x11010B5C, 0x8F659EFF, 0xF862AE69, 0x616BFFD3, 0x166CCF45,
	0xA00AE278, 0xD70DD2EE, 0x4E048354, 0x3903B3C2, 0xA7672661, 0xD06016F7, 0x4969474D, 0x3E6E77DB,
	0xAED16A4A, 0xD9D65ADC, 0x40DF0B66, 0x37D83BF0, 0xA9BCAE53, 0xDEBB9EC5, 0x47B2CF7F, 0x30B5FFE9,
	0xBDBDF21C, 0xCABAC28A, 0x53B39330, 0x24B4A3A6, 0xBAD03605, 0xCDD70693, 0x54DE5729, 0x23D967BF,
	0xB3667A2E, 0xC4614AB8, 0x5D681B02, 0x2A6F2B94, 0xB40BBE37, 0xC30C8EA1, 0x5A05DF1B, 0x2D02EF8D,
};
uint32_t retain_crc;


/* Calculate CRC32 for len bytes from pointer buf with init starting value.  */
uint32_t GenerateCRC32Sum(const void* buf, unsigned int len, uint32_t init)
{
	uint32_t crc = ~init;
	unsigned char* current = (unsigned char*) buf;
	while (len--)
		crc = crc32_table[(crc ^ *current++) & 0xFF] ^ (crc >> 8);
	return ~crc;
}

/* Calc CRC32 for retain file byte by byte.  */
int CheckFileCRC(FILE* file_buffer)
{
	/* Set the magic constant for one-pass CRC calc according to ZIP CRC32.  */
	const uint32_t magic_number = 0x2144df1c;

	/* CRC initial state.  */
	uint32_t calc_crc32 = 0;
	char data_block = 0;

	while(!feof(file_buffer)){
		if (fread(&data_block, sizeof(data_block), 1, file_buffer))
			calc_crc32 = GenerateCRC32Sum(&data_block, sizeof(data_block), calc_crc32);
	}

	/* Compare crc result with a magic number.  */
	return (calc_crc32 == magic_number) ? 1 : 0;
}

/* Compare current hash with hash from file byte by byte.  */
int CheckFilehash(void)
{
	unsigned int k;
	int offset = sizeof(retain_info.retain_size);

	rewind(retain_buffer);
	fseek(retain_buffer, offset , SEEK_SET);

	uint32_t size;
	fread(&size, sizeof(size), 1, retain_buffer);
	if (size != retain_info.hash_size)
		return 0;

	for(k = 0; k < retain_info.hash_size; k++){
		uint8_t file_digit;
		fread(&file_digit, sizeof(file_digit), 1, retain_buffer);
		if (file_digit != *(retain_info.hash+k))
			return 0;
	}

	return 1;
}

void InitRetain(void)
{
	unsigned int i;

	/* Get retain size in bytes */
	retain_info.retain_size = GetRetainSize();

	/* Hash stored in retain file as array of char in hex digits
	   (that's why we divide strlen in two).  */
	retain_info.hash_size = PLC_ID ? strlen(PLC_ID)/2 : 0;
	//retain_info.hash_size = 0;
	retain_info.hash = malloc(retain_info.hash_size);

	/* Transform hash string into byte sequence.  */
	for (i = 0; i < retain_info.hash_size; i++) {
		int byte = 0;
		sscanf((PLC_ID + i*2), "%02X", &byte);
		retain_info.hash[i] = byte;
	}

	/* Calc header offset.  */
	retain_info.header_offset = sizeof(retain_info.retain_size) + \
		sizeof(retain_info.hash_size) + \
		retain_info.hash_size;

	/*  Set header CRC initial state.  */
	retain_info.header_crc = 0;

	/* Calc crc for header.  */
	retain_info.header_crc = GenerateCRC32Sum(
		&retain_info.retain_size,
		sizeof(retain_info.retain_size),
		retain_info.header_crc);

	retain_info.header_crc = GenerateCRC32Sum(
		&retain_info.hash_size,
		sizeof(retain_info.hash_size),
		retain_info.header_crc);

	retain_info.header_crc = GenerateCRC32Sum(
		retain_info.hash,
		retain_info.hash_size,
		retain_info.header_crc);
}

void CleanupRetain(void)
{
	/* Free hash memory.  */
	free(retain_info.hash);
}

int CheckRetainFile(const char * file)
{
	retain_buffer = fopen(file, "rb");
	if (retain_buffer) {
		/* Check CRC32 and hash.  */
		if (CheckFileCRC(retain_buffer))
			if (CheckFilehash())
				return 1;
		fclose(retain_buffer);
		retain_buffer = NULL;
	}
	return 0;
}

int CheckRetainBuffer(void)
{
	retain_buffer = NULL;
	if (!retain_info.retain_size)
		return 1;

	/* Check latest retain file.  */
	if (CheckRetainFile(rb_file))
		return 1;

	/* Check if we have backup.  */
	if (CheckRetainFile(rb_file_bckp))
		return 1;

	/* We don't have any valid retain buffer - nothing to remind.  */
	return 0;
}

#ifndef FILE_RETAIN_SAVE_PERIOD_S
#define FILE_RETAIN_SAVE_PERIOD_S 1.0
#endif

static double CalcDiffSeconds(IEC_TIME* t1, IEC_TIME *t2)
{
	IEC_TIME dt ={
		t1->tv_sec  - t2->tv_sec,
		t1->tv_nsec - t2->tv_nsec
	};

	if ((dt.tv_nsec < -1000000000) || ((dt.tv_sec > 0) && (dt.tv_nsec < 0))){
		dt.tv_sec--;
		dt.tv_nsec += 1000000000;
	}
	if ((dt.tv_nsec > +1000000000) || ((dt.tv_sec < 0) && (dt.tv_nsec > 0))){
		dt.tv_sec++;
		dt.tv_nsec -= 1000000000;
	}
	return dt.tv_sec + 1e-9*dt.tv_nsec;
}


int RetainSaveNeeded(void)
{
	int ret = 0;
	static IEC_TIME last_save;
	IEC_TIME now;
	double diff_s;

	/* no retain */
	if (!retain_info.retain_size)
		return 0;

	/* periodic retain flush to avoid high I/O load */
	PLC_GetTime(&now);

	diff_s = CalcDiffSeconds(&now, &last_save);

	if ((diff_s > FILE_RETAIN_SAVE_PERIOD_S) || ForceSaveRetainReq()) {
		ret = 1;
		last_save = now;
	}
	return ret;
}

void ValidateRetainBuffer(void)
{
	if (!retain_buffer)
		return;

	/* Add retain data CRC to the end of buffer file.  */
	fseek(retain_buffer, 0, SEEK_END);
	fwrite(&retain_crc, sizeof(retain_crc), 1, retain_buffer);

	/* Sync file buffer and close file.  */
#ifdef __WIN32
	fflush(retain_buffer);
#else
	fsync(fileno(retain_buffer));
#endif

	fclose(retain_buffer);
	retain_buffer = NULL;
}

void InValidateRetainBuffer(void)
{
	if (!RetainSaveNeeded())
		return;

	/* Rename old retain file into *.bak if it exists.  */
	rename(rb_file, rb_file_bckp);

	/* Set file CRC initial value.  */
	retain_crc = retain_info.header_crc;

	/* Create new retain file.  */
	retain_buffer = fopen(rb_file, "wb+");
	if (!retain_buffer) {
		fprintf(stderr, "Failed to create retain file : %s\n", rb_file);
		return;
	}

	/* Write header to the new file.  */
	fwrite(&retain_info.retain_size,
		sizeof(retain_info.retain_size), 1, retain_buffer);
	fwrite(&retain_info.hash_size,
		sizeof(retain_info.hash_size),   1, retain_buffer);
	fwrite(retain_info.hash ,
		sizeof(char), retain_info.hash_size, retain_buffer);
}

void Retain(unsigned int offset, unsigned int count, void *p)
{
	if (!retain_buffer)
		return;

	/* Generate CRC 32 for each data block.  */
	retain_crc = GenerateCRC32Sum(p, count, retain_crc);

	/* Save current var in file.  */
	fseek(retain_buffer, retain_info.header_offset+offset, SEEK_SET);
	fwrite(p, count, 1, retain_buffer);
}

void Remind(unsigned int offset, unsigned int count, void *p)
{
	/* Remind variable from file.  */
	fseek(retain_buffer, retain_info.header_offset+offset, SEEK_SET);
	fread((void *)p, count, 1, retain_buffer);
}
#endif // !HAVE_RETAIN
/**
 * Tail of code common to all C targets
 **/

/** 
 * LOGGING
 **/
#ifndef TARGET_LOGGING_DISABLE

#ifndef LOG_BUFFER_SIZE
#define LOG_BUFFER_SIZE (1<<14) /*16Ko*/
#endif
#ifndef LOG_BUFFER_ATTRS
#define LOG_BUFFER_ATTRS
#endif

#define LOG_BUFFER_MASK (LOG_BUFFER_SIZE-1)

static char LogBuff[LOG_LEVELS][LOG_BUFFER_SIZE] LOG_BUFFER_ATTRS;
static void inline copy_to_log(uint8_t level, uint32_t buffpos, void* buf, uint32_t size){
    if(buffpos + size < LOG_BUFFER_SIZE){
        memcpy(&LogBuff[level][buffpos], buf, size);
    }else{
        uint32_t remaining = LOG_BUFFER_SIZE - buffpos; 
        memcpy(&LogBuff[level][buffpos], buf, remaining);
        memcpy(LogBuff[level], (char*)buf + remaining, size - remaining);
    }
}
static void inline copy_from_log(uint8_t level, uint32_t buffpos, void* buf, uint32_t size){
    if(buffpos + size < LOG_BUFFER_SIZE){
        memcpy(buf, &LogBuff[level][buffpos], size);
    }else{
        uint32_t remaining = LOG_BUFFER_SIZE - buffpos; 
        memcpy(buf, &LogBuff[level][buffpos], remaining);
        memcpy((char*)buf + remaining, LogBuff[level], size - remaining);
    }
}

/* Log buffer structure

 |<-Tail1.msgsize->|<-sizeof(mTail)->|<--Tail2.msgsize-->|<-sizeof(mTail)->|...
 |  Message1 Body  |      Tail1      |   Message2 Body   |      Tail2      |

*/
typedef struct {
    uint32_t msgidx;
    uint32_t msgsize;
    unsigned long tick;
    IEC_TIME time;
} mTail;

/* Log cursor : 64b
   |63 ... 32|31 ... 0|
   | Message | Buffer |
   | counter | Index  | */
static uint64_t LogCursor[LOG_LEVELS] LOG_BUFFER_ATTRS = {0x0,0x0,0x0,0x0};

void ResetLogCount(void) {
	uint8_t level;
	for(level=0;level<LOG_LEVELS;level++){
		LogCursor[level] = 0;
	}
}

/* Store one log message of give size */
int LogMessage(uint8_t level, char* buf, uint32_t size){
    if(size < LOG_BUFFER_SIZE - sizeof(mTail)){
        uint32_t buffpos;
        uint64_t new_cursor, old_cursor;

        mTail tail;
        tail.msgsize = size;
        tail.tick = __tick;
        PLC_GetTime(&tail.time);

        /* We cannot increment both msg index and string pointer 
           in a single atomic operation but we can detect having been interrupted.
           So we can try with atomic compare and swap in a loop until operation
           succeeds non interrupted */
        do{
            old_cursor = LogCursor[level];
            buffpos = (uint32_t)old_cursor;
            tail.msgidx = (old_cursor >> 32); 
            new_cursor = ((uint64_t)(tail.msgidx + 1)<<32) 
                         | (uint64_t)((buffpos + size + sizeof(mTail)) & LOG_BUFFER_MASK);
        }while(AtomicCompareExchange64(
            (long long*)&LogCursor[level],
            (long long)old_cursor,
            (long long)new_cursor)!=(long long)old_cursor);

        copy_to_log(level, buffpos, buf, size);
        copy_to_log(level, (buffpos + size) & LOG_BUFFER_MASK, &tail, sizeof(mTail));

        return 1; /* Success */
    }else{
    	char mstr[] = "Logging error : message too big";
        LogMessage(LOG_CRITICAL, mstr, sizeof(mstr));
    }
    return 0;
}

uint32_t GetLogCount(uint8_t level){
    return (uint64_t)LogCursor[level] >> 32;
}

/* Return message size and content */
uint32_t GetLogMessage(uint8_t level, uint32_t msgidx, char* buf, uint32_t max_size, uint32_t* tick, uint32_t* tv_sec, uint32_t* tv_nsec){
    uint64_t cursor = LogCursor[level];
    if(cursor){
        /* seach cursor */
        uint32_t stailpos = (uint32_t)cursor; 
        uint32_t smsgidx;
        mTail tail;
        tail.msgidx = cursor >> 32;
        tail.msgsize = 0;

        /* Message search loop */
        do {
            smsgidx = tail.msgidx;
            stailpos = (stailpos - sizeof(mTail) - tail.msgsize ) & LOG_BUFFER_MASK;
            copy_from_log(level, stailpos, &tail, sizeof(mTail));
        }while((tail.msgidx == smsgidx - 1) && (tail.msgidx > msgidx));

        if(tail.msgidx == msgidx){
            uint32_t sbuffpos = (stailpos - tail.msgsize ) & LOG_BUFFER_MASK; 
            uint32_t totalsize = tail.msgsize;
            *tick = tail.tick; 
            *tv_sec = tail.time.tv_sec; 
            *tv_nsec = tail.time.tv_nsec; 
            copy_from_log(level, sbuffpos, buf, 
                          totalsize > max_size ? max_size : totalsize);
            return totalsize;
        }
    }
    return 0;
}

#endif

#ifndef TARGET_EXT_SYNC_DISABLE

#define CALIBRATED -2
#define NOT_CALIBRATED -1
static int calibration_count = NOT_CALIBRATED;
static IEC_TIME cal_begin;
static long long Tsync = 0;
static long long FreqCorr = 0;
static int Nticks = 0;
static unsigned long last_tick = 0;

/*
 * Called on each external periodic sync event
 * make PLC tick synchronous with external sync
 * ratio defines when PLC tick occurs between two external sync
 * @param sync_align_ratio 
 *          0->100 : align ratio
 *          < 0 : no align, calibrate period
 **/
void align_tick(int sync_align_ratio)
{
	/*
	printf("align_tick(%d)\n", calibrate);
	*/
	if(sync_align_ratio < 0){ /* Calibration */
		if(calibration_count == CALIBRATED)
			/* Re-calibration*/
			calibration_count = NOT_CALIBRATED;
		if(calibration_count == NOT_CALIBRATED)
			/* Calibration start, get time*/
			PLC_GetTime(&cal_begin);
		calibration_count++;
	}else{ /* do alignment (if possible) */
		if(calibration_count >= 0){
			/* End of calibration */
			/* Get final time */
			IEC_TIME cal_end;
			PLC_GetTime(&cal_end);
			/*adjust calibration_count*/
			calibration_count++;
			/* compute mean of Tsync, over calibration period */
			Tsync = ((long long)(cal_end.tv_sec - cal_begin.tv_sec) * (long long)1000000000 +
					(cal_end.tv_nsec - cal_begin.tv_nsec)) / calibration_count;
			if( (Nticks = (Tsync / common_ticktime__)) > 0){
				FreqCorr = (Tsync % common_ticktime__); /* to be divided by Nticks */
			}else{
				FreqCorr = Tsync - (common_ticktime__ % Tsync);
			}
			/*
			printf("Tsync = %ld\n", Tsync);
			printf("calibration_count = %d\n", calibration_count);
			printf("Nticks = %d\n", Nticks);
			*/
			calibration_count = CALIBRATED;
		}
		if(calibration_count == CALIBRATED){
			/* Get Elapsed time since last PLC tick (__CURRENT_TIME) */
			IEC_TIME now;
			long long elapsed;
			long long Tcorr;
			long long PhaseCorr;
			long long PeriodicTcorr;
			PLC_GetTime(&now);
			elapsed = (now.tv_sec - __CURRENT_TIME.tv_sec) * 1000000000 + now.tv_nsec - __CURRENT_TIME.tv_nsec;
			if(Nticks > 0){
				PhaseCorr = elapsed - (common_ticktime__ + FreqCorr/Nticks)*sync_align_ratio/100; /* to be divided by Nticks */
				Tcorr = common_ticktime__ + (PhaseCorr + FreqCorr) / Nticks;
				if(Nticks < 2){
					/* When Sync source period is near Tick time */
					/* PhaseCorr may not be applied to Periodic time given to timer */
					PeriodicTcorr = common_ticktime__ + FreqCorr / Nticks;
				}else{
					PeriodicTcorr = Tcorr;
				}
			}else if(__tick > last_tick){
				last_tick = __tick;
				PhaseCorr = elapsed - (Tsync*sync_align_ratio/100);
				PeriodicTcorr = Tcorr = common_ticktime__ + PhaseCorr + FreqCorr;
			}else{
				/*PLC did not run meanwhile. Nothing to do*/
				return;
			}
			/* DO ALIGNEMENT */
			PLC_SetTimer(Tcorr - elapsed, PeriodicTcorr);
		}
	}
}

#endif
