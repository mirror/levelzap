// Nullable.cpp
// (c) 2008-2011, Charles Lechasseur
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
#include "Nullable.h"


//
// Default constructor.
//
ValueNotSetException::ValueNotSetException()
    : std::exception()
{
}

//
// Copy constructor.
//
ValueNotSetException::ValueNotSetException(const ValueNotSetException& p_Exception)
    : std::exception(p_Exception)
{
}

//
// Destructor.
//
ValueNotSetException::~ValueNotSetException()
{
}

//
// Assignment operator.
//
ValueNotSetException& ValueNotSetException::operator=(const ValueNotSetException& p_Exception)
{
    if (this != &p_Exception) {
        std::exception::operator=(p_Exception);
    }
    return *this;
}

//
// Exception description.
//
const char* ValueNotSetException::what() const
{
    return "ValueNotSetException";
}
