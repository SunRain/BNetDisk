import QtQuick 2.4
import Material 0.3

Item {
    id: pullToRefresh

    anchors {
        left: target.left
        right: target.right
    }

    y: -(internals.contentY + height)

    implicitHeight: 48 * Units.dp

    property Flickable target: parent

    /*!
      The property holds the offset the component is pulled from the \l target
      Flickable's \a topMargin. The property can be used to provide animations
      in custom contents.
      */
    readonly property real offset: -(target.topMargin - (y + height))

    property alias content: contentItem.children

    /*!
      The property notifies the component about the ongoing refresh operation.
      */
    property bool refreshing: false

    onRefreshingChanged: {
        if (!internals.ready || !enabled) {
            return;
        }
        if (!internals.releaseToRefresh && pullToRefresh.refreshing) {
            internals.flickableTopMargin = target.topMargin;
            internals.wasAtYBeginning = target.atYBeginning;
        }

        /*
          We cannot bind refreshing state activation clause with the
          control.refreshing property dirrectly as when the model is
          refreshed automatically (not manually via the component), we
          need to remember the flickable's topMargin in order to proceed
          with a proper Flickable rebinding. If we use property binding,
          the Connections' onRefreshingChanged will update the flickableTopMargin
          only after the binding is evaluated.
          */
         internals.refreshing = pullToRefresh.refreshing;
    }

    /*!
      Signal emitted when the model refresh is initiated by the component.
      */
    signal refresh()


    QtObject {
        id: internals

        property real contentY: pullToRefresh.target.contentY - pullToRefresh.target.originY

        /*!
          The property specifies when the component is ready to trigger the refresh()
          signal. The logic is defined by the style and the value is transferred
          from the style's releaseToRefresh property. The property can be used to
          define custom visuals for \l content.
          */
        property bool releaseToRefresh: false

        // specifies the component completion
        property bool ready: false

        // property to store Flickable's toipMargin at the time the pull or auto-refresh is started
        property real flickableTopMargin: 0.0

        // store when the drag has happened at the beginning of the Flickable's content
        property bool wasAtYBeginning: false

        // initial contentY value when pull started
        property real initialContentY: 0.0

        // point of release used in rebind animation between the ready-to-refresh and refreshing states
        property real pointOfRelease

        /*!
          The property holds the threshold value when the component should enter
          into \a ready-to-refresh state when dragged manually. The default value
          is the height of the component styled.
          */
        property real activationThreshold: pullToRefresh.height

        // drives the refreshing state
        property bool refreshing: false
    }

    Component.onCompleted: {
        if (target && target.header) {
            target.header.visibleChanged.connect(fixTopMargin);
            target.header.heightChanged.connect(fixTopMargin);
        }
        internals.ready = true;
    }

    function fixTopMargin() {
        if (state === "refreshing") {
            /*
              Fetch the topMargin, force state to disabled (idle will be turned on
              automatically when refreshing completes) and set the topMargin from
              the header change.
              */
            var topMargin = target.topMargin;
            state = "disabled";
            target.topMargin = topMargin;
        }
    }

    Label {
        id: labelComponent
        visible: pullToRefresh.enabled && !contentItem.visible
        width: parent.width
        height: parent.height
        text: internals.releaseToRefresh ? qsTr("Release to refresh...") : qsTr("Pull to refresh...")
        horizontalAlignment: Text.AlignHCenter
        verticalAlignment: Text.AlignVCenter
        Behavior on text {
            SequentialAnimation {
                NumberAnimation {
                    target: labelComponent
                    property: "opacity"
                    from: 1.0
                    to: 0.0
                }
                NumberAnimation {
                    target: labelComponent
                    property: "opacity"
                    from: 0.0
                    to: 1.0
                }
            }
        }
    }

    Item {
        id: contentItem
        width: parent.width
        height: parent.height
        visible: children.length > 0
    }

    Rectangle {
        id: refreshIndicatorItem
        width: childrenRect.width + 6 * Units.dp
        height: childrenRect.height + 6 * Units.dp
        anchors.centerIn: parent

        property bool running: false

        radius: width /2
        opacity: running ? 1 : 0

        Behavior on opacity {
            NumberAnimation { duration: 500; easing.type: Easing.InOutQuad }
        }

        ProgressCircle {
            id: circle
            anchors.centerIn: parent
            indeterminate: refreshIndicatorItem.running
            visible: refreshIndicatorItem.running
            Behavior on visible {
                NumberAnimation {duration: 400}
            }
            SequentialAnimation {
                running: refreshIndicatorItem.running
                loops: Animation.Infinite

                ColorAnimation {
                    from: "red"
                    to: "blue"
                    target: circle
                    properties: "color"
                    easing.type: Easing.InOutQuad
                    duration: 2400
                }

                ColorAnimation {
                    from: "blue"
                    to: "green"
                    target: circle
                    properties: "color"
                    easing.type: Easing.InOutQuad
                    duration: 1560
                }

                ColorAnimation {
                    from: "green"
                    to: "#FFCC00"
                    target: circle
                    properties: "color"
                    easing.type: Easing.InOutQuad
                    duration:  840
                }

                ColorAnimation {
                    from: "#FFCC00"
                    to: "red"
                    target: circle
                    properties: "color"
                    easing.type: Easing.InOutQuad
                    duration:  1200
                }
            }
        }
    }

    Connections {
        target: pullToRefresh.target
        onMovementStarted: {
            internals.wasAtYBeginning = target.atYBeginning;
            internals.initialContentY = target.contentY;
            internals.refreshing = false;
            internals.releaseToRefresh = false;
        }
        onMovementEnded: {
            internals.wasAtYBeginning = target.atYBeginning;
        }

        // catch when to initiate refresh
        onDraggingChanged: {
            if (!target.dragging && internals.releaseToRefresh) {
                internals.pointOfRelease = -(target.contentY - target.originY)
                internals.flickableTopMargin = target.topMargin;
                internals.refreshing = true;
                internals.releaseToRefresh = false;
            }
        }
        onContentYChanged: {
            if (internals.wasAtYBeginning && enabled && target.dragging) {
                internals.releaseToRefresh = ((internals.initialContentY - target.contentY) > internals.activationThreshold);
            }
        }

    }

    onStateChanged: {
        /*
           Label might not be ready when the component enters in refreshing
           state, therefore the visible property will not be properly returned to
           true. Because of the same reason we cannot have a PropertyChanges either
           as the target is not yet ready at that point.
           */
        if (contentItem.children > 0) {
            contentItem.visible = (state === "idle" || state === "ready-to-refresh");
        }
    }


    states: [
        State {
            name: "disabled"
            when: !enabled
        },
        State {
            name: "idle"
            extend: ""
            when: internals.ready && enabled && !internals.refreshing && !internals.releaseToRefresh
        },
        State {
            name: "ready-to-refresh"
            when: internals.ready && enabled && internals.releaseToRefresh && !internals.refreshing
        },
        State {
            name: "refreshing"
            when: internals.ready && enabled && internals.wasAtYBeginning && internals.refreshing
            PropertyChanges {
                target: refreshIndicatorItem
                running: true
            }
            PropertyChanges {
                target: pullToRefresh.target
                topMargin: internals.flickableTopMargin + height
            }
            PropertyChanges {
                target: labelComponent
                visible: false
            }
        }
    ]

    transitions: [
        Transition {
            from: "ready-to-refresh"
            to: "refreshing"
            SequentialAnimation {
                NumberAnimation {
                    target: target
                    property: "topMargin"
                    from: internals.pointOfRelease
                    to: internals.flickableTopMargin + height
                }
                ScriptAction {
                    script: {
                        refresh();
                    }
                }
            }
        },
        // transition to be applied when the model is auto-updating
        Transition {
            from: "idle"
            to: "refreshing"
            NumberAnimation {
                target: target
                property: "contentY"
                from: -internals.flickableTopMargin
                to: -internals.flickableTopMargin - height
            }
        },
        Transition {
            from: "refreshing"
            to: "idle"
            NumberAnimation {
                target: target
                property: "topMargin"
            }
        }
    ]
}
