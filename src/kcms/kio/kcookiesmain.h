/*    kcookiesmain.h - Cookies configuration
 *
 *    First version of cookies configuration:
 *        Copyright (C) Waldo Bastian <bastian@kde.org>
 *    This dialog box:
 *        Copyright (C) David Faure <faure@kde.org>
 *
 */

#ifndef __KCOOKIESMAIN_H
#define __KCOOKIESMAIN_H

#include <kcmodule.h>

class QTabWidget;
class KCookiesPolicies;
class KCookiesManagement;

class KCookiesMain : public KCModule
{
    Q_OBJECT
public:
    KCookiesMain(QWidget *parent, const QVariantList &args);
    ~KCookiesMain();

    KCookiesPolicies* policyDlg() { return policies; }

    void save() override;
    void load() override;
    void defaults() override;
    QString quickHelp() const override;

private:

    QTabWidget* tab;
    KCookiesPolicies* policies;
    KCookiesManagement* management;
};

#endif // __KCOOKIESMAIN_H
