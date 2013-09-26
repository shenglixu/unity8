/*
 * Copyright (C) 2013 Canonical, Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; version 3.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

import QtQuick 2.0
import Ubuntu.Components 0.1
import "../../Components"
import "../../Components/ListItems" as ListItems
import ".."

DashPreview {
    id: genericPreview

    previewWidthRatio: 0.6

    property url url: previewData.image

    header: UbuntuShape {
        id: urlLoader
        anchors.left: parent.left
        anchors.right: parent.right
        height: width * previewImage.sourceSize.height / previewImage.sourceSize.width
        radius: "medium"
        image: Image {
            id: previewImage
            asynchronous: true
            source: genericPreview.url
            fillMode: Image.PreserveAspectCrop
        }
    }

    title: Label {
        id: title
        objectName: "titleLabel"
        anchors {
            top: parent.top
            topMargin: units.gu(2)
            left: parent.left
            right: parent.right
        }

        elide: Text.ElideRight
        fontSize: "x-large"
        font.weight: Font.Light
        color: Theme.palette.selected.backgroundText
        opacity: 0.9
        text: previewData.title
        style: Text.Raised
        styleColor: "black"
        maximumLineCount: 2
        wrapMode: Text.WordWrap
    }

    buttons: GridView {
        id: buttons
        model: genericPreview.previewData.actions

        property int buttonMaxWidth: units.gu(34)
        property int numOfColumns: Math.ceil((width + spacing) / (buttonMaxWidth + spacing))
        property int numOfRows: Math.ceil(count / numOfColumns)
        property int spacing: units.gu(1)
        height: Math.max(units.gu(4), units.gu(4)*numOfRows + spacing*(numOfRows - 1))

        cellWidth: width / numOfColumns
        cellHeight: buttonHeight + spacing
        property int buttonWidth: cellWidth - spacing / 2
        property int buttonHeight: units.gu(4)

        delegate: Button {
            width: Math.max(units.gu(4), buttons.buttonWidth)
            height: buttons.buttonHeight
            color: "#dd4814"
            text: modelData.displayName
            iconSource: modelData.iconHint
            iconPosition: "right"
            visible: true
            onClicked: {
                genericPreview.previewData.execute(modelData.id, { })
            }
        }
        focus: false
    }

    body: Column {
        spacing: units.gu(2)

        Label {
            anchors { left: parent.left; right: parent.right }
            visible: text != ""
            fontSize: "medium"
            opacity: 0.6
            color: "white"
            text: previewData.subtitle.replace(/[\r\n]+/g, "<br />")
            style: Text.Raised
            styleColor: "black"
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            // FIXME: workaround for https://bugreports.qt-project.org/browse/QTBUG-33020
            onWidthChanged: { wrapMode = Text.NoWrap; wrapMode = Text.WordWrap }
        }

        Label {
            anchors { left: parent.left; right: parent.right }
            visible: text != ""
            fontSize: "small"
            opacity: 0.6
            color: "white"
            text: previewData.description.replace(/[\r\n]+/g, "<br />")
            style: Text.Raised
            styleColor: "black"
            wrapMode: Text.WordWrap
            textFormat: Text.RichText
            // FIXME: workaround for https://bugreports.qt-project.org/browse/QTBUG-33020
            onWidthChanged: { wrapMode = Text.NoWrap; wrapMode = Text.WordWrap }
        }
    }
}
