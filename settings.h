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

signals:
    void loginChanged();
    void accessTokenChanged();
    void maxImageWidthChanged();
    void autoloadImagesChanged(bool);
    void hideShortPostsChanged();
    void hideNegativeRatedChanged();

public slots:

private:
    QSettings _settings;

};

#endif // SETTINGS_H
