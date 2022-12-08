import QtQuick 2.0

QtObject {
    property string defaultFamily: "Microsoft YaHei"
    property string defaultIconFamily: "Font Awesome 5 Pro Regular"

    property color  softTitleColor: "#E4E4E4"
    property int    softTitleFontSize: 18

    property color  defaultFontColor: "#FFFFFF"
    property color  defaultBtnBgColor: "#007dff"
    property color  defaultBtnColor: "#f0f0f0"
    property color  closeBtnColor: "#00000000"
    property int    textFontSize: 14

    property color  lineColor: "#999999"

    /// 按钮样式
    // 无法编辑
    property color  btnDisableBgColor: "#d7d7d7"
    property color  btnDisableColor: "#999999"

    // 默认：蓝白-蓝
    property int    btnFontSize: 14
    property color  btnColor: "#409eff"
    property color  btnHoverColor: "#ffffff"
    property color  btnBgColor: "#f0f7ff"
    property color  btnHoverBgColor: "#409eff"

    property int    selectBtnFontSize: 20

    // 注重：蓝-蓝白
    property color  empBtnColor: "#ffffff"
    property color  empBtnHoverColor: "#ffffff"
    property color  empBtnBgColor: "#409eff"
    property color  empBtnHoverBgColor: "#66b1ff"

    //停止：淡红-红
    property color  stopBtnColor: "#f56c6c"
    property color  stopBtnHoverColor: "#ffffff"
    property color  stopBtnBgColor: "#fdf2f2"
    property color  stopBtnHoverBgColor: "#f56c6c"

    // 重置1：红-淡红
    property int    resetBtnFontSize: 16
    property color  resetBtnColor: "#ffffff"
    property color  resetBtnHoverColor: "#ffffff"
    property color  resetBtnBgColor: "#f56c6c"
    property color  resetBtnHoverBgColor: "#f68888"

    // 重置2：深红-淡红
    property int    reset2BtnFontSize: 12
    property color  reset2BtnColor: "#ffd9001b"
    property color  reset2BtnHoverColor: "#ffec808d"

    // 刷新按钮
    property color  refushBtnColor: "#ffffff"
    property color  refushBtnBgColor: "#00000000"
    property color  refushBtnHoverBgColor: "#66b1ff"


    /// 标题样式
    property int    titleFontSize: 16
    property int    titleBtnFontSize: 14
    property color  titleBtnBgColor: "#00007dff"
    property color  titleBtnCheckedBgColor: "#80FFFFFF"
    property color  titleBtnCheckedColor: "#373D41"
    property color  titleBgColor: "#aa373D41"
    property color  titleCheckedBgColor: "#aa0d0d0d"

    /// 参数设置
    property int    settingTitleFontSize: 16
    property int    settingFontSize: 14
    property int    settingBtnFontSize: 14
    property int    settingSpacing: 10
    property color  settingBgColor: "#aa373D41"

    property int    polarFontSize: 28
    property color  polarBgColor: "#aa000000"
    property color  polarContentBgColor: "#FF333333"

    // 扫描编辑区域
    property int    scanModeTipSize: 12
    property color  scanModeTipColor: "#999999"

    property color  scanBorderColor: "#ff707070"
    property color  scanStartBgColor: "#ff6600"
    property color  scanStartHoverBgColor: "#ff8800"

    property color  scanEndBgColor: "#409eff"
    property color  scanBgColor: "#fa373d41"

    property int    lightCobFontSize: 14
    property int    lightIndexFontSize: 12
    property int    lightBtnSize: 16
    property color  lightBgColor: "#fa373d41"
    property color  lightLoopBgColor: "#fccd05"
    property color  lightOpenAllBgColor: "#349d05"
    property color  lightCloseAllBgColor: "#fc3403"

    property color  devConnectBgColor: "#70b603"
    property color  devDisconnectBgColor: "#e4001b"

    property int    cameraTitleFontSize: 16
    property int    camSetTextFontSize: 16

    // 解析界面
    property color  parseLookColor: "#409eff"
    property color  parseStartColor: "#f59a23"
    property color  parseWarnColor: "#d9001b"

    property color  shootBgColor: "#aaffffff"

    // 提示窗口
    property color  msgTitleColor: "#666666"
    property color  msgTitleBgColor: "#f5f5f5"
    property color  msgContentColor: "#999999"
    property int    msgContentSize: 14
    property int    msgBtnFontSize: 12
    property color  msgBtnColor: "#999999"
    property color  msgBtnHoverColor: "#409eff"
    property color  msgBtnBgColor: "#ffffff"
    property color  msgBtnHoverBgColor: "#f0f7ff"
    property color  msgConfirmBtnHoverBgColor: "#66b1ff"
    property color  msgLineColor: "#ececec"
    property color  msgIconColor: "#ff9900"

}
