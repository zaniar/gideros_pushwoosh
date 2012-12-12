package com.nightspade.gideros.android.plugins.pushwoosh;

import java.lang.ref.WeakReference;

import org.json.JSONException;
import org.json.JSONObject;

import com.arellomobile.android.push.PushManager;

import android.app.Activity;
import android.content.Intent;

public class GPushWoosh {
	private static WeakReference<Activity> sActivity;
	private static GPushWoosh sInstance;
	private static long sData;
	private static PushManager sPushManager;
	private static boolean sEnable;
	
	public static void onCreate(Activity activity)
	{
		sActivity = new WeakReference<Activity>(activity);
	}
	
	static public void init(long data)
	{
		sData = data;
		sInstance = new GPushWoosh(sActivity.get());
	}
	
	static public void cleanup()
	{
		if (sInstance != null)
		{
			sData = 0;
			sInstance = null;
		}
	}
	
	public GPushWoosh(Activity activity)
	{
		sEnable = false;
	}
	
	static public void onDestroy()
	{
		cleanup();
	}
	
	static public void setId(String appId, String senderId)
	{
		sPushManager = new PushManager(sActivity.get(), appId, senderId);
		sPushManager.onStartup(sActivity.get());
        
        sEnable = true;
        
        checkMessage(sActivity.get().getIntent());
	}
	
	static public void startTrackingGeoPushes()
	{
		sPushManager.startTrackingGeoPushes();
	}
	
	static public void stopTrackingGeoPushes()
	{
		sPushManager.stopTrackingGeoPushes();
	}
	
	static public void unregister()
	{
		sPushManager.unregister();
	}
	
	static public String getCustomData()
	{
		return sPushManager.getCustomData();
	}
	
	static public void setMultiNotificationMode()
	{
		sPushManager.setMultiNotificationMode();
	}
	
	static public void setSimpleNotificationMode()
	{
		sPushManager.setSimpleNotificationMode();
	}
	
	static public void checkMessage(Intent intent)
	{
		if (!sEnable)
		{
			return;
		}
		
		if (null != intent)
    	{
    		if (intent.hasExtra(PushManager.PUSH_RECEIVE_EVENT))
            {    			
    			if (sData != 0) {
    				JSONObject json;
					try {
						json = new JSONObject(intent.getExtras().getString(PushManager.PUSH_RECEIVE_EVENT));
						onPushReceive(json.getString("title"), sData);
					} catch (JSONException e) {
						e.printStackTrace();
					}
    			}
            }
            else if (intent.hasExtra(PushManager.REGISTER_EVENT))
            {
            	if (sData != 0)
            		onRegister(sData);
            }
            else if (intent.hasExtra(PushManager.UNREGISTER_EVENT))
            {
            	if (sData != 0)
            		onUnregister(sData);
            }
            else if (intent.hasExtra(PushManager.REGISTER_ERROR_EVENT))
            {
            	if (sData != 0)
            		onRegisterError(sData);
            }
            else if (intent.hasExtra(PushManager.UNREGISTER_ERROR_EVENT))
            {
            	if (sData != 0)
            		onUnregisterError(sData);
            }
    		
    		resetIntentValues();
    	}
	}
	
	static public void resetIntentValues()
	{
		Intent mainAppIntent = sActivity.get().getIntent();
	
		if (mainAppIntent.hasExtra(PushManager.PUSH_RECEIVE_EVENT))
		{
			mainAppIntent.putExtra(PushManager.PUSH_RECEIVE_EVENT, (String) null);
		}
		else if (mainAppIntent.hasExtra(PushManager.REGISTER_EVENT))
		{
			mainAppIntent.putExtra(PushManager.REGISTER_EVENT, (String) null);
		}
		else if (mainAppIntent.hasExtra(PushManager.UNREGISTER_EVENT))
		{
			mainAppIntent.putExtra(PushManager.UNREGISTER_EVENT, (String) null);
		}
		else if (mainAppIntent.hasExtra(PushManager.REGISTER_ERROR_EVENT))
		{
			mainAppIntent.putExtra(PushManager.REGISTER_ERROR_EVENT, (String) null);
		}
		else if (mainAppIntent.hasExtra(PushManager.UNREGISTER_ERROR_EVENT))
		{
			mainAppIntent.putExtra(PushManager.UNREGISTER_ERROR_EVENT, (String) null);
		}
	
		sActivity.get().setIntent(mainAppIntent);
	}
	
	private static native void onPushReceive(String message, long data);
	private static native void onRegister(long data);
	private static native void onUnregister(long data);
	private static native void onRegisterError(long data);
	private static native void onUnregisterError(long data);
}
