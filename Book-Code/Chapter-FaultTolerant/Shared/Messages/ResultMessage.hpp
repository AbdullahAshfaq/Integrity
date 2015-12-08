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

#ifndef _RESULTMESSAGE_HPP_
#define _RESULTMESSAGE_HPP_

#include "Message.hpp"

namespace Messages
{
	class ResultMessage : public Message
	{
	public:
		ResultMessage(Type type) :
			Message(type),
			m_taskId(0)
		{
		}
		ResultMessage(Type type, uint32_t id) :
			Message(type),
			m_taskId(id)
		{
		}

		uint32_t getTaskId()	{ return m_taskId; }

	protected:
		virtual void encodeMessage() override
		{
			this->encode(m_taskId);
			this->encodeResult();
		}
		virtual void decodeMessage() override
		{
			this->decode(m_taskId);
			this->decodeResult();
		}

		virtual void encodeResult() = 0;
		virtual void decodeResult() = 0;

	private:
		uint32_t m_taskId;
	};
}

#endif // _RESULTMESSAGE_HPP_