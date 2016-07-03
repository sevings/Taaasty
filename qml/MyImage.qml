import QtQuick 2.7
import ImageCache 2.0


AnimatedImage {
    id: image
    asynchronous: true
    cache: true
    smooth: true
    clip: true
    source: ''
    property string url: ''
    property string extension: ''
    property CachedImage cachedImage: Cache.image()
    property color backgroundColor: '#373737'
    property Pane popBody: Pane { }
    property bool savable: false
    signal available
    function showImage() {
        image.source = cachedImage.source;
        back.visible = false;
        image.available()
    }
    function hideImage() {
        back.visible = true;
    }
    onUrlChanged: {
        cachedImage = Cache.image(image.url);

        if (!cachedImage.extension)
            cachedImage.extension = image.extension

        if (cachedImage.available)
            showImage();
        else
            hideImage();
    }
    Connections {
        target: cachedImage
        onAvailable: {
            showImage();
        }
    }
    Component.onCompleted: {
        if ((width > 0 && width < 12 * mm)
                || (height > 0 && height < 12 * mm)) {
            cachedImage.download();
        }
    }
    Poppable {
        body: popBody
        visible: image.savable && !back.visible
        propagateComposedEvents: true
        onPressAndHold: {
            window.saveImage(cachedImage);
        }
    }
    Rectangle {
        id: back
        anchors.fill: parent
        color: image.backgroundColor
        Rectangle {
            id: downloadButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 1 * mm
            width: cachedImage.kbytesTotal > 0 ? cachedImage.kbytesReceived * (parent.width - 12 * mm)
                                                                              / cachedImage.kbytesTotal + 12 * mm
                                                                            : 12 * mm
            height: 12 * mm
            radius: height / 2
            color: window.greenColor
            visible: image.url && parent.width > 12 * mm && parent.height > 12 * mm
            Behavior on width {
                NumberAnimation { duration: 100 }
            }
            Behavior on scale {
                NumberAnimation { easing.overshoot: 5; easing.type: Easing.OutBack; duration: 400; }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    mouse.accepted = true;

                    if (cachedImage.isDownloading)
                        cachedImage.abortDownload();
                    else
                        cachedImage.download();
                }
                onDoubleClicked: {
                    // supress second click
                }
                onPressedChanged: {
                    if (pressed)
                        parent.scale = 0.8;
                    else
                        parent.scale = 1;
                }
            }
            Text {
                id: bytesText
                font.pointSize: window.fontSmallest
                text: (cachedImage.isDownloading && cachedImage.kbytesTotal > 0
                       ? cachedImage.kbytesReceived + ' / ' : '')
                      + (cachedImage.kbytesTotal > 0
                         ? cachedImage.kbytesTotal + ' KB ' : '')
                      + (cachedImage.extension ? '\n' + cachedImage.extension : '')
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 1 * mm
                horizontalAlignment: Text.AlignHCenter
                wrapMode: Text.WordWrap
                color: 'black'
            }
        }
    }
}
