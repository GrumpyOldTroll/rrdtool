#ifndef FIX_CYGWIN_H
#define FIX_CYGWIN_H

#include <lcms.h>

// From w32api/winbase.h
#define FORMAT_MESSAGE_ALLOCATE_BUFFER 256
#define FORMAT_MESSAGE_IGNORE_INSERTS 512
#define FORMAT_MESSAGE_FROM_STRING 1024
#define FORMAT_MESSAGE_FROM_HMODULE 2048
#define FORMAT_MESSAGE_FROM_SYSTEM 4096
#define FORMAT_MESSAGE_ARGUMENT_ARRAY 8192
#define FORMAT_MESSAGE_MAX_WIDTH_MASK 255

// from w32api/winnt.h
#define LANG_NEUTRAL	0x00
#define SUBLANG_NEUTRAL	0x00
#define SUBLANG_DEFAULT	0x01
#define MAKELANGID(p,s)	((((WORD)(s))<<10)|(WORD)(p))
typedef wchar_t WCHAR;
typedef WCHAR *PWCHAR,*LPWCH,*PWCH,*NWPSTR,*LPWSTR,*PWSTR;

// from w32api/windef.h
#ifndef CONST
#define CONST const
#endif
typedef CONST void *PCVOID,*LPCVOID;

// from w32api/winsock2.h
struct hostent;
struct in_addr;
char * inet_ntoa(struct in_addr);
unsigned long inet_addr(const char*);

// from w32api/ws2tcpip.h
#define NI_MAXHOST	1025
struct addrinfo {
	int     ai_flags;
	int     ai_family;
	int     ai_socktype;
	int     ai_protocol;
	size_t  ai_addrlen;
	char   *ai_canonname;
	struct sockaddr  *ai_addr;
	struct addrinfo  *ai_next;
};

static __inline int getaddrinfo(const char* addr, const char* port,
        struct addrinfo* ai_hints, struct addrinfo** ai_resp) {
    (void)ai_hints;
    int status = 0;
    struct hostent* host = gethostbyname(addr);
    char* host_ip = inet_ntoa(*(struct in_addr*)*host->h_addr_list);
    struct addrinfo* ai_res;
    ai_res = (struct addrinfo*) malloc(sizeof(struct addrinfo));
    ai_res->ai_next = NULL;
    ai_res->ai_family = AF_INET;
    ai_res->ai_protocol = IPPROTO_TCP;
    struct sockaddr_in *fix_addr = (struct sockaddr_in*)malloc(sizeof(struct sockaddr_in));
    fix_addr->sin_family = AF_INET;
    fix_addr->sin_addr.s_addr = inet_addr(host_ip);
    unsigned short read_port;
    sscanf(port, "%hu", &read_port);
    fix_addr->sin_port = htons(read_port);

    ai_res->ai_addrlen = sizeof(*fix_addr);
    ai_res->ai_canonname = strdup(addr);
    ai_res->ai_addr = (struct sockaddr*)fix_addr;
    status = errno;
    *ai_resp = ai_res;
    return status; // tbd: better errors?
}
static __inline void freeaddrinfo(struct addrinfo* ai) {
    free(ai->ai_canonname);
    free(ai->ai_addr);
    free(ai);
}

static __inline const char* gai_strerror(int status) {
    (void)status;
    return "unknown error";
}

#if 0
#pragma comment(lib, "Ws2_32") 
void __declspec(dllimport) freeaddrinfo (struct addrinfo*);
int __declspec(dllimport) getaddrinfo (const char*,const char*,const struct addrinfo*,
		        struct addrinfo**);
int __declspec(dllimport) getnameinfo(const struct sockaddr*,socklen_t,char*,DWORD,
		       char*,DWORD,int);

DWORD __declspec(dllimport) FormatMessageA(DWORD,PCVOID,DWORD,DWORD,LPSTR,DWORD,va_list*);
DWORD __declspec(dllimport) FormatMessageW(DWORD,PCVOID,DWORD,DWORD,LPWSTR,DWORD,va_list*);

static __inline char*
gai_strerrorA(int ecode)
{
	static char message[1024+1];
	DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM
	              | FORMAT_MESSAGE_IGNORE_INSERTS
		      | FORMAT_MESSAGE_MAX_WIDTH_MASK;
	DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
  	FormatMessageA(dwFlags, NULL, ecode, dwLanguageId, (LPSTR)message, 1024, NULL);
	return message;
}
static __inline WCHAR*
gai_strerrorW(int ecode)
{
	static WCHAR message[1024+1];
	DWORD dwFlags = FORMAT_MESSAGE_FROM_SYSTEM
	              | FORMAT_MESSAGE_IGNORE_INSERTS
		      | FORMAT_MESSAGE_MAX_WIDTH_MASK;
	DWORD dwLanguageId = MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT);
  	FormatMessageW(dwFlags, NULL, ecode, dwLanguageId, (LPWSTR)message, 1024, NULL);
	return message;
}
#ifdef UNICODE
#define gai_strerror gai_strerrorW
#else
#define gai_strerror gai_strerrorA
#endif
#endif

#endif // FIX_CYGWIN_H

