#include <stdio.h>
#include <windows.h>
#include <tchar.h>
#include <time.h>
#include <windef.h>
#include <fcntl.h>
#include "psapi.h"

#define ACT_SEQ 1
#define ACT_TIME 2
#define ACT_TICK 4
#define ACT_PID 8
#define ACT_NAME 16
#define ACT_TAB 32
#define ACT_ASIS 64

#define WINDOW_TITLE "bug"

/*
   print OutputDebugString to stdout until Esc

   compiled to just 7k using tinycc
   http://bellard.org/tcc/

   get psapi.h from mingw package or seek on koders.com
   get psapi.def:
        tiny_impdef.exe psapi.dll
   compile:
        tcc bug.c psapi.def
*/

struct dbwin_buffer {
    DWORD   pid;
    char    data[4096 - sizeof(DWORD)];
};

typedef struct  {
    int len;
    DWORD *_;
} aDWORD;

typedef struct {
    int len;
    char **_;
} astring;

DWORD NameOfPid(DWORD pid, char *name)
{
    HANDLE hProcess = OpenProcess(
                          PROCESS_QUERY_INFORMATION | PROCESS_VM_READ,
                          FALSE,
                          pid);
    if (NULL != hProcess) {
        HMODULE hMod;
        DWORD cbNeeded, r = 0, i;
        if (EnumProcessModules(hProcess, &hMod, sizeof(hMod),
                               &cbNeeded))
            r = GetModuleBaseName(hProcess, hMod, name, MAX_PATH);
        CloseHandle(hProcess);
        for (i = r; i > 0; i--) {
            if (name[i] == '.') name[i] = 0;
        }
        return(r);
    }
    else
        return(0);
}

int aaDWORD(aDWORD *a, DWORD v)
{
    DWORD *x;
    x = (DWORD *) realloc((*a)._, ((*a).len + 1) * sizeof(DWORD));
    if (x != NULL) {
        (*a)._ = x;
        (*a)._[(*a).len] = v;
        (*a).len ++;
    }
    return((*a).len);
}

int aastring(astring *a, char *v)
{
    char **x;
    x = (char **)  realloc((*a)._, ((*a).len + 1) * sizeof(char *));
    if (x != NULL) {
        (*a)._ = x;
        int l = strlen(v);
        (*a)._[(*a).len] = (char *) malloc((l + 1) * sizeof(char *));
        memset((*a)._[(*a).len], 0, l + 1);
        strcpy((*a)._[(*a).len], v);
        (*a).len++;
    }
    return((*a).len);
}

int astringpos(char *needle, astring a)
{
    int j = -1;
    int i = 0;
    for (i = 0; (i < a.len) && (j < 0); i ++)
        if (stricmp(needle, a._[i]) == 0) j = i;
    return(j);
}

int aDWORDpos(DWORD needle, aDWORD a)
{
    int j = -1;
    int i = 0;
    for (i = 0; (i < a.len) && (j < 0); i ++)
        if (a._[i] == needle) j = i;
    return(j);
}

void fmtTick(DWORD t, char *c)
{
    DWORD h, n, s, r;
    s = t / 1000;
    r = t % 1000;
    n = s / 60;
    s = s % 60;
    h = n / 60;
    n = n % 60;
    if (h > 0)
        sprintf(c, "%02d:%02d:%02d.%03d", h, n, s, r);
    else {
        if (n > 0)
            sprintf(c, "%02d:%02d.%03d", n, s, r);
        else
            sprintf(c, "%02d.%03d", s, r);
    }
}

void ShowHelp()
{
    printf(
        "bug -option[s] allowed\n"
        "print OutputDebugString to stdout until Esc\n\n"
        "options :\n"
        "-seq:  sequence\n"
        "-time: show time\n"
        "-tick: hh:nn:ss.ss.zzz tick\n"
        "-pid:  process id\n"
        "-name: process name\n"
        "-asis: no linebreak\n"
        "-tab \"char\": set delimiter\n\n"
        "allowed: allowed process name\n\n");
    exit(0);
}

int freeastring(astring *a)
{
    int i ;
    for (i = 0; i < (*a).len; i++) {
        free((*a)._[i]);
    }
    free((*a)._);
    free(*a);
}

int freeaDWORD(aDWORD *a)
{
    int i ;
    for (i = 0; i < (*a).len; i++) {
        free((*a)._[i]);
    }
    free(*a);
}

BOOL run = TRUE;
aDWORD apid = {0, NULL};
aDWORD act = {0, NULL};
aDWORD aban = {0, NULL};
astring a = {0, NULL};
astring aName = {0, NULL};
HANDLE hInput;
BOOL bAsIs = FALSE;
char *separator = "\t";
struct dbwin_buffer *p;
char *process;
DWORD started, seq, tick;

DWORD ParseParam(int argc, char *argv[])
{
    int i, j;
    char *ax[] = {"seq", "time", "tick", "pid",
                  "name", "asis", "tab"
                 };
    int ai[] = {ACT_SEQ, ACT_TIME, ACT_TICK, ACT_PID,
                ACT_NAME, ACT_ASIS, ACT_TAB
               };
    DWORD acts = 0;
    DWORD axt = 0;
    for (i = 1; i < argc; i++) {
        if (strncmp(argv[i], "-", 1) == 0) {
            axt = 0;
            for (j = 0; j < 7; j++) {
                if (stricmp(argv[i] + 1, ax[j]) == 0) {
                    axt = ai[j];
                    break;
                }
            }
            if (axt > 0) {
                acts = acts | axt;
                switch (axt)  {
                case ACT_ASIS:
                    bAsIs = TRUE;
                    break;
                case ACT_TAB:
                    if (argc >= (i + 1)) {
                        separator = (char *)
                                    malloc(strlen(argv[i + 1])
                                           * sizeof(char));
                        strcpy(separator, argv[i + 1]);
                        ++i;
                    }
                    break;
                default:
                    aaDWORD(&act, axt);
                }
            }
            else ShowHelp();
        }
        else {
            aastring(&a, argv[i]);
        }
    }
}

void PrintLine()
{
    int i;
    time_t now;
    struct tm *mytime;
    char buffer[32];
    for (i = 0; i < act.len ; i++) {
        switch (act._[i]) {
        case ACT_SEQ:
            printf("%d%s", seq, separator);
            break;
        case ACT_TIME:
            if (time(&now) != (time_t)(-1) && mytime) {
                mytime = localtime(&now);
                if (strftime(buffer, sizeof buffer, "%X", mytime))
                    printf("%s%s", buffer, separator);
            }
            break;
        case ACT_TICK:
            tick = GetTickCount() - started;
            fmtTick(tick, buffer);
            printf("%s%s", buffer, separator);
            break;
        case ACT_PID:
            printf("%d%s", p->pid, separator);
            break;
        case ACT_NAME:
            printf("%s%s", process, separator);
            break;
        }
    }
    printf("%s", p->data);
    if (!bAsIs)
        printf("\n");
}

BOOL Allowed()
{
    int i, n, k;
    BOOL r = TRUE;
    do {
        if (a.len > 0) {
            n = aDWORDpos(p->pid, aban);
            if (n >= 0) {
                r = FALSE;
                break;
            }
            n = aDWORDpos(p->pid, apid);
            if (n < 0) {
                if (NameOfPid(p->pid, process))
                    k = astringpos(process, a);
                else
                    k = 0;
                if (k >= 0) {
                    aaDWORD(&apid, p->pid);
                    aastring(&aName, process);
                }
                else {
                    aaDWORD(&aban, p->pid);
                    r = FALSE;
                    break;
                }
            }
            else {
                strcpy(process, aName._[n]);
            }
        }
        else {
            n = aDWORDpos(p->pid, apid);
            if (n < 0) {
                if (NameOfPid(p->pid, process)) {
                    i = aaDWORD(&apid, p->pid);
                    i = aastring(&aName, process);
                }
            }
            else {
                strcpy(process, aName._[n]);
            }
        }
    }
    while (FALSE);
    return(r);
}

void OnKeyUp(WORD key)
{
    int i;
    switch (key) {
    case VK_ESCAPE:
        run = FALSE;
        break;
    case VK_SPACE:
        for (i = 0; i < apid.len; i++)
            printf("[%d] %d %s\n", i, apid._[i], aName._[i]);
        break;
    }
}

void OnKeyEvent()
{
    int i, j;
    DWORD numEvents = 0;
    DWORD numEventsRead = 0;
    GetNumberOfConsoleInputEvents(hInput, &numEvents);
    if (numEvents != 0) {
        INPUT_RECORD *eventBuffer =
            (INPUT_RECORD *)
            malloc(numEvents * sizeof(INPUT_RECORD));
        ReadConsoleInput(
            hInput,
            eventBuffer,
            numEvents,
            &numEventsRead);
        for (i = 0; i < numEventsRead; ++i)
            if (eventBuffer[i].EventType == KEY_EVENT
                && !eventBuffer[i].Event.KeyEvent.bKeyDown)
                OnKeyUp((WORD)
                        eventBuffer[i].Event.KeyEvent.wVirtualKeyCode);
        for (i = numEvents - 1; i >= 0; i--)
            free(eventBuffer + i);
    }
}

int main(int argc, char *argv[])
{
    HANDLE hOk, hData, hs;
    HANDLE *handles;
    started = GetTickCount();
    ParseParam(argc, argv);
    do {
        p = NULL;
        hData = 0;
        hs = 0;
        hOk = CreateEvent(
                  NULL,
                  FALSE,    // auto-reset
                  TRUE,    // initial state: signaled
                  "DBWIN_BUFFER_READY");
        if (hOk == 0) {
            printf("not ready\n");
            break;
        }
        hData = CreateEvent(
                    NULL,
                    FALSE,    // auto-reset
                    FALSE,    // initial state: nonsignaled
                    "DBWIN_DATA_READY");
        if (hData == 0) {
            printf("no data\n");
            break;
        }
        hs = CreateFileMapping(
                 INVALID_HANDLE_VALUE,
                 NULL,
                 PAGE_READWRITE,
                 0,
                 sizeof(struct dbwin_buffer),
                 "DBWIN_BUFFER");
        if (hData == 0) {
            printf("no buffer\n");
            break;
        }
        p = MapViewOfFile(hs, SECTION_MAP_READ, 0, 0, 0);
        if (p == NULL) {
            printf("no view\n");
            break;
        }
        seq = 0;
        process = (char *) malloc(MAX_PATH * sizeof(char));
        memset(process, 0, MAX_PATH);
        hInput = GetStdHandle(STD_INPUT_HANDLE);
        handles = (HANDLE *) malloc(2 * sizeof(HANDLE));
        handles[0] = hData;
        handles[1] = hInput;
        SetConsoleTitle(WINDOW_TITLE);
        if (bAsIs) setmode(STDOUT_FILENO, _O_BINARY);
        DWORD r;
        do {
            SetEvent(hOk);
            r = WaitForMultipleObjects(2, handles, FALSE, INFINITE);
            switch (r) {
            case WAIT_OBJECT_0:
                seq++;
                if (Allowed()) PrintLine();
                break;
            case WAIT_OBJECT_0 + 1:
                OnKeyEvent();
                break;
            }
        }
        while (run && (r == WAIT_OBJECT_0 || r == (WAIT_OBJECT_0 + 1)));
    }
    while (FALSE);
    if (p != NULL) UnmapViewOfFile(p);
    if (hOk != 0) CloseHandle(hOk);
    if (hInput != 0) CloseHandle(hInput);
    if (hData != 0) CloseHandle(hData);
    if (hs != 0) CloseHandle(hs);
    free(handles);
    free(separator);
    free(process);
    freeastring(&a);
    freeastring(&aName);
    freeaDWORD(&apid);
    freeaDWORD(&aban);
    freeaDWORD(&act);
}
