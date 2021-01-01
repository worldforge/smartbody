/*
 *  me_controller_context_proxy.hpp - part of SmartBody-lib's Motion Engine
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
 */

#ifndef ME_CONTROLLER_CONTEXT_PROXY_HPP
#define ME_CONTROLLER_CONTEXT_PROXY_HPP


#include "controllers/me_controller_context.hpp"

/**
 *  Convience class for controllers that wish to reimplement or intercept 
 *  calls between their child controllers and the current MeControllerContext.
 */
class MeControllerContextProxy
	: public MeControllerContext
{
public:
	//////////////////////////////////////////////////////////////////////////
	//  Public Constants
	static std::string CONTEXT_TYPE;

protected:
	//////////////////////////////////////////////////////////////////////////
	//  Private Data
	boost::intrusive_ptr<MeControllerContext> _context;

	//////////////////////////////////////////////////////////////////////////
	//  Private Constructors

	explicit MeControllerContextProxy( boost::intrusive_ptr<MeControllerContext> context = nullptr );

public:
	~MeControllerContextProxy() override;

	//!  Implements MeControllerContext::context_type()
	const std::string& context_type() const override {	return CONTEXT_TYPE; }

	//!  Implements MeControllerContext::channels()
	SkChannelArray& channels() override;

	/**
	 *  Update the inner context.
	 */
	void context( boost::intrusive_ptr<MeControllerContext> context );

	/**
	 *  Returns the float buffer index coresponding 
	 *  to the given channel index.
     */
    int toBufferIndex( int chanIndex ) override;

};


/**
 *  Convience class for controllers that wish to reimplement or intercept 
 *  calls between their child controllers and the current MeFrameData.
 */
class MeFrameDataProxy
	: public MeFrameData
{
protected:
	/** Owned by this MeControllerContext instance...
	 *  Don't ref back (would cause a cyclical reference).
	 */
	MeControllerContextProxy* const _context;
	/**  Proxied frame data  */
	MeFrameData*                    _frame;

public:
	explicit MeFrameDataProxy( MeControllerContextProxy* context );

	/**
	 *  Sets the proxied MeFrameData.
	 *  MUST be called before this MeFrameDataProxy is a valid MeFrameData.
	 */
	virtual void set_proxied_frame( MeFrameData* frame );

	//!  Implements MeFrameData
	MeControllerContext* context() const override;

	//!  Implements MeFrameData
	int toBufferIndex( int chanIndex ) const override;

	//!  Implements MeFrameData
	SrBuffer<float>& buffer() override;

	//!  Implements MeFrameData
	void channelUpdated( unsigned int n ) override;

	//!  Implements MeFrameData
	bool isChannelUpdated( unsigned int n ) const override;


	///**
	// *  Implement runtime debugging hook called at the beginning of MeController::evaluate(..)
	// */
	//virtual void ct_pre_evaluate( MeController* ct, double time ) const;

	///**
	// *  Implement runtime debugging hook called at the end of MeController::evaluate(..)
	// */
	//virtual void ct_post_evaluate( MeController* ct, double time ) const;
};

#endif // ME_CONTROLLER_CONTEXT_PROXY_HPP
