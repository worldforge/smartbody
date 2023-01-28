#ifndef INTRUSIVE_PTR_FROM_PYTHON_HPP
#define INTRUSIVE_PTR_FROM_PYTHON_HPP

#include <boost/python/handle.hpp>
#include <boost/python/converter/shared_ptr_deleter.hpp>
#include <boost/python/converter/from_python.hpp>
#include <boost/python/converter/rvalue_from_python_data.hpp>
#include <boost/python/converter/registered.hpp>
#include <boost/intrusive_ptr.hpp>

namespace boostPatch {

using namespace boost;
using namespace boost::python;
using namespace boost::python::converter;

template<class T>
struct intrusive_ptr_from_python {
	intrusive_ptr_from_python() {
		converter::registry::insert(&convertible, &construct,
									type_id<intrusive_ptr<T> >());
	}

private:
	static void* convertible(PyObject* p) {
		if (p == Py_None)
			return p;

		return converter::get_lvalue_from_python(p,
												 registered<T>::converters);
	}

	static void construct(PyObject* source,
						  rvalue_from_python_stage1_data* data) {
		void* const storage =
				((converter::rvalue_from_python_storage<intrusive_ptr<T>
				>*) data)->storage.bytes;
		// Deal with the "None" case.
		if (data->convertible == source)
			new(storage) intrusive_ptr<T>();
		else
			new(storage) intrusive_ptr<T>(
					static_cast<T*>(data->convertible)
			);

		data->convertible = storage;
	}
};

} // namespace  boostPatch

#endif // INTRUSIVE_PTR_FROM_PYTHON_HPP