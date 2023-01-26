import QtQuick 2.0
import QtQuick.Window 2.0
import QOfono 0.2

Window {
    width: 480
    height: 560
    visible: true

    Column {

        anchors.fill: parent

        Text {
            text: context1.active ? "online" : "offline"
        }

//        Text {
//            text:  "modem Manufacturer " + modemDefault.manufacturer
//        }

        Text {
            id: textLine2

            text: manager.available ? netreg.name : "Ofono not available"
        }

        Text {
            text: "simManager.pinRequired " + simManager.pinRequired
            visible: simManager.pinRequired !== OfonoSimManager.NoPin
        }

        Repeater {
            model: manager.modems

            Text {
                text: modelData
                font.bold: modelData === manager.defaultModem
            }

        }

    }

    MouseArea {
        anchors.fill: parent
        onClicked: {
            context1.active = !context1.active
            console.log("context1.active " + context1.active)
        }
    }

    OfonoManager {
        id: manager

        onAvailableChanged: {
            console.log("Ofono is " + available)
        }
        onModemAdded: {
            console.log("modem added " + modem)
        }
        onModemRemoved: {
            console.log("modem removed")
        }
        onModemsChanged: {
            console.log("ModemsChanged " + manager.modems)
        }
    }

    OfonoConnMan {
        id: ofono1

        modemPath: manager.defaultModem
        onValidChanged: {
            if (ofono1.valid) {
                console.log("OfonoConnMan.contexts " + ofono1.contexts)
            }
        }
    }

    OfonoModem {
        id: modemDefault

        modemPath: manager.defaultModem

        onValidChanged: {
            console.log(modemDefault.modemPath + ": modemDefault.valid " + modemDefault.valid)
            if (modemDefault.valid) {
//                console.log("modemDefault.name " + modemDefault.name)
//                console.log("modemDefault.model " + modemDefault.model)
                console.log("modemDefault.manufacturer " + modemDefault.manufacturer)
                console.log("modemDefault.features " + modemDefault.features)
            }
        }
    }

    OfonoSimManager {
        id: simManager

        modemPath: modemDefault.modemPath
        onValidChanged: {
            if (valid) {
                console.log("manager.pinRequired " + simManager.pinRequired)
            }
        }
    }

    OfonoContextConnection {
        id: context1

        contextPath: (ofono1.contexts !== undefined && ofono1.contexts.length > 0) ? ofono1.contexts[0] : ""
    }

    OfonoNetworkRegistration {
        id: netreg

        modemPath: manager.defaultModem
        Component.onCompleted: {
            netreg.scan()
        }
        onScanFinished: {
            if (valid) {
                console.log("[scanFinished] netreg.networkOperators: " + netreg.networkOperators)
            }
        }
        onNetworkOperatorsChanged: {
            if (valid) {
                console.log("[operatorsChanged] netreg.networkOperators: " + netreg.networkOperators)
            }
        }
        onValidChanged: {
            if (valid) {
                console.log("[validChanged] netreg.networkOperators: " + netreg.networkOperators)
            }
        }
    }

    OfonoNetworkOperator {
        id: netop

        onValidChanged: {
            if (valid) {
                console.log("netop: " + name + " / " + operatorPath)
            }
        }
    }

}
