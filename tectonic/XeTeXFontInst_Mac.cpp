/****************************************************************************\
 Part of the XeTeX typesetting system
 Copyright (c) 1994-2008 by SIL International
 Copyright (c) 2009 by Jonathan Kew
 Copyright (c) 2012, 2013 by Jiang Jiang
 Copyright (c) 2012-2015 by Khaled Hosny

 SIL Author(s): Jonathan Kew

Permission is hereby granted, free of charge, to any person obtaining
a copy of this software and associated documentation files (the
"Software"), to deal in the Software without restriction, including
without limitation the rights to use, copy, modify, merge, publish,
distribute, sublicense, and/or sell copies of the Software, and to
permit persons to whom the Software is furnished to do so, subject to
the following conditions:

The above copyright notice and this permission notice shall be
included in all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,
EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF
MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND
NONINFRINGEMENT. IN NO EVENT SHALL THE COPYRIGHT HOLDERS BE LIABLE
FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF
CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION
WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

Except as contained in this notice, the name of the copyright holders
shall not be used in advertising or otherwise to promote the sale,
use or other dealings in this Software without prior written
authorization from the copyright holders.
\****************************************************************************/

/*
 *   file name:  XeTeXFontInst_Mac.cpp
 *
 *   created on: 2005-10-22
 *   created by: Jonathan Kew
 */

#include <tectonic/tectonic.h>
#include <tectonic/XeTeXFontInst_Mac.h>
#include <tectonic/XeTeX_ext.h>

XeTeXFontInst_Mac::XeTeXFontInst_Mac(CTFontDescriptorRef descriptor, float pointSize, int &status)
    : XeTeXFontInst(NULL, 0, pointSize, status)
    , m_descriptor(descriptor)
    , m_fontRef(0)
{
    initialize(status);
}

XeTeXFontInst_Mac::~XeTeXFontInst_Mac()
{
    if (m_descriptor != 0)
        CFRelease(m_descriptor);
    if (m_fontRef != 0)
        CFRelease(m_fontRef);
}

void
XeTeXFontInst_Mac::initialize(int &status)
{
    if (m_descriptor == 0) {
        status = 1;
        return;
    }

    if (status != 0)
        m_descriptor = 0;

    // Create a copy of original font descriptor with font cascading (fallback) disabled
    CFArrayRef emptyCascadeList = CFArrayCreate(NULL, NULL, 0, &kCFTypeArrayCallBacks);
    const void* values[] = { emptyCascadeList };
    static const void* attributeKeys[] = { kCTFontCascadeListAttribute };
    CFDictionaryRef attributes = CFDictionaryCreate(NULL, attributeKeys, values, 1,
        &kCFTypeDictionaryKeyCallBacks, &kCFTypeDictionaryValueCallBacks);
    CFRelease(emptyCascadeList);

    m_descriptor = CTFontDescriptorCreateCopyWithAttributes(m_descriptor, attributes);
    CFRelease(attributes);
    m_fontRef = CTFontCreateWithFontDescriptor(m_descriptor, m_pointSize * 72.0 / 72.27, NULL);
    if (m_fontRef) {
        char *pathname;
        uint32_t index;
        pathname = getFileNameFromCTFont(m_fontRef, &index);

        XeTeXFontInst::initialize(pathname, index, status);
    } else {
        status = 1;
        CFRelease(m_descriptor);
        m_descriptor = 0;
    }
}
