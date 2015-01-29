/*
 * Copyright (C) 2013-2014, The Linux Foundation. All rights reserved.
 * Not a Contribution.
 *
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#define LOG_TAG "BluetoothAvrcpServiceJni"

//#define LOG_NDEBUG 0

#include "com_android_bluetooth.h"
#include "hardware/bt_rc.h"
#include "utils/Log.h"
#include "android_runtime/AndroidRuntime.h"

#include <string.h>

namespace android {
static jmethodID method_getRcFeatures;
static jmethodID method_getPlayStatus;
static jmethodID method_onListPlayerAttributeRequest;
static jmethodID method_getElementAttr;
static jmethodID method_registerNotification;
static jmethodID method_volumeChangeCallback;
static jmethodID method_handlePassthroughCmd;
static jmethodID method_handlePassthroughRsp;
static jmethodID method_getFolderItems;
static jmethodID method_setAddressedPlayer;
static jmethodID method_setBrowsedPlayer;
static jmethodID method_changePath;
static jmethodID method_playItem;
static jmethodID method_getItemAttr;
static jmethodID method_onListPlayerAttributeValues;
static jmethodID method_onGetPlayerAttributeValues;
static jmethodID method_setPlayerAppSetting;
static jmethodID method_getplayerattribute_text;
static jmethodID method_getplayervalue_text;
static jmethodID method_onConnectionStateChanged;

static const btrc_interface_t *sBluetoothMultiAvrcpInterface = NULL;
static jobject mCallbacksObj = NULL;
static JNIEnv *sCallbackEnv = NULL;

static bool checkCallbackThread() {
    // Always fetch the latest callbackEnv from AdapterService.
    // Caching this could cause this sCallbackEnv to go out-of-sync
    // with the AdapterService's ENV if an ASSOCIATE/DISASSOCIATE event
    // is received
    sCallbackEnv = getCallbackEnv();

    JNIEnv* env = AndroidRuntime::getJNIEnv();
    if (sCallbackEnv != env || sCallbackEnv == NULL) return false;
    return true;
}

static void btavrcp_remote_features_callback(bt_bdaddr_t* bd_addr, btrc_remote_features_t features) {
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for remote features");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }

    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_getRcFeatures, addr,
                                                         (jint)features, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);

}

static void btavrcp_get_play_status_callback(bt_bdaddr_t* bd_addr) {
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for get play status");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);

    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_getPlayStatus, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_get_player_seeting_value_callback(btrc_player_attr_t player_att,
                                                     bt_bdaddr_t* bd_addr) {
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for player seeting");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);

    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj ,method_onListPlayerAttributeValues,
                                    (jbyte)player_att, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_get_player_attribute_id_callback(bt_bdaddr_t* bd_addr) {
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for player attribute");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);

    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj,method_onListPlayerAttributeRequest, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_getcurrent_player_app_setting_values( uint8_t num_attr,
                                                          btrc_player_attr_t *p_attrs,
                                                          bt_bdaddr_t* bd_addr) {
    jintArray attrs;
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for player app setting");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    attrs = (jintArray)sCallbackEnv->NewIntArray(num_attr);
    if (!attrs) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetIntArrayRegion(attrs, 0, num_attr, (jint *)p_attrs);

    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj,method_onGetPlayerAttributeValues,
                                     (jbyte)num_attr,attrs, addr);
    }
    else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(attrs);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_set_playerapp_setting_value_callback(btrc_player_settings_t *attr,
                                                         bt_bdaddr_t* bd_addr)
{
    jbyteArray attrs_ids;
    jbyteArray attrs_value;
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for set playerapp");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    attrs_ids   = (jbyteArray)sCallbackEnv->NewByteArray(attr->num_attr);
    if (!attrs_ids) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(attrs_ids, 0, attr->num_attr, (jbyte *)attr->attr_ids);
    attrs_value = (jbyteArray)sCallbackEnv->NewByteArray(attr->num_attr);
    if (!attrs_value) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(attrs_value, 0, attr->num_attr, (jbyte *)attr->attr_values);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_setPlayerAppSetting,
                            (jbyte)attr->num_attr ,attrs_ids ,attrs_value, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    sCallbackEnv->DeleteLocalRef(attrs_ids);
    sCallbackEnv->DeleteLocalRef(attrs_value);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_getPlayer_app_attribute_text(uint8_t num , btrc_player_attr_t *att,
                                                 bt_bdaddr_t* bd_addr)
{
    jbyteArray attrs;
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for getPlayer app");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    attrs   = (jbyteArray)sCallbackEnv->NewByteArray(num);
    if (!attrs) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(attrs, 0, num, (jbyte *)att);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_getplayerattribute_text,
                                     (jbyte) num ,attrs, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    sCallbackEnv->DeleteLocalRef(attrs);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_getPlayer_app_value_text(uint8_t attr_id , uint8_t num_val , uint8_t *value,
                                             bt_bdaddr_t* bd_addr)
{
    jbyteArray Attr_Value ;
    ALOGV("%s", __FUNCTION__);
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for getPlayer app");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    Attr_Value   = (jbyteArray)sCallbackEnv->NewByteArray(num_val);
    if (!Attr_Value) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(Attr_Value, 0, num_val, (jbyte *)value);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_getplayervalue_text,(jbyte) attr_id,
                                     (jbyte) num_val , Attr_Value, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    sCallbackEnv->DeleteLocalRef(Attr_Value);
}

static void btavrcp_get_element_attr_callback(uint8_t num_attr, btrc_media_attr_t *p_attrs,
                                              bt_bdaddr_t* bd_addr) {
    jintArray attrs;
    jbyteArray addr;

    ALOGV("%s", __FUNCTION__);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for element attr");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    attrs = (jintArray)sCallbackEnv->NewIntArray(num_attr);
    if (!attrs) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetIntArrayRegion(attrs, 0, num_attr, (jint *)p_attrs);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_getElementAttr, (jbyte)num_attr,
                                    attrs, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(attrs);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_register_notification_callback(btrc_event_id_t event_id, uint32_t param,
                                                   bt_bdaddr_t* bd_addr) {
    jbyteArray addr;
    ALOGV("%s", __FUNCTION__);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for register notification");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_registerNotification,
                                    (jint)event_id, (jint)param, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_volume_change_callback(uint8_t volume, uint8_t ctype, bt_bdaddr_t* bd_addr) {

    jbyteArray addr;
    ALOGV("%s", __FUNCTION__);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for volume change");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_volumeChangeCallback, (jint)volume,
                                    (jint)ctype, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }

    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_get_folder_items_callback(btrc_browse_folderitem_t scope ,
                                                        btrc_getfolderitem_t *param,
                                                        bt_bdaddr_t* bd_addr) {
    jlong start = param->start_item;
    jlong end = param->end_item;
    jint size = param->size;
    jint num_attr = param->attr_count;
    jintArray attrs;
    jbyteArray addr;

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for get folder items");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (num_attr == 0xff) {
        num_attr = 0; // 0xff signifies no attribute required in response
    } else if (num_attr == 0) {
        num_attr = 7; // 0x00 signifies all attributes required in response
    }

    attrs = (jintArray)sCallbackEnv->NewIntArray(num_attr);
    if (!attrs) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetIntArrayRegion(attrs, 0, num_attr, (jint *)param->attrs);

    ALOGV("%s", __FUNCTION__);
    ALOGI("scope: %d", scope);
    ALOGI("start entry: %d", start);
    ALOGI("end entry: %d", end);
    ALOGI("size: %d", size);

    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_getFolderItems, (jbyte)scope,
                                     start, end, size, num_attr, attrs, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }

    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(attrs);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_passthrough_command_callback(int id, int pressed , bt_bdaddr_t* bd_addr) {
    jbyteArray addr;
    ALOGV("%s", __FUNCTION__);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for passthrough command");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_handlePassthroughCmd, (jint)id,
                                     (jint)pressed, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_set_addressed_player_callback(uint32_t player_id, bt_bdaddr_t* bd_addr) {
    jbyteArray addr;
    ALOGV("%s", __FUNCTION__);
    ALOGV("player id: %d", player_id);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for set addressed player");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);

    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_setAddressedPlayer, (jint)player_id,
                                     addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }

    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_set_browsed_player_callback(uint32_t player_id, bt_bdaddr_t* bd_addr) {
    jbyteArray addr;
    ALOGV("%s", __FUNCTION__);
    ALOGV("player id: %d", player_id);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for set browsed player");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_setBrowsedPlayer, (jint)player_id,
                                     addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_change_path_callback(uint8_t direction, uint64_t uid, bt_bdaddr_t* bd_addr) {
    jbyteArray addr;
    ALOGV("%s", __FUNCTION__);
    ALOGV("direction: %d, uid: %lu", direction, uid);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for change path");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_changePath, (jbyte)direction,
                                     (jlong)uid, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_play_item_callback(uint8_t scope, uint64_t uid, bt_bdaddr_t* bd_addr) {
    jbyteArray addr;
    ALOGV("%s", __FUNCTION__);
    ALOGV("scope: %d, uid: %lu", scope, uid);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for play item");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_playItem, (jbyte)scope, (jlong)uid,
                                     addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }

    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_get_item_attr_callback(uint8_t scope, uint64_t uid,
                                    uint8_t num_attr, btrc_media_attr_t *p_attrs,
                                    bt_bdaddr_t* bd_addr) {
    jintArray attrs;
    jbyteArray addr;

    if (num_attr == 0xff) {
        num_attr = 0; // 0xff signifies no attribute required in response
    } else if (num_attr == 0) {
        num_attr = 7; // 0x00 signifies all attributes required in response
    }

    ALOGV("%s", __FUNCTION__);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }
    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for get item attr");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    attrs = (jintArray)sCallbackEnv->NewIntArray(num_attr);
    if (!attrs) {
        ALOGE("Fail to new jintArray for attrs");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }
    sCallbackEnv->SetIntArrayRegion(attrs, 0, num_attr, (jint *)p_attrs);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_getItemAttr, (jbyte)scope, (jlong)uid,
                                     (jbyte)num_attr, attrs, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }

    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(attrs);
    sCallbackEnv->DeleteLocalRef(addr);
}

static void btavrcp_connection_state_callback(bool state, bt_bdaddr_t* bd_addr) {
    jbyteArray addr;

    ALOGI("%s", __FUNCTION__);
    ALOGI("conn state: %d", state);

    if (!checkCallbackThread()) {
        ALOGE("Callback: '%s' is not called on the correct thread", __FUNCTION__);
        return;
    }

    addr = sCallbackEnv->NewByteArray(sizeof(bt_bdaddr_t));
    if (!addr) {
        ALOGE("Fail to new jbyteArray bd addr for connection state");
        checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
        return;
    }

    sCallbackEnv->SetByteArrayRegion(addr, 0, sizeof(bt_bdaddr_t), (jbyte*) bd_addr);
    if (mCallbacksObj) {
        sCallbackEnv->CallVoidMethod(mCallbacksObj, method_onConnectionStateChanged,
                                            (jboolean) state, addr);
    } else {
        ALOGE("%s: mCallbacksObj is null", __FUNCTION__);
    }
    checkAndClearExceptionFromCallback(sCallbackEnv, __FUNCTION__);
    sCallbackEnv->DeleteLocalRef(addr);
}


static btrc_callbacks_t sBluetoothAvrcpCallbacks = {
    sizeof(sBluetoothAvrcpCallbacks),
    btavrcp_remote_features_callback,
    btavrcp_get_play_status_callback,
    btavrcp_get_player_attribute_id_callback,
    btavrcp_get_player_seeting_value_callback,
    btavrcp_getcurrent_player_app_setting_values,
    btavrcp_getPlayer_app_attribute_text,
    btavrcp_getPlayer_app_value_text,
    btavrcp_set_playerapp_setting_value_callback,
    btavrcp_get_element_attr_callback,
    btavrcp_register_notification_callback,
    btavrcp_volume_change_callback,
    btavrcp_passthrough_command_callback,
    btavrcp_get_folder_items_callback,
    btavrcp_set_addressed_player_callback,
    btavrcp_set_browsed_player_callback,
    btavrcp_change_path_callback,
    btavrcp_play_item_callback,
    btavrcp_get_item_attr_callback,
    btavrcp_connection_state_callback
};

static void classInitNative(JNIEnv* env, jclass clazz) {
    method_getRcFeatures =
        env->GetMethodID(clazz, "getRcFeatures", "([BI)V");
    method_getPlayStatus =
        env->GetMethodID(clazz, "getPlayStatus", "([B)V");
    method_onListPlayerAttributeRequest =
        env->GetMethodID(clazz , "onListPlayerAttributeRequest" , "([B)V");
    method_onListPlayerAttributeValues =
        env->GetMethodID(clazz , "onListPlayerAttributeValues" , "(B[B)V");
    method_getElementAttr =
        env->GetMethodID(clazz, "getElementAttr", "(B[I[B)V");
    method_setPlayerAppSetting =
        env->GetMethodID(clazz, "setPlayerAppSetting","(B[B[B[B)V");
    method_getplayerattribute_text =
        env->GetMethodID(clazz, "getplayerattribute_text" , "(B[B[B)V");
    method_getplayervalue_text =
        env->GetMethodID(clazz, "getplayervalue_text" , "(BB[B[B)V");
    method_registerNotification =
        env->GetMethodID(clazz, "registerNotification", "(II[B)V");
    method_onGetPlayerAttributeValues =
        env->GetMethodID(clazz, "onGetPlayerAttributeValues", "(B[I[B)V");
    method_volumeChangeCallback =
        env->GetMethodID(clazz, "volumeChangeCallback", "(II[B)V");
    method_handlePassthroughCmd =
        env->GetMethodID(clazz, "handlePassthroughCmd", "(II[B)V");
    //setAddressedPlayer: attributes to pass: Player ID
    method_setAddressedPlayer =
        env->GetMethodID(clazz, "setAddressedPlayer", "(I[B)V");
    //getFolderItems: attributes to pass: Scope, Start, End, Attr Cnt
    method_getFolderItems =
        env->GetMethodID(clazz, "getFolderItems", "(BJJII[I[B)V");
    method_setBrowsedPlayer =
        env->GetMethodID(clazz, "setBrowsedPlayer", "(I[B)V");
    method_changePath =
        env->GetMethodID(clazz, "changePath", "(BJ[B)V");
    method_playItem =
        env->GetMethodID(clazz, "playItem", "(BJ[B)V");
    method_getItemAttr =
        env->GetMethodID(clazz, "getItemAttr", "(BJB[I[B)V");
    method_onConnectionStateChanged =
        env->GetMethodID(clazz, "onConnectionStateChanged", "(Z[B)V");
    ALOGV("%s: succeeds", __FUNCTION__);
}

static void initNative(JNIEnv *env, jobject object,
        jint maxAvrcpConnections) {
    const bt_interface_t* btInf;
    bt_status_t status;

    if ( (btInf = getBluetoothInterface()) == NULL) {
        ALOGE("Bluetooth module is not loaded");
        return;
    }

    if (sBluetoothMultiAvrcpInterface !=NULL) {
         ALOGW("Cleaning up Avrcp Interface before initializing...");
         sBluetoothMultiAvrcpInterface->cleanup();
         sBluetoothMultiAvrcpInterface = NULL;
    }

    if (mCallbacksObj != NULL) {
         ALOGW("Cleaning up Avrcp callback object");
         env->DeleteGlobalRef(mCallbacksObj);
         mCallbacksObj = NULL;
    }

    if ( (sBluetoothMultiAvrcpInterface = (btrc_interface_t *)
          btInf->get_profile_interface(BT_PROFILE_AV_RC_ID)) == NULL) {
        ALOGE("Failed to get Bluetooth Avrcp Interface");
        return;
    }

    if ((status = sBluetoothMultiAvrcpInterface->init(&sBluetoothAvrcpCallbacks,
            maxAvrcpConnections)) !=
         BT_STATUS_SUCCESS) {
        ALOGE("Failed to initialize Bluetooth Avrcp, status: %d", status);
        sBluetoothMultiAvrcpInterface = NULL;
        return;
    }

    mCallbacksObj = env->NewGlobalRef(object);
}

static void cleanupNative(JNIEnv *env, jobject object) {
    const bt_interface_t* btInf;

    if ( (btInf = getBluetoothInterface()) == NULL) {
        ALOGE("Bluetooth module is not loaded");
        return;
    }

    if (sBluetoothMultiAvrcpInterface !=NULL) {
        sBluetoothMultiAvrcpInterface->cleanup();
        sBluetoothMultiAvrcpInterface = NULL;
    }

    if (mCallbacksObj != NULL) {
        env->DeleteGlobalRef(mCallbacksObj);
        mCallbacksObj = NULL;
    }
}

static jboolean getPlayStatusRspNative(JNIEnv *env, jobject object, jint playStatus,
                                       jint songLen, jint songPos, jbyteArray address) {
    bt_status_t status;
    jbyte *addr;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    if ((status = sBluetoothMultiAvrcpInterface->get_play_status_rsp((btrc_play_status_t)playStatus,
                                            songLen, songPos,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed get_play_status_rsp, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean getListPlayerappAttrRspNative(JNIEnv *env ,jobject object , jbyte numAttr,
                                              jbyteArray attrIds , jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    btrc_player_attr_t *pAttrs = NULL;
    int i;
    jbyte *attr;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    if( numAttr > BTRC_MAX_APP_ATTR_SIZE) {
        ALOGE("get_element_attr_rsp: number of attributes exceed maximum");
        return JNI_FALSE;
    }
    ALOGI("getListPlayerappAttrRspNative");
    pAttrs = new btrc_player_attr_t[numAttr];
    if (!pAttrs) {
        ALOGE("getListPlayerappAttrRspNative: not have enough memeory");
        return JNI_FALSE;
    }
    attr = env->GetByteArrayElements(attrIds, NULL);
    if( !attr) {
        delete[] pAttrs;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE ;
    }
    for (i = 0; i < numAttr; ++i) {
        pAttrs[i] = (btrc_player_attr_t)attr[i];
    }
    if (i < numAttr) {
        delete[] pAttrs;
        env->ReleaseByteArrayElements(attrIds, attr, 0);
        return JNI_FALSE;
    }
    //Call Stack Method
    if ((status = sBluetoothMultiAvrcpInterface->list_player_app_attr_rsp(numAttr, pAttrs,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed list_player_app_attr_rsp, status: %d", status);
    }
    delete[] pAttrs;
    env->ReleaseByteArrayElements(attrIds, attr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}


static jboolean getPlayerAppValueRspNative(JNIEnv *env ,jobject object , jbyte numvalue,
                                           jbyteArray value ,jbyteArray address)
{
    bt_status_t status;
    jbyte *addr;
    uint8_t *pAttrs = NULL;
    int i;
    jbyte *attr;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    if( numvalue > BTRC_MAX_APP_ATTR_SIZE) {
        ALOGE("get_element_attr_rsp: number of attributes exceed maximum");
        return JNI_FALSE;
    }
    pAttrs = new uint8_t[numvalue];
    if (!pAttrs) {
        ALOGE("getPlayerAppValueRspNative: not have enough memeory");
        return JNI_FALSE;
    }
    attr = env->GetByteArrayElements(value, NULL);

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    if (!attr) {
        delete[] pAttrs;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    for (i = 0; i < numvalue; ++i) {
        pAttrs[i] = (uint8_t)attr[i];
    }
    if (i < numvalue) {
        delete[] pAttrs;
        env->ReleaseByteArrayElements(value, attr, 0);
        return JNI_FALSE;
    }
    if ((status = sBluetoothMultiAvrcpInterface->list_player_app_value_rsp(numvalue, pAttrs,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed list_player_app_value_rsp, status: %d", status);
    }
    delete[] pAttrs;
    env->ReleaseByteArrayElements(value, attr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean SendCurrentPlayerValueRspNative(JNIEnv *env, jobject object ,
                                                jbyte numattr ,jbyteArray value ,jbyteArray address) {
    btrc_player_settings_t *pAttrs = NULL ;
    bt_status_t status;
    jbyte *addr;
    int i;
    jbyte *attr;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if( numattr > BTRC_MAX_APP_ATTR_SIZE || numattr == 0) {
        ALOGE("SendCurrentPlayerValueRspNative: number of attributes exceed maximum");
        return JNI_FALSE;
    }
    pAttrs = new btrc_player_settings_t;
    if (!pAttrs) {
        ALOGE("SendCurrentPlayerValueRspNative: not have enough memeory");
        return JNI_FALSE;
    }
    attr = env->GetByteArrayElements(value, NULL);
    if (!attr) {
        delete pAttrs;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    pAttrs->num_attr = numattr/2 ;
    for(i =0 ; i < numattr; i+=2)
    {
        pAttrs->attr_ids[i/2]    =  attr[i];
        pAttrs->attr_values[i/2] =  attr[i+1];
    }
    if ((status = sBluetoothMultiAvrcpInterface->get_player_app_value_rsp(pAttrs,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed get_player_app_value_rsp, status: %d", status);
    }
    delete pAttrs;
    env->ReleaseByteArrayElements(value, attr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}


//JNI Method called to Respond to PDU 0x14
static jboolean SendSetPlayerAppRspNative(JNIEnv *env, jobject object,
                                        jint attr_status, jbyteArray address)
{
    bt_status_t status;
    jbyte *addr;
    btrc_status_t player_rsp = (btrc_status_t) attr_status;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if ((status = sBluetoothMultiAvrcpInterface->set_player_app_value_rsp(player_rsp,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed set_player_app_value_rsp, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}


//JNI Method Called to Respond to PDU 0x15
static jboolean sendSettingsTextRspNative(JNIEnv *env, jobject object, jint num_attr,
                                jbyteArray attr,jint length , jobjectArray textArray,
                                jbyteArray address) {
    btrc_player_setting_text_t *pAttrs = NULL;
    bt_status_t status;
    jbyte *addr;
    int i;
    jstring text;
    const char* textStr;
    jbyte *arr ;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if (num_attr > BTRC_MAX_ELEM_ATTR_SIZE) {
        ALOGE("get_element_attr_rsp: number of attributes exceed maximum");
        return JNI_FALSE;
    }
    pAttrs = new btrc_player_setting_text_t[num_attr];
    if (!pAttrs) {
        ALOGE("sendSettingsTextRspNative: not have enough memeory");
        return JNI_FALSE;
    }
    arr = env->GetByteArrayElements(attr, NULL);
    if (!arr) {
        delete[] pAttrs;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    for (i = 0; i < num_attr ; ++i) {
        text = (jstring) env->GetObjectArrayElement(textArray, i);
        textStr = env->GetStringUTFChars(text, NULL);
        if (!textStr) {
            ALOGE("get_element_attr_rsp: GetStringUTFChars return NULL");
            env->DeleteLocalRef(text);
            break;
        }
        pAttrs[i].id = arr[i];
        if (strlen(textStr) >= BTRC_MAX_ATTR_STR_LEN) {
            ALOGE("sendSettingsTextRspNative: string length exceed maximum");
        }
        strlcpy((char *)pAttrs[i].text, textStr, BTRC_MAX_ATTR_STR_LEN);
        //Check out if release need to be done in for loop
        env->ReleaseStringUTFChars(text, textStr);
        env->DeleteLocalRef(text);
    }
    //Call Stack Methos to Respond PDU 0x16
    if ((status = sBluetoothMultiAvrcpInterface->get_player_app_attr_text_rsp(num_attr, pAttrs,
                                            (bt_bdaddr_t *)addr)) !=  BT_STATUS_SUCCESS) {
        ALOGE("Failed get_player_app_attr_text_rsp, status: %d", status);
    }
    delete[] pAttrs;
    env->ReleaseByteArrayElements(attr, arr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

//JNI Method Called to respond to PDU 0x16
static jboolean sendValueTextRspNative(JNIEnv *env, jobject object, jint num_attr,
                                       jbyteArray attr, jint length , jobjectArray textArray,
                                       jbyteArray address) {
    btrc_player_setting_text_t *pAttrs = NULL;
    bt_status_t status;
    jbyte *addr;
    int i;
    jstring text ;
    const char* textStr;
    jbyte *arr ;

    //ALOGE("sendValueTextRspNative");
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if (num_attr > BTRC_MAX_ELEM_ATTR_SIZE) {
        ALOGE("sendValueTextRspNative: number of attributes exceed maximum");
        return JNI_FALSE;
    }
    pAttrs = new btrc_player_setting_text_t[num_attr];
    if (!pAttrs) {
        ALOGE("sendValueTextRspNative: not have enough memeory");
        return JNI_FALSE;
    }
    arr = env->GetByteArrayElements(attr, NULL);
    if (!arr) {
        delete[] pAttrs;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    for (i = 0; i < num_attr ; ++i) {
        text = (jstring) env->GetObjectArrayElement(textArray, i);
        textStr = env->GetStringUTFChars(text, NULL);
        if (!textStr) {
            ALOGE("sendValueTextRspNative: GetStringUTFChars return NULL");
            env->DeleteLocalRef(text);
            break;
        }
        pAttrs[i].id = arr[i];
        if (strlen(textStr) >= BTRC_MAX_ATTR_STR_LEN) {
           ALOGE("sendValueTextRspNative: string length exceed maximum");
        }
        strlcpy((char *)pAttrs[i].text, textStr, BTRC_MAX_ATTR_STR_LEN);
        env->ReleaseStringUTFChars(text, textStr);
        env->DeleteLocalRef(text);
    }
    //Call Stack Method to Respond to PDU 0x16
    if ((status = sBluetoothMultiAvrcpInterface->get_player_app_value_text_rsp(num_attr, pAttrs,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed get_player_app_value_text_rsp, status: %d", status);
    }
    delete[] pAttrs;
    env->ReleaseByteArrayElements(attr, arr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

  static jboolean getElementAttrRspNative(JNIEnv *env, jobject object, jbyte numAttr,
                                          jintArray attrIds, jobjectArray textArray,
                                          jbyteArray address) {
    jint *attr;
    bt_status_t status;
    jbyte *addr;
    jstring text;
    int i;
    btrc_element_attr_val_t *pAttrs = NULL;
    const char* textStr;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if (numAttr > BTRC_MAX_ELEM_ATTR_SIZE) {
        ALOGE("get_element_attr_rsp: number of attributes exceed maximum");
        return JNI_FALSE;
    }

    pAttrs = new btrc_element_attr_val_t[numAttr];
    if (!pAttrs) {
        ALOGE("get_element_attr_rsp: not have enough memeory");
        return JNI_FALSE;
    }

    attr = env->GetIntArrayElements(attrIds, NULL);
    if (!attr) {
        delete[] pAttrs;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    for (i = 0; i < numAttr; ++i) {
        text = (jstring) env->GetObjectArrayElement(textArray, i);
        textStr = env->GetStringUTFChars(text, NULL);
        if (!textStr) {
            ALOGE("get_element_attr_rsp: GetStringUTFChars return NULL");
            env->DeleteLocalRef(text);
            break;
        }

        pAttrs[i].attr_id = attr[i];
        if (strlen(textStr) >= BTRC_MAX_ATTR_STR_LEN) {
            ALOGE("get_element_attr_rsp: string length exceed maximum");
        }
        strlcpy((char *)pAttrs[i].text, textStr, BTRC_MAX_ATTR_STR_LEN);
        env->ReleaseStringUTFChars(text, textStr);
        env->DeleteLocalRef(text);
    }

    if (i < numAttr) {
        delete[] pAttrs;
        env->ReleaseIntArrayElements(attrIds, attr, 0);
        return JNI_FALSE;
    }

    if ((status = sBluetoothMultiAvrcpInterface->get_element_attr_rsp(numAttr, pAttrs,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed get_element_attr_rsp, status: %d", status);
    }

    delete[] pAttrs;
    env->ReleaseIntArrayElements(attrIds, attr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean registerNotificationPlayerAppRspNative(JNIEnv *env, jobject object ,jint type,
                                                jbyte numattr ,jbyteArray value ,
                                                jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    int i;
    jbyte *attr;
    btrc_register_notification_t *param= NULL;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if( numattr > BTRC_MAX_APP_ATTR_SIZE || numattr == 0) {
        ALOGE("registerNotificationPlayerAppRspNative: number of attributes exceed maximum");
        return JNI_FALSE;
    }
    param = new btrc_register_notification_t;

    if (!param) {
        ALOGE("registerNotificationPlayerAppRspNative: not have enough memeory");
        return JNI_FALSE;
    }
    attr = env->GetByteArrayElements(value, NULL);
    if (!attr) {
        delete param;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    param->player_setting.num_attr  = numattr/2;
    for(i =0 ; i < numattr; i+=2)
    {
        param->player_setting.attr_ids[i/2] = attr[i];
        param->player_setting.attr_values[i/2] =  attr[i+1];
    }
    //Call Stack Method
    if ((status =
                sBluetoothMultiAvrcpInterface->register_notification_rsp(
                BTRC_EVT_APP_SETTINGS_CHANGED,
                (btrc_notification_type_t)type,param,
                (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed register_notification_rsp, status: %d", status);
    }
    delete param;
    env->ReleaseByteArrayElements(value, attr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean registerNotificationRspPlayStatusNative(JNIEnv *env, jobject object,
                                                        jint type, jint playStatus,
                                                        jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    btrc_register_notification_t param;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    param.play_status = (btrc_play_status_t)playStatus;
    if ((status =
                sBluetoothMultiAvrcpInterface->register_notification_rsp(
                BTRC_EVT_PLAY_STATUS_CHANGED,
                (btrc_notification_type_t)type, &param, (bt_bdaddr_t *)addr)) !=
                BT_STATUS_SUCCESS) {
        ALOGE("Failed register_notification_rsp play status, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean registerNotificationRspTrackChangeNative(JNIEnv *env, jobject object,
                                                         jint type, jbyteArray track,
                                                         jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    btrc_register_notification_t param;
    jbyte *trk;
    int i;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    trk = env->GetByteArrayElements(track, NULL);
    if (!trk) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    for (i = 0; i < BTRC_UID_SIZE; ++i) {
      param.track[i] = trk[i];
    }

    if ((status = sBluetoothMultiAvrcpInterface->register_notification_rsp(BTRC_EVT_TRACK_CHANGE,
                  (btrc_notification_type_t)type, &param, (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed register_notification_rsp track change, status: %d", status);
    }

    env->ReleaseByteArrayElements(track, trk, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean registerNotificationRspPlayPosNative(JNIEnv *env, jobject object,
                                                    jint type, jint playPos,
                                                    jbyteArray address) {
    bt_status_t status;
    jbyte *addr;

    btrc_register_notification_t param;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    param.song_pos = (uint32_t)playPos;
    if ((status = sBluetoothMultiAvrcpInterface->register_notification_rsp(
                  BTRC_EVT_PLAY_POS_CHANGED,
                  (btrc_notification_type_t)type, &param, (bt_bdaddr_t *)addr)) !=
                  BT_STATUS_SUCCESS) {
        ALOGE("Failed register_notification_rsp play position, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean setVolumeNative(JNIEnv *env, jobject object, jint volume,
                                jbyteArray address) {
    bt_status_t status;
    jbyte *addr;

    //TODO: delete test code
    ALOGV("%s: jint: %d, uint8_t: %u", __FUNCTION__, volume, (uint8_t) volume);

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if ((status = sBluetoothMultiAvrcpInterface->set_volume((uint8_t)volume,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed set_volume, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean registerNotificationRspAddressedPlayerChangedNative (JNIEnv *env,
                                                            jobject object, jint type,
                                                            jint playerId,
                                                            jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    btrc_register_notification_t param;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    ALOGV("playerId: %d", playerId);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    param.player_id = (uint16_t)playerId;
    if ((status = sBluetoothMultiAvrcpInterface->register_notification_rsp(
                   BTRC_EVT_ADDRESSED_PLAYER_CHANGED, (btrc_notification_type_t)type,
                   &param, (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed registerNotificationRspAddressedPlayerChangedNative, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;

}

static jboolean registerNotificationRspAvailablePlayersChangedNative (JNIEnv *env,
                                                                      jobject object, jint type,
                                                                      jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    btrc_register_notification_t param;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if ((status = sBluetoothMultiAvrcpInterface->register_notification_rsp(
                   BTRC_EVT_AVAILABLE_PLAYERS_CHANGED, (btrc_notification_type_t)type,
                   &param, (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed registerNotificationRspAvailablePlayersChangedNative, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean registerNotificationRspNowPlayingContentChangedNative(JNIEnv *env,
                                                                    jobject object, jint type,
                                                                    jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    btrc_register_notification_t param;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if ((status = sBluetoothMultiAvrcpInterface->register_notification_rsp(
            BTRC_EVT_NOW_PLAYING_CONTENT_CHANGED, (btrc_notification_type_t)type, &param,
            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed registerNotificationRspNowPlayingContentChangedNative, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean getFolderItemsRspNative(JNIEnv *env, jobject object, jbyte statusCode,
                            jlong numItems, jintArray itemType, jlongArray uid, jintArray type,
                            jbyteArray playable, jobjectArray displayName, jbyteArray numAtt,
                            jobjectArray attValues, jintArray attIds , jbyteArray address) {
    bt_status_t status = BT_STATUS_SUCCESS;
    jbyte *addr;
    btrc_folder_list_entries_t param;
    int32_t *itemTypeElements;
    int64_t *uidElements;
    int32_t *typeElements;
    int8_t *playableElements;
    jstring *displayNameElements;
    int8_t *numAttElements;
    jstring *attValuesElements;
    int32_t *attIdsElements;
    jint count;
    jstring text;
    const char* textStr;
    jsize utfStringLength = 0;
    int num_attr;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    param.status = statusCode;
    param.uid_counter = 0;
    param.item_count = numItems;

    if (numItems > 0) {
        itemTypeElements = env->GetIntArrayElements(itemType, NULL);
        if (!itemTypeElements) {
            jniThrowIOException(env, EINVAL);
            return JNI_FALSE;
        }

        uidElements = env->GetLongArrayElements(uid, NULL);
        if (!uidElements) {
            jniThrowIOException(env, EINVAL);
            return JNI_FALSE;
        }

        typeElements = env->GetIntArrayElements(type, NULL);
        if (!typeElements) {
            jniThrowIOException(env, EINVAL);
            return JNI_FALSE;
        }

        playableElements = env->GetByteArrayElements(playable, NULL);
        if (!playableElements) {
            jniThrowIOException(env, EINVAL);
            return JNI_FALSE;
        }

        numAttElements = env->GetByteArrayElements(numAtt, NULL);
        if (!numAttElements) {
            jniThrowIOException(env, EINVAL);
            return JNI_FALSE;
        }

        attIdsElements = env->GetIntArrayElements(attIds, NULL);
        if (!attIdsElements) {
            jniThrowIOException(env, EINVAL);
            return JNI_FALSE;
        }
    }

    param.p_item_list = new btrc_folder_list_item_t[numItems];

    for (count = 0; count < numItems; count++) {
        param.p_item_list[count].item_type = (uint8_t)itemTypeElements[count];
        ALOGI("getFolderItemsRspNative: item_type: %d", param.p_item_list[count].item_type);
        if (itemTypeElements[count] == BTRC_TYPE_FOLDER) {
            param.p_item_list[count].u.folder.uid = uidElements[count];
            ALOGI("getFolderItemsRspNative: uid: %lu", param.p_item_list[count].u.folder.uid);
            param.p_item_list[count].u.folder.type = (uint8_t)typeElements[count];
            ALOGI("getFolderItemsRspNative: type: %d", param.p_item_list[count].u.folder.type);
            param.p_item_list[count].u.folder.playable = playableElements[count];

            text = (jstring) env->GetObjectArrayElement(displayName, count);
            if (text == NULL) {
                ALOGE("getFolderItemsRspNative: App string is NULL, bail out");
                break;
            }
            utfStringLength = env->GetStringUTFLength(text);
            if (!utfStringLength) {
                ALOGE("getFolderItemsRspNative: GetStringUTFLength return NULL");
                env->DeleteLocalRef(text);
                break;
            }
            ALOGI("getFolderItemsRspNative: Disp Elem Length: %d", utfStringLength);

            textStr = env->GetStringUTFChars(text, NULL);
            if (!textStr) {
                ALOGE("getFolderItemsRspNative: GetStringUTFChars return NULL");
                env->DeleteLocalRef(text);
                break;
            }
            param.p_item_list[count].u.folder.name.charset_id = BTRC_CHARSET_UTF8;
            param.p_item_list[count].u.folder.name.str_len = utfStringLength;
            param.p_item_list[count].u.folder.name.p_str = new uint8_t[utfStringLength + 1];
            strlcpy((char *)param.p_item_list[count].u.folder.name.p_str, textStr,
                                                                    utfStringLength + 1);
            env->ReleaseStringUTFChars(text, textStr);
            env->DeleteLocalRef(text);
        } else if (itemTypeElements[count] == BTRC_TYPE_MEDIA_ELEMENT) {
            num_attr = 0;
            param.p_item_list[count].u.media.uid = uidElements[count];
            ALOGI("getFolderItemsRspNative: uid: %l", param.p_item_list[count].u.folder.uid);
            param.p_item_list[count].u.media.type = (uint8_t)typeElements[count];
            ALOGI("getFolderItemsRspNative: type: %d", param.p_item_list[count].u.folder.type);
            text = (jstring) env->GetObjectArrayElement(displayName, count);
            if (text == NULL) {
                ALOGE("getFolderItemsRspNative: App string is NULL, bail out");
                break;
            }
            utfStringLength = env->GetStringUTFLength(text);
            if (!utfStringLength) {
                ALOGE("getFolderItemsRspNative: GetStringUTFLength return NULL");
                env->DeleteLocalRef(text);
                break;
            }
            ALOGI("getFolderItemsRspNative: Disp Elem Length: %d", utfStringLength);

            textStr = env->GetStringUTFChars(text, NULL);
            if (!textStr) {
                ALOGE("getFolderItemsRspNative: GetStringUTFChars return NULL");
                env->DeleteLocalRef(text);
                break;
            }
            param.p_item_list[count].u.media.name.charset_id = BTRC_CHARSET_UTF8;
            param.p_item_list[count].u.media.name.str_len = utfStringLength;
            param.p_item_list[count].u.media.name.p_str = new uint8_t[utfStringLength + 1];
            strlcpy((char *)param.p_item_list[count].u.media.name.p_str, textStr,
                                                                    utfStringLength + 1);
            env->ReleaseStringUTFChars(text, textStr);
            env->DeleteLocalRef(text);
            ALOGI("getFolderItemsRspNative: numAttr: %d", numAttElements[count]);
            param.p_item_list[count].u.media.p_attr_list =
                            new btrc_attr_entry_t[numAttElements[count]];

            for (int i = 0; i < numAttElements[count]; i++) {
                text = (jstring) env->GetObjectArrayElement(attValues, (7 * count) + i);
                if (text == NULL) {
                    ALOGE("getFolderItemsRspNative: Attribute string is NULL, continue to next");
                    continue;
                }
                utfStringLength = env->GetStringUTFLength(text);
                if (!utfStringLength) {
                    ALOGE("getFolderItemsRspNative: GetStringUTFLength return NULL");
                    env->DeleteLocalRef(text);
                    continue;
                }
                textStr = env->GetStringUTFChars(text, NULL);
                if (!textStr) {
                    ALOGE("getFolderItemsRspNative: GetStringUTFChars return NULL");
                    env->DeleteLocalRef(text);
                    continue;
                }
                param.p_item_list[count].u.media.p_attr_list[num_attr].attr_id =
                                                    attIdsElements[(7 * count) + i];
                ALOGI("getFolderItemsRspNative: Attr id: %d",
                    param.p_item_list[count].u.media.p_attr_list[num_attr].attr_id);
                param.p_item_list[count].u.media.p_attr_list[num_attr].name.charset_id =
                                                                        BTRC_CHARSET_UTF8;
                param.p_item_list[count].u.media.p_attr_list[num_attr].name.str_len =
                                                                        utfStringLength;
                ALOGI("getFolderItemsRspNative: Attr Length: %d",
                    param.p_item_list[count].u.media.p_attr_list[num_attr].name.str_len);
                param.p_item_list[count].u.media.p_attr_list[num_attr].name.p_str =
                                                            new uint8_t[utfStringLength + 1];
                strlcpy((char *)param.p_item_list[count].u.media.p_attr_list[num_attr].
                                                name.p_str, textStr, utfStringLength + 1);
                num_attr++;
                env->ReleaseStringUTFChars(text, textStr);
                env->DeleteLocalRef(text);
            }
            param.p_item_list[count].u.media.attr_count = num_attr;
            ALOGI("getFolderItemsRspNative: effective numAttr: %d",
                            param.p_item_list[count].u.media.attr_count);
        }
    }

    if ((status = sBluetoothMultiAvrcpInterface->get_folder_items_rsp(&param,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed get_folder_items_rsp, status: %u", status);
    }

    if (numItems > 0) {
        env->ReleaseIntArrayElements(itemType, itemTypeElements, 0);
        env->ReleaseLongArrayElements(uid, uidElements, 0);
        env->ReleaseIntArrayElements(type, typeElements, 0);
        env->ReleaseByteArrayElements(playable, playableElements, 0);
        env->ReleaseByteArrayElements(numAtt, numAttElements, 0);
        env->ReleaseIntArrayElements(attIds, attIdsElements, 0);
    }

    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

// MediaPlayerItems are populated as byte stream from the apps
static jboolean getMediaPlayerListRspNative(JNIEnv *env, jobject object, jbyte statusCode,
                                    jint uidCounter, jint itemCount, jbyteArray folderItems,
                                    jintArray folderItemLengths, jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    int8_t *folderElements;
    int32_t *folderElementLengths;
    int32_t count = 0;
    int32_t countElementLength = 0;
    int32_t countTotalBytes = 0;
    int32_t countTemp = 0;
    int32_t checkLength = 0;
    btrc_folder_list_entries_t param;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    folderElements = env->GetByteArrayElements(folderItems, NULL);
    if (!folderElements) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    folderElementLengths = env->GetIntArrayElements(folderItemLengths, NULL);
    if (!folderElementLengths) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    param.status = statusCode;
    param.uid_counter = uidCounter;
    param.item_count = itemCount;
    ALOGI("status: %d, item count: %d", param.status, param.item_count);
    param.p_item_list = new btrc_folder_list_item_t[itemCount];
    ALOGI("Intermediate List entries:");
    for (; count < itemCount; count++) {
        param.p_item_list[count].item_type = folderElements[countTotalBytes]; countTotalBytes++;
        param.p_item_list[count].u.player.player_id =
            (uint16_t)(folderElements[countTotalBytes] & 0x00ff); countTotalBytes++;
        param.p_item_list[count].u.player.player_id +=
            (uint16_t)((folderElements[countTotalBytes] << 8) & 0xff00); countTotalBytes++;
        param.p_item_list[count].u.player.major_type =
            folderElements[countTotalBytes]; countTotalBytes++;
        param.p_item_list[count].u.player.sub_type =
            (uint32_t)(folderElements[countTotalBytes] & 0x000000ff); countTotalBytes++;
        param.p_item_list[count].u.player.sub_type +=
            (uint32_t)((folderElements[countTotalBytes] << 8) & 0x0000ff00); countTotalBytes++;
        param.p_item_list[count].u.player.sub_type +=
            (uint32_t)((folderElements[countTotalBytes] << 16) & 0x00ff0000); countTotalBytes++;
        param.p_item_list[count].u.player.sub_type +=
            (uint32_t)((folderElements[countTotalBytes] << 24) & 0xff000000); countTotalBytes++;
        param.p_item_list[count].u.player.play_status =
            folderElements[countTotalBytes]; countTotalBytes++;
        for (countTemp = 0; countTemp < 16; countTemp ++) {
            param.p_item_list[count].u.player.features[countTemp] =
                    folderElements[countTotalBytes]; countTotalBytes++;
        }
        param.p_item_list[count].u.player.name.charset_id =
            (uint16_t)(folderElements[countTotalBytes] & 0x00ff); countTotalBytes++;
        param.p_item_list[count].u.player.name.charset_id +=
            (uint16_t)((folderElements[countTotalBytes] << 8) & 0xff00); countTotalBytes++;
        param.p_item_list[count].u.player.name.str_len =
            (uint16_t)(folderElements[countTotalBytes] & 0x00ff); countTotalBytes++;
        param.p_item_list[count].u.player.name.str_len +=
            (uint16_t)((folderElements[countTotalBytes] << 8) & 0xff00); countTotalBytes++;
        param.p_item_list[count].u.player.name.p_str =
            new uint8_t[param.p_item_list[count].u.player.name.str_len];
        for (countTemp = 0; countTemp < param.p_item_list[count].u.player.name.str_len;
                                                                        countTemp ++) {
            param.p_item_list[count].u.player.name.p_str[countTemp] =
                        folderElements[countTotalBytes]; countTotalBytes++;
        }
        /*To check if byte feeding went well*/
        checkLength += folderElementLengths[count];
        if (checkLength != countTotalBytes) {
            ALOGE("Error Populating Intermediate Folder Entry");
            ALOGE("checkLength = %u countTotalBytes = %u", checkLength, countTotalBytes);
        }
        ALOGI("entry: %u", count);
        ALOGI("item type: %u", param.p_item_list[count].item_type);
        ALOGI("player id: %u", param.p_item_list[count].u.player.player_id);
        ALOGI("major type: %u", param.p_item_list[count].u.player.major_type);
        ALOGI("sub type: %u", param.p_item_list[count].u.player.sub_type);
        ALOGI("play status: %u", param.p_item_list[count].u.player.play_status);
        ALOGI("features: ");
        for (countTemp = 0; countTemp < 16; countTemp ++)
            ALOGI("%u", param.p_item_list[count].u.player.features[countTemp]);
        ALOGI("charset id: %u", param.p_item_list[count].u.player.name.charset_id);
        ALOGI("name len: %u", param.p_item_list[count].u.player.name.str_len);
        ALOGI("name: ");
        for (countTemp = 0; countTemp < param.p_item_list[count].u.player.name.str_len;
                                                                            countTemp ++) {
            ALOGI("%u", param.p_item_list[count].u.player.name.p_str[countTemp]);
        }
    }

    if ((status = sBluetoothMultiAvrcpInterface->get_folder_items_rsp(&param,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed getMediaPlayerListRspNative, status: %u", status);
    }

    env->ReleaseByteArrayElements(folderItems, folderElements, 0);
    env->ReleaseIntArrayElements(folderItemLengths, folderElementLengths, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean setAdressedPlayerRspNative(JNIEnv *env, jobject object, jbyte statusCode,
                                            jbyteArray address) {
    bt_status_t status;
    jbyte *addr;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if ((status = sBluetoothMultiAvrcpInterface->set_addressed_player_rsp((btrc_status_t)statusCode,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed setAdressedPlayerRspNative, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean getItemAttrRspNative(JNIEnv *env, jobject object, jbyte numAttr,
                                     jintArray attrIds, jobjectArray textArray,
                                     jbyteArray address) {
    jint *attr;
    jbyte *addr;
    bt_status_t status;
    jstring text;
    int i;
    btrc_element_attr_val_t *pAttrs = NULL;
    const char* textStr;
    jsize utfStringLength = 0;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    if (numAttr > BTRC_MAX_ELEM_ATTR_SIZE) {
        ALOGE("get_item_attr_rsp: number of attributes exceed maximum");
        return JNI_FALSE;
    }

    pAttrs = new btrc_element_attr_val_t[numAttr];
    if (!pAttrs) {
        ALOGE("get_item_attr_rsp: not have enough memeory");
        return JNI_FALSE;
    }

    attr = env->GetIntArrayElements(attrIds, NULL);
    if (!attr) {
        delete[] pAttrs;
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }

    for (i = 0; i < numAttr; ++i) {
        text = (jstring) env->GetObjectArrayElement(textArray, i);

        utfStringLength = env->GetStringUTFLength(text);
        if (!utfStringLength) {
            ALOGE("setBrowsedPlayerRspNative: GetStringUTFLength return NULL");
            env->DeleteLocalRef(text);
            break;
        }

        textStr = env->GetStringUTFChars(text, NULL);
        if (!textStr) {
            ALOGE("get_item_attr_rsp: GetStringUTFChars return NULL");
            env->DeleteLocalRef(text);
            break;
        }

        pAttrs[i].attr_id = attr[i];
        if (utfStringLength >= BTRC_MAX_ATTR_STR_LEN) {
            ALOGE("get_item_attr_rsp: string length exceed maximum");
        }
        strlcpy((char *)pAttrs[i].text, textStr, BTRC_MAX_ATTR_STR_LEN);
        env->ReleaseStringUTFChars(text, textStr);
        env->DeleteLocalRef(text);
    }

    if (i < numAttr) {
        delete[] pAttrs;
        env->ReleaseIntArrayElements(attrIds, attr, 0);
        return JNI_FALSE;
    }

    if ((status = sBluetoothMultiAvrcpInterface->get_item_attr_rsp(numAttr, pAttrs,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed get_item_attr_rsp, status: %d", status);
    }

    delete[] pAttrs;
    env->ReleaseIntArrayElements(attrIds, attr, 0);
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean setBrowsedPlayerRspNative(JNIEnv *env, jobject object,
                                          jbyte statusCode, jint uidCounter,
                                          jint itemCount, jint folderDepth,
                                          jint charId, jobjectArray folderNames,
                                          jbyteArray address) {
    bt_status_t status;
    jbyte *addr;
    int32_t count = 0;
    jstring text;
    const char* textStr;
    jsize utfStringLength = 0;

    btrc_set_browsed_player_rsp_t param;

    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    param.status = statusCode;
    param.uid_counter = uidCounter;
    param.num_items = itemCount;
    param.charset_id = charId;
    param.folder_depth = folderDepth;

    ALOGI("statusCode: %d", statusCode);
    ALOGI("uidCounter: %d", uidCounter);
    ALOGI("itemCount: %d", itemCount);
    ALOGI("charId: %d", charId);
    ALOGI("folderDepth: %d", folderDepth);

    param.p_folders = new btrc_name_t[folderDepth];

    for (count = 0; count < folderDepth; ++count) {
        text = (jstring) env->GetObjectArrayElement(folderNames, count);

        utfStringLength = env->GetStringUTFLength(text);
        if (!utfStringLength) {
            ALOGE("setBrowsedPlayerRspNative: GetStringUTFLength return NULL");
            env->DeleteLocalRef(text);
            break;
        }

        textStr = env->GetStringUTFChars(text, NULL);
        if (!textStr) {
            ALOGE("setBrowsedPlayerRspNative: GetStringUTFChars return NULL");
            env->DeleteLocalRef(text);
            break;
        }

        param.p_folders[count].str_len = utfStringLength;
        param.p_folders[count].p_str = new uint8_t[utfStringLength + 1];
        strlcpy((char *)param.p_folders[count].p_str, textStr, utfStringLength + 1);
        env->ReleaseStringUTFChars(text, textStr);
        env->DeleteLocalRef(text);

    }

    if ((status = sBluetoothMultiAvrcpInterface->set_browsed_player_rsp(&param,
                                            (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed setBrowsedPlayerRspNative, status: %u", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean changePathRspNative(JNIEnv *env, jobject object, jint errStatus, jlong itemCount,
                                    jbyteArray address) {
    bt_status_t status;
    jbyte *addr;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    ALOGV("status: %d, itemCount: %l", errStatus, itemCount);

    if ((status = sBluetoothMultiAvrcpInterface->change_path_rsp((uint8_t)errStatus,
                                        (uint32_t)itemCount,
                                        (bt_bdaddr_t *)addr)) != BT_STATUS_SUCCESS) {
        ALOGE("Failed sending change path response, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean playItemRspNative(JNIEnv *env, jobject object, jint errStatus,
                                  jbyteArray address) {
    bt_status_t status;
    jbyte *addr;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    ALOGV("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);
    ALOGV("status: %d", errStatus);

    if ((status = sBluetoothMultiAvrcpInterface->play_item_rsp((uint8_t)errStatus,
                                            (bt_bdaddr_t *)addr))!= BT_STATUS_SUCCESS) {
        ALOGE("Failed sending play item response, status: %d", status);
    }
    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}

static jboolean isDeviceActiveInHandOffNative(JNIEnv *env, jobject object, jbyteArray address) {
    bt_status_t status = BT_STATUS_SUCCESS;
    jbyte *addr;

    if (!sBluetoothMultiAvrcpInterface) return JNI_FALSE;

    addr = env->GetByteArrayElements(address, NULL);
    if (!addr) {
        jniThrowIOException(env, EINVAL);
        return JNI_FALSE;
    }
    ALOGI("%s: sBluetoothMultiAvrcpInterface: %p", __FUNCTION__, sBluetoothMultiAvrcpInterface);

    status = sBluetoothMultiAvrcpInterface->is_device_active_in_handoff((bt_bdaddr_t *)addr);

    ALOGI("isDeviceActiveInHandOffNative: status: %d", status);

    env->ReleaseByteArrayElements(address, addr, 0);
    return (status == BT_STATUS_SUCCESS) ? JNI_TRUE : JNI_FALSE;
}



static JNINativeMethod sMethods[] = {
    {"classInitNative", "()V", (void *) classInitNative},
    {"initNative", "(I)V", (void *) initNative},
    {"cleanupNative", "()V", (void *) cleanupNative},
    {"getPlayStatusRspNative", "(III[B)Z", (void *) getPlayStatusRspNative},
    {"getElementAttrRspNative", "(B[I[Ljava/lang/String;[B)Z", (void *) getElementAttrRspNative},
    {"getListPlayerappAttrRspNative", "(B[B[B)Z", (void *) getListPlayerappAttrRspNative},
    {"getPlayerAppValueRspNative", "(B[B[B)Z", (void *) getPlayerAppValueRspNative},
    {"registerNotificationRspPlayStatusNative", "(II[B)Z",
     (void *) registerNotificationRspPlayStatusNative},
    {"SendCurrentPlayerValueRspNative", "(B[B[B)Z",
     (void *) SendCurrentPlayerValueRspNative},
    {"registerNotificationPlayerAppRspNative", "(IB[B[B)Z",
     (void *) registerNotificationPlayerAppRspNative},
    {"registerNotificationRspTrackChangeNative", "(I[B[B)Z",
     (void *) registerNotificationRspTrackChangeNative},
    {"SendSetPlayerAppRspNative", "(I[B)Z",
     (void *) SendSetPlayerAppRspNative},
    {"sendSettingsTextRspNative" , "(I[BI[Ljava/lang/String;[B)Z",
     (void *) sendSettingsTextRspNative},
    {"sendValueTextRspNative" , "(I[BI[Ljava/lang/String;[B)Z",
     (void *) sendValueTextRspNative},
    {"registerNotificationRspPlayPosNative", "(II[B)Z",
     (void *) registerNotificationRspPlayPosNative},
    {"setVolumeNative", "(I[B)Z",
     (void *) setVolumeNative},
    {"setAdressedPlayerRspNative", "(B[B)Z",
     (void *) setAdressedPlayerRspNative},
    {"getMediaPlayerListRspNative", "(BII[B[I[B)Z",
     (void *) getMediaPlayerListRspNative},
    {"registerNotificationRspAddressedPlayerChangedNative", "(II[B)Z",
     (void *) registerNotificationRspAddressedPlayerChangedNative},
    {"registerNotificationRspAvailablePlayersChangedNative", "(I[B)Z",
     (void *) registerNotificationRspAvailablePlayersChangedNative},
    {"registerNotificationRspNowPlayingContentChangedNative", "(I[B)Z",
     (void *) registerNotificationRspNowPlayingContentChangedNative},
    {"setBrowsedPlayerRspNative", "(BIIII[Ljava/lang/String;[B)Z",
                                (void *) setBrowsedPlayerRspNative},
    {"changePathRspNative", "(IJ[B)Z", (void *) changePathRspNative},
    {"playItemRspNative", "(I[B)Z", (void *) playItemRspNative},
    {"getItemAttrRspNative", "(B[I[Ljava/lang/String;[B)Z", (void *) getItemAttrRspNative},
    {"getFolderItemsRspNative", "(BJ[I[J[I[B[Ljava/lang/String;[B[Ljava/lang/String;[I[B)Z",
                                                            (void *) getFolderItemsRspNative},
    {"isDeviceActiveInHandOffNative", "([B)Z", (void *) isDeviceActiveInHandOffNative},
};

int register_com_android_bluetooth_avrcp(JNIEnv* env)
{
    return jniRegisterNativeMethods(env, "com/android/bluetooth/avrcp/Avrcp",
                                    sMethods, NELEM(sMethods));
}

}
