#ifndef SETTINGS_H
#define SETTINGS_H

#include <QObject>
#include <QSettings>
#include <QDateTime>



class Settings : public QObject
{
    Q_OBJECT

    Q_PROPERTY(QString  login               READ login              WRITE setLogin              NOTIFY loginChanged)
    Q_PROPERTY(QString  accessToken         READ accessToken        WRITE setAccessToken        NOTIFY accessTokenChanged)
    Q_PROPERTY(int      maxImageWidth       READ maxImageWidth      WRITE setMaxImageWidth      NOTIFY maxImageWidthChanged)
    Q_PROPERTY(bool     autoloadImages      READ autoloadImages     WRITE setAutoloadImages     NOTIFY autoloadImagesChanged)
    Q_PROPERTY(bool     hideShortPosts      READ hideShortPosts     WRITE setHideShortPosts     NOTIFY hideShortPostsChanged)
    Q_PROPERTY(bool     hideNegativeRated   READ hideNegativeRated  WRITE setHideNegativeRated  NOTIFY hideNegativeRatedChanged)
    Q_PROPERTY(bool     darkTheme           READ darkTheme          WRITE setDarkTheme          NOTIFY darkThemeChanged)
    Q_PROPERTY(QString  lastTitle           READ lastTitle          WRITE setLastTitle          NOTIFY lastTitleChanged)
    Q_PROPERTY(QString  lastText            READ lastText           WRITE setLastText           NOTIFY lastTextChanged)

public:
    explicit Settings(QObject *parent = 0);

    QString login() const;
    void setLogin(const QString login);

    QString accessToken() const;
    void setAccessToken(const QString token);

    QDateTime expiresAt() const;
    void setExpiresAt(const QString date);

    int maxImageWidth() const;
    void setMaxImageWidth(const int width);

    bool autoloadImages() const;
    void setAutoloadImages(bool load);

    bool hideShortPosts() const;
    void setHideShortPosts(bool hide);

    bool hideNegativeRated() const;
    void setHideNegativeRated(bool hide);

    bool darkTheme() const;
    void setDarkTheme(bool dark);

    QString lastTitle() const;
    void setLastTitle(const QString title);

    QString lastText() const;
    void setLastText(const QString text);

signals:
    void loginChanged();
    void accessTokenChanged();
    void maxImageWidthChanged();
    void autoloadImagesChanged(bool);
    void hideShortPostsChanged();
    void hideNegativeRatedChanged();
    void darkThemeChanged();
    void lastTitleChanged();
    void lastTextChanged();

public slots:

private:
    QSettings _settings;

};

#endif // SETTINGS_H
