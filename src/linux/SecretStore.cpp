#include "linux/SecretStore.h"

#include "infrastructure/logging/Logging.h"

#include <QDBusArgument>
#include <QDBusConnection>
#include <QDBusError>
#include <QDBusInterface>
#include <QDBusMessage>
#include <QDBusMetaType>
#include <QDBusObjectPath>
#include <QDBusVariant>
#include <QMap>
#include <QMetaType>
#include <QVariant>
#include <QVariantMap>

namespace MyFin::Linux {

using SecretAttributes = QMap<QString, QString>;

struct SecretValue {
    QDBusObjectPath session;
    QByteArray parameters;
    QByteArray value;
    QString contentType;
};

QDBusArgument& operator<<(QDBusArgument& argument, const SecretValue& secret)
{
    argument.beginStructure();
    argument << secret.session << secret.parameters << secret.value << secret.contentType;
    argument.endStructure();
    return argument;
}

const QDBusArgument& operator>>(const QDBusArgument& argument, SecretValue& secret)
{
    argument.beginStructure();
    argument >> secret.session >> secret.parameters >> secret.value >> secret.contentType;
    argument.endStructure();
    return argument;
}

}  // namespace MyFin::Linux

Q_DECLARE_METATYPE(MyFin::Linux::SecretValue)
Q_DECLARE_METATYPE(MyFin::Linux::SecretAttributes)

namespace MyFin::Linux {

namespace {

constexpr auto kSecretServiceName = "org.freedesktop.secrets";
constexpr auto kSecretServicePath = "/org/freedesktop/secrets";
constexpr auto kSecretServiceInterface = "org.freedesktop.Secret.Service";
constexpr auto kSecretCollectionInterface = "org.freedesktop.Secret.Collection";
constexpr auto kSecretItemInterface = "org.freedesktop.Secret.Item";
constexpr auto kMyFinSecretLabel = "MyFin Jellyfin Session";

void registerDbusTypes()
{
    static const bool registered = [] {
        qRegisterMetaType<SecretValue>();
        qRegisterMetaType<SecretAttributes>();
        qDBusRegisterMetaType<SecretValue>();
        qDBusRegisterMetaType<SecretAttributes>();
        return true;
    }();

    Q_UNUSED(registered);
}

QDBusInterface serviceInterface()
{
    return {QString::fromLatin1(kSecretServiceName),
            QString::fromLatin1(kSecretServicePath),
            QString::fromLatin1(kSecretServiceInterface),
            QDBusConnection::sessionBus()};
}

SecretAttributes makeAttributes(const QString& accountKey)
{
    return {
        {QStringLiteral("application"), QStringLiteral("MyFin")},
        {QStringLiteral("service"), QStringLiteral("jellyfin")},
        {QStringLiteral("account"), accountKey},
    };
}

bool promptRequired(const QDBusObjectPath& promptPath)
{
    return !promptPath.path().isEmpty() && promptPath.path() != QStringLiteral("/");
}

template <typename T>
T unpackDbusValue(const QVariant& value)
{
    if (value.canConvert<T>()) {
        return value.value<T>();
    }

    if (value.userType() == qMetaTypeId<QDBusArgument>()) {
        return qdbus_cast<T>(value.value<QDBusArgument>());
    }

    return {};
}

QDBusObjectPath openPlainSession()
{
    registerDbusTypes();

    QDBusInterface service = serviceInterface();
    if (!service.isValid()) {
        return {};
    }

    QDBusMessage reply = service.call(QStringLiteral("OpenSession"),
                                      QStringLiteral("plain"),
                                      QVariant::fromValue(QDBusVariant(QString())));
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().size() < 2) {
        qWarning(lcApp) << "Failed to open Secret Service session:" << reply.errorMessage();
        return {};
    }

    return unpackDbusValue<QDBusObjectPath>(reply.arguments().at(1));
}

QDBusObjectPath defaultCollection()
{
    QDBusInterface service = serviceInterface();
    if (!service.isValid()) {
        return {};
    }

    QDBusMessage reply = service.call(QStringLiteral("ReadAlias"), QStringLiteral("default"));
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().isEmpty()) {
        qWarning(lcApp) << "Failed to resolve Secret Service default collection:" << reply.errorMessage();
        return {};
    }

    return unpackDbusValue<QDBusObjectPath>(reply.arguments().constFirst());
}

QList<QDBusObjectPath> unlockedItemsForAccount(const QString& accountKey)
{
    registerDbusTypes();

    QDBusInterface service = serviceInterface();
    if (!service.isValid()) {
        return {};
    }

    QDBusMessage reply = service.call(QStringLiteral("SearchItems"), QVariant::fromValue(makeAttributes(accountKey)));
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().isEmpty()) {
        qWarning(lcApp) << "Failed to search Secret Service items:" << reply.errorMessage();
        return {};
    }

    const QList<QDBusObjectPath> unlocked = unpackDbusValue<QList<QDBusObjectPath>>(reply.arguments().constFirst());
    if (reply.arguments().size() > 1) {
        const QList<QDBusObjectPath> locked = unpackDbusValue<QList<QDBusObjectPath>>(reply.arguments().at(1));
        if (!locked.isEmpty()) {
            qInfo(lcApp) << "Secret Service returned locked items for MyFin account; skipping locked entries";
        }
    }

    return unlocked;
}

}  // namespace

bool SecretStore::isAvailable() const
{
    registerDbusTypes();

    QDBusInterface service = serviceInterface();
    if (!service.isValid()) {
        return false;
    }

    const QDBusObjectPath collectionPath = defaultCollection();
    return !collectionPath.path().isEmpty() && collectionPath.path() != QStringLiteral("/");
}

bool SecretStore::storeToken(const QString& accountKey, const QString& token)
{
    if (accountKey.isEmpty() || token.isEmpty()) {
        return false;
    }

    const QDBusObjectPath collectionPath = defaultCollection();
    if (collectionPath.path().isEmpty() || collectionPath.path() == QStringLiteral("/")) {
        qWarning(lcApp) << "Secret Service default collection is not available";
        return false;
    }

    const QDBusObjectPath sessionPath = openPlainSession();
    if (sessionPath.path().isEmpty() || sessionPath.path() == QStringLiteral("/")) {
        return false;
    }

    QDBusInterface collection(QString::fromLatin1(kSecretServiceName),
                              collectionPath.path(),
                              QString::fromLatin1(kSecretCollectionInterface),
                              QDBusConnection::sessionBus());
    if (!collection.isValid()) {
        qWarning(lcApp) << "Secret Service collection interface is not valid";
        return false;
    }

    QVariantMap properties;
    properties.insert(QStringLiteral("org.freedesktop.Secret.Item.Label"), QString::fromLatin1(kMyFinSecretLabel));
    properties.insert(QStringLiteral("org.freedesktop.Secret.Item.Attributes"),
                      QVariant::fromValue(makeAttributes(accountKey)));

    SecretValue secret{
        .session = sessionPath,
        .parameters = {},
        .value = token.toUtf8(),
        .contentType = QStringLiteral("text/plain"),
    };

    QDBusMessage reply = collection.call(QStringLiteral("CreateItem"),
                                         properties,
                                         QVariant::fromValue(secret),
                                         true);
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().size() < 2) {
        qWarning(lcApp) << "Failed to store token in Secret Service:" << reply.errorMessage();
        return false;
    }

    const QDBusObjectPath promptPath = unpackDbusValue<QDBusObjectPath>(reply.arguments().at(1));
    if (promptRequired(promptPath)) {
        qWarning(lcApp) << "Secret Service requested a prompt for storing credentials, which is not handled yet";
        return false;
    }

    qInfo(lcApp) << "Stored Jellyfin token in Secret Service for account" << accountKey;
    return true;
}

QString SecretStore::readToken(const QString& accountKey) const
{
    if (accountKey.isEmpty()) {
        return {};
    }

    const QList<QDBusObjectPath> items = unlockedItemsForAccount(accountKey);
    if (items.isEmpty()) {
        return {};
    }

    const QDBusObjectPath sessionPath = openPlainSession();
    if (sessionPath.path().isEmpty() || sessionPath.path() == QStringLiteral("/")) {
        return {};
    }

    QDBusInterface item(QString::fromLatin1(kSecretServiceName),
                        items.constFirst().path(),
                        QString::fromLatin1(kSecretItemInterface),
                        QDBusConnection::sessionBus());
    if (!item.isValid()) {
        qWarning(lcApp) << "Secret Service item interface is not valid";
        return {};
    }

    QDBusMessage reply = item.call(QStringLiteral("GetSecret"), QVariant::fromValue(sessionPath));
    if (reply.type() == QDBusMessage::ErrorMessage || reply.arguments().isEmpty()) {
        qWarning(lcApp) << "Failed to read token from Secret Service:" << reply.errorMessage();
        return {};
    }

    const SecretValue secret = unpackDbusValue<SecretValue>(reply.arguments().constFirst());
    return QString::fromUtf8(secret.value);
}

bool SecretStore::removeToken(const QString& accountKey)
{
    if (accountKey.isEmpty()) {
        return false;
    }

    const QList<QDBusObjectPath> items = unlockedItemsForAccount(accountKey);
    if (items.isEmpty()) {
        return true;
    }

    bool removedAny = false;
    for (const QDBusObjectPath& itemPath : items) {
        QDBusInterface item(QString::fromLatin1(kSecretServiceName),
                            itemPath.path(),
                            QString::fromLatin1(kSecretItemInterface),
                            QDBusConnection::sessionBus());
        if (!item.isValid()) {
            continue;
        }

        QDBusMessage reply = item.call(QStringLiteral("Delete"));
        if (reply.type() == QDBusMessage::ErrorMessage) {
            qWarning(lcApp) << "Failed to delete Secret Service item:" << reply.errorMessage();
            continue;
        }

        if (!reply.arguments().isEmpty()) {
            const QDBusObjectPath promptPath = unpackDbusValue<QDBusObjectPath>(reply.arguments().constFirst());
            if (promptRequired(promptPath)) {
                qWarning(lcApp) << "Secret Service requested a prompt for deleting credentials, which is not handled yet";
                continue;
            }
        }

        removedAny = true;
    }

    return removedAny;
}

}  // namespace MyFin::Linux
