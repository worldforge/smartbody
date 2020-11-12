/*************************************************************
Copyright (C) 2017 University of Southern California

This file is part of Smartbody.

Smartbody is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

Smartbody is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public License
along with Smartbody.  If not, see <http://www.gnu.org/licenses/>.

**************************************************************/

#ifndef XERCESC_UTILS_HPP
#define XERCESC_UTILS_HPP

#include <sb/SBTypes.h>
#include <string>
#include <sstream>

#include <xercesc/parsers/XercesDOMParser.hpp>
#include <xercesc/dom/DOM.hpp>
#include <xercesc/util/PlatformUtils.hpp>
#include <xercesc/util/XMLString.hpp>
#include <xercesc/sax/HandlerBase.hpp>
#include <xercesc/sax/SAXException.hpp>

#include <boost/shared_ptr.hpp>

XERCES_CPP_NAMESPACE_USE

namespace xml_utils {

////////////
	// recent additions for clean parsing:

	// patches for rampant wstring_t
	// Only if necessary
	std::string xml_w2s( std::wstring w );
	std::wstring xml_s2w( std::string s );

	// memory managed transcode
	bool xml_translate( std::string *str_p, const XMLCh* xml_str );
	
	SBAPI std::string xml_translate_string( const XMLCh* xml_str, std::string dfl = "" );
	std::wstring xml_translate_wide( const XMLCh* xml_str, std::string dfl = "" );

	double xml_translate_double( const XMLCh* xml_str, double dfl = 0.0 );
	float xml_translate_float( const XMLCh* xml_str, float dfl = 0.0f );
	int xml_translate_int( const XMLCh* xml_str, int dfl = 0 );

	// reverse transcode with requisite release
	// Try to minimize use...
	static XMLCh* xmlch_translate( std::string s ) {
		return( XMLString::transcode( s.c_str() ) );
	}
	static void xmlch_release( XMLCh ** xml_ch_pp )	{
		XMLString::release( xml_ch_pp );
	}
	
	// detailed error message
	void xml_parse_error( const XMLCh* attr, DOMElement* elem );

	// parse: extract/translate attribute value
	bool xml_parse_string( 
		std::string *str_p, 
		const XMLCh* attr, 
		DOMElement* elem,
		bool verbose = false
	);
	
	bool xml_parse_double( 
		double *d_p,
		const XMLCh* attr, 
		DOMElement* elem,
		bool verbose = false
	);

	bool xml_parse_float( 
		float *f_p,
		const XMLCh* attr, 
		DOMElement* elem,
		bool verbose = false
	);

	bool xml_parse_int( 
		int *i_p,
		const XMLCh* attr, 
		DOMElement* elem,
		bool verbose = false
	);

	int xml_parse_double( 
		double *d_arr,
		int arr_count,
		const XMLCh* attr, 
		DOMElement* elem,
		bool verbose = false
	);

	int xml_parse_float( 
		float *f_arr,
		int arr_count,
		const XMLCh* attr, 
		DOMElement* elem,
		bool verbose = false
	);

	int xml_parse_int( 
		int *i_arr,
		int arr_count,
		const XMLCh* attr, 
		DOMElement* elem,
		bool verbose = false
	);

	// parse: shorthand
	std::string 
	xml_parse_string( const XMLCh* A, DOMElement* E, std::string dfl = "", bool v = false );

	double 
	xml_parse_double( const XMLCh* A, DOMElement* E, double dfl = 0.0, bool v = false );

	float 
	xml_parse_float( const XMLCh* A, DOMElement* E, float dfl = 0.0f, bool v = false );

	int 
	xml_parse_int( const XMLCh* A, DOMElement* E, int dfl = 0, bool v = false );

	// end of recent additions... Apr 2011
////////////

	//  Get first child DOMElement
	DOMElement* getFirstChildElement( const DOMNode* node );

	//  Get next sibling DOMElement
	DOMElement* getNextElement( const DOMNode* node );

	//  parse a c-string as XML
	int parseCString( const char* data, AbstractDOMParser* parser );

	//  Convert XMLCh string to ascii c-string.
	//  chars > 127 are represented as 127 (an unused character in both ascii and Unicode)
  std::string asciiString( const XMLCh* wstr );

	//  Read XMLCh string as a float
	//  assumes the presence of wcstod(..).
	//  resets, but does not check errno for errors
	const float xcstof( const XMLCh* str );

	//  Read XMLCh string as a double
	//  assumes the presence of wcstod(..).
	//  resets, but does not check errno for errors
	const double xcstod( const XMLCh* str );

	//  Convert Text and Elements of DOMNode (and children) into a 8bit-per-char string
	void xmlToString( const DOMNode* node, std::string& converted );

    //  Takes in a C string (nth element from srArgBuffer, most likely).
    //  If is begins with <, assumes it is an XML string and parses it
    //  (returning a DOMDocument), otherwise assumes it is a filename 
    //  and parses the file contents as XML (again, returning the DOMDocument).
    //
    //  Returns NULL if there was an error during parsing.
	XERCES_CPP_NAMESPACE::DOMDocument* parseMessageXml( XercesDOMParser* xmlParser, const char *str );

	//  STL Comparason operator for XMLCh* using XMLString::compareString(..)
	class XMLStringCmp {
	public:
		bool operator() (const XMLCh*, const XMLCh*) const; 
	};

  template <typename Ch>
  struct TemporaryString {
    template <typename OtherCh>
    explicit TemporaryString(const OtherCh* otherStr)
    : _str(XMLString::transcode(otherStr))
    {}

    ~TemporaryString()
    {
      XMLString::release(&_str);
    }

    operator const Ch* () const { return _str; }

    // it's a temporary object, so no assignments to it.
    TemporaryString() = delete;
    TemporaryString(const TemporaryString&) = delete;
    TemporaryString(TemporaryString&&) = delete;
    TemporaryString& operator =(const TemporaryString&) = delete;
    TemporaryString& operator =(TemporaryString&&) = delete;
  private:
    Ch* _str;
  };

  typedef TemporaryString<char> UTF8;
  typedef TemporaryString<XMLCh> UTF16;

  inline std::string
  operator + (const std::string& lhs, const UTF8& rhs) {
    return lhs + (const char*)rhs;
  }
};

std::string convertWStringToString(std::wstring w);

#endif  //  XERCESC_UTILS_HPP
