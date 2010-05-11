/*
   Copyright (C) 2009 Paul Walger <metaxy@walger.name>

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.

   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY; without even the implied warranty of
   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the
   GNU General Public License for more details.

   You should have received a copy of the GNU General Public License
   along with this program; if not, write to the Free Software
   Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301, USA.
*/
#ifndef KIO_FILEBOXES_H
#define KIO_BOOKBOXES_H
#include <kio/forwardingslavebase.h>
#include <QtCore/QSettings>
#include <QtCore/QFile>
#include <fileboxes/boxesbackend.h>
class FileBoxesProtocol : public KIO::ForwardingSlaveBase
{
    Q_OBJECT
public:
    FileBoxesProtocol(const QByteArray& protocol, const QByteArray &pool, const QByteArray &app);
    virtual ~FileBoxesProtocol();
    void listDir( const KUrl& url );
    void stat( const KUrl& url );
    void del( const KUrl& url, bool isfile );
    void mimetype( const KUrl& url );
    void put( const KUrl& url, int permissions, KIO::JobFlags flags );

protected:

        void prepareUDSEntry( KIO::UDSEntry &entry,
                              bool listing = false ) const;
        bool rewriteUrl( const KUrl& url, KUrl& newURL );
private:
    QString m_fileBoxesHome;
    BoxesBackend *m_backend;
    KIO::UDSEntry createLink( QUrl url);
    KIO::UDSEntry createBox( const QString& boxID );

    int parseUrl( const KUrl& url );
};

#endif
