import QtQuick 2.5
import ImageCache 1.0

ImageCache {
    id: cache
    clip: true
    property int fillMode: Image.PreserveAspectFit
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
        if (width < 100 || height < 100 || !Ctrl.cashedImages) {
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
            anchors.bottomMargin: 10
            width: cache.isDownloading && cache.kbytesTotal > 0 ? cache.kbytesReceived * (parent.width - 100) / cache.kbytesTotal + 100 : 100
            height: 100
            radius: width / 2
            color: window.brightColor
            visible: parent.width > 100 && parent.height > 100
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
                font.pointSize: 18
                text: cache.isDownloading ? cache.kbytesReceived + ' / ' + cache.kbytesTotal + ' KB' : cache.extension
                anchors.verticalCenter: parent.verticalCenter
                anchors.left: parent.left
                anchors.right: parent.right
                anchors.margins: 10
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
        sourceSize.width: 800
        visible: false
    }
    AnimatedImage {
        id: animated
        fillMode: cache.fillMode
        asynchronous: image.asynchronous
        anchors.fill: parent
        cache: image.cache
        visible: false
    }
}
