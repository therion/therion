#!/usr/bin/perl

$myname = "cclass.pl";
$nargin = $#ARGV + 1;


die "$myname: missing class name\n" if ($nargin < 1);

if ($nargin < 2) {
  $father = "dataobject";
}
else {
  $father = $ARGV[1];
}

$class = $ARGV[0];
$thclass = "th$class";
$thfather = "th$father";
$CLASS = uc($class);
$FATHER = uc($father);

die "$myname: $thclass.h already exists\n" if (open(TST,"$thclass.h"));
die "$myname: $thclass.cxx already exists\n" if (open(TST,"$thclass.cxx"));
die "$myname: $thfather.h does not exist\n" unless (open(TST,"$thfather.h"));
$level = 0;
while (<TST>) {
  if (/TT\_$FATHER\_UNKNOWN\s*\=\s*(\d+)/) {
    $level = $1;
  }
}
die "$myname: uknown level\n" if ($level == 0);
close(TST);
$level += 1000;
$levelp = $level + 1;

print "class $thclass : public class $thfather at level $level\n";
print "writing $thclass.h ... ";
open(OUT,">$thclass.h");
print OUT <<ENDHEADER;
/**
 * \@file $thclass.h
 * $class module.
 */
  
/* Copyright (C) 2000 Stacho Mudrak
 * 
 * \$Date: \$
 * \$RCSfile: \$
 * \$Revision: \$
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
 
#ifndef $thclass\_h
#define $thclass\_h


#include \"$thfather.h\"

/**
 * $class command options tokens.
 */
 
enum {
  TT_$CLASS\_UNKNOWN = $level,
  TT_$CLASS\_OPTION = $levelp,
};


/**
 * $class command options parsing table.
 */
 
static const thstok thtt_$class\_opt[] = {
  {\"option\", TT_$CLASS\_OPTION},
  {NULL, TT_$CLASS\_UNKNOWN},
};


/**
 * $class class.
 */

class $thclass : public $thfather {

  protected:

  // insert here real properties
  int opt;  ///< $class option.

  public:

  /**
   * Standard constructor.
   */
  
  $thclass();
  
  
  /**
   * Standard destructor.
   */
   
  virtual ~$thclass();
  
  
  /**
   * Return class identifier.
   */
  
  virtual int get_class_id();
  
  
  /**
   * Return class name.
   */
   
  virtual char * get_class_name() {return \"$thclass\";};
  
  
  /**
   * Return true, if son of given class.
   */
  
  virtual bool is(int class_id);
  
  
  /**
   * Return number of command arguments.
   */
   
  virtual int get_cmd_nargs();
  
  
  /**
   * Return command name.
   */
   
  virtual char * get_cmd_name();
  
  
  /**
   * Return command end option.
   */
   
  virtual char * get_cmd_end();
  
  
  /**
   * Return option description.
   */
   
  virtual thcmd_option_desc get_cmd_option_desc(char * opts);
  
  
  /**
   * Set command option.
   *
   * \@param cod Command option description.
   * \@param args Option arguments arry.
   * \@param argenc Arguments encoding.
   */
   
  virtual void set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline);


  /**
   * Delete this object.
   *
   * \@warn Always use this methos instead of delete function.
   */
   
  virtual void self_delete();


  /**
   * Print object properties.
   */
   
  virtual void self_print_properties(FILE * outf); 
  

};


#endif


ENDHEADER
close(OUT);
print "done.\n";

print "writing $thclass.cxx ... ";
open(OUT,">$thclass.cxx");
print OUT <<ENDBODY;
/**
 * \@file $thclass.cxx
 */
  
/* Copyright (C) 2000 Stacho Mudrak
 * 
 * \$Date: \$
 * \$RCSfile: \$
 * \$Revision: \$
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
 
#include \"$thclass.h\"
#include "thexception.h"
#include "thchenc.h"

$thclass\::$thclass()
{
  // replace this by setting real properties initialization
  this->opt = 0;
}


$thclass\::~$thclass()
{
}


int $thclass\::get_class_id() 
{
  return TT_$CLASS\_CMD;
}


bool $thclass\::is(int class_id)
{
  if (class_id == TT_$CLASS\_CMD)
    return true;
  else
    return $thfather\::is(class_id);
}

int $thclass\::get_cmd_nargs() 
{
  // replace by real number of arguments
  return 0;
}


char * $thclass\::get_cmd_end()
{
  // insert endcommand if multiline command
  return NULL;
}


char * $thclass\::get_cmd_name()
{
  // insert command name here
  return \"$class\";
}


thcmd_option_desc $thclass\::get_cmd_option_desc(char * opts)
{
  int id = thmatch_token(opts, thtt_$class\_opt);
  if (id == TT_$CLASS\_UNKNOWN)
    return $thfather\::get_cmd_option_desc(opts);
  else
    return thcmd_option_desc(id);
}


void $thclass\::set(thcmd_option_desc cod, char ** args, int argenc, unsigned long indataline)
{
  switch (cod.id) {

    // replace this by real properties setting
    case TT_$CLASS\_OPTION:
      this->opt = sscanf("%d", *args);
      break;
    
    // if not found, try to set fathers properties  
    default:
      $thfather\::set(cod, args, argenc, indataline);
  }
}


void $thclass\::self_delete()
{
  delete this;
}

void $thclass\::self_print_properties(FILE * outf)
{
  $thfather\::self_print_properties(outf);
  fprintf(outf,"$thclass:\\n");
  // insert intended print of object properties here
}


ENDBODY
close(OUT);
print "done.\n";
