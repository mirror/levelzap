// GuidString.cpp
// (c) 2011, Charles Lechasseur
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
// 
// The above copyright notice and this permission notice shall be included in
// all copies or substantial portions of the Software.
// 
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
// THE SOFTWARE.

#include "stdafx.h"
#include "GuidString.h"

//
// Default constructor. Creates a random string.
//
GuidString::GuidString()
    : m_Id(),
      m_String()
{
    // First create a random GUID.
    ::CoCreateGuid(&m_Id);

    // Now get string representation.
    OLECHAR buffer[GUIDSTRING_MAX];
    int returned = ::StringFromGUID2(m_Id, buffer, GUIDSTRING_MAX);
    if (returned != 0) {
        m_String.assign(buffer);
    }
}

//
// Copy constructor. Creates a copy of another GuidString.
//
// @param p_GuidString Object to copy.
//
GuidString::GuidString(const GuidString& p_GuidString)
    : m_Id(p_GuidString.m_Id),
      m_String(p_GuidString.m_String)
{
}

//
// Destructor.
//
GuidString::~GuidString()
{
}

//
// Assignment operator. Copies another GuidString.
//
// @param p_GuidString Object to copy.
// @return Reference to this object.
//
GuidString& GuidString::operator=(const GuidString& p_GuidString)
{
    if (this != &p_GuidString) {
        m_Id = p_GuidString.m_Id;
        m_String = p_GuidString.m_String;
    }
    return *this;
}

//
// Returns the ID used to generate the string.
//
// @return GUID used to create string.
//
const GUID& GuidString::Id() const
{
    return m_Id;
}

//
// Returns the random string representation.
//
// @param Random string.
//
const std::wstring& GuidString::String() const
{
    return m_String;
}