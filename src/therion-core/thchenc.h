/**
 * @file thchenc.h
 * Therion character encoding module.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
 * 
 * $Date: $
 * $RCSfile: $
 * $Revision: $
 *
 * -------------------------------------------------------------------- 
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 * 
 * You should have received a copy of the GNU General Public License
 * along with this program; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */
 
#ifndef thchenc_h
#define thchenc_h

#include <string>


/**
 * Encode given string into UTF-8 internal encoding.
 *
 *  @return UTF-8 string.
 *  @param src Source string.
 *  @param srcenc Source encoding.
 */
std::string thencode(std::string_view src, int srcenc);
 
// TODO [[deprecated("Use thencode(std::string_view src, int srcenc) instead")]]
void thencode(std::string * dest, const char * src, int srcenc);


/**
 * Decode give string from internal UTF-8 encoding to some external.
 *
 *  @return Decoded string.
 *  @param destenc Destination encoding.
 *  @param src Source string.
 */
std::string thdecode(int destenc, std::string_view src);
 
// TODO [[deprecated("Use thdecode(int destenc, std::string_view src) instead")]]
void thdecode(std::string * dest, int destenc, const char * src);


/**
 * Writes supported encodings into output.
 */
 
void thprint_encodings();


/**
 * Parse encoding.
 */
 
int thparse_encoding(char * encstr);


#endif


