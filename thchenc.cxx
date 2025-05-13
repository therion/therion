/**
 * @file thchenc.cxx
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
 
#include "thchenc.h"
#include "thchencdata.cxx"
#include "therion.h"
#include "thexception.h"


void thencode(thbuffer * dest, const char * src, int srcenc)
{
  // check if source is not UTF-8
  if (srcenc == TT_UTF_8) {
    thdecode(dest,TT_ASCII,src);
    dest->strcpy(src);
    return;
  }
  
  size_t srcln = strlen(src), srcx = 0;
  unsigned char * srcp, * dstp;
  // check buffer size
  dest->guarantee(srcln + srcln + srcln + srcln + srcln + srcln);
  dstp = (unsigned char *) dest->get_buffer();
  srcp = (unsigned char *) src;
  
  while (srcx < srcln) {
  
    // check if encoding isn't needed
    if (*srcp < thchenc_facc) {
      *dstp = *srcp;
      dstp++;
    }
    // we have to encode
    else {

      const auto dch = thencode_tbl[*srcp - thchenc_facc][srcenc];

      // two byte UTF-8 character
      if (dch < 0X800) {
        *dstp = 192 + (dch / 64);
        dstp++;
        *dstp = 128 + (dch % 64);
        dstp++;
      }

      // three byte UTF-8 character
      else if (dch < 0X10000) {
        *dstp = 224 + (dch / 4096);
        dstp++;
        *dstp = 128 + ((dch % 4096) / 64);
        dstp++;
        *dstp = 128 + (dch % 64);
        dstp++;
      } 
      
      // longer chars not supported
      else
        therror(("unicode character over 0xFFFF not supported"));
    }
    
    srcx++;
    srcp++;
  }
  
  // end destination string with 0
  *dstp = 0;
  
}

 
void thdecode(thbuffer * dest, int destenc, const char * src)
{
  // chack if source is not UTF-8
  if (destenc == TT_UTF_8) {
    dest->strcpy(src);
    return;
  }
  
  size_t srcln = strlen(src), srcx = 0;
  unsigned char * srcp, * dstp;
  dest->guarantee(srcln);  // check buffer size
  dstp = (unsigned char*) dest->get_buffer();
  srcp = (unsigned char*) src;
  char32_t sch = 0;    
  
  while (srcx < srcln) {
  
    // check if decoding isn't needed
    if (*srcp < thchenc_facc)
      *dstp = *srcp;
    // we have to decode
    else {
      // one byte character
      if (*srcp < 0X7F)
        sch = static_cast<char32_t>(*srcp);
      // two byte character
      else if ((*srcp / 32) == 6) {
        sch = 64 * (*srcp % 32);
        srcp++;
        srcx++;
        if ((srcx >= srcln) || (*srcp < 128))
          therror(("invalid UTF-8 string -- \"%s\"",src));
        sch += *srcp % 64;
      }
      // three byte UTF-8 character
      else if ((*srcp / 16) == 14) {
        sch = 4096 * (*srcp % 16);
        srcp++;
        srcx++;
        if ((srcx >= srcln) || (*srcp < 128))
          therror(("invalid UTF-8 string -- \"%s\"",src));
        sch += 64 * (*srcp % 64);
        srcp++;
        srcx++;
        if ((srcx >= srcln) || (*srcp < 128))
          therror(("invalid UTF-8 string -- \"%s\"",src));
        sch += *srcp % 64;
      } 
      
      // longer chars not supported
      else
        therror(("invalid UTF-8 string -- \"%s\"",src));
        
      // now we have whchar_t value of UTF-8 character in sch
      if (sch < thchenc_fucc)
        *dstp = (char) sch;
      else {
      
        // let's binsearch it's position in the table
        long a = 0, b = (unsigned long)thdecode_tbl_size, x, ix = -1, r, sv = (unsigned long)sch;
        while (a <= b) {
          x = (unsigned long)((a + b) / 2);
          r = sv - long(thdecode_tbl_idx[x]);
          if (r == 0) {
            ix = x;
            break;
          }
          if (r > 0)
            a = x + 1;
          else
            b = x - 1;
        }

        if (ix == -1)
          *dstp = thdecode_undef;
        else
          *dstp = thdecode_tbl[ix][destenc];
      }  
    }  // end of decoding
    
    srcx++;
    srcp++;
    dstp++;
  }

  // end destination string with 0
  *dstp = 0;
  
}


void thprint_encodings()
{
  for(int i = 0; i <= TT_UTF_8; i++) {
    thprintf("%s\n", thtt_encoding[i].s);
  }
}


int thparse_encoding(char * encstr)
{
  int eid = thmatch_token(encstr, thtt_encoding);
  if (eid == TT_UNKNOWN_ENCODING)
    throw thexception(fmt::format("invalid encoding -- {}", encstr));
  return eid;
}
