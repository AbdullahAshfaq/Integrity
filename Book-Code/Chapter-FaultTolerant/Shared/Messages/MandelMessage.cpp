/*
Copyright (c) 2015 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "MandelMessage.hpp"

namespace Messages
{
	// ------------------------------------------------------------------
	//
	// @details Encodes the mandel message details.
	//
	// ------------------------------------------------------------------
	void MandelMessage::encodeParameters()
	{
		this->encode(m_startRow);
		this->encode(m_endRow);
		this->encode(m_sizeX);
		this->encode(m_startX);
		this->encode(m_startY);
		this->encode(m_deltaX);
		this->encode(m_deltaY);
		this->encode(m_maxIterations);
	}

	// ------------------------------------------------------------------
	//
	// @details Decodes the mandel message details.
	//
	// ------------------------------------------------------------------
	void MandelMessage::decodeParameters()
	{
		this->decode(m_startRow);
		this->decode(m_endRow);
		this->decode(m_sizeX);
		this->decode(m_startX);
		this->decode(m_startY);
		this->decode(m_deltaX);
		this->decode(m_deltaY);
		this->decode(m_maxIterations);
	}
}
