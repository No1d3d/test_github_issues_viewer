import QtQuick
import QtQuick.Controls
import QtQuick.Layouts

ApplicationWindow {
    visible: true
    width: 900
    height: 600
    title: "Github Issues Viewer"

    ColumnLayout {
        anchors.fill: parent
        spacing: 10
        Layout.margins: 10

        RowLayout {
            spacing: 5
            Layout.fillWidth: true

            TextField {
                id: repoField
                placeholderText: "owner/repo or https://github.com/owner/repo"
                Layout.fillWidth: true
            }

            TextField {
                id: tokenField
                placeholderText: "GitHub token (optional)"
                echoMode: TextInput.Password
                width: 250
            }

            Button {
                text: "Load"
                onClicked: {
                    issuesModel.setToken(tokenField.text)
                    issuesModel.load(repoField.text)
                }
            }
        }

        Text {
            text: issuesModel ? issuesModel.error : ""
            color: "red"
            Layout.fillWidth: true
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: issuesModel
            clip: true

            delegate: Rectangle {
                width: ListView.view.width
                height: 50
                border.width: 1

                Text {
                    anchors.centerIn: parent
                    text: title
                    wrapMode: Text.Wrap
                }
            }

            onContentYChanged: {
                if (contentY + height >= contentHeight - 50) {
                    issuesModel.loadNextPage()
                }
            }
        }
    }
}
