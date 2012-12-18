#ifndef GPUSHWOOSH_H
#define GPUSHWOOSH_H

#include <gglobal.h>
#include <gevent.h>

enum
{
	GPUSHWOOSH_PUSH_RECEIVE_EVENT,
	GPUSHWOOSH_REGISTER_EVENT,
	GPUSHWOOSH_UNREGISTER_EVENT,
	GPUSHWOOSH_REGISTER_ERROR_EVENT,
	GPUSHWOOSH_UNREGISTER_ERROR_EVENT
};

typedef struct gpushwoosh_PushReceiveEvent
{
	const char *title;
} gpushwoosh_PushReceiveEvent;

#ifdef __cplusplus
extern "C" {
#endif

G_API int gpushwoosh_isAvailable();

G_API void gpushwoosh_init();
G_API void gpushwoosh_cleanup();

G_API void gpushwoosh_setId(const char* appId, const char* senderId);
G_API void gpushwoosh_startTrackingGeoPushes();
G_API void gpushwoosh_stopTrackingGeoPushes();
G_API void gpushwoosh_unregister();
G_API const char* gpushwoosh_getCustomData();
G_API void gpushwoosh_setMultiNotificationMode();
G_API void gpushwoosh_setSimpleNotificationMode();

G_API g_id gpushwoosh_addCallback(gevent_Callback callback, void *udata);
G_API void gpushwoosh_removeCallback(gevent_Callback callback, void *udata);
G_API void gpushwoosh_removeCallbackWithGid(g_id gid);

#ifdef __cplusplus
}
#endif

#endif
