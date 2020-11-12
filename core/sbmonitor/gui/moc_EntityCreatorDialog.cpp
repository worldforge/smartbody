/****************************************************************************
** Meta object code from reading C++ file 'EntityCreatorDialog.h'
**
** Created: Tue May 29 15:01:03 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "EntityCreatorDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'EntityCreatorDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_EntityCreatorDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
       3,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      26,   21,   20,   20, 0x08,
      53,   20,   20,   20, 0x08,
      62,   20,   20,   20, 0x08,

       0        // eod
};

static const char qt_meta_stringdata_EntityCreatorDialog[] = {
    "EntityCreatorDialog\0\0text\0"
    "EntityTypeChanged(QString)\0accept()\0"
    "reject()\0"
};

void EntityCreatorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        EntityCreatorDialog *_t = static_cast<EntityCreatorDialog *>(_o);
        switch (_id) {
        case 0: _t->EntityTypeChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 1: _t->accept(); break;
        case 2: _t->reject(); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData EntityCreatorDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject EntityCreatorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_EntityCreatorDialog,
      qt_meta_data_EntityCreatorDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &EntityCreatorDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *EntityCreatorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *EntityCreatorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_EntityCreatorDialog))
        return static_cast<void*>(const_cast< EntityCreatorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int EntityCreatorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 3)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 3;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
