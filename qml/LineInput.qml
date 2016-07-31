import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0

Q.TextField {
    id: back
    anchors.margins: 1 * mm
    padding: 1 * mm
    font.pointSize: window.fontNormal
    implicitHeight: contentHeight + 2 * mm
    implicitWidth: 30 * mm
}
