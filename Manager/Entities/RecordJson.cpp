#include "RecordJson.h"
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <stdexcept>

namespace {
QJsonObject parseObject(const std::string &text) {
    QJsonParseError error;
    const auto document = QJsonDocument::fromJson(QByteArray::fromStdString(text), &error);
    if (error.error != QJsonParseError::NoError) {
        throw std::runtime_error("Invalid record JSON: " + error.errorString().toStdString());
    }
    if (!document.isObject()) {
        throw std::runtime_error("Expected a JSON object in library record");
    }
    return document.object();
}

std::string readString(const QJsonObject &object, const char *key) {
    const auto value = object.value(QLatin1String(key));
    if (!value.isString()) {
        throw std::runtime_error(std::string("Missing or invalid text property: ") + key);
    }
    return value.toString().toStdString();
}

std::string serialize(const QJsonObject &object) {
    return QJsonDocument(object).toJson(QJsonDocument::Compact).toStdString();
}
}

std::string Field::to_string() const {
    return serialize({{"name", QString::fromStdString(name_)}, {"content", QString::fromStdString(content_)}});
}

Field Field::from_string(const std::string &data) {
    const auto object = parseObject(data);
    Field parsed(readString(object, "name"));
    parsed.set_content(readString(object, "content"));
    *this = std::move(parsed);
    return *this;
}

RecordJson::Data RecordJson::decode(const std::string &serialized) {
    const auto object = parseObject(serialized);
    Data data{readString(object, "name"), readString(object, "type"), readString(object, "filepath"), {}};
    // Early libraries may omit fields_ for plain documents. Saved fields are JSON-encoded strings.
    const auto storedFields = object.value("fields_");
    if (!storedFields.isUndefined() && !storedFields.isArray()) {
        throw std::runtime_error("Invalid fields_ array in library record");
    }
    for (const auto &value : storedFields.toArray()) {
        if (!value.isString()) {
            throw std::runtime_error("Invalid field entry in library record");
        }
        Field field;
        field.from_string(value.toString().toStdString());
        data.fields.push_back(std::move(field));
    }
    return data;
}

std::string RecordJson::encode(const std::string &name, const std::string &type, const std::string &path,
                               const std::vector<Field> &fields) {
    QJsonArray values;
    for (const auto &field : fields) {
        values.append(QString::fromStdString(field.to_string()));
    }
    return serialize({{"name", QString::fromStdString(name)}, {"type", QString::fromStdString(type)},
                      {"filepath", QString::fromStdString(path)}, {"fields_", values}});
}
