/*
 * <one line to give the library's name and an idea of what it does.>
 * Copyright (C) 2012  Vishesh Handa <me@vhanda.in>
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 */

#include "extractorplugin.h"
#include "extractorpluginmanager.h"

#include <KService>
#include <KMimeType>
#include <KServiceTypeTrader>
#include <KDebug>

using namespace KFileMetaData;

ExtractorPluginManager::ExtractorPluginManager(QObject* parent): QObject(parent)
{
    QList<ExtractorPlugin*> all = allExtractors();

    foreach (ExtractorPlugin* ex, all) {
        foreach (const QString& type, ex->mimetypes()) {
            m_extractors.insertMulti(type, ex);
        }
    }
}

ExtractorPluginManager::~ExtractorPluginManager()
{
    qDeleteAll(m_extractors.values().toSet());
}


QList<ExtractorPlugin*> ExtractorPluginManager::allExtractors()
{
    // Get all the plugins
    KService::List plugins = KServiceTypeTrader::self()->query("KFileMetaDataExtractor");

    QList<ExtractorPlugin*> extractors;
    KService::List::const_iterator it;
    for (it = plugins.constBegin(); it != plugins.constEnd(); it++) {
        KService::Ptr service = *it;

        QString error;
        ExtractorPlugin* ex = service->createInstance<ExtractorPlugin>(this, QVariantList(), &error);
        if (!ex) {
            kError() << "Could not create Extractor: " << service->library();
            kError() << error;
            continue;
        }

        extractors << ex;
    }

    return extractors;
}

QList<ExtractorPlugin*> ExtractorPluginManager::fetchExtractors(const QString& mimetype)
{
    QList<ExtractorPlugin*> plugins = m_extractors.values(mimetype);
    if (plugins.isEmpty()) {
        QHash<QString, ExtractorPlugin*>::const_iterator it = m_extractors.constBegin();
        for (; it != m_extractors.constEnd(); it++) {
            if (mimetype.startsWith(it.key()))
                plugins << it.value();
        }
    }

    return plugins;
}
