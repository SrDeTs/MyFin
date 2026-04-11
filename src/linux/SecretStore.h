#pragma once

#include <QString>

namespace MyFin::Linux {

class SecretStore final {
public:
    bool isAvailable() const;
    bool storeToken(const QString& accountKey, const QString& token);
    QString readToken(const QString& accountKey) const;
    bool removeToken(const QString& accountKey);
};

}  // namespace MyFin::Linux
