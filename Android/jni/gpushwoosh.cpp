#include <gpushwoosh.h>
#include <jni.h>
#include <stdlib.h>
#include <glog.h>

#include <android/log.h>

extern "C" {
JavaVM *g_getJavaVM();
JNIEnv *g_getJNIEnv();
}

class GPushWoosh
{
public:
	GPushWoosh()
	{
		gid_ = g_nextgid();

		JNIEnv *env = g_getJNIEnv();

		jclass localClass = env->FindClass("com/nightspade/gideros/android/plugins/pushwoosh/GPushWoosh");
		cls_ = (jclass)env->NewGlobalRef(localClass);
		env->DeleteLocalRef(localClass);

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "init", "(J)V"), (jlong)this);
	}

	~GPushWoosh()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "cleanup", "()V"));

		env->DeleteGlobalRef(cls_);

		gevent_removeEventsWithGid(gid_);
	}

	void onPushReceive(jstring jtitle)
	{
		JNIEnv *env = g_getJNIEnv();

		const char *title = env->GetStringUTFChars(jtitle, NULL);

		gpushwoosh_PushReceiveEvent *event = (gpushwoosh_PushReceiveEvent*)gevent_createEventStruct1(
			sizeof(gpushwoosh_PushReceiveEvent),
			offsetof(gpushwoosh_PushReceiveEvent, title), title
		);

		env->ReleaseStringUTFChars(jtitle, title);

		gevent_enqueueEvent(gid_, callback_s, GPUSHWOOSH_PUSH_RECEIVE_EVENT, event, 1, this);
	}

	void onRegister()
	{
		gevent_enqueueEvent(gid_, callback_s, GPUSHWOOSH_REGISTER_EVENT, NULL, 1, this);
	}

	void onUnregister()
	{
		gevent_enqueueEvent(gid_, callback_s, GPUSHWOOSH_UNREGISTER_EVENT, NULL, 1, this);
	}

	void onRegisterError()
	{
		gevent_enqueueEvent(gid_, callback_s, GPUSHWOOSH_REGISTER_ERROR_EVENT, NULL, 1, this);
	}

	void onUnregisterError()
	{
		gevent_enqueueEvent(gid_, callback_s, GPUSHWOOSH_UNREGISTER_ERROR_EVENT, NULL, 1, this);
	}

	void setId(const char *appId, const char *senderId)
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "setId", "(Ljava/lang/String;Ljava/lang/String;)V"), env->NewStringUTF(appId), env->NewStringUTF(senderId));
	}

	void startTrackingGeoPushes()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "startTrackingGeoPushes", "(L)V"));
	}

	void stopTrackingGeoPushes()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "stopTrackingGeoPushes", "(L)V"));
	}

	void unregister()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "unregister", "(L)V"));
	}

	const char * getCustomData()
	{
		JNIEnv *env = g_getJNIEnv();

		jstring jresult = (jstring) env->CallStaticObjectMethod(cls_, env->GetStaticMethodID(cls_, "unregister", "(L)Ljava/lang/String;"));

		const char* result = env->GetStringUTFChars(jresult, NULL);

		env->ReleaseStringUTFChars(jresult, result);

		return result;
	}

	void setMultiNotificationMode()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "setMultiNotificationMode", "(L)V"));
	}

	void setSimpleNotificationMode()
	{
		JNIEnv *env = g_getJNIEnv();

		env->CallStaticVoidMethod(cls_, env->GetStaticMethodID(cls_, "setSimpleNotificationMode", "(L)V"));
	}

	g_id addCallback(gevent_Callback callback, void *udata)
	{
		return callbackList_.addCallback(callback, udata);
	}

	void removeCallback(gevent_Callback callback, void *udata)
	{
		callbackList_.removeCallback(callback, udata);
	}

	void removeCallbackWithGid(g_id gid)
	{
		callbackList_.removeCallbackWithGid(gid);
	}

private:
	static void callback_s(int type, void *event, void *udata)
	{
		((GPushWoosh*)udata)->callback(type, event);
	}

	void callback(int type, void *event)
	{
		callbackList_.dispatchEvent(type, event);
	}

private:
	gevent_CallbackList callbackList_;

private:
	jclass cls_;
	g_id gid_;
};

extern "C" {
	void Java_com_nightspade_gideros_android_plugins_pushwoosh_GPushWoosh_onPushReceive(JNIEnv *env, jclass clz, jstring message, jlong data)
	{
		((GPushWoosh*)data)->onPushReceive(message);
	}

	void Java_com_nightspade_gideros_android_plugins_pushwoosh_GPushWoosh_onRegister(JNIEnv *env, jclass clz, jlong data)
	{
		((GPushWoosh*)data)->onRegister();
	}

	void Java_com_nightspade_gideros_android_plugins_pushwoosh_GPushWoosh_onUnregister(JNIEnv *env, jclass clz, jlong data)
	{
		((GPushWoosh*)data)->onUnregister();
	}

	void Java_com_nightspade_gideros_android_plugins_pushwoosh_GPushWoosh_onRegisterError(JNIEnv *env, jclass clz, jlong data)
	{
		((GPushWoosh*)data)->onRegisterError();
	}

	void Java_com_nightspade_gideros_android_plugins_pushwoosh_GPushWoosh_onUnregisterError(JNIEnv *env, jclass clz, jlong data)
	{
		((GPushWoosh*)data)->onUnregisterError();
	}
}

static GPushWoosh *s_pushwoosh = NULL;

extern "C" {

int gpushwoosh_isAvailable()
{
	return 1;
}

void gpushwoosh_init()
{
	s_pushwoosh = new GPushWoosh();
}

void gpushwoosh_cleanup()
{
	delete s_pushwoosh;
	s_pushwoosh = NULL;
}

void gpushwoosh_setId(const char* appId, const char* senderId)
{
	s_pushwoosh->setId(appId, senderId);
}

void gpushwoosh_startTrackingGeoPushes()
{
	s_pushwoosh->startTrackingGeoPushes();
}

void gpushwoosh_stopTrackingGeoPushes()
{
	s_pushwoosh->stopTrackingGeoPushes();
}

void gpushwoosh_unregister()
{
	s_pushwoosh->unregister();
}

const char* gpushwoosh_getCustomData()
{
	return s_pushwoosh->getCustomData();
}

void gpushwoosh_setMultiNotificationMode()
{
	s_pushwoosh->setMultiNotificationMode();
}

void gpushwoosh_setSimpleNotificationMode()
{
	s_pushwoosh->setSimpleNotificationMode();
}

g_id gpushwoosh_addCallback(gevent_Callback callback, void *udata)
{
	return s_pushwoosh->addCallback(callback, udata);
}

void gpushwoosh_removeCallback(gevent_Callback callback, void *udata)
{
	s_pushwoosh->removeCallback(callback, udata);
}

void gpushwoosh_removeCallbackWithGid(g_id gid)
{
	s_pushwoosh->removeCallbackWithGid(gid);
}

}
