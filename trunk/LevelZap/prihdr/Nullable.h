// Nullable.h
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

#pragma once

#include <exception>

//
// Nullable<T>
//
// Template class representing a nullable value (e.g., a value that can be uninitialized).
// Can be used to make value types nullable. Inspired by .NET's Nullable<T> class.
//
template<class T>
class Nullable
{
public:
                        //
                        // Default constructor.
                        //
                        Nullable()
                            : m_Value(),
                              m_HasValue(false)
                        {
                        }

                        //
                        // Constructor with value.
                        //
                        Nullable(const T& p_Value)
                            : m_Value(p_Value),
                              m_HasValue(true)
                        {
                        }

                        //
                        // Copy constructor.
                        //
                        Nullable(const Nullable<T>& p_Nullable)
                            : m_Value(p_Nullable.m_Value),
                              m_HasValue(p_Nullable.m_HasValue)
                        {
                        }

                        //
                        // Destructor.
                        //
                        ~Nullable()
                        {
                        }

                        //
                        // Assignment operator.
                        //
    Nullable<T>&        operator=(const Nullable<T>& p_Nullable)
                        {
                            if (this != &p_Nullable) {
                                m_Value = p_Nullable.m_Value;
                                m_HasValue = p_Nullable.m_HasValue;
                            }
                            return *this;
                        }

                        //
                        // Templated assignment operator.
                        // Works as long as a static_cast
                        // between U and T is possible.
                        //
                        template<class U>
    Nullable<T>&        operator=(const Nullable<U>& p_Nullable)
                        {
                            m_Value = static_cast<T>(p_Nullable.m_Value);
                            m_HasValue = p_Nullable.m_HasValue;
                            return *this;
                        }

                        //
                        // Value assignment operator.
                        //
    Nullable<T>&        operator=(const T& p_Value)
                        {
                            SetValue(p_Value);
                            return *this;
                        }

                        //
                        // Templated value assignment operator.
                        // Works as long as a static_cast
                        // between U and T is possible.
                        //
                        template<class U>
    Nullable<T>&        operator=(const U& p_Value)
                        {
                            SetValue(static_cast<T>(p_Value));
                            return *this;
                        }

                        //
                        // Sets the value.
                        // After this method is called, HasValue() will return true.
                        //
    void                SetValue(const T& p_Value)
                        {
                            m_Value = p_Value;
                            m_HasValue = true;
                        }

                        //
                        // Cast operator.
                        //
                        operator T() const
                        {
                            return Value();
                        }

                        //
                        // Returns the value stored.
                        // If value wasn't set, an exception is thrown.
                        //
    T                   Value() const
                        {
                            EnsureValue();
                            return m_Value;
                        }

                        //
                        // Checks if value is set.
                        //
                        // @return true if value is set, false otherwise.
                        //
    bool                HasValue() const
                        {
                            return m_HasValue;
                        }

private:
    T                   m_Value;        // The value itself.
    bool                m_HasValue;     // Flag indicating if we have a value.

                        //
                        // Makes sure we have a value set.
                        // If not, an exception is thrown.
                        //
    void                EnsureValue() const
                        {
                            if (!m_HasValue) {
                                throw ValueNotSetException();
                            }
                        }
};


//
// ValueNotSetException
//
// Exception thrown by the Nullable<T> class if the value is accessed before being initialized.
//
class ValueNotSetException : public std::exception
{
public:
                            ValueNotSetException();
                            ValueNotSetException(const ValueNotSetException& p_Exception);
    virtual                 ~ValueNotSetException();

    ValueNotSetException&   operator=(const ValueNotSetException& p_Exception);

    virtual const char*     what() const;
};
