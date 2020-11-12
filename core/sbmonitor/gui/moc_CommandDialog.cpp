/****************************************************************************
** Meta object code from reading C++ file 'CommandDialog.h'
**
** Created: Tue May 29 15:01:03 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "CommandDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'CommandDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_CommandDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       2,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      15,   14,   14,   14, 0x0a,
      25,   14,   14,   14, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_CommandDialog[] = {
    "CommandDialog\0\0RunCode()\0ClearOutputBox()\0"
};

void CommandDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        CommandDialog *_t = static_cast<CommandDialog *>(_o);
        switch (_id) {
        case 0: _t->RunCode(); break;
        case 1: _t->ClearOutputBox(); break;
        default: ;
        }
    }
    Q_UNUSED(_a);
}

const QMetaObjectExtraData CommandDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject CommandDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_CommandDialog,
      qt_meta_data_CommandDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &CommandDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *CommandDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *CommandDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_CommandDialog))
        return static_cast<void*>(const_cast< CommandDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int CommandDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 2)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 2;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
