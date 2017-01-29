// This is an open source non-commercial project. Dear PVS-Studio, please check it.
// PVS-Studio Static Code Analyzer for C, C++ and C#: http://www.viva64.com
/****************************************************************************
**
** Copyright (C) 2016 The Qt Company Ltd.
** Contact: https://www.qt.io/licensing/
**
** This file is part of the examples of the Qt Toolkit.
**
** $QT_BEGIN_LICENSE:BSD$
** Commercial License Usage
** Licensees holding valid commercial Qt licenses may use this file in
** accordance with the commercial license agreement provided with the
** Software or, alternatively, in accordance with the terms contained in
** a written agreement between you and The Qt Company. For licensing terms
** and conditions see https://www.qt.io/terms-conditions. For further
** information use the contact form at https://www.qt.io/contact-us.
**
** BSD License Usage
** Alternatively, you may use this file under the terms of the BSD license
** as follows:
**
** "Redistribution and use in source and binary forms, with or without
** modification, are permitted provided that the following conditions are
** met:
**   * Redistributions of source code must retain the above copyright
**     notice, this list of conditions and the following disclaimer.
**   * Redistributions in binary form must reproduce the above copyright
**     notice, this list of conditions and the following disclaimer in
**     the documentation and/or other materials provided with the
**     distribution.
**   * Neither the name of The Qt Company Ltd nor the names of its
**     contributors may be used to endorse or promote products derived
**     from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
**
** $QT_END_LICENSE$
**
****************************************************************************/

#include "texthandler.h"



TextHandler::TextHandler(QObject *parent)
    : QObject(parent)
    , _target(nullptr)
    , _doc(nullptr)
    , _cursorPosition(-1)
    , _selectionStart(0)
    , _selectionEnd(0)
{

}



QQuickItem*TextHandler::target()
{
    return _target;
}



void TextHandler::setTarget(QQuickItem* target)
{
    _doc    = 0;
    _target = target;
    if (!_target)
        return;

    auto doc = _target->property("textDocument");
    auto qqdoc = doc.value<QQuickTextDocument*>();
    if (qqdoc)
        _doc = qqdoc->textDocument();

    emit targetChanged();
}



int TextHandler::cursorPosition() const
{
    return _cursorPosition;
}



int TextHandler::selectionStart() const
{
    return _selectionStart;
}



int TextHandler::selectionEnd() const
{
    return _selectionEnd;
}



void TextHandler::setCursorPosition(int position)
{
    if (position == _cursorPosition)
        return;

    _cursorPosition = position;
    _reset();
}



void TextHandler::setSelectionStart(int position)
{
    _selectionStart = position;
}



void TextHandler::setSelectionEnd(int position)
{
    _selectionEnd = position;
}



bool TextHandler::bold() const
{
    auto cursor = _textCursor();
    if (cursor.isNull())
        return false;

    return cursor.charFormat().fontWeight() == QFont::Bold;
}



bool TextHandler::italic() const
{
    auto cursor = _textCursor();
    if (cursor.isNull())
        return false;

    return cursor.charFormat().fontItalic();
}



bool TextHandler::underline() const
{
    auto cursor = _textCursor();
    if (cursor.isNull())
        return false;

    return cursor.charFormat().fontUnderline();
}



void TextHandler::setBold(bool arg)
{
    QTextCharFormat fmt;
    fmt.setFontWeight(arg ? QFont::Bold : QFont::Normal);
    _mergeFormat(fmt);
    emit boldChanged();
}



void TextHandler::setItalic(bool arg)
{
    QTextCharFormat fmt;
    fmt.setFontItalic(arg);
    _mergeFormat(fmt);
    emit italicChanged();
}



void TextHandler::setUnderline(bool arg)
{
    QTextCharFormat fmt;
    fmt.setFontUnderline(arg);
    _mergeFormat(fmt);
    emit underlineChanged();
}



TextHandler::ListFormat TextHandler::list() const
{
    auto cursor = _textCursor();
    if (cursor.isNull())
        return ListNone;

    auto list = cursor.currentList();
    if (!list)
        return ListNone;

    return (ListFormat)list->format().style();
}



void TextHandler::setList(ListFormat list)
{
    auto cursor = _textCursor();
    if (cursor.isNull())
        return;

    auto block = _doc->findBlock(cursor.selectionStart());
    auto removeList = [&]()
    {
        while (block.position() < cursor.selectionEnd())
        {
            QTextCursor cursor(block);
            block = block.next();
            auto list = cursor.currentList();
            if (!list)
                continue;

            list->remove(block);

            auto format = cursor.blockFormat();
            format.setIndent(format.indent() - 1);
            cursor.mergeBlockFormat(format);
        }
    };

    switch (list) {
    case ListNone:
        removeList();
        break;
    case ListDecimal:
    case ListDisk:
    {
        QTextList* textList = nullptr;
        auto b = block.previous();
        while (!textList && b.position() < cursor.selectionEnd())
        {
            textList = b.textList();
            b = b.next();
        }

        if (!textList)
        {
            auto tl = b.textList();
            if (tl && tl->format().style() == (QTextListFormat::Style)list)
                textList = tl;
        }
        else if (textList->format().style() != (QTextListFormat::Style)list)
            removeList();

        if (!textList)
            textList = cursor.createList((QTextListFormat::Style)list);

        while (block.position() < cursor.selectionEnd())
        {
            textList->add(block);
            block = block.next();
        }
    }
        break;
    default:
        qDebug() << "Unknown list format:" << list;
        break;
    }

    emit listChanged();
}



QString TextHandler::text() const
{
    return _text;
}



void TextHandler::setText(const QString& arg)
{
    if (_text == arg)
        return;

    _text = arg;
    emit textChanged();
}



void TextHandler::_reset()
{
    emit boldChanged();
    emit italicChanged();
    emit underlineChanged();
}



QTextCursor TextHandler::_textCursor() const
{
    if (!_doc)
        return QTextCursor();

    QTextCursor cursor(_doc);

    if (_selectionStart != _selectionEnd)
    {
        cursor.setPosition(_selectionStart);
        cursor.setPosition(_selectionEnd, QTextCursor::KeepAnchor);
    }
    else
    {
        cursor.setPosition(_cursorPosition);
    }

    return cursor;
}



void TextHandler::_mergeFormat(const QTextCharFormat& format)
{
    auto cursor = _textCursor();
    if (!cursor.hasSelection())
        cursor.select(QTextCursor::WordUnderCursor);

    cursor.mergeCharFormat(format);
}
