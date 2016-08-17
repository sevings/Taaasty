import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import ImageCache 2.0

Loader {
    id: image
    asynchronous: true
    property string url: ''
    property string extension: ''
    property color backgroundColor: window.darkTheme ? Qt.darker('#9E9E9E') : '#9E9E9E'
    property bool savable: false
    property bool acceptClick: true
    property bool paused: false
    signal available
    signal clicked
    property Pane popBody
    onUrlChanged: {
        cachedImage = Cache.image(image.url);

        if (!cachedImage.extension)
            cachedImage.extension = image.extension

        if (cachedImage.available)
            showImage();
        else
            hideImage();
    }
    onLoaded: {
        item.source = cachedImage.source
        if (cachedImage.format == CachedImage.GifFormat)
            item.paused = Qt.binding(function() { return image.paused; })
    }
    property CachedImage cachedImage: Cache.image()
    function showImage() {
        back.visible = false;
        image.sourceComponent = cachedImage.format == CachedImage.GifFormat
                ? animatedImage : staticImage
        image.available()
    }
    function hideImage() {
        back.visible = true;
        image.sourceComponent = undefined;
    }
    Connections {
        target: cachedImage
        onAvailable: {
            showImage();
        }
    }
    Component.onCompleted: {
        if (cachedImage.available)
            showImage();
        else if ((width > 0 && width < 12 * mm)
                || (height > 0 && height < 12 * mm)) {
            cachedImage.download();
        }
    }
    Component.onDestruction: {
        image.sourceComponent = undefined;
    }
    Component {
        id: animatedImage
        AnimatedImage {
            cache: true
            smooth: true
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }
    }
    Component {
        id: staticImage
        Image {
            cache: true
            smooth: true
            asynchronous: true
            fillMode: Image.PreserveAspectCrop
        }
    }
    Poppable {
        body: popBody
        propagateComposedEvents: !acceptClick
        onClicked: {
            mouse.accepted = acceptClick;
            if (acceptClick)
                image.clicked();
        }
        onPressAndHold: {
            if (image.savable && !back.visible)
                window.saveImage(cachedImage);
        }
    }
    Rectangle {
        id: back
        anchors.fill: parent
        color: image.backgroundColor
        Loader {
            active: image.url && !cachedImage.available && back.width > 12 * mm && back.height > 12 * mm
            anchors {
                verticalCenter: parent.verticalCenter
                horizontalCenter: parent.horizontalCenter
                bottomMargin: 1 * mm
            }
            sourceComponent: Rectangle {
                id: downloadButton
                width: cachedImage.kbytesTotal > 0 ? cachedImage.kbytesReceived * (back.width - 12 * mm)
                                                     / cachedImage.kbytesTotal + 12 * mm
                                                   : 12 * mm
                height: 12 * mm
                radius: height / 2
                color: Material.primary
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
                Q.Label {
                    id: bytesText
                    font.pointSize: window.fontSmallest
                    text: (cachedImage.isDownloading && cachedImage.kbytesTotal > 0
                           ? cachedImage.kbytesReceived + ' / ' : '')
                          + (cachedImage.kbytesTotal > 0
                             ? cachedImage.kbytesTotal + ' KB ' : '')
                          + (cachedImage.extension ? '\n' + cachedImage.extension : '')
                    anchors {
                        verticalCenter: parent.verticalCenter
                        left: parent.left
                        right: parent.right
                        margins: 1 * mm
                    }
                    horizontalAlignment: Text.AlignHCenter
                    wrapMode: Text.WordWrap
                }
            }
        }
    }
}
