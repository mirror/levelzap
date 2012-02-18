// ArrayAutoPtr.h
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

//
// ArrayAutoPtr
//
// Equivalent of std::auto_ptr for arrays. Destroys arrays with delete[].
//
template <typename T>
class ArrayAutoPtr
{
public:
                        //
                        // Constructor that initializes a null array.
                        //
                        ArrayAutoPtr()
                            : m_pArray(0)
                        {
                        }

                        //
                        // Constructor that assumes ownership of an existing array.
                        //
                        // @param p_pArray Array to wrap. We assume ownership.
                        //
                        ArrayAutoPtr(T* const p_pArray)
                            : m_pArray(p_pArray)
                        {
                        }

                        //
                        // Destructor. Destroys our array with delete[].
                        //
                        ~ArrayAutoPtr()
                        {
                            Set(0);
                        }

                        //
                        // Returns a reference to our array.
                        //
                        // @returns Array reference. Can be null.
                        //
    T*                  Get() const
                        {
                            return m_pArray;
                        }

                        //
                        // Cast operator that allows an ArrayAutoPtr
                        // to be cast to an array pointer of its type.
                        //
                        // @returns Array reference. Can be null.
                        //
                        operator T*() const
                        {
                            return m_pArray;
                        }

                        //
                        // Checks if this ArrayAutoPtr wraps a null array.
                        //
                        // @returns true if this array is null.
                        //
    bool                IsNull() const
                        {
                            return m_pArray == 0;
                        }

                        //
                        // Assignment operator. Assumes ownership of an array.
                        // If we already had an array, it is destroyed.
                        //
                        // @param p_pArray New array to wrap. We assume ownership.
                        //
    void                operator=(T* const p_pArray)
                        {
                            Set(p_pArray);
                        }

                        //
                        // Sets a new array to wrap.
                        // If we already had an array, it is destroyed.
                        //
                        // @param p_pArray New array to wrap. We assume ownership.
                        //
    void                Set(T* const p_pArray)
                        {
                            if (p_pArray != m_pArray) {
                                if (m_pArray != 0) {
                                    delete [] m_pArray;
                                }
                                m_pArray = p_pArray;
                            }
                        }

                        //
                        // Releases any array we have, clearing it so that
                        // this ArrayAutoPtr won't destroy it automatically.
                        // 
                        // @returns Array reference. Caller assumes ownership.
                        //
    T*                  Release()
                        {
                            T* pArray = m_pArray;
                            m_pArray = 0;
                            return pArray;
                        }

private:
    T*                  m_pArray;

    // THESE METHODS ARE NOT IMPLEMENTED.
                        ArrayAutoPtr(const ArrayAutoPtr<T>&);
    ArrayAutoPtr<T>&    operator=(const ArrayAutoPtr<T>&);
};
