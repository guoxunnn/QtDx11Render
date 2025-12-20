#ifndef CLIENT_MACRO_H
#define CLIENT_MACRO_H
#include <QObject>

#define REGISTER_Q_PROPERTY(type, name, initValue)                                                                     \
private:                                                                                                               \
    Q_PROPERTY(type name READ get##name WRITE set##name NOTIFY name##Changed)                                          \
public Q_SLOTS:                                                                                                          \
    void set##name(type value) {                                                                                       \
        if (value != this->name) {                                                                                     \
            this->name = value;                                                                                        \
            emit name##Changed();                                                                                      \
        }                                                                                                              \
    }                                                                                                                  \
    type get##name() const {                                                                                           \
        return this->name;                                                                                             \
    }                                                                                                                  \
Q_SIGNALS:                                                                                                             \
    void name##Changed();                                                                                              \
                                                                                                                       \
protected:                                                                                                             \
    type name = initValue;
#endif // CLIENT_MACRO_H
