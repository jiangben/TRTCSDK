/**
 * TRTC 房间用户展示的子控件
 *
 * - 控制单个用户的音频、视频行为
 * -
 * - 静音：参见muteAudio实现，控制本地静音和远端用户静音
 * - 静画：参加muteVideo实现，控制本地静画和远端用户静画
 * - 设置图像渲染参数：参见setRenderParams，设置本地图像和远端图像的渲染参数，包含旋转，镜像，填充模式
 * - 设置显示远端画面的类型：支持主画面(TRTCVideoStreamTypeBig),小画面(TRTCVideoStreamTypeSmall),辅流(屏幕分享 TRTCVideoStreamTypeSub)
 */

#ifndef USERVIDEOITEM_H
#define USERVIDEOITEM_H

#include <QWidget>
#include "trtc_cloud_callback_default_impl.h"

#include "ui_TestUserVideoItem.h"

namespace TEST_VIDEO_ITEM {
    // 画面类型
    enum ViewItemType
    {
        LocalView,
        RemoteView,
        ScreenSharingView
    };
    // 禁用类型
    enum MuteAllType{
        MuteAudio,
        MuteVideo
    };
} // namespace TEST_VIDEO_ITEM

class TestUserVideoItem:public QWidget,public TrtcCloudCallbackDefaultImpl{
    Q_OBJECT

public:
    TestUserVideoItem(QWidget *parent = nullptr,
                      trtc::ITRTCCloud* cloud = nullptr,
                      int roomid = 0,
                      std::string userid = nullptr,
                      TEST_VIDEO_ITEM::ViewItemType type = TEST_VIDEO_ITEM::ViewItemType::RemoteView);
    ~TestUserVideoItem();
private:
    void muteAudio(bool mute);
    void muteVideo(bool mute);
    void setRenderParams();
    void setRemoteVideoStreamType();

private slots:
    void on_audioMuteBt_clicked();
    void on_videoMuteBt_clicked();
    void on_fitScreenBt_clicked();
    void on_preSmallVideoBt_clicked();
    void on_mirrorBt_clicked();
    void on_roateBt_clicked();

public:
    void updateAVMuteView(bool mute, TEST_VIDEO_ITEM::MuteAllType muteType);
    void setVolume(int volume);
    WId getVideoWId();
    std::string& getUserId();
    int getRoomId();
    TEST_VIDEO_ITEM::ViewItemType getViewType();
    void updateAVMuteItems(bool mute, TEST_VIDEO_ITEM::MuteAllType muteType);
    void initViews();
    void setVideoMuteEnabled(bool enabled);
    void setAudioMuteEnabled(bool enabled);

private:
    std::unique_ptr<Ui::TestUserVideoItem> ui_video_item_;
    trtc::ITRTCCloud* trtccloud_;
    int room_id_;
    std::string user_id_;
    TEST_VIDEO_ITEM::ViewItemType viewtype_;

    bool audio_mute_ = false;
    bool video_mute_ = false;

    trtc::TRTCVideoRotation rotation_ = trtc::TRTCVideoRotation0;
    trtc::TRTCVideoFillMode fill_mode_ = trtc::TRTCVideoFillMode_Fit;
    trtc::TRTCVideoMirrorType mirror_type_ = trtc::TRTCVideoMirrorType_Disable;
    trtc::TRTCVideoStreamType video_stream_type_ = trtc::TRTCVideoStreamType::TRTCVideoStreamTypeBig;
};

#endif // USERVIDEOITEM_H


