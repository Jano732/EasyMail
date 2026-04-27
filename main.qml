import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15
import QtWebEngine



ApplicationWindow {

    visible: true
    width: 800
    height: 600
    title: "Easy Mail"

    Connections{
        target: service
        function onHtmlReady(html){
            emailWebView.loadHtml(html)
        }
        function onAttachmentsReady(){
            attachmentList.visible = attachmentModel.count > 0
        }
    }

    ColumnLayout{

        anchors.fill: parent
        spacing: 1

        Rectangle {
            Layout.alignment: Qt.AlignTo
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#6bb1d1"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 5

                Rectangle {
                    width: 30
                    height: 30
                    color: "transparent"
                    // border.color: "black"
                    // radius: 3

                    ColumnLayout {
                        anchors.centerIn: parent
                        spacing: 5

                        Rectangle { width: 30; height: 4; color: "black"; radius: 5 }
                        Rectangle { width: 30; height: 4; color: "black"; radius: 5 }
                        Rectangle { width: 30; height: 4; color: "black"; radius: 5 }
                    }

                    MouseArea {
                        anchors.fill: parent
                        onClicked: drawer.open()
                    }
                }

                Item { Layout.fillWidth: true }
            }

            Drawer {
                id: drawer
                width: 250
                height: parent.height
                edge: Qt.LeftEdge

                ColumnLayout {
                    anchors.fill: parent
                    anchors.margins: 16
                    spacing: 8

                    Text {
                        text: "Skrzynki odbiorcze"
                        font.pointSize: 16
                        font.bold: true
                    }

                    Rectangle { height: 1; Layout.fillWidth: true; color: "#ddd" }

                    ListView {
                        model: mailboxModel
                        Layout.fillWidth: true
                        Layout.fillHeight: true
                        clip: true

                        delegate: Rectangle {

                            width: ListView.view.width
                            height: 50
                            color: "white"

                            MouseArea{
                                anchors.fill: parent
                                onClicked: {
                                    service.changeMailbox(model.name)
                                }

                            }

                            RowLayout {
                                anchors.fill: parent
                                anchors.leftMargin: 8
                                anchors.rightMargin: 8

                                Text {
                                    Layout.fillWidth: true
                                    text: model.name
                                    color: "black"
                                    elide: Text.ElideRight
                                    verticalAlignment: Text.AlignVCenter
                                    font.bold: true
                                }

                                Text {
                                    text: model.elements
                                    color: "gray"
                                    verticalAlignment: Text.AlignVCenter
                                }
                            }
                        }
                    }

                    // Item { Layout.fillHeight: true }
                }
            }
        }

        SplitView {

            Layout.fillHeight: true
            Layout.fillWidth: true

            ListView {
                model: emailModel
                Layout.fillWidth: true
                Layout.fillHeight: true
                SplitView.minimumWidth: 401
                clip: true

                delegate: Rectangle {

                    MouseArea {
                        anchors.fill: parent
                        onClicked: {
                            attachmentModel.clear()
                            var email = service.getEmailByUid(model.uid)
                            service.requestBodyOfAnEmail(email)
                            service.changeReadState(email)
                        }
                    }

                    width: ListView.view.width
                    height: 70

                    Rectangle {
                        anchors.bottom: parent.bottom
                        width: parent.width
                        height: 1
                        color: "#eeeeee"
                    }

                    RowLayout {
                        anchors.fill: parent
                        anchors.leftMargin: 8
                        anchors.rightMargin: 8
                        spacing: 8

                        Rectangle {
                            radius: 50
                            width: 10
                            height: 10
                            color: "#6bb1d1"
                            Layout.alignment: Qt.AlignTop
                            Layout.topMargin: 16
                            visible: !model.isRead
                        }

                        ColumnLayout {
                            Layout.fillWidth: true
                            Layout.fillHeight: true
                            spacing: 4

                            Text {
                                text: model.subject
                                font.pointSize: 12
                                font.bold: true
                                Layout.fillWidth: true
                                elide: Text.ElideRight
                            }

                            RowLayout {
                                Layout.fillWidth: true

                                Text {
                                    text: model.from
                                    color: "gray"
                                    elide: Text.ElideRight
                                    Layout.fillWidth: true
                                }

                                Text {
                                    text: model.date
                                    color: "gray"
                                    font.pointSize: 10
                                }
                            }
                        }
                    }
                }
            }

            Rectangle {
                width: 500
                height: 500

                WebEngineView {
                    id: emailWebView
                    anchors.top: parent.top
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: attachmentList.y > 0 ? attachmentList.y - 8 : parent.height
                }

                ListView {
                    id: attachmentList
                    anchors.bottom: parent.bottom
                    anchors.left: parent.left
                    anchors.right: parent.right
                    height: count > 0 ? contentHeight : 0
                    model: attachmentModel

                    delegate: Rectangle {
                        width: parent.width
                        height: 48
                        color: "#f5f5f5"
                        radius: 6

                        Row {
                            anchors.verticalCenter: parent.verticalCenter
                            anchors.left: parent.left
                            anchors.leftMargin: 12
                            spacing: 10

                            Text {
                                text: model.filename
                                font.pixelSize: 14
                            }

                            Text {
                                text: model.mimeType
                                font.pixelSize: 12
                                color: "#888"
                            }
                        }

                        MouseArea {
                            anchors.fill: parent
                            onClicked: service.openAttachment(model.index)
                        }
                    }
                }
            }
        }
    }
}
