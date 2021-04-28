#include "fastfetch.h"

#include <string.h>
#include <dirent.h>
#include <pthread.h>

#define FF_WM_MODULE_NAME "WM"
#define FF_WM_NUM_FORMAT_ARGS 3

const char* ffGetSessionDesktop()
{
    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static char* sessionDesktop = NULL;

    pthread_mutex_lock(&mutex);

    if(sessionDesktop == NULL)
        sessionDesktop = getenv("XDG_CURRENT_DESKTOP");

    if(sessionDesktop == NULL)
        sessionDesktop = getenv("XDG_SESSION_DESKTOP");

    pthread_mutex_unlock(&mutex);

    return sessionDesktop;
}

static void getFromProcDir(FFWMResult* result)
{
    DIR* proc = opendir("/proc/");
    if(proc == NULL)
        return;

    struct dirent* dirent;

    while((dirent = readdir(proc)) != NULL)
    {
        if(dirent->d_type != DT_DIR)
            continue;

        char path[20];
        sprintf(path, "/proc/%.8s/comm", dirent->d_name);

        ffGetFileContent(path, &result->processName);

        if(ffStrbufIgnCaseCompS(&result->processName, "kwin_wayland") == 0)
        {
            ffStrbufSetS(&result->prettyName, "KWin");
            ffStrbufSetS(&result->protocolName, "Wayland");
        }
        else if(ffStrbufIgnCaseCompS(&result->processName, "kwin_x11") == 0)
        {
            ffStrbufSetS(&result->prettyName, "KWin");
            ffStrbufSetS(&result->protocolName, "X11");
        }
        else if(ffStrbufIgnCaseCompS(&result->processName, "openbox") == 0)
            ffStrbufSetS(&result->prettyName, "Openbox");
		else if(ffStrbufIgnCaseCompS(&result->processName, "cinnamon") == 0)
            ffStrbufSetS(&result->prettyName, "Muffin");
		else if(ffStrbufIgnCaseCompS(&result->processName, "xfwm4") == 0)
            ffStrbufSetS(&result->prettyName, "XFWM");
		else if(ffStrbufIgnCaseCompS(&result->processName, "wayfire") == 0)
            ffStrbufSetS(&result->prettyName, "Wayfire");

        if(result->prettyName.length > 0)
            break;
    }

    closedir(proc);
}

static void getSessionType(FFstrbuf* sessionType)
{
    const char* xdgSessionType = getenv("XDG_SESSION_TYPE");
    if (xdgSessionType == NULL)
        return;

    else if(strcasecmp(xdgSessionType, "wayland") == 0)
        ffStrbufSetS(sessionType, "Wayland");
    else if(strcasecmp(xdgSessionType, "x11") == 0)
        ffStrbufSetS(sessionType, "X11");
    else if(strcasecmp(xdgSessionType, "tty") == 0)
        ffStrbufSetS(sessionType, "TTY");
    else if(strcasecmp(xdgSessionType, "mir") == 0)
        ffStrbufSetS(sessionType, "Mir");
    else
        ffStrbufSetS(sessionType, xdgSessionType);
}

const FFWMResult* ffDetectWM(FFinstance* instance)
{
    UNUSED(instance);

    static pthread_mutex_t mutex = PTHREAD_MUTEX_INITIALIZER;
    static FFWMResult result;
    static bool init = false;
    pthread_mutex_lock(&mutex);
    if(init)
    {
        pthread_mutex_unlock(&mutex);
        return &result;
    }
    init = true;

    ffStrbufInit(&result.processName);
    ffStrbufInit(&result.prettyName);
    ffStrbufInit(&result.protocolName);
    ffStrbufInit(&result.error);

    getFromProcDir(&result);

    if(result.prettyName.length == 0)
    {
        const char* sessionDesktop = ffGetSessionDesktop();
        ffStrbufSetS(&result.processName, sessionDesktop);
        ffStrbufSetS(&result.prettyName, sessionDesktop);
    }

    if(result.protocolName.length == 0)
        getSessionType(&result.protocolName);

    if(result.prettyName.length == 0 && result.protocolName.length == 0)
    {
        ffStrbufClear(&result.processName);
        ffStrbufSetS(&result.error, "No WM or WM protocol type found");
    }

    pthread_mutex_unlock(&mutex);

    return &result;
}

void ffPrintWM(FFinstance* instance)
{
    const FFWMResult* result = ffDetectWM(instance);

    if(result->error.length > 0)
    {
        ffPrintError(instance, FF_WM_MODULE_NAME, 0, &instance->config.wmKey, &instance->config.wmFormat, FF_WM_NUM_FORMAT_ARGS, result->error.chars);
        return;
    }

    if(instance->config.wmFormat.length == 0)
    {
        ffPrintLogoAndKey(instance, "WM", 0, &instance->config.wmKey);

        if(result->prettyName.length == 0 && result->processName.length == 0)
        {
            ffStrbufPutTo(&result->protocolName, stdout);
        }
        else
        {
            if(result->prettyName.length > 0)
                ffStrbufWriteTo(&result->prettyName, stdout);
            else
                ffStrbufWriteTo(&result->processName, stdout);

            if(result->protocolName.length > 0)
            {
                fputs(" (", stdout);
                ffStrbufWriteTo(&result->protocolName, stdout);
                putchar(')');
            }

            putchar('\n');
        }
    }
    else
    {
        ffPrintFormatString(instance, FF_WM_MODULE_NAME, 0, &instance->config.wmKey, &instance->config.wmFormat, NULL, FF_WM_NUM_FORMAT_ARGS, (FFformatarg[]){
            {FF_FORMAT_ARG_TYPE_STRBUF, &result->processName},
            {FF_FORMAT_ARG_TYPE_STRBUF, &result->prettyName},
            {FF_FORMAT_ARG_TYPE_STRBUF, &result->protocolName}
        });
    }
}
