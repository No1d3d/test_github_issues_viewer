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
        Layout.margins: 10   // <-- заменили padding на Layout.margins

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

            Button {
                text: "Next page"
                onClicked: issuesModel.loadNextPage()
            }
        }

        Text {
            text: issuesModel ? issuesModel.error : ""
            color: "red"
            Layout.fillWidth: true
        }

        ListView {
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: issuesModel

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
        }
    }
}
