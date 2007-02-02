/**
 * @file thattr.cxx
 */
  
/* Copyright (C) 2007 Stacho Mudrak
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */

 
#include "thattr.h"
#include "extern/shapefil.h"
#include "thexception.h"
#include "thchenc.h"
#include <cctype>
#include <cmath>
#include <set>

thattr::thattr()
{
  this->m_num_fields = 0;
  this->m_num_objects = 0;
  this->m_obj_last = NULL;
}


thattr_field * thattr::insert_field(const char * name)
{
  thattr_field tmp, * r;
  tmp.m_id = this->m_num_fields++;
  tmp.m_name = name;
  tmp.m_parent = this;
  r = &(*this->m_field_list.insert(this->m_field_list.end(), tmp));
  for(size_t i = 0; i < tmp.m_name.size(); ++i)
    tmp.m_name[i] = tolower(tmp.m_name[i]);
  this->m_field_map[tmp.m_name] = r;
  return r;
}
  

thattr_field * thattr::get_field(const char * name, bool ins)
{
  thattr_name2field_map::iterator it;
  std::string str(name);
  for(size_t i = 0; i < str.size(); ++i)
    str[i] = tolower(str[i]);
  it = this->m_field_map.find(str);
  if (it == this->m_field_map.end()) {
    if (!ins)
      return NULL;
    else {
      return this->insert_field(name);
    }
  } else
    return it->second;
}


thattr_obj * thattr::get_object(long user_id)
{
  if ((this->m_obj_last != NULL) && (this->m_obj_last->m_user_id == user_id))
    return this->m_obj_last;
  thattr_usrid2obj_map::iterator it;
  it = this->m_obj_usrmap.find(user_id);
  if (it == this->m_obj_usrmap.end())
    return NULL;
  else
    return it->second;
}


thattr_obj * thattr::insert_object(void * data, long user_id)
{
  thattr_obj tmp, * r;
  tmp.m_id = this->m_num_objects++;
  tmp.m_data = data;
  tmp.m_parent = this;
  r = &(*this->m_obj_list.insert(this->m_obj_list.end(), tmp));
  this->m_obj_map[tmp.m_id] = r;
  if (user_id > -1) {
    r->m_user_id = user_id;
    this->m_obj_usrmap[user_id] = r;
  }
  this->m_obj_last = r;
  return r;
}


void thattr::insert_attribute(const char * name, thattr_attr & attr, long user_id)
{
  // Find object.
  thattr_obj * o;
  if (user_id < 0)
    o = this->m_obj_last;
  else
    o = this->get_object(user_id);

  if (o == NULL)
    return;

  // Find field.
  thattr_field * f = this->get_field(name, true);

  // Insert attribute.
  if ((attr.m_type == THATTR_STRING) && (attr.m_val_string.length() == 0)) {
    o->m_attributes.erase(f->m_id);
  } else {
    attr.m_field = f;
    attr.m_obj = o;
    o->m_attributes[f->m_id] = attr;
  }

}


void thattr::insert_attribute(const char * name, const char * value, long user_id)
{
  thattr_attr a;
  a.m_type = THATTR_STRING;
  if (value != NULL)
    a.m_val_string = std::string(value);
  else
    a.m_val_string = std::string("");
  this->insert_attribute(name, a, user_id);
}


void thattr::insert_attribute(const char * name, double value, long user_id)
{
  thattr_attr a;
  a.m_type = THATTR_DOUBLE;
  a.m_val_double = value;
  this->insert_attribute(name, a, user_id);
}


void thattr::insert_attribute(const char * name, long value, long user_id)
{
  thattr_attr a;
  a.m_type = THATTR_INTEGER;
  a.m_val_long = value;
  this->insert_attribute(name, a, user_id);
}


void thattr::copy_attributes(thattr_obj * object, long user_id)
{
  thattr_id2attr_map::iterator it;
  it = object->m_attributes.begin();
  thattr_attr tmp;
  while (it != object->m_attributes.end()) {
    tmp = it->second;
    this->insert_attribute(it->second.m_field->m_name.c_str(), tmp, user_id);
    ++it;
  }
}


std::string expdbf_field_name(std::string name, std::set<std::string> & fset)
{
  std::string nn;
  nn = name;
  int i;
  char x[20];
  if (nn.length() > 11) {
    do {
      i = 0;
      sprintf(x, "%d", i);
      nn = name.substr(0, 10 - strlen(x));
      nn += "_";
      nn += x;
      i++;
    } while (fset.find(nn) != fset.end());
  }
  fset.insert(nn);
  return nn;
}


void thattr::analyze_fields()
{
  // modify record names to match DBF format
  size_t i;
  std::set<std::string> fnset;
  thattr_field * cf;
  thattr_field_list::iterator fli;
  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    cf->m_type = THATTR_INTEGER;
    cf->m_present = false;
    cf->m_xdbf_decimals = 0;
    cf->m_xdbf_width = 1;
    cf->m_xdbf_name = expdbf_field_name(cf->m_name, fnset);
    cf->m_xmp_name = "ATTR_";
    cf->m_xmp_name += cf->m_name;
    for(i = 5; i < cf->m_xmp_name.size(); ++i) {
      if (cf->m_xmp_name[i] <= '9')
        cf->m_xmp_name[i] = cf->m_xmp_name[i] + 8;
      else if ((cf->m_xmp_name[i] < 'a'))
        cf->m_xmp_name[i] = tolower(cf->m_xmp_name[i]);
    }
    cf->m_xmp_last_numeric = 4004.0;
    cf->m_xmp_last_string = std::string("");
    cf->m_maxs = 0;
  }

  // check record types and limits
  long cvall;
  double cvald;
  size_t cslen;
  int ctype;
  char b[64];
  thattr_attr * ca;
  thattr_obj_list::iterator oi;
  thattr_id2attr_map::iterator ai;
  for(oi = this->m_obj_list.begin(); oi != this->m_obj_list.end(); ++oi) {
    for(ai = oi->m_attributes.begin(); ai != oi->m_attributes.end(); ai++) {
      ca = &(ai->second);
      cf = ca->m_field;
      ctype = ca->m_type;
      switch (ctype) {
        case THATTR_INTEGER:
          cvall = ca->m_val_long;
          ca->m_val_double = double(cvall);
          sprintf(b,"%ld", cvall);
          ca->m_val_string = b;
          break;
        case THATTR_DOUBLE:
          cvald = ai->second.m_val_double;
          ca->m_val_long = long(cvald);
          sprintf(b,"%g", cvald);
          ca->m_val_string = b;
          if (cf->m_type == THATTR_INTEGER)
            cf->m_type = THATTR_DOUBLE;
          break;
        case THATTR_STRING:
          if ((cf->m_type == THATTR_INTEGER) || (cf->m_type == THATTR_DOUBLE))
            cf->m_type = THATTR_STRING;
          break;
      }
      cslen = ca->m_val_string.length();
      if ((!cf->m_present) && ((ctype == THATTR_INTEGER) || (ctype == THATTR_DOUBLE))) {
        cf->m_maxd = ca->m_val_double;
        cf->m_mind = ca->m_val_double;
        cf->m_maxi = ca->m_val_long;
        cf->m_mini = ca->m_val_long;
      }
      cf->m_present = true;
      if (cslen > cf->m_maxs) cf->m_maxs = cslen;
      if ((ctype == THATTR_INTEGER) || (ctype == THATTR_DOUBLE)) {
        if (cf->m_maxd < ca->m_val_double) cf->m_maxd = ca->m_val_double;
        if (cf->m_mind > ca->m_val_double) cf->m_mind = ca->m_val_double;
        if (cf->m_maxi < ca->m_val_long) cf->m_maxi = ca->m_val_long;
        if (cf->m_mini > ca->m_val_long) cf->m_mini = ca->m_val_long;
      }
    }
  }

  
  // postprocess fields
#define myabs(x) ((x) > 0 ? (x) : ((x) < 0 ? -(x) : 1))
  int w2;
  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    cf->m_xmp_numeric = false;
    cf->m_xmp_transform = false;
    switch (cf->m_type) {
      case THATTR_INTEGER:
        cf->m_xdbf_decimals = 0;
        cf->m_xdbf_width = (int) log10(double(myabs(cf->m_maxi)));
        w2 = (int) log10(double(myabs(cf->m_mini)));
        if (cf->m_xdbf_width < w2) cf->m_xdbf_width = w2;
        ++cf->m_xdbf_width;
        cf->m_xmp_numeric = true;
        if ((cf->m_mini < -4000) || (cf->m_maxi > 4000))
          cf->m_xmp_transform = true;
        break;
      case THATTR_DOUBLE:
        cf->m_xdbf_width = (int) log10(myabs(cf->m_maxd));
        w2 = (int) log10(myabs(cf->m_mind));
        if (cf->m_xdbf_width < w2) cf->m_xdbf_width = w2;
        ++cf->m_xdbf_width;
        cf->m_xdbf_decimals = 0;
        if (cf->m_xdbf_width < 12) {
          cf->m_xdbf_decimals = 12 - cf->m_xdbf_width;
          cf->m_xdbf_width = 12;
        }
        cf->m_xmp_numeric = true;
        if ((cf->m_mind < -4000.0) || (cf->m_maxd > 4000.0))
          cf->m_xmp_transform = true;
        break;
      case THATTR_STRING:
        cf->m_xdbf_width = int(cf->m_maxs + 1);
        break;
    }
  }
  

}


void thattr::export_dbf(const char * fname, int encoding)
{
  // Create file.
  DBFHandle h;
  thattr_attr * ca;
  thattr_field * cf;
  thattr_obj_list::iterator oi;
  thattr_id2attr_map::iterator ai;
  thattr_field_list::iterator fli;
  thbuffer enc;

  this->analyze_fields();
  h = DBFCreate(fname);
  if (h == NULL) {
    thwarning(("unable to open file for output -- %s", fname));
    return;
  }

  // Create fields.
  DBFFieldType ftype(FTInvalid);
  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    switch (cf->m_type) {
      case THATTR_INTEGER:
        ftype = FTInteger;
        break;
      case THATTR_DOUBLE:
        ftype = FTDouble;
        break;
      case THATTR_STRING:
        ftype = FTString;
        break;
    }
    cf->m_xdbf_field = DBFAddField(h, cf->m_xdbf_name.c_str(), ftype, cf->m_xdbf_width, cf->m_xdbf_decimals); 
    if (cf->m_xdbf_field == -1) {
      thwarning(("error writing to %s", fname))
      goto EXPORT_DBF_EXIT;
    }
  }

  // Insert objects and write fields.
 for(oi = this->m_obj_list.begin(); oi != this->m_obj_list.end(); ++oi) {
    for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
      cf = &(*fli);
      ai = oi->m_attributes.find(cf->m_id);
      if (ai == oi->m_attributes.end()) {
        DBFWriteNULLAttribute(h, (int) oi->m_id, cf->m_xdbf_field);
      } else {
        ca = &(ai->second);
        switch (cf->m_type) {
          case THATTR_INTEGER:
            DBFWriteIntegerAttribute(h, (int) oi->m_id, cf->m_xdbf_field, (int) ca->m_val_long);
            break;
          case THATTR_DOUBLE:
            DBFWriteDoubleAttribute(h, (int) oi->m_id, cf->m_xdbf_field, ca->m_val_double);
            break;
          case THATTR_STRING:
            thdecode(&enc, encoding, ca->m_val_string.c_str());
            DBFWriteStringAttribute(h, (int) oi->m_id, cf->m_xdbf_field, enc.get_buffer());
            //DBFWriteStringAttribute(h, (int) oi->m_id, cf->m_xdbf_field, ca->m_val_string.c_str());
            break;
        }
      }
    }
  }

  // Close DBF file.
EXPORT_DBF_EXIT:
  DBFClose(h);
}


void thattr::export_mp_header(FILE * f)
{
  if (this->m_num_fields == 0)
    return;
  this->analyze_fields();
  thattr_field * cf;
  thattr_field_list::iterator fli;
  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    fprintf(f,"boolean %s_numeric;\n", cf->m_xmp_name.c_str());
    fprintf(f,"%s_numeric := %s;\n", cf->m_xmp_name.c_str(), (cf->m_xmp_numeric ? "true" : "false"));
    if (cf->m_xmp_numeric) {
      fprintf(f,"boolean %s_transform;\n", cf->m_xmp_name.c_str());
      fprintf(f,"%s_transform := %s;\n", cf->m_xmp_name.c_str(), (cf->m_xmp_transform ? "true" : "false"));
      fprintf(f,"numeric %s;\n", cf->m_xmp_name.c_str());
      fprintf(f,"%s := 4004;\n", cf->m_xmp_name.c_str());
      cf->m_xmp_last_numeric = 4004;
    } else {
      fprintf(f,"string %s;\n", cf->m_xmp_name.c_str());
      fprintf(f,"%s := \"\";\n", cf->m_xmp_name.c_str());
      cf->m_xmp_last_string = std::string("");
    }
  }
}


void thattr::export_mp_object(FILE * f, long user_id)
{
  if (this->m_num_fields == 0)
    return;
  thattr_obj * o = this->get_object(user_id);
  if (o == NULL)
    return;

  thattr_attr * ca;
  thattr_field * cf;
  thattr_id2attr_map::iterator ai;
  thattr_field_list::iterator fli;
  thbuffer enc;

  std::string news;
  double newd(0.0);

  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    ai = o->m_attributes.find(cf->m_id);
    if (ai == o->m_attributes.end()) {
      news = std::string("");
      newd = 4004.0;
    } else {
      ca = &(ai->second);
      switch (cf->m_type) {
        case THATTR_INTEGER:          
          newd = double(ca->m_val_long);
          break;
        case THATTR_DOUBLE:
          newd = ca->m_val_double;
          break;
        case THATTR_STRING:
          news = ca->m_val_string;
          break;
      }
      if (cf->m_xmp_transform) {
        newd = 8000.0 * (1.0 / (1.0 + exp(-newd/2000.0))) - 4000.0;
      }
    }
    if (cf->m_xmp_numeric) {
      if (cf->m_xmp_last_numeric != newd) {
        fprintf(f,"%s := %g;\n", cf->m_xmp_name.c_str(), newd);
        cf->m_xmp_last_numeric = newd;
      }
    } else {
      if (cf->m_xmp_last_string != news) {
        thdecode_mp(&enc, news.c_str());
        fprintf(f,"%s := \"%s\";\n", cf->m_xmp_name.c_str(), enc.get_buffer());
        cf->m_xmp_last_string = news;
      }
    }
  }
}




