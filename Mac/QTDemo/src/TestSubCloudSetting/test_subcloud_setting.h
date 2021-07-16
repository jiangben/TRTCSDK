/**
 * TRTC 子房间示例
 *
 * - 核心逻辑：创建子 TRTCCloud 实例后调用enterRoom, 用于进入其他房间，观看其他房间角色为主播用户的音视频流，此时你在其他房间的角色是观众，房间内用户无法看到你的音视频流。
 * -
 * - 具体使用场景和API说明参见：https://liteav.sdk.qcloud.com/doc/api/zh-cn/group__ITRTCCloud__cplusplus.html#a5ce8b3f393ad3e46a3af39b045c1c5a2
 * - 调用方式参考：enterSubCloudRoom() / exitSubCloudRoom()
 * -
 * - 关注的回调可对标主房间的回调设置：
 * - onEnterRoom(int result)        :进房回调
 * - onExitRoom(int reason)         :退房回调
 * - onUserVideoAvailable           :子房间主播用户的视频通知
 * - onUserAudioAvailable           :子房间主播用户的音频通知
 * - onRemoteUserEnterRoom          :子房间用户的进房通知
 * - onRemoteUserLeaveRoom          :子房间用户的退房通知
 */

#ifndef TESTSUBCLOUDSETTING_H
#define TESTSUBCLOUDSETTING_H

#include <QDialog>

#include "test_user_video_group.h"
#include "trtc_cloud_callback_default_impl.h"

class TestSubCloudSetting:public QObject,public TrtcCloudCallbackDefaultImpl
{
    Q_OBJECT
public:
    explicit TestSubCloudSetting(std::shared_ptr<TestUserVideoGroup> testUserVideoGroup);
    ~TestSubCloudSetting();

    void enterSubCloudRoom(uint32_t roomId, std::string userId, trtc::TRTCAppScene appScene);
    void exitSubCloudRoom();

private:
    //============= ITRTCCloudCallback start ===============//
    void onEnterRoom(int result) override;
    void onExitRoom(int reason) override;
    void onUserVideoAvailable(const char* userId, bool available) override;
    void onUserAudioAvailable(const char* userId, bool available) override;
    void onRemoteUserEnterRoom (const char *userId) override;
    void onRemoteUserLeaveRoom (const char *userId, int reason) override;
    //============= ITRTCCloudCallback end =================//
signals:
    void onEnterSubRoom(bool result);
    void onExitSubRoom();

private:
    trtc::ITRTCCloud *sub_cloud_ = nullptr;
    std::shared_ptr<TestUserVideoGroup> test_user_video_group_;

    uint32_t room_id_;
    std::string user_id_;
};

#endif // TESTSUBCLOUDSETTING_H
