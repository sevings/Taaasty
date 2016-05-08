import QtQuick 2.3
import ImageCache 2.0


AnimatedImage {
    id: image
    asynchronous: true
    cache: true
    smooth: true
    clip: true
    source: ''
    property string url: ''
    property CachedImage cachedImage: Cache.image(url)
    property color backgroundColor: '#373737'
    readonly property string extension: {
        switch (cachedImage.format)
        {
        case CachedImage.GifFormat:
            'gif'; break;
        case CachedImage.JpegFormat:
            'jpeg'; break;
        case CachedImage.PngFormat:
            'png'; break;
        default:
            ''; break;
        }
    }
    signal available
    function showImage() {
        image.source = cachedImage.source;
        back.visible = false;
        image.available()
    }
    function hideImage() {
        back.visible = true;
    }
    onCachedImageChanged: {
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
        onReadyToDownload: {
            if ((width > 0 && width < 12 * mm)
                    || (height > 0 && height < 12 * mm)) {
                cachedImage.download();
            }
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
            width: cachedImage.isDownloading && cachedImage.kbytesTotal > 0 ? cachedImage.kbytesReceived * (parent.width - 12 * mm)
                                                                              / cachedImage.kbytesTotal + 12 * mm
                                                                            : 12 * mm
            height: 12 * mm
            radius: width / 2
            color: window.brightColor
            visible: parent.width > 12 * mm && parent.height > 12 * mm
            Behavior on width {
                NumberAnimation { duration: 100 }
            }
            MouseArea {
                anchors.fill: parent
                onClicked: {
                    if (cachedImage.isDownloading)
                        cachedImage.abortDownload();
                    else
                        cachedImage.download();
                }
            }
            Text {
                id: bytesText
                font.pointSize: window.fontSmaller
                text: (cachedImage.isDownloading ? cachedImage.kbytesReceived + ' / ' : '')
                      + (cachedImage.kbytesTotal > 0 ? cachedImage.kbytesTotal + ' KB' : '')
                      + (image.extension ? '\n' + image.extension : '')
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
