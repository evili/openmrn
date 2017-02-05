/** \copyright
 * Copyright (c) 2016, Balazs Racz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are  permitted provided that the following conditions are met:
 *
 *  - Redistributions of source code must retain the above copyright notice,
 *    this list of conditions and the following disclaimer.
 *
 *  - Redistributions in binary form must reproduce the above copyright notice,
 *    this list of conditions and the following disclaimer in the documentation
 *    and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 *
 * \file format_utils.hxx
 *
 * Simple codefor some trivial formatting uses.
 *
 * @author Balazs Racz
 * @date 7 Feb 2016
 */

#ifndef _UTILS_FORMAT_UTILS_HXX_
#define _UTILS_FORMAT_UTILS_HXX_

#include <string>

/** Renders an integer to string, left-justified. @param buffer must be an at
 * @param buffer must be an at least 10 character long array.
 * @param value will be rendered into the buffer.
 * @returns the pointer to the null character at the end of the rendering.
 */
char* integer_to_buffer(int value, char* buffer);

/** Renders an unsigned integer to string, left-justified.
 * @param buffer must be an at least 10 character long array.
 * @param value will be rendered into the buffer.
 * @returns the pointer to the null character at the end of the rendering.
 */
char* unsigned_integer_to_buffer(int value, char* buffer);

/** Renders an unsigned integer to string, left-justified.
 * @param buffer must be an at least 10 character long array.
 * @param value will be rendered into the buffer.
 * @returns the pointer to the null character at the end of the rendering.
 */
char* unsigned_integer_to_buffer_hex(int value, char* buffer);

/// Formats a MAC address to string. Works both for Ethernet addresses as well
/// as for OpenLCB node IDs.
///
/// @param mac a 6-byte array storing the MAC address. mac[0] will be printed
/// at the beginning.
/// @param colons true to print colons, else false to exclude the colon
/// seperators
///
/// @return a string containing a colon-separated hexadecimal printout of the
/// given MAC address.
///
string mac_to_string(uint8_t mac[6], bool colons = true);

#endif // _UTILS_FORMAT_UTILS_HXX_
