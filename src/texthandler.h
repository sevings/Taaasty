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

#ifndef TEXTHANDLER_H
#define TEXTHANDLER_H

#include <QObject>
#include <QQuickTextDocument>
#include <QTextCharFormat>
#include <QTextCodec>
#include <QTextCursor>



class TextHandler : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QQuickItem* target       READ target         WRITE setTarget         NOTIFY targetChanged)

    Q_PROPERTY(int      cursorPosition  READ cursorPosition WRITE setCursorPosition NOTIFY cursorPositionChanged)
    Q_PROPERTY(int      selectionStart  READ selectionStart WRITE setSelectionStart NOTIFY selectionStartChanged)
    Q_PROPERTY(int      selectionEnd    READ selectionEnd   WRITE setSelectionEnd   NOTIFY selectionEndChanged)

    Q_PROPERTY(bool     bold            READ bold           WRITE setBold           NOTIFY boldChanged)
    Q_PROPERTY(bool     italic          READ italic         WRITE setItalic         NOTIFY italicChanged)
    Q_PROPERTY(bool     underline       READ underline      WRITE setUnderline      NOTIFY underlineChanged)

    Q_PROPERTY(QString  text            READ text           WRITE setText           NOTIFY textChanged)

public:
    explicit TextHandler(QObject *parent = 0);

    QQuickItem* target();
    void    setTarget(QQuickItem* target);

    int     cursorPosition() const;
    int     selectionStart() const;
    int     selectionEnd() const;

    void    setCursorPosition(int position);
    void    setSelectionStart(int position);
    void    setSelectionEnd(int position);

    bool    bold() const;
    bool    italic() const;
    bool    underline() const;

    void    setBold(bool arg);
    void    setItalic(bool arg);
    void    setUnderline(bool arg);

    QString text() const;
    void    setText(const QString& arg);

signals:
    void targetChanged();

    void cursorPositionChanged();
    void selectionStartChanged();
    void selectionEndChanged();

    void boldChanged();
    void italicChanged();
    void underlineChanged();

    void textChanged();

    void error(QString message);

private:
    void        _reset();
    QTextCursor _textCursor() const;
    void        _mergeFormat(const QTextCharFormat& format);

    QQuickItem*     _target;
    QTextDocument*  _doc;

    int             _cursorPosition;
    int             _selectionStart;
    int             _selectionEnd;
    QString         _text;
};

#endif // TEXTHANDLER_H
