import QtQuick 2.3
import ImageCache 1.0

ImageCache {
    id: cache
    clip: true
    property int fillMode: Image.PreserveAspectFit
    property string type
    onSourceChanged: {
        //window.busy++;
        image.visible = false;
        animated.visible = false;
        image.source = '';
        animated.source = '';
    }
    onAvailable: {
        if (source.split('.')[1] === 'gif') {
            animated.source = source;
            animated.visible = true;
            image.visible = false;
        }
        else {
            image.source = source;
            image.visible = true;
            animated.visible = false;
        }
        //window.busy--;
    }
    onReadyToDownload: {
        if (!window.cashedImages
                || (width > 0 && width < 100)
                || (height > 0 && height < 100)) {
            download();
        }
    }
    Rectangle {
        anchors.fill: parent
        color: '#373737'
        visible: image.source == '' && animated.source == ''
        Rectangle {
            id: downloadButton
            anchors.verticalCenter: parent.verticalCenter
            anchors.horizontalCenter: parent.horizontalCenter
            anchors.bottomMargin: 1 * mm
            width: cache.isDownloading && cache.kbytesTotal > 0 ? cache.kbytesReceived * (parent.width - 10 * mm) / cache.kbytesTotal + 10 * mm : 10 * mm
            height: 10 * mm
            radius: width / 2
            color: window.brightColor
            visible: parent.width > 10 * mm && parent.height > 10 * mm
            Behavior on width {
                NumberAnimation { duration: 100 }
            }

            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (cache.isDownloading)
                        cache.abortDownload();
                    else
                        cache.download();
                }
            }
            Text {
                id: bytesText
                font.pointSize: window.fontSmaller
                text: cache.isDownloading ? cache.kbytesReceived + ' / ' + cache.kbytesTotal + ' KB' : cache.type//cache.extension
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
    Image {
        id: image
        fillMode: cache.fillMode
        asynchronous: true
        anchors.fill: parent
        cache: true
        sourceSize.width: 1920
        smooth: true
        visible: false
    }
    AnimatedImage {
        id: animated
        fillMode: cache.fillMode
        asynchronous: image.asynchronous
        anchors.fill: parent
        cache: image.cache
//        sourceSize.width: window.width
        visible: false
    }
}
