/**
 * @file thexport.cxx
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
 * Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA
 * --------------------------------------------------------------------
 */
 
#include "thexport.h"
#include "thexception.h"
#include "thconfig.h"
#include "thdatabase.h"
#include "thexporter.h"
#include "thcs.h"
#include <stdio.h>
#include <algorithm>
#include <array>
#include <cstdint>
#include <numeric>
#include <fstream>
#include <streambuf>
#include <iomanip>
#include <fmt/ostream.h>



thexport::thexport() {
  this->layout = new thlayout;
  this->layout->assigndb(&thdb);
  this->layout->id = ++thdb.objid;
  this->outpt = "";
  this->outpt_def = false;
  this->export_mode = 0;
  this->cs = TTCS_LOCAL;
}

thexport::~thexport() {
  delete this->layout;
}

void thexport::assign_config(class thconfig * cptr) 
{
  this->cfgptr = cptr;
  this->cfgpath = cptr->cfg_file.get_cif_path();
}


void thexport::parse(int nargs, char ** args)
{
  int ax = 0, oax;
  this->parse_arguments(ax, nargs, args);
  while (ax < nargs) {
    oax = ax;
    this->parse_options(ax, nargs, args);
    if (oax == ax)
      ththrow("unknown option -- \"{}\"", args[ax]);
  }
}


void thexport::parse_arguments(int & /*argx*/, int /*nargs*/, char ** /*args*/)
{
}

  
void thexport::parse_options(int & argx, int nargs, char ** args)
{
  int optid = thmatch_token(args[argx], thtt_exp_opt);
  int optx = argx;
  switch (optid) {
    case TT_EXP_OPT_OUTPUT:  
      argx++;
      if (argx >= nargs)
        ththrow("missing output file name -- \"{}\"",args[optx]);
      if (strlen(args[argx]) > 0) {
        this->outpt = this->cfgptr->get_db()->strstore(args[argx]);
        outpt_def = true;
      }
      else
        ththrow("empty file name not allowed -- \"{}\"",args[optx]);
      argx++;
      break;
    case TT_EXP_OPT_CS:  
      argx++;
      if (argx >= nargs)
        ththrow("missing coordiate system -- \"{}\"",args[optx]);
      {
        int id = thcs_parse(args[argx]);
        if (id == TTCS_UNKNOWN) {
          ththrow("unknown coordinate system -- {}", args[argx]);
        }
        if ((thcfg.outcs != id) && (id == TTCS_LOCAL))
          ththrow("mixing local and global coordinate system -- {}", args[argx]);
        this->cs = id;
      }
      argx++;
      break;
    default:
      break;
  }
}


void thexport::dump(FILE * xf)
{
  this->dump_header(xf);
  this->dump_body(xf);
  fprintf(xf,"\n");
}


void thexport::dump_header(FILE * xf)
{
  fprintf(xf,"export %s", thmatch_string(this->export_mode, thtt_exporter));
}


void thexport::dump_body(FILE * xf)
{
  if (this->outpt_def) {
    thdecode_arg(&(this->cfgptr->bf1), this->outpt);
    fprintf(xf," -output %s",this->cfgptr->bf1.get_buffer());
  }  
}


const char * thexport::get_output(const char * defname)
{
  static thbuffer outptfname;
  outptfname = this->cfgpath.get_buffer();
  if (this->outpt_def) {
    if (thpath_is_absolute(this->outpt))
      return this->outpt;
    else
      outptfname += this->outpt;
  } else {
    outptfname += defname;
  }
  return outptfname.get_buffer();
}

// Generates a lookup table for the checksums of all 8-bit values.
std::array<std::uint_fast32_t, 256> generate_crc_lookup_table() noexcept
{
  auto const reversed_polynomial = std::uint_fast32_t{0xEDB88320uL};

  // This is a function object that calculates the checksum for a value,
  // then increments the value, starting from zero.
  struct byte_checksum
  {
    std::uint_fast32_t operator()() noexcept
    {
      auto checksum = static_cast<std::uint_fast32_t>(n++);

      for (auto i = 0; i < 8; ++i)
        checksum = (checksum >> 1) ^ ((checksum & 0x1u) ? reversed_polynomial : 0);

      return checksum;
    }

    unsigned n = 0;
  };

  auto table = std::array<std::uint_fast32_t, 256>{};
  std::generate(table.begin(), table.end(), byte_checksum{});

  return table;
}


// Calculates the CRC for any sequence of values. (You could use type traits and a
// static assert to ensure the values can be converted to 8 bits.)
template <typename InputIterator>
std::uint_fast32_t crc(InputIterator first, InputIterator last)
{
  // Generate lookup table only on first use then cache it - this is thread-safe.
  static auto const table = generate_crc_lookup_table();

  // Calculate the checksum - make sure to clip to 32 bits, for systems that don't
  // have a true (fast) 32-bit type.
  return std::uint_fast32_t{0xFFFFFFFFuL} &
    ~std::accumulate(first, last,
      ~std::uint_fast32_t{0} & std::uint_fast32_t{0xFFFFFFFFuL},
        [](std::uint_fast32_t checksum, std::uint_fast8_t value)
          { return table[(checksum ^ value) & 0xFFu] ^ (checksum >> 8); });
}


bool thexport::check_crc() {
	bool ok = true;
	for(auto& file : output_files) {
		std::ifstream of(file.fnm, std::ios_base::binary);
    if (!of.is_open()) {
			file.res = "output file does not exist";
			ok = false;
      continue;
		}
		const auto actual_crc = crc(std::istreambuf_iterator<char>(of), {});

		if (thcfg.crc_generate) {
			std::ofstream crcof(fmt::format("{}.crc", file.fnm));
      fmt::print(crcof, "{:08x}\n", actual_crc);
			file.res = "OK";
      continue;
    }

		if (thcfg.crc_verify) {
			std::uint_fast32_t read_crc = 0;
      std::ifstream crcif(fmt::format("{}.crc", file.fnm));
      if (!crcif.is_open()) {
        file.res = ".crc file not found -- use --generate-output-crc before";
        ok = false;
        continue;
      }

      crcif >> std::hex >> read_crc;
			if (actual_crc == read_crc) {
				file.res = "OK";
			} else {
				file.res = fmt::format("CRC32 error: was {:08x}, expected {:08x}", actual_crc, read_crc);
				ok = false;
			}
		}
	}
	return ok;
}


void thexport::register_output(std::string fnm) {
	this->output_files.push_back(thexport_output_crc(fnm));
}

