/*
 *  bml_exception.hpp - part of SmartBody-lib
 *  Copyright (C) 2008  University of Southern California
 *
 *  SmartBody-lib is free software: you can redistribute it and/or
 *  modify it under the terms of the Lesser GNU General Public License
 *  as published by the Free Software Foundation, version 3 of the
 *  license.
 *
 *  SmartBody-lib is distributed in the hope that it will be useful,
 *  but WITHOUT ANY WARRANTY; without even the implied warranty of
 *  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *  Lesser GNU General Public License for more details.
 *
 *  You should have received a copy of the Lesser GNU General Public
 *  License along with SmartBody-lib.  If not, see:
 *      http://www.gnu.org/licenses/lgpl-3.0.txt
 *
 *  CONTRIBUTORS:
 *      Andrew n marshall, USC
 *      Corne Versloot, while at USC
 *      Ed Fast, USC
 */

#ifndef BML_EXCEPTION_HPP
#define BML_EXCEPTION_HPP

#include <exception>


namespace BML {
	// Class Definitions
	/** Base class for all BML exceptions. */
	class BmlException : public std::runtime_error {

public:
		BmlException() : std::runtime_error("Unknown Exception") {}

		explicit BmlException(const std::string& str)
		:	std::runtime_error(str)
		{
		}

			virtual const char* type()
		{	return "BML::BmlException"; }
	};

	/** Exception of errors that occur during parsing. */
	class ParsingException : public BmlException {
	public:
		explicit ParsingException(const std::string& what_string )
		:	BmlException( what_string )
		{}

		const char* type() override
		{	return "BML::ParsingException"; }
	};
	/** Exception of errors that occur during scheduling. */
	class SchedulingException : public BmlException {
	public:
		explicit SchedulingException(const std::string& what_string )
		:	BmlException( what_string )
		{}

		const char* type() override
		{	return "BML::SchedulingException"; }
	};
	/** Exception of errors that occur during realizing. */
	class RealizingException : public BmlException {
	public:
		explicit RealizingException(const std::string& what_string )
		:	BmlException( what_string )
		{}

		const char* type() override
		{	return "BML::RealizingException"; }
	};
} // namespace BML

#endif  // BML_EXCEPTION_HPP
