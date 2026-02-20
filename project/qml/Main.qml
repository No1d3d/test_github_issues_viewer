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
                text: "qt/qtbase" // Example repo for testing
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
                text: "Clear Cache"
                onClicked: issuesModel.clearCache()
            }
        }

        Text {
            text: issuesModel ? issuesModel.error : ""
            color: "red"
            Layout.fillWidth: true
            wrapMode: Text.WordWrap
        }

        // Page and total issues info
        Text {
            text: "Page " + issuesModel.currentPage + " of " + 
                  (issuesModel.totalPages > 0 ? issuesModel.totalPages : "?") + 
                  (issuesModel.totalIssues > 0 ? " (Total issues: " + issuesModel.totalIssues + ")" : "")
            font.bold: true
            Layout.alignment: Qt.AlignHCenter
        }

        ListView {
            id: listView
            Layout.fillWidth: true
            Layout.fillHeight: true
            model: issuesModel
            clip: true
            spacing: 2

            delegate: Rectangle {
                width: ListView.view.width
                height: 60
                color: index % 2 === 0 ? "#f5f5f5" : "white"
                border.color: "#ddd"
                border.width: 1
                radius: 3

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 8
                    spacing: 4

                    Text {
                        text: title
                        font.bold: true
                        elide: Text.ElideRight
                        Layout.fillWidth: true
                    }

                    Text {
                        text: url
                        color: "blue"
                        font.pixelSize: 10
                        elide: Text.ElideRight
                        Layout.fillWidth: true

                        MouseArea {
                            anchors.fill: parent
                            onClicked: Qt.openUrlExternally(url)
                        }
                    }
                }
            }

            ScrollBar.vertical: ScrollBar {
                active: true
            }
        }

        // Pagination controls
        RowLayout {
            Layout.alignment: Qt.AlignHCenter
            spacing: 5

            Button {
                text: "First"
                enabled: issuesModel.hasPreviousPage
                onClicked: issuesModel.loadPage(1)
            }

            Button {
                text: "Previous"
                enabled: issuesModel.hasPreviousPage
                onClicked: issuesModel.previousPage()
            }

            // Page number input
            SpinBox {
                id: pageSpinBox
                from: 1
                to: Math.max(1, issuesModel.totalPages)
                value: issuesModel.currentPage
                editable: true
                
                onValueModified: {
                    if (value !== issuesModel.currentPage) {
                        issuesModel.loadPage(value)
                    }
                }
            }

            Button {
                text: "Next"
                enabled: issuesModel.hasNextPage
                onClicked: issuesModel.nextPage()
            }

            Button {
                text: "Last"
                enabled: issuesModel.totalPages > 0
                onClicked: issuesModel.loadPage(issuesModel.totalPages)
            }
        }

        // Page size info
        Text {
            text: "Showing " + listView.count + " issues (10 per page)"
            font.pixelSize: 10
            color: "gray"
            Layout.alignment: Qt.AlignHCenter
        }
    }
}