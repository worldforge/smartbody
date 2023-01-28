/****************************************************************************
** Meta object code from reading C++ file 'BMLCreatorDialog.h'
**
** Created: Tue May 29 15:01:03 2012
**      by: The Qt Meta Object Compiler version 63 (Qt 4.8.0)
**
** WARNING! All changes made in this file will be lost!
*****************************************************************************/

#include "BMLCreatorDialog.h"
#if !defined(Q_MOC_OUTPUT_REVISION)
#error "The header file 'BMLCreatorDialog.h' doesn't include <QObject>."
#elif Q_MOC_OUTPUT_REVISION != 63
#error "This file was generated using the moc from 4.8.0. It"
#error "cannot be used with the include files from this version of Qt."
#error "(The moc has changed too much.)"
#endif

QT_BEGIN_MOC_NAMESPACE
static const uint qt_meta_data_BmlCreatorDialog[] = {

 // content:
       6,       // revision
       0,       // classname
       0,    0, // classinfo
      11,   14, // methods
       0,    0, // properties
       0,    0, // enums/sets
       0,    0, // constructors
       0,       // flags
       0,       // signalCount

 // slots: signature, parameters, type, tag, flags
      18,   17,   17,   17, 0x0a,
      38,   17,   17,   17, 0x0a,
      68,   63,   17,   17, 0x0a,
     102,   63,   17,   17, 0x0a,
     143,  137,   17,   17, 0x0a,
     160,   63,   17,   17, 0x0a,
     192,  186,   17,   17, 0x0a,
     218,   17,   17,   17, 0x0a,
     228,   17,   17,   17, 0x0a,
     237,   17,   17,   17, 0x0a,
     256,  248,   17,   17, 0x0a,

       0        // eod
};

static const char qt_meta_stringdata_BmlCreatorDialog[] = {
    "BmlCreatorDialog\0\0TypingTextChanged()\0"
    "TypingSelectionChanged()\0text\0"
    "ComboCurrentIndexChanged(QString)\0"
    "CharacterSelectionChanged(QString)\0"
    "value\0SliderMoved(int)\0SpinValueChanged(QString)\0"
    "state\0CheckBoxStateChanged(int)\0"
    "Refresh()\0RunBml()\0ResetBml()\0currTab\0"
    "ChangedTab(int)\0"
};

void BmlCreatorDialog::qt_static_metacall(QObject *_o, QMetaObject::Call _c, int _id, void **_a)
{
    if (_c == QMetaObject::InvokeMetaMethod) {
        Q_ASSERT(staticMetaObject.cast(_o));
        BmlCreatorDialog *_t = static_cast<BmlCreatorDialog *>(_o);
        switch (_id) {
        case 0: _t->TypingTextChanged(); break;
        case 1: _t->TypingSelectionChanged(); break;
        case 2: _t->ComboCurrentIndexChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 3: _t->CharacterSelectionChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 4: _t->SliderMoved((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 5: _t->SpinValueChanged((*reinterpret_cast< const QString(*)>(_a[1]))); break;
        case 6: _t->CheckBoxStateChanged((*reinterpret_cast< int(*)>(_a[1]))); break;
        case 7: _t->Refresh(); break;
        case 8: _t->RunBml(); break;
        case 9: _t->ResetBml(); break;
        case 10: _t->ChangedTab((*reinterpret_cast< int(*)>(_a[1]))); break;
        default: ;
        }
    }
}

const QMetaObjectExtraData BmlCreatorDialog::staticMetaObjectExtraData = {
    0,  qt_static_metacall 
};

const QMetaObject BmlCreatorDialog::staticMetaObject = {
    { &QDialog::staticMetaObject, qt_meta_stringdata_BmlCreatorDialog,
      qt_meta_data_BmlCreatorDialog, &staticMetaObjectExtraData }
};

#ifdef Q_NO_DATA_RELOCATION
const QMetaObject &BmlCreatorDialog::getStaticMetaObject() { return staticMetaObject; }
#endif //Q_NO_DATA_RELOCATION

const QMetaObject *BmlCreatorDialog::metaObject() const
{
    return QObject::d_ptr->metaObject ? QObject::d_ptr->metaObject : &staticMetaObject;
}

void *BmlCreatorDialog::qt_metacast(const char *_clname)
{
    if (!_clname) return 0;
    if (!strcmp(_clname, qt_meta_stringdata_BmlCreatorDialog))
        return static_cast<void*>(const_cast< BmlCreatorDialog*>(this));
    return QDialog::qt_metacast(_clname);
}

int BmlCreatorDialog::qt_metacall(QMetaObject::Call _c, int _id, void **_a)
{
    _id = QDialog::qt_metacall(_c, _id, _a);
    if (_id < 0)
        return _id;
    if (_c == QMetaObject::InvokeMetaMethod) {
        if (_id < 11)
            qt_static_metacall(this, _c, _id, _a);
        _id -= 11;
    }
    return _id;
}
QT_END_MOC_NAMESPACE
