import QtQuick 2.15
import QtQuick.Controls 2.15
import QtQuick.Dialogs 1.2
import QtQuick.Shapes 1.15
import Qt.labs.qmlmodels 1.0
import QTherion 1.0

ApplicationWindow {
    width: 640
    height: 480
    visible: true
    title: QTherion.projectName

    FileDialog {
        id: projectDialog
        selectMultiple: false
        selectExisting: true
        selectFolder: true
        onAccepted: {
            QTherion.openProject(fileUrl)
        }
    }

    Button {
        anchors.left: parent.left
        anchors.top: parent.top
        text: "Open..."
        z: 10
        onClicked: {
            console.log("click")
            projectDialog.open()
        }
    }

    SplitView {
        anchors.fill: parent
        orientation: Qt.Horizontal

        Item {
            id: surface
            clip: true
            SplitView.fillWidth: true

            Repeater {
                model: QTherion.scrapModel

                Repeater {
                    model: objectsRole
                    DelegateChooser {
                        role: "classNameRole"
                        DelegateChoice {
                            roleValue: "point"
                            Rectangle {
                                color: pointHover.hovered ? "red" : "white"
                                width: 5
                                height: width
                                x: xRole-width/2
                                y: yRole-width/2
                                onXChanged: xRole = x+width/2
                                onYChanged: yRole = y+width/2
                                DragHandler {}

                                HoverHandler {
                                    id: pointHover
                                }
                            }
                        }
                        DelegateChoice {
                            roleValue: "line"
                            Repeater {
                                model: linePointsRole

                                Item {
                                    Shape {
                                        visible: hasCpsRole
                                        ShapePath {
                                            strokeWidth: 1
                                            strokeColor: "blue"
                                            fillColor: "transparent"
                                            startX: xRole
                                            startY: yRole
                                            PathLine {
                                                x: cp1xRole
                                                y: cp1yRole
                                            }
                                        }
                                        ShapePath {
                                            strokeWidth: 1
                                            strokeColor: "blue"
                                            fillColor: "transparent"
                                            startX: xRole
                                            startY: yRole
                                            PathLine {
                                                x: cp2xRole
                                                y: cp2yRole
                                            }
                                        }
                                    }
                                    Shape {
                                        ShapePath {
                                            strokeWidth: 2
                                            strokeColor: "grey"
                                            strokeStyle: ShapePath.SolidLine
                                            fillColor: "transparent"
                                            PathSvg {
                                                path: lineSvgRole
                                            }
                                        }
                                    }
                                    Rectangle {
                                        color: lineHover.hovered ? "red" : "yellow"
                                        width: 5
                                        height: width
                                        x: xRole-width/2
                                        y: yRole-width/2
                                        z: 10
                                        onXChanged: xRole = x+width/2
                                        onYChanged: yRole = y+width/2
                                        DragHandler {}
                                        HoverHandler {id: lineHover}
                                    }
                                    Rectangle {
                                        color: cp1Hover.hovered ? "red" : "green"
                                        width: 5
                                        height: width
                                        x: cp1xRole-width/2
                                        y: cp1yRole-width/2
                                        visible: hasCpsRole
                                        onXChanged: cp1xRole = x+width/2
                                        onYChanged: cp1yRole = y+width/2
                                        DragHandler {}
                                        HoverHandler {
                                            id: cp1Hover
                                        }
                                    }
                                    Rectangle {
                                        color: cp2Hover.hovered ? "red" : "green"
                                        width: 5
                                        height: width
                                        x: cp2xRole-width/2
                                        y: cp2yRole-width/2
                                        visible: hasCpsRole
                                        onXChanged: cp2xRole = x+width/2
                                        onYChanged: cp2yRole = y+width/2
                                        DragHandler {}
                                        HoverHandler {
                                            id: cp2Hover
                                        }
                                    }
                                }
                            }
                        }
                    }
                }
            }
        }

        ListView {
            implicitWidth: 200
            model: QTherion.scrapModel
            delegate: ItemDelegate {
                text: nameRole
                width: ListView.view.width
            }
        }
    }
}
