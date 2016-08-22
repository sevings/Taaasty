import QtQuick 2.7
import QtQuick.Controls 2.0 as Q
import QtQuick.Controls.Material 2.0
import org.binque.taaasty 1.0

Pane {
    id: back
    Poppable {
        body: back
    }
    Flickable {
        anchors {
            top: parent.top
            left: parent.left
            right: parent.right
        }
        height: contentHeight > parent.height ? parent.height : contentHeight
        flickableDirection: Flickable.VerticalFlick
        contentWidth: parent.width
        contentHeight: column.height
        interactive: back.x == 0
        Poppable {
            body: back
        }
        Column {
            id: column
            width: window.width
            spacing: 1 * mm
            padding: 1 * mm
            Component.onCompleted: {
                imagesBox.checked = Settings.autoloadImages;
                shortBox.checked  = Settings.hideShortPosts;
                nbcBox.checked    = Settings.hideNegativeRated;
                darkBox.checked   = Settings.darkTheme;
            }
            ThemedText {
                text: 'Настройки'
                width: parent.width - parent.padding * 2
                font.pointSize: window.fontBigger
                horizontalAlignment: Text.AlignHCenter
            }
            ThemedCheckBox {
                id: imagesBox
                text: 'Загружать изображения'
                onCheckedChanged: { Settings.autoloadImages = checked; }
            }
            ThemedCheckBox {
                id: shortBox
                text: 'Скрывать короткие посты'
                onCheckedChanged: { Settings.hideShortPosts = checked; }
            }
            ThemedCheckBox {
                id: nbcBox
                text: 'Включить НБК'
                onCheckedChanged: { Settings.hideNegativeRated = checked; }
            }
            ThemedCheckBox {
                id: darkBox
                text: 'Темная тема'
                onCheckedChanged: { Settings.darkTheme = checked; }
            }
            MenuSeparator {
                width: implicitWidth - parent.padding * 2
            }
            ThemedText {
                width: parent.width - parent.padding * 2
                horizontalAlignment: Text.AlignHCenter
                text: Tasty.isAuthorized ? 'Вы вошли как <i>' + Settings.login + '</i>' : 'Вы не вошли'
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                highlighted: true
                text: Tasty.isAuthorized ? 'Сменить тлог' : 'Войти'
                onClicked: window.pushLoginDialog()
            }
            MenuSeparator {
                width: implicitWidth - parent.padding * 2
            }
            ThemedButton {
                anchors.horizontalCenter: parent.horizontalCenter
                width: 40 * mm
                highlighted: true
                text: 'О клиенте'
                onClicked: { window.pushAbout(); }
            }
        }
    }
}
