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


#include <map>
#include <cmath>
#include <gwiz_math.h>
#include "sbm/sbm_speech.hpp"
#include "SBUtilities.h"

using namespace std;
using namespace SmartBody;


VisemeData::VisemeData( const std::string& id, int numKeys, const std::string& curveInfo )
	: _id( id ), _weight( 1.0 ), _time( 0.0 ), _duration( 0.0 ), _numKeys( numKeys ), _curveInfo( curveInfo ), _curveMode(true), _trapezoidMode(false), _floatCurveMode(false), _motionMode(false)
{
	// NOTE: We are parsing twice - once here in order to extract the proper duration of the curve,
	//       and later again when processing the command. These two parses should be unified here for
	//       efficiency reasons. The parse below is a 'light' parse - although we need to tokenize the
	//       entire curve data string, we are only interested in the last control point.

	std::vector<std::string> tokens;
	SmartBody:util::tokenize(curveInfo, tokens, " ");
	if (tokens.size() > 0 && numKeys > 0)
	{
		// make sure the curve info is properly created
		int index = 4 * (numKeys - 1);
		if (tokens.size() <= (unsigned int) index)
		{
			SmartBody::util::log("Curve %s is not properly created - insufficient number of keys (%d) - expected %d.", id.c_str(), tokens.size() / 4, numKeys * 4);
		}
		else
		{
			_duration = float(atof(tokens[index].c_str()));
		}
	}
}


ostream& operator << ( ostream& strm, const VisemeData& v ) {
	return strm << "VisemeData: " << (v.id()) << " (" << (v.weight()) <<") @ " << (v.time());
}

namespace SmartBody {

// this function scale curve c1 with weights
// the weight is also in the format of (t, v)
// assuming both curves are in ascending order
std::vector<float> scaleCurve(std::vector<float>& c1, std::vector<float>& weights)
{
	// return vector
	std::vector<float> ret;

	for (size_t i = 1; i < c1.size() / 2; ++i)
	{
		if (c1[i * 2] < c1[(i - 1) * 2])
		{
			SmartBody::util::log("scaleCurve: curve 1 is not in ascending order: %f %f", c1[(i - 1) * 2], c1[i * 2]);
		}
	}

	// empty protection
	if (c1.size() == 0 || weights.size() == 0)
	{
		for (size_t i = 0; i < c1.size(); i++)
			ret.emplace_back(c1[i]);
		return ret;
	}

	// going through two curves
	size_t index1 = 0;
	size_t index2 = 0;
	while (index1 < c1.size() / 2 && index2 < weights.size() / 2)
	{
		float x = c1[index1 * 2];
		float y = c1[index1 * 2 + 1];
		float t = weights[index2 * 2];
		float v = weights[index2 * 2 + 1];
		if (index2 == 0)	// edge case
		{
			if (x <= t)
			{
				ret.emplace_back(x);
				ret.emplace_back(y * v);
				index1++;
				continue;
			}
			else
			{
				index2++;
				continue;
			}
		}
		else
		{
			float tPrev = weights[(index2 - 1) * 2];
			float vPrev = weights[(index2 - 1) * 2 + 1];

			if (x >= tPrev && x <= t)
			{
				float scaleV = 1.0f;
				if (fabs(t - tPrev) != 0)
				{
					scaleV = (v - vPrev) * (x - tPrev) / (t - tPrev) + vPrev;
				}

				ret.emplace_back(x);
				ret.emplace_back(y * scaleV);
				index1++;
				continue;
			}
			else
			{
				index2++;
				continue;
			}
		}
	}

	// handle the left overs
	if (index2 == (weights.size() / 2 - 1))
	{
		for (size_t i = index1; i < c1.size() / 2; ++i)
		{
			float scaleY = weights[weights.size() - 1];
			ret.emplace_back(c1[index1 * 2]);
			ret.emplace_back(c1[index1 * 2 + 1] * scaleY);
		}
	}

	return ret;
}


// add curve function assume the two curves are in ascending order
std::vector<float> addCurve(std::vector<float>& c1, std::vector<float>& c2)
{
	std::vector<float> ret;

	for (size_t i = 1; i < c1.size() / 2; ++i)
	{
		if (c1[i * 2] < c1[(i - 1) * 2])
		{
			//SmartBody::util::log("addCurve: curve 1 is not in ascending order: %f %f", c1[(i - 1) * 2], c1[i * 2]);
		}
	}
	for (size_t i = 1; i < c2.size() / 2; ++i)
	{
		if (c2[i * 2] < c2[(i - 1) * 2])
		{
			//SmartBody::util::log("addCurve: curve 2 is not in ascending order: %f %f", c2[(i - 1) * 2], c2[i * 2]);
		}
	}

	size_t index1 = 0;
	size_t index2 = 0;
	while (index1 < c1.size() / 2 && index2 < c2.size() / 2)
	{
		float x1 = c1[index1 * 2];
		float y1 = c1[index1 * 2 + 1];
		float x2 = c2[index2 * 2];
		float y2 = c2[index2 * 2 + 1];
		if (x1 == x2)
		{
			ret.emplace_back(x1);
			ret.emplace_back(y1 + y2);
			index1++;
			index2++;
			continue;
		}
		else if (x1 < x2)
		{
			if (index2 == 0)	// edge case
			{
				ret.emplace_back(x1);
				ret.emplace_back(y1);
			}
			else
			{
				float x2Prev = c2[(index2 - 1) * 2];
				float y2Prev = c2[(index2 - 1) * 2 + 1];
				if (x1 >= x2Prev)
				{
					float curY2 = 0.0f;
					if (fabs(x2 - x2Prev) != 0)
					{
						curY2 = y2Prev + (y2 - y2Prev) * (x1 - x2Prev) / (x2 - x2Prev);
					}
					ret.emplace_back(x1);
					ret.emplace_back(y1 + curY2);
				}
				else
				{
					SmartBody::util::log("addCurve Warning: should not be here, point1(%f, %f), point2(%f, %f), previous point2(%f, %f)", x1, y1, x2, y2, x2Prev, y2Prev);
					ret.emplace_back(x1);
					ret.emplace_back(y1);
				}
			}
			index1++;
			continue;
		}
		else
		{
			if (index1 == 0)
			{
				ret.emplace_back(x2);
				ret.emplace_back(y2);
			}
			else
			{
				float x1Prev = c1[(index1 - 1) * 2];
				float y1Prev = c1[(index1 - 1) * 2 + 1];
				if (x2 >= x1Prev)
				{
					float curY1 = 0.0f;
					if (fabs(x1 - x1Prev) != 0)
					{
						curY1 = y1Prev + (y1 - y1Prev) * (x2 - x1Prev) / (x1 - x1Prev);
					}
					ret.emplace_back(x2);
					ret.emplace_back(y2 + curY1);
				}
				else
				{
					SmartBody::util::log("addCurve Warning: should not be here, point1(%f, %f), point2(%f, %f), previous point1(%f, %f)", x1, y1, x2, y2, x1Prev, y1Prev);
					ret.emplace_back(x2);
					ret.emplace_back(y2);
				}
			}
			index2++;
			continue;
		}
	}

	// handle the leftovers
	for (size_t i = index1; i < c1.size() / 2; i++)
	{
		ret.emplace_back(c1[i * 2]);
		ret.emplace_back(c1[i * 2 + 1]);
	}
	for (size_t i = index2; i < c2.size() / 2; i++)
	{
		ret.emplace_back(c2[i * 2]);
		ret.emplace_back(c2[i * 2 + 1]);
	}

	return ret;
}

// line intersection reference: http://cboard.cprogramming.com/c-programming/154196-check-if-two-line-segments-intersect.html
bool getLineIntersection(float p0_x, float p0_y, float p1_x, float p1_y, float p2_x, float p2_y, float p3_x, float p3_y, float& i_x, float& i_y)
{
	float s1_x, s1_y, s2_x, s2_y;
	s1_x = p1_x - p0_x;     s1_y = p1_y - p0_y;
	s2_x = p3_x - p2_x;     s2_y = p3_y - p2_y;

	float s, t;
	s = (-s1_y * (p0_x - p2_x) + s1_x * (p0_y - p2_y)) / (-s2_x * s1_y + s1_x * s2_y);
	t = ( s2_x * (p0_y - p2_y) - s2_y * (p0_x - p2_x)) / (-s2_x * s1_y + s1_x * s2_y);

	if (s >= 0 && s <= 1 && t >= 0 && t <= 1)
	{
		i_x = p0_x + (t * s1_x);
		i_y = p0_y + (t * s1_y);
		return true;
	}
	return false; // No collision
}

/*
// find the outline of 2 curves
std::vector<float> BML::SpeechRequest::stitchCurve(std::vector<float>& c1, std::vector<float>& c2)
{
	std::vector<float> retc;
	int size1 = c1.size();
	int size2 = c2.size();
	if (size1 == 0 || size2 == 0)
		return retc;

	// find the boundary of first curve
	int boundary = -1;
	if (c1[0] > c2[0])	// curve1 is actually starting from right side of curve2 ...
	{
		boundary = 0;
	}
	else
	{
		for (int i = size1 / 2 - 2; i >= 0; i--)
		{
			if (c1[i * 2] <= c2[0] && c1[(i + 1) * 2] > c2[0])
			{
				boundary = i;
				break;
			}
		}
	}
	if (boundary < 0) 	// two curve does not overlap, just add them together
	{
		for (int i = 0; i < size1; ++i)
			retc.emplace_back(c1[i]);
		for (int i = 0; i < size2; ++i)
			retc.emplace_back(c2[i]);
		return retc;
	}

	// first fill in the non-overlapping curves
	for (int i = 0; i < boundary; ++i)
	{
		retc.emplace_back(c1[i * 2]);
		retc.emplace_back(c1[i * 2 + 1]);
	}


	int id1 = boundary;
	int id2 = 0;
	int idi = 0;
	bool insertFirst = true;

	// find the intersection of the overlapping curves (O(n2))
	for (int i = boundary; i < (size1 / 2) - 1; ++i)
	{
		for (int j = 0; j < (size2 / 2) - 1; ++j)
		{
			float ix, iy;
			if (getLineIntersection(c1[i * 2], c1[i * 2 + 1], c1[(i + 1) * 2], c1[(i + 1) * 2 + 1], c2[j * 2], c2[j * 2 + 1], c2[(j + 1) * 2], c2[(j + 1) * 2 + 1], ix , iy))
			{
				if (idi == 0)
				{
					if (c1[boundary * 2] <= c2[0])
						insertFirst = true;
					else
						insertFirst = false;
				}

				if (insertFirst) // insert c1
				{
					for (int k = id1; k <= i; ++k)
					{
						retc.emplace_back(c1[k * 2]);
						retc.emplace_back(c1[k * 2 + 1]);
					}
					insertFirst = false;
				}
				else						// insert c2
				{
					for (int k = id2; k <= j; ++k)
					{
						retc.emplace_back(c2[k * 2]);
						retc.emplace_back(c2[k * 2 + 1]);
					}
					insertFirst = true;
				}
				id1 = i + 1;
				id2 = j + 1;
				retc.emplace_back(ix);
				retc.emplace_back(iy);
				idi++;
			}
		}
	}

	// left overs
	if (id1 < size1 / 2 || id2 < size2 / 2)
	{
		if (c1[size1 - 2] >= c2[size2 - 2])						// insert c1
		{
			for (int k = id1; k < size1 / 2; ++k)
			{
				retc.emplace_back(c1[k * 2]);
				retc.emplace_back(c1[k * 2 + 1]);
			}
		}
		else													// insert c2
		{
			for (int k = id2; k < size2 / 2; ++k)
			{
				retc.emplace_back(c2[k * 2]);
				retc.emplace_back(c2[k * 2 + 1]);
			}
		}
	}

	return retc;
}
*/

// use blend-in, blend-out methods, assuming c1 and c2 are in ascending order
std::vector<float> stitchCurve(std::vector<float>& c1, std::vector<float>& c2)
{
	std::vector<float> retc;
	int size1 = c1.size();
	int size2 = c2.size();
	if (size1 == 0 || size2 == 0)
		return retc;

	for (int i = 1; i < size1 / 2; ++i)
	{
		if (c1[i * 2] < c1[(i - 1) * 2])
		{
			//SmartBody::util::log("stitchCurve: curve 1 is not in ascending order: %f %f", c1[(i - 1) * 2], c1[i * 2]);
		}
	}
	for (int i = 1; i < size2 / 2; ++i)
	{
		if (c2[i * 2] < c2[(i - 1) * 2])
		{
			//SmartBody::util::log("stitchCurve: curve 2 is not in ascending order: %f %f", c2[(i - 1) * 2], c2[i * 2]);
		}
	}

	// determining the overlapping area
	if (c1[size1 - 2] < c2[0])	// no overlapping, c1 on the left
	{
		for (int i = 0; i < size1; ++i)
			retc.emplace_back(c1[i]);
		for (int i = 0; i < size2; ++i)
			retc.emplace_back(c2[i]);
		return retc;
	}
	if (c2[size2 - 2] < c1[0])	// no overlapping, c2 on the left
	{
		for (int i = 0; i < size2; ++i)
			retc.emplace_back(c2[i]);
		for (int i = 0; i < size1; ++i)
			retc.emplace_back(c1[i]);
		return retc;
	}

	float leftX = -1.0f;
	float rightX = -1.0f;
	if (c1[size1 - 2] >= c2[0] && c1[size1 - 2] <= c2[size2 - 2])
	{
		if (c1[0] >= c2[0])	// overlapping area: entire curve 1
		{
			leftX = c1[0];
			rightX = c1[size1 - 2];
		}
		else				// overlapping area: c2[0] -> c1[n]
		{
			leftX = c2[0];
			rightX = c1[size1 - 2];
		}
	}
	if (c2[size2 - 2] >= c1[0] && c2[size2 - 2] <= c1[size1 - 2])
	{
		if (c2[0] >= c1[0])	// overlapping area: entire curve 2
		{
			leftX = c2[0];
			rightX = c2[size2 - 2];
		}
		else				// overlapping area: c1[0] -> c2[n]
		{
			leftX = c1[0];
			rightX = c2[size2 - 2];
		}
	}
	if (leftX < 0 || rightX < 0)
	{
		SmartBody::util::log("BML::SpeechRequest::stitchCurve ERR: Did not find overlapping area.");
	}

	int index1 = 0;
	int index2 = 0;
	while (index1 < size1 / 2 && index2 < size2 / 2)
	{
		float curX1 = c1[index1 * 2 + 0];
		float curY1 = c1[index1 * 2 + 1];
		float curX2 = c2[index2 * 2 + 0];
		float curY2 = c2[index2 * 2 + 1];
		if (curX1 == curX2) // (fabs(curX1 - curX2) < 0.001f)
		{
			float ratioTransition = 0.0f;
			if (rightX != leftX)
				float ratioTransition = (curX1 - leftX) / (rightX - leftX);
			float finalY = (1 - ratioTransition) * curY1 + ratioTransition * curY2;
			retc.emplace_back(curX1);
			retc.emplace_back(finalY);
			index1++;
			index2++;
			continue;
		}
		else if (curX1 < curX2)
		{
			if (index2 > 0)
			{
				float prevX2 = c2[(index2 - 1) * 2 + 0];
				float prevY2 = c2[(index2 - 1) * 2 + 1];
				if (curX1 > prevX2)
				{
					float ratioCurve2 = (curX1 - prevX2) / (curX2 - prevX2);
					float curY2_X1 = ratioCurve2 * (curY2 - prevY2) + prevY2;
					float ratioTransition = (curX1 - leftX) / (rightX - leftX);
					float finalY = (1 - ratioTransition) * curY1 + ratioTransition * curY2_X1;
					retc.emplace_back(curX1);
					retc.emplace_back(finalY);
					index1++;
					continue;
				}
			}
			retc.emplace_back(curX1);
			retc.emplace_back(curY1);
			index1++;
			continue;
		}
		else
		{
			if (index1 > 0)
			{
				float prevX1 = c1[(index1 - 1) * 2 + 0];
				float prevY1 = c1[(index1 - 1) * 2 + 1];
				if (curX2 > prevX1)
				{
					float ratioCurve1 = (curX2 - prevX1) / (curX1 - prevX1);
					float curY1_X2 = ratioCurve1 * (curY1 - prevY1) + prevY1;
					float ratioTransition = (curX2 - leftX) / (rightX - leftX);
					float finalY = ratioTransition * curY2 + (1 - ratioTransition) * curY1_X2;
					retc.emplace_back(curX2);
					retc.emplace_back(finalY);
					index2++;
					continue;
				}
			}
			retc.emplace_back(curX2);
			retc.emplace_back(curY2);
			index2++;
			continue;
		}
	}

	bool blendIntoC2 = true;
	if (c1[0] > c2[0])
		blendIntoC2 = false;


	// handle the left over, discard if needed after blend into another one
	if (!blendIntoC2)
	{
		for (; index1 < size1 / 2; ++index1)
		{
			retc.emplace_back(c1[index1 * 2 + 0]);
			retc.emplace_back(c1[index1 * 2 + 1]);
		}
	}
	else
	{
		for (; index2 < size2 / 2; ++index2)
		{
			retc.emplace_back(c2[index2 * 2 + 0]);
			retc.emplace_back(c2[index2 * 2 + 1]);
		}
	}

	return retc;
}


void filterCurve(std::vector<float>&c, float speedLimit)
{
	if (speedLimit <= 0)
		return;

	std::vector<float> x;
	std::vector<float> y;
	std::vector<bool> markDelete;
	for (size_t i = 0; i < c.size(); i++)
	{
		if ((i % 2) == 0)
			x.emplace_back(c[i]);
		else
			y.emplace_back(c[i]);
	}

	// apply low pass filter
	for (size_t i = 0; i < x.size() - 1; ++i)
	{
		float speed = fabs(y[i] - y[i + 1]) / fabs(x[i] - x[i + 1]);
		if (speed > speedLimit)
		{
			//SmartBody::util::log("[%d]: %f", i, speed);
			float sign = (y[i] <= y[i + 1]) ? 1.0f : -1.0f;

			bool isZero = false;
			// if the point is 0, append a 0 after it
			if (fabs(y[i + 1]) < gwiz::epsilon4())
				isZero = true;

			y[i + 1] = fabs(x[i] - x[i + 1]) * speedLimit * sign + y[i];
			// clamp it
			if (y[i + 1] < 0)
				y[i + 1]  = 0.0f;
			if (isZero && i < (x.size() - 2))	// append one if y[i + 1] is zero
			{
				if ((x[i + 2] - x[i + 1]) > 0.1)	// 0.1 is adhoc
				{
					x.insert(x.begin() + i + 2, x[i + 1] + 0.1f);
					y.insert(y.begin() + i + 2, 0.0f);
					i = i + 2;
					continue;
				}
			}

			if (i == x.size() - 2)	// append one if last point has been changed
			{
				x.emplace_back(x[i] + 0.3f);	// 0.3 is adhoc
				y.emplace_back(0.0f);
			}

		}
	}


	// final stage
	c.clear();
	for (size_t i = 0; i < x.size(); i++)
	{
		c.emplace_back(x[i]);
		c.emplace_back(y[i]);
	}
}


void smoothCurve(std::vector<float>& c, std::vector<float>& timeMarkers, float windowSize)
{
	if (windowSize <= 0)
		return;

	bool keepSmoothing = true;
	int numIter = 0;

	while (keepSmoothing)
	{
		numIter++;
		std::vector<float> x;
		std::vector<float> y;
		std::vector<bool> markDelete;
		for (size_t i = 0; i < c.size(); i++)
		{
			if ((i % 2) == 0)
			{
				x.emplace_back(c[i]);
				markDelete.emplace_back(false);
			}
			else
				y.emplace_back(c[i]);
		}


		// global smoothing by window size
		std::vector<int> localMaxId;
		for (size_t i = 1; i < x.size() - 1; ++i)
		{
			if ((y[i] - y[i - 1]) >= 0 &&
				(y[i] - y[i + 1]) >= 0)
			{
				localMaxId.emplace_back(i);
			}
		}

		if (localMaxId.size() == 0)
			return;

		for (size_t i = 0; i < localMaxId.size() - 1; ++i)
		{
			if (x[localMaxId[i + 1]] - x[localMaxId[i]] <= windowSize)
			{
				for (int markeId = (localMaxId[i] + 1); markeId < localMaxId[i + 1]; ++markeId)
				{
					float toDeleteY = y[markeId];
					float ratio = (x[markeId] - x[localMaxId[i]]) / (x[localMaxId[i +1]] - x[localMaxId[i]]);
					float actualY = ratio * (y[localMaxId[i + 1]] - y[localMaxId[i]]) + y[localMaxId[i]];
					if (actualY >= toDeleteY)
						markDelete[markeId] = true;
				}
			}
		}

		c.clear();
		for (size_t i = 0; i < markDelete.size(); i++)
		{
			if (!markDelete[i])
			{
				c.emplace_back(x[i]);
				c.emplace_back(y[i]);
			}
		}

		if (c.size() == x.size() * 2)
			keepSmoothing = false;
	}
//	SmartBody::util::log("Number of smoothing iteration %d", numIter);
}

void constrainCurve(std::vector<float>& openCurve, std::vector<float>& otherCurve, float ratio)
{
	std::vector<float> openX;
	std::vector<float> openY;
	std::vector<float> otherX;
	std::vector<float> otherY;
	for (size_t i = 0; i < openCurve.size(); ++i)
	{
		if (i % 2 == 0)
			openX.emplace_back(openCurve[i]);
		else
			openY.emplace_back(openCurve[i]);
	}
	for (size_t i = 0; i < otherCurve.size(); ++i)
	{
		if (i % 2 == 0)
			otherX.emplace_back(otherCurve[i]);
		else
			otherY.emplace_back(otherCurve[i]);
	}

	float secStart = 0.0f;
	float secEnd = 0.0f;
	int secStartId = -1;
	int secEndId = -1;
	for (size_t i = 0; i < otherY.size() - 1; ++i)
	{
		if (fabs(otherY[i]) < .001 && otherY[i + 1] > 0)
		{
			secStart = otherX[i];
			secStartId = i;
		}
		if (otherY[i] > 0 && fabs(otherY[i + 1]) < .001)
		{
			secEnd = otherX[i + 1];
			secEndId = i + 1;

			if (secStartId < 0)
				continue;

			// looping through open curve points
			for (size_t j = 0; j < openX.size(); ++j)
			{
				if (openX[j] < otherX[secStartId] || openX[j] > otherX[secEndId])
					continue;

				for (int k = secStartId; k < secEndId - 1; ++k)
				{
					if (openX[j] >= otherX[k] && openX[j] <= otherX[k + 1])
					{
						float currPBMY = (openX[j] - otherX[k]) * (otherY[k + 1] - otherY[k]) / (otherX[k + 1] - otherX[k]) + otherY[k];
						float constrainOpenY = ratio - currPBMY;
						if (constrainOpenY < 0)
							constrainOpenY = 0;
						if (openY[j] > constrainOpenY)
							openY[j] = constrainOpenY;
					}
				}
			}

			// looping through other curve points
			for (int j = secStartId + 1; j < secEndId; ++j)
			{
				for (size_t k = 0; k < openX.size() - 1; ++k)
				{
					if (openX[k] <= otherX[j] && openX[k + 1] >= otherX[j])
					{
						float currOpenY = (otherX[j] - openX[k]) * (openY[k + 1] - openY[k]) / (openX[k + 1] - openX[k]) + openY[k];
						float constrainOpenY = ratio - otherY[j];
						if (constrainOpenY < 0)
							constrainOpenY = 0;
						if (currOpenY > constrainOpenY)
						{
							openX.insert(openX.begin() + k + 1, otherX[j]);
							openY.insert(openY.begin() + k + 1, constrainOpenY);
						}
						break;
					}
				}
			}
		}
	}

	openCurve.clear();
	for (size_t i = 0; i < openX.size(); ++i)
	{
		openCurve.emplace_back(openX[i]);
		openCurve.emplace_back(openY[i]);
	}
}

void constrainWidenCurve(std::vector<float>& widenCurve, float amount)
{
	std::vector<float> widenX;
	std::vector<float> widenY;

	for (size_t i = 0; i < widenCurve.size(); ++i)
	{
		if (i % 2 == 0)
			widenX.emplace_back(widenCurve[i]);
		else
			widenY.emplace_back(widenCurve[i]);
	}

	widenCurve.clear();
	for (size_t i = 0; i < widenY.size(); ++i)
	{
		if (widenY[i] > .99)
		{
			double offsetAmount = amount;
			if (i > 0)
			{
				if (widenX[i - 1] < widenX[i] - amount)
					offsetAmount = widenX[i - 1] - widenX[i] + .001;
			}
			widenCurve.emplace_back(widenX[i] - offsetAmount);
			widenCurve.emplace_back(widenY[i]);
			widenCurve.emplace_back(widenX[i]);
			widenCurve.emplace_back(widenY[i]);
			if (i < widenY.size() - 1)
			{
				if (widenX[i + 1] < widenX[i] + amount)
					offsetAmount = widenX[i + 1] - widenX[i] - .001;
			}
			widenCurve.emplace_back(widenX[i] + offsetAmount);
			widenCurve.emplace_back(widenY[i]);
		}
		else
		{
			widenCurve.emplace_back(widenX[i]);
			widenCurve.emplace_back(widenY[i]);
		}
	}

}

}
