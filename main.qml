import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Layouts 1.15

ApplicationWindow {
    visible: true
    width: 800
    height: 600
    title: "Easy Mail"

    ColumnLayout{

        anchors.fill: parent
        spacing: 1

        Rectangle {
            Layout.alignment: Qt.AlignTop
            Layout.fillWidth: true
            Layout.preferredHeight: 50
            color: "#6bb1d1"

            RowLayout {
                anchors.fill: parent
                anchors.margins: 5
                Item {
                    Layout.fillWidth: true
                }

                Rectangle{
                    Layout.alignment: Qt.AlignVCenter
                    width: 120
                    height: 30
                    color: "transparent"
                    border.color: "black"
                    radius: 3
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
                            var email = service.getEmailByUid(model.uid)
                            service.requestBodyByUid(email)
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

                    ColumnLayout {
                        anchors.margins: 5
                        anchors.fill: parent
                        spacing: 5

                        Text {
                            text: model.subject
                            font.pointSize: 12
                            font.bold: true
                            Layout.fillWidth: true
                            elide: Text.ElideRight
                        }

                        RowLayout {

                            Text {

                                text: model.from
                                color: "gray"
                            }

                            Item {
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

            Rectangle {
                width: 500
            }
        }
    }
}


