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
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301 USA
 * --------------------------------------------------------------------
 */

 
#include "thattr.h"
#include "shapefil.h"
#include "therion.h"
#include "thchenc.h"
#include <cctype>
#include <cmath>
#include <cstring>
#include <set>

thattr::thattr()
{
  this->m_num_fields = 0;
  this->m_num_objects = 0;
  this->m_obj_last = NULL;
}


bool thattr_attr::has_flag(unsigned long flag)
{
  return ((this->m_flags & flag) != 0);
}

void thattr_attr::set_flag(unsigned long flag, bool value)
{
  if (value)
    this->m_flags |= flag;
  else
    this->m_flags &= (~flag);
}


void thattr_obj::set_tree_level(size_t level) {
  m_tree_level = level;
}



thattr_field * thattr::insert_field(const char * name)
{
  thattr_field tmp = {}, * r;
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



thattr_obj * thattr::get_object() {
    return this->m_obj_last;
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


thattr_obj * thattr::insert_object(thdataobject * data, long user_id)
{
  thattr_obj tmp, * r;
  r = NULL;
  if (user_id >= 0) {
    r = this->get_object(user_id);
  }
  if (r == NULL) {
    tmp.m_id = this->m_num_objects++;
    tmp.m_data = data;
    tmp.m_parent = this;
    r = &(*this->m_obj_list.insert(this->m_obj_list.end(), tmp));
    this->m_obj_map[tmp.m_id] = r;
    if (user_id > -1) {
      r->m_user_id = user_id;
      this->m_obj_usrmap[user_id] = r;
    }
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
    o->m_last_attribute = &(o->m_attributes[f->m_id]);
  }

}



thattr_attr * thattr::get_attribute()
{
  thattr_obj * o = this->m_obj_last;
  if (o == NULL) return NULL;
  thattr_attr * a = o->m_last_attribute;
  return a;
}



void thattr::insert_attribute(const char * name, const char * value, long user_id)
{

  int sv(0);
  long lv(0);
  double d(0.0);
  thattr_attr a;
  a.m_type = THATTR_STRING;

  // cast value type
  if ((value != NULL) && (strlen(value) > 0)) {
    thparse_double(sv, d, value);
    if (sv == TT_SV_NUMBER) {
      a.m_type = THATTR_DOUBLE;
      lv = long(d);
      if (double(lv) == d) {
        a.m_type = THATTR_INTEGER;
      }
    }
  }

  // insert attribute
  switch (a.m_type) {
    case THATTR_INTEGER:
      a.m_val_long = lv;
      break;
    case THATTR_DOUBLE:
      a.m_val_double = d;
      break;
    case THATTR_STRING:
      if (value != NULL)
        a.m_val_string = std::string(value);
      else
        a.m_val_string = std::string("");
      break;
  }

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
  if (object == NULL) return;
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
  std::string nn, x;
  nn = name;
  int i;
  if (nn.length() > 11) {
    i = 0;
    do {
      x = std::to_string(i);
      nn = name.substr(0, 10 - x.length());
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
    if (cf->m_double_format.empty())
      cf->m_double_format = "%.1f";
    cf->m_xmp_name = "ATTR_";
    cf->m_xmp_name += cf->m_name;
    for(i = 5; i < cf->m_xmp_name.size(); ++i) {
      if (cf->m_xmp_name[i] <= '9')
        cf->m_xmp_name[i] = cf->m_xmp_name[i] + 8;
      else if ((cf->m_xmp_name[i] < 'a'))
        cf->m_xmp_name[i] = tolower(cf->m_xmp_name[i]);
    }
    cf->m_maxs = 0;
  }

  // check record types and limits
  long cvall;
  double cvald;
  size_t cslen;
  int ctype;
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
          ca->m_val_string = std::to_string(cvall);
          break;
        case THATTR_DOUBLE:
          cvald = ai->second.m_val_double;
          ca->m_val_long = long(cvald);
          ca->m_val_string = std::to_string(cvald);
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
    switch (cf->m_type) {
      case THATTR_INTEGER:
        cf->m_xdbf_decimals = 0;
        cf->m_xdbf_width = (int) log10(double(myabs(cf->m_maxi)));
        w2 = (int) log10(double(myabs(cf->m_mini)));
        if (cf->m_xdbf_width < w2) cf->m_xdbf_width = w2;
        ++cf->m_xdbf_width;
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
    thwarning(fmt::format("unable to open file for output -- {}", fname));
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
      thwarning(fmt::format("error writing to {}", fname))
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


void thattr::export_mp_header(FILE * /*f*/) // TODO unused parameter f
{
  if (this->m_num_fields == 0)
    return;
  this->analyze_fields();
}


void thattr::export_mp_object_begin(FILE * f, long user_id)
{
  if (this->m_num_fields == 0)
    return;
  thattr_obj * o = this->get_object(user_id);
  if (o == NULL)
    return;
  if (f == NULL) return;

  thattr_attr * ca;
  thattr_field * cf;
  thattr_id2attr_map::iterator ai;
  thattr_field_list::iterator fli;
  thbuffer enc;

  std::string news;

  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    ai = o->m_attributes.find(cf->m_id);
    if (ai != o->m_attributes.end()) {
      ca = &(ai->second);
      news = ca->m_val_string;
      thdecode_mp(&enc, news.c_str());
      fprintf(f,"string %s;\n", cf->m_xmp_name.c_str());
      fprintf(f,"%s := \"%s\";\n", cf->m_xmp_name.c_str(), enc.get_buffer());
    }
  }
}


void thattr::export_mp_object_end(FILE * f, long user_id)
{
  if (this->m_num_fields == 0)
    return;
  thattr_obj * o = this->get_object(user_id);
  if (o == NULL)
    return;
  if (f == NULL) return;

  thattr_field * cf;
  thattr_id2attr_map::iterator ai;
  thattr_field_list::iterator fli;

  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    ai = o->m_attributes.find(cf->m_id);
    if (ai != o->m_attributes.end()) {
      fprintf(f,"save %s;\n", cf->m_xmp_name.c_str());
    }
  }
}




void thattr::export_txt(const char * fname, int /*encoding*/) // TODO unused parameter encoding
{
  // Create file.
  FILE * f;
  unsigned i;
  thattr_attr * ca;
  thattr_obj_list::iterator oi;
  thattr_id2attr_map::iterator ai;
  thattr_field_list::iterator fli;
  thbuffer enc;

  this->analyze_fields();

  f = fopen(fname, "wb");
  if (f == NULL) {
    thwarning(fmt::format("unable to open file for output -- {}", fname));
    return;
  }

  // Create fields.
  bool hasone = false;
  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    fprintf(f,"%s%s",hasone ? "\t" : "",fli->m_name.c_str());
    hasone = true;
  }
  fprintf(f,"\n");

  // Insert objects and write fields.
  const char * value;
  for(oi = this->m_obj_list.begin(); oi != this->m_obj_list.end(); ++oi) {
    hasone = false;    
    for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
      ai = oi->m_attributes.find(fli->m_id);
      if (ai == oi->m_attributes.end()) {
        value = "";
      } else {
        ca = &(ai->second);
        value = ca->m_val_string.c_str();
      }
      if (m_tree && (!hasone)) {
        for(i = 0; i <  oi->m_tree_level; i++) fprintf(f,"  ");
      }
      fprintf(f,"%s%s",hasone ? "\t" : "",value);
      hasone = true;
    }
    fprintf(f,"\n");
  }

  fclose(f);

}



void thattr::export_kml(const char * fname, const char * name_field, const char * title)
{
  // Create file.
  FILE * f;
  thattr_obj_list::iterator oi, oinext;
  thattr_id2attr_map::iterator ai;
  thattr_field_list::iterator fli;
  thbuffer enc;

  this->analyze_fields();
  thattr_field * lat = get_field("Latitude", false), 
      * lon = get_field("Longitude", false),
      * alt = get_field("Altitude", false),
      * namef = get_field(name_field, false);
  double dlon, dlat, dalt;
  
  if ((lat == NULL) || (lon == NULL)) {
    thwarning("geographical reference is not associated with table");
    return;
  }
  
  f = fopen(fname, "wb");
  if (f == NULL) {
    thwarning(fmt::format("unable to open file for output -- {}", fname));
    return;
  }

  fprintf(f,"<?xml version=\"1.0\" encoding=\"UTF-8\"?>\n<kml xmlns=\"http://earth.google.com/kml/2.0\">\n<Document>\n");
  fprintf(f, "<name><![CDATA[%s]]></name>\n", title);

  
  // Insert objects and write fields.
  int clevel, nlevel;
  clevel = 0;
  for(oi = this->m_obj_list.begin(); oi != this->m_obj_list.end();) {
    oinext = oi;
    oinext ++;
    if ((oinext != this->m_obj_list.end()) && (oinext->m_tree_level > oi->m_tree_level)) {
        ai = oi->m_attributes.find(namef->m_id);
        fprintf(f,"<Folder>\n<name><![CDATA[%s]]></name>\n",ai->second.m_val_string.c_str());
        clevel++;
    } else {      
      ai = oi->m_attributes.find(lat->m_id);
      dlat = ai->second.m_val_double;
      ai = oi->m_attributes.find(lon->m_id);
      dlon = ai->second.m_val_double;
      ai = oi->m_attributes.find(alt->m_id);
      dalt = ai->second.m_val_double;
      ai = oi->m_attributes.end();
      if (namef != NULL)
        ai = oi->m_attributes.find(namef->m_id);
      fprintf(f,"<Placemark>\n<name><![CDATA[%s]]></name>\n<Point>\n<coordinates>%.14f,%.14f,%.14f</coordinates>\n</Point>\n</Placemark>\n",
				((namef != NULL) && (ai != oi->m_attributes.end())) ? ai->second.m_val_string.c_str() : "", dlon, dlat, dalt);
    }

    //for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    //  cf = &(*fli);
    //  ai = oi->m_attributes.find(cf->m_id);
    //  if (ai == oi->m_attributes.end()) {
    //    value = "";
    //  } else {
    //    ca = &(ai->second);
    //    value = ca->m_val_string.c_str();
    //  }
    //  fprintf(f,"%s%s",hasone ? "\t" : "",value);
    //}
    oi++;
    nlevel = 0;
    if (oi != this->m_obj_list.end()) nlevel = oi->m_tree_level;
    while (nlevel < clevel) {
        fprintf(f,"</Folder>\n");
        clevel--;
    }
  }
  
  fprintf(f,"</Document>\n</kml>\n");  
  fclose(f);
  
}



void thattr::export_html(const char * fname, const char * title, int /*encoding*/) // TODO unused parameter encoding
{
  // Create file.
  FILE * f;
  thattr_attr * ca;
  thattr_field * cf;
  thattr_obj_list::iterator oi, oinext;
  thattr_id2attr_map::iterator ai;
  thattr_field_list::iterator fli;
  thbuffer enc;
  const char * alstr;

  this->analyze_fields();

  f = fopen(fname, "wb");
  if (f == NULL) {
    thwarning(fmt::format("unable to open file for output -- {}", fname));
    return;
  }

  // Create fields.
  fprintf(f,"<html>\n<head>\n<title>%s</title>\n", title);
  fprintf(f, "<meta http-equiv=\"Content-Type\" content=\"text/html; charset=utf-8\">\n"
    "<script language=\"javascript\">\n"
    "function xTree(node)\n"
    "{\n"
    "  var fsub = true;\n"
    "  var show = false;\n"
    "  var last = node.length;\n"
    "  var all = document.getElementsByTagName(\"tr\");\n"
    "  for(i = 1; i < all.length; i++) {\n"
    "    if ((all[i].id.length > node.length) && (all[i].id.substr(0, node.length) == node)) {\n"
    "      if (fsub) {\n"
    "        fsub = false;\n"
    "        show = (all[i].style.display == \"none\");\n"
    "        last = all[i].id.lastIndexOf(\".\");\n"
    "      }\n"
    "      if (show) {\n"
    "        if (all[i].id.lastIndexOf(\".\") == last)\n"
    "        all[i].style.display = \"\";\n"
    "      } else {\n"
    "        all[i].style.display = \"none\";\n"
    "      }\n"
    "    }\n"
    "  }\n"
    "}\n"
    "</script>\n"
    "<style type=\"text/css\">\n"
    "body {font-family: Verdana, Helvetica, Arial, sans-serif; line-height: 1.5;}\n"
    "table {font-size:80%%; border-spacing: 0px;}\n"
    "th {border-bottom:2px solid #a7a9aa; padding-left:8px; padding-right:8px; padding-top:0px; padding-bottom:1px;}\n"
    "td {border-bottom:1px solid #a7a9aa; padding-left:8px; padding-right:8px; padding-top:0px; padding-bottom:0px;}\n"
    "</style>\n"
    "</head>\n<body>\n");
  fprintf(f,"<table>\n<tr>");
  for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
    cf = &(*fli);
    if ((cf->m_type ==  THATTR_DOUBLE) || (cf->m_type ==  THATTR_INTEGER)) {
      alstr = "right";
    } else {
      alstr = "left";
    }
    fprintf(f,"<th align=\"%s\">%s</th>", alstr, fli->m_name.c_str());
  }
  fprintf(f,"</tr>\n");

  // Insert objects and write fields.
  const char * value;
  bool header_value;
  thbuffer valb;
  valb.guarantee(128);
  std::string value_plus;
  for(oi = this->m_obj_list.begin(); oi != this->m_obj_list.end(); ++oi) {
    fprintf(f,"<tr id=\"%s\">", oi->m_tree_node_id);
    header_value = true;
    for(fli = this->m_field_list.begin(); fli != this->m_field_list.end(); ++fli) {
      cf = &(*fli);
      if ((cf->m_type ==  THATTR_DOUBLE) || (cf->m_type ==  THATTR_INTEGER)) {
        alstr = "right";
      } else {
        alstr = "left";
      }
      ai = oi->m_attributes.find(cf->m_id);
      ca = NULL;
      if (ai == oi->m_attributes.end()) {
        value = "&nbsp;";
      } else {
        ca = &(ai->second);
        if (ca->m_type == THATTR_DOUBLE) {
          std::snprintf(valb.get_buffer(), 127, cf->m_double_format.c_str(), ca->m_val_double);
          value = valb.get_buffer();
        } else
          value = ca->m_val_string.c_str();
      }
      fprintf(f,"<td align=\"%s\"", alstr);
      if (m_tree && header_value) {
        fprintf(f," style=\"padding-left: %upx\"", 12 * (unsigned)oi->m_tree_level);
        oinext = oi;
        oinext++;
        if ((oinext != this->m_obj_list.end()) && (oinext->m_tree_level > oi->m_tree_level)) {
          value_plus = "<a href=\"javascript:xTree('";
          value_plus += oi->m_tree_node_id;
          value_plus += "')\">";
          value_plus += value;
          value_plus += "</a>";
          value = value_plus.c_str();
        }
      }
      if ((ca != NULL) && (ca->has_flag(THATTRFLAG_HIDDEN)))
        fprintf(f," style=\"color:#808080\"");
      fprintf(f,">%s</td>", value);
      header_value = false;
    }
    fprintf(f,"</tr>\n");
  }
  fprintf(f,"</table>\n");
  fprintf(f,"</body>\n</html>\n");

  fclose(f);
}





