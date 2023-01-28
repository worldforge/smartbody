#ifndef REGISTER_INTRUSIVE_PTR_FROM_PYTHON_HPP
#define REGISTER_INTRUSIVE_PTR_FROM_PYTHON_HPP

#include <boost/python/object/class_metadata.hpp>

#include "intrusive_ptr_from_python.hpp"

namespace boostPatch {

// This function was based on register_shared_ptr_from_python_and_casts() from class_metadata.hpp.
//
// Preamble of register_class.  Also used for callback classes, which
// need some registration of their own.
//
template<class T, class Bases>
inline void register_intrusive_ptr_from_python_and_casts(T*, Bases) {
	using namespace boost::python::objects;

	// Constructor performs registration
	python::detail::force_instantiate(intrusive_ptr_from_python<T>());

	//
	// register all up/downcasts here.  We're using the alternate
	// interface to mpl::for_each to avoid an MSVC 6 bug.
	//
	register_dynamic_id<T>();
	mpl::for_each(register_base_of<T>(), (Bases*) 0,
				  (boost::add_pointer<mpl::_>*) 0);
}

} // namespace boostPatch

#endif // INTRUSIVE_PTR_FROM_PYTHON_HPP
