#include "test_screen_share_select_screen.h"
#include <QMessageBox>
#include "ITRTCCloud.h"

#include "screen_share_selection_item.h"

TestScreenShareSelectScreen::TestScreenShareSelectScreen(
        std::shared_ptr<TestUserVideoGroup> testUserVideoGroup,
        trtc::TRTCScreenCaptureProperty captureProperty
        ,trtc::TRTCVideoEncParam params
        ,RECT captureRect
        ,QWidget* parent
        ,trtc::TRTCVideoStreamType type)
    :QDialog(parent)
    ,ui_test_screen_share_select_screen_(new Ui::TestScreenShareSelectScreenDialog)
    ,test_user_video_group_(testUserVideoGroup){
    ui_test_screen_share_select_screen_->setupUi(this);
    setWindowFlags(windowFlags()&~Qt::WindowContextHelpButtonHint);
    enc_param_ = params;
    stream_type_ = type;
    capture_property_ = captureProperty;
    sharing_rect_ = captureRect;

    initScreenSharingScreenSelections();

    getTRTCShareInstance()->addCallback(this);
}

TestScreenShareSelectScreen::~TestScreenShareSelectScreen() {
    exitScreenSharing();
    getTRTCShareInstance()->removeCallback(this);
}

void TestScreenShareSelectScreen::stopScreenSharing(){
    getTRTCShareInstance()->stopScreenCapture();
    if(test_user_video_group_){
        getTRTCShareInstance()->startLocalPreview(test_user_video_group_->getLocalVideoTxView());
    }
}

void TestScreenShareSelectScreen::startScreenSharing(){
    getTRTCShareInstance()->startScreenCapture(reinterpret_cast<trtc::TXView>(ui_test_screen_share_select_screen_->screenSharedPreview->winId()),
        stream_type_,
        &enc_param_);
}

void TestScreenShareSelectScreen::resumeScreenCapture(){
    getTRTCShareInstance()->resumeScreenCapture();
}

void TestScreenShareSelectScreen::pauseScreenCapture(){
    getTRTCShareInstance()->pauseScreenCapture();
}

void TestScreenShareSelectScreen::releaseScreenCaptureSourceList(){
    if (screen_capture_list_ != nullptr) {
        screen_capture_list_->release();
        screen_capture_list_ = nullptr;
    }
}

void TestScreenShareSelectScreen::selectScreenCaptureTarget(trtc::TRTCScreenCaptureSourceInfo sourceInfo){
    getTRTCShareInstance()->selectScreenCaptureTarget(
        sourceInfo
        , sharing_rect_
        , capture_property_);
}

void TestScreenShareSelectScreen::initScreenCaptureSources(){
    SIZE thumb_size;
#ifndef _WIN32
    thumb_size.width = 300;
    thumb_size.height = 300;
#else
    thumb_size.cx = 300;
    thumb_size.cy = 300;
#endif
    SIZE icon_size = thumb_size;
    screen_capture_list_ = getTRTCShareInstance()->getScreenCaptureSources(thumb_size, icon_size);
}

void TestScreenShareSelectScreen::updateScreenSharingParams(
        trtc::TRTCScreenCaptureProperty captureProperty
        ,trtc::TRTCVideoEncParam params
        ,RECT rect
        ,trtc::TRTCVideoStreamType type){
    sharing_rect_ = rect;
    enc_param_ = params;
    capture_property_ = captureProperty;
    if(!started){
        stream_type_ = type;
        return;
    }
    stopScreenSharing();
    selectScreenCaptureTarget(select_item_->getScreenCaptureSourceinfo());
    startScreenSharing();
}

void TestScreenShareSelectScreen::initScreenSharingScreenSelections(){
    initScreenCaptureSources();

    if (screen_capture_list_ == nullptr) {
        return;
    }

    int screen_capture_size = screen_capture_list_->getCount();
    trtc::TRTCScreenCaptureSourceInfo screen_capture_source_info;
    int child_window_item_index = 0;
    for (int screen_index = 0; screen_index < screen_capture_size; screen_index++) {
        screen_capture_source_info = screen_capture_list_->getSourceInfo(screen_index);
        if (screen_capture_source_info.type == trtc::TRTCScreenCaptureSourceTypeScreen) {
            addScreenSharingWindowItem(screen_capture_source_info, child_window_item_index);
        }else{
            break;
        }
        child_window_item_index++;
    }
    ui_test_screen_share_select_screen_->widgetScreenWindows->adjustSize();
}

//============= ITRTCCloudCallback start ===============//
#ifdef win32
void TestScreenSharingWithScreen::onScreenCaptureCovered() {
    QMessageBox::warning(this,"ScreenCaptureCovered","Please Remove Covered Window",QMessageBox::Ok);
}
#endif
void TestScreenShareSelectScreen::onScreenCaptureStarted() {
    started = !started;
    ui_test_screen_share_select_screen_->btnPauseScreenCapture->setEnabled(false);
    ui_test_screen_share_select_screen_->btnStartScreenCapture->setEnabled(true);
    ui_test_screen_share_select_screen_->btnStartScreenCapture->setText(QString::fromLocal8Bit("结束分享").toUtf8());
}
void TestScreenShareSelectScreen::onScreenCapturePaused(int reason) {
    paused = !paused;
    ui_test_screen_share_select_screen_->btnPauseScreenCapture->setEnabled(true);
    ui_test_screen_share_select_screen_->btnPauseScreenCapture->setText(QString::fromLocal8Bit("恢复分享").toUtf8());
}
void TestScreenShareSelectScreen::onScreenCaptureResumed(int reason) {
    paused = !paused;
    ui_test_screen_share_select_screen_->btnPauseScreenCapture->setEnabled(true);
    ui_test_screen_share_select_screen_->btnPauseScreenCapture->setText(QString::fromLocal8Bit("暂停分享").toUtf8());
}
void TestScreenShareSelectScreen::onScreenCaptureStoped(int reason) {
    started = !started;
    ui_test_screen_share_select_screen_->btnPauseScreenCapture->setEnabled(false);
    ui_test_screen_share_select_screen_->btnStartScreenCapture->setEnabled(true);
    ui_test_screen_share_select_screen_->btnStartScreenCapture->setText(QString::fromLocal8Bit("开始分享").toUtf8());
}
//============= ITRTCCloudCallback end =================//

void TestScreenShareSelectScreen::addScreenSharingWindowItem(trtc::TRTCScreenCaptureSourceInfo screen_capture_source_info, int child_window_item_index)
{
    ScreenShareSelectionItem* test_screensharing_item = new ScreenShareSelectionItem(screen_capture_source_info
        , ui_test_screen_share_select_screen_->widgetScreenWindows);
    if(child_window_item_index == 0){
        test_screensharing_item->setSelected(true);
        select_item_ = test_screensharing_item;
    }
    connect(test_screensharing_item
        , &ScreenShareSelectionItem::onCheckStatusChanged
        , this
        , &TestScreenShareSelectScreen::onScreenSharingItemStatusChanged);
    test_screensharing_item->setGeometry((child_window_item_index % 4) * test_screensharing_item->width()
        , (child_window_item_index / 4) * test_screensharing_item->height()
        , test_screensharing_item->width()
        , test_screensharing_item->height());
    test_screensharing_item->show();
    test_screensharing_item->raise();

    all_sharing_items_.push_back(test_screensharing_item);
}
void TestScreenShareSelectScreen::closeEvent(QCloseEvent * closeEvent){
    exitScreenSharing();
}

void TestScreenShareSelectScreen::exitScreenSharing() {
    if (started) {
        stopScreenSharing();
    }

    releaseScreenCaptureSourceList();
}

void TestScreenShareSelectScreen::on_btnStartScreenCapture_clicked() {
    ui_test_screen_share_select_screen_->btnStartScreenCapture->setEnabled(false);
    if (started) {
        stopScreenSharing();
    }else {
        if (select_item_ == nullptr) {
            QMessageBox::warning(this, "StartScreenCapture Failed", "please select least one item");
            return;
        }
        selectScreenCaptureTarget(select_item_->getScreenCaptureSourceinfo());
        startScreenSharing();
    }
}

void TestScreenShareSelectScreen::on_btnPauseScreenCapture_clicked() {
    ui_test_screen_share_select_screen_->btnPauseScreenCapture->setEnabled(false);
    if (!started) {
        return;
    }

    if (paused) {
        resumeScreenCapture();
    } else {
        pauseScreenCapture();
    }
}
void TestScreenShareSelectScreen::onScreenSharingItemStatusChanged(ScreenShareSelectionItem* item, bool status) {
    if(!started){
        if(select_item_ != nullptr && select_item_ != item && status){
            select_item_->setSelected(false);
        }
        if(status){
            select_item_ = item;
        }
        return;
    }

    if(item == select_item_){
        select_item_->setSelected(true);
        return;
    }else{
        if(select_item_ != nullptr){
            select_item_->setSelected(false);
        }
        selectScreenCaptureTarget(item->getScreenCaptureSourceinfo());
        select_item_ = item;
    }
}

