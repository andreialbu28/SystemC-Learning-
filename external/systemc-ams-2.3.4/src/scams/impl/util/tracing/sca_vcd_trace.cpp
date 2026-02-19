/*****************************************************************************

    Copyright 2010-2013
    Fraunhofer-Gesellschaft zur Foerderung der angewandten Forschung e.V.


   Licensed under the Apache License, Version 2.0 (the "License");
   you may not use this file except in compliance with the License.
   You may obtain a copy of the License at

       http://www.apache.org/licenses/LICENSE-2.0

   Unless required by applicable law or agreed to in writing, software
   distributed under the License is distributed on an "AS IS" BASIS,
   WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
   See the License for the specific language governing permissions and
   limitations under the License.

 *****************************************************************************/

/*****************************************************************************

 sca_vcd_trace.cpp - description

 Original Author: Karsten Einwich Fraunhofer IIS/EAS Dresden

 Created on: Jan 23, 2010

 SVN Version       :  $Revision: 2320 $
 SVN last checkin  :  $Date: 2023-06-08 12:47:38 +0000 (Thu, 08 Jun 2023) $
 SVN checkin by    :  $Author: pehrlich $
 SVN Id            :  $Id: sca_vcd_trace.cpp 2320 2023-06-08 12:47:38Z pehrlich $

 *****************************************************************************/

/*****************************************************************************/
#define SC_INCLUDE_FX

#include "systemc-ams"
#include "scams/impl/util/tracing/sca_vcd_trace.h"
#include <ctime>

namespace sca_util
{
namespace sca_implementation
{

static const std::string UNKNOWN_TYPE_TOKEN(" ");

//number of characters for max string length in VCD file
static const unsigned int NUM_VCD_STRING_CHAR=16;

void sca_vcd_trace::construct()
{
	warned_ac = false;
}

//////////////////////////////////////////////////////////////////////////////

sca_vcd_trace::sca_vcd_trace(const std::string& name)
{

	if(name.find('.')==std::string::npos)
	{
		create_trace_file(name+".vcd");
	}
	else
	{
		create_trace_file(name);
	}
	construct();
}


void sca_vcd_trace::reopen_impl(const std::string& name, std::ios_base::openmode m)
{
	std::string nm;

	if(name.find('.')==std::string::npos)
	{
		nm=name+".vcd";
	}
	else
	{
		nm=name;
	}

	sca_implementation::sca_trace_file_base::reopen(nm,m);

}

//////////////////////////////////////////////////////////////////////////////

sca_vcd_trace::sca_vcd_trace(std::ostream& str)
{
	create_trace_file(str);
	construct();
}

//////////////////////////////////////////////////////////////////////////////

std::string sca_vcd_trace::create_shortcut(unsigned long n)
{
	static const unsigned long first_ch = 33; //IEEE1364-2001(E) 18.2.1 p.356
	static const unsigned long last_ch = 126 + 1;
	static const unsigned long ch_diff = last_ch - first_ch;

	std::string str;
	unsigned long rem_ch = n;

	do
	{
		str += char(rem_ch % ch_diff + first_ch);
		rem_ch /= ch_diff;
	} while (rem_ch != 0);

	return str;
}

//////////////////////////////////////////////////////////////////////////////

bool sca_vcd_trace::write_type_def(sca_type_explorer_base& typeinfo)
{
	switch (typeinfo.type_id)
	{
	case sca_type_explorer_base::BOOL:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::INT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::LONG:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SHORT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::UINT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::ULONG:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::USHORT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::INT64:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::UINT64:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::FLOAT:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::DOUBLE:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::CHAR:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::UCHAR:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_INT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_INT_BASE:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_UINT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return false;
	case sca_type_explorer_base::SC_UINT_BASE:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return false;
	case sca_type_explorer_base::SC_LOGIC:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_BV:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return false;
	case sca_type_explorer_base::SC_BV_BASE:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return false;
	case sca_type_explorer_base::SC_LV:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return false;
	case sca_type_explorer_base::SC_LV_BASE:
		(*outstr) << "wire " << typeinfo.length << ' ';
		return false;
	case sca_type_explorer_base::SC_BIGINT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_SIGNED:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_BIGUINT:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_UNSIGNED:
		(*outstr) << "integer " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_FIXED:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_FIX:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_UFIXED:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_UFIX:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_FIXED_FAST:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_FIX_FAST:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_UFIXED_FAST:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::SC_UFIX_FAST:
		(*outstr) << "real " << typeinfo.length << ' ';
		return  false;
	case sca_type_explorer_base::STRING:
		//we limit the string length to 16 characters - vcd does not support
		//dynamic lengths
		(*outstr) << "integer " << 8*NUM_VCD_STRING_CHAR << ' ';
		return false;
	case sca_type_explorer_base::SCA_COMPLEX:
		//for complex we write the absolute value
		(*outstr) << "real " << 1 << ' ';
		return false;
	default:
		(*outstr) << "$comment  Unnkown type for the next variable $end" << std::endl;
		(*outstr) << "real " << 1 << ' ';
		return true;

	}

	return true;
}


std::string sca_vcd_trace::integer2string(sc_dt::uint64 val,unsigned long wl)
{
	std::string str;

	sc_dt::uint64 bit_mask = sc_dt::uint64(1) << (wl-1);
	unsigned long index=0;
	for (unsigned long bitindex = 0; bitindex < wl; bitindex++)
	{
		bool bit=(val & bit_mask);
		bit_mask = bit_mask >> 1;
		if(!bit && index==0) continue;  //remove leading '0'
		++index;
		str+= bit ? '1' : '0';
	}

	if(index==0) str+='0';

	return str;
}

//////////////////////////////////////////////////

void sca_vcd_trace::write_value(sca_trace_value_handler_base* value)
{
	const void* data;
	const sca_type_explorer_base& typeinfo=value->get_typed_value(data);

	switch (typeinfo.type_id)
	{
	case sca_type_explorer_base::BOOL:
		if(*(static_cast<const bool*>(data))) (*outstr) << '1';
		else (*outstr) << '0';
		return;
	case sca_type_explorer_base::INT:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const int*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::LONG:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const long*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::SHORT:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const short*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::UINT:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const unsigned int*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::ULONG:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const unsigned long*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::USHORT:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const unsigned short*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::INT64:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const sc_dt::int64*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::UINT64:
		(*outstr) << 'b' <<
		integer2string(*(static_cast<const sc_dt::uint64*>(data)),8*sizeof(sc_dt::uint64));
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::FLOAT:
		(*outstr) << 'r'  << std::setprecision(16) << *(static_cast<const float*>(data));
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::DOUBLE:
		(*outstr) << 'r' << std::setprecision(16) << *(static_cast<const double*>(data));
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::CHAR:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const char*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::UCHAR:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64(*(static_cast<const unsigned char*>(data))),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::SC_INT:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64((static_cast<const sc_dt::sc_int_base*>(data))->value()),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::SC_INT_BASE:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64((static_cast<const sc_dt::sc_int_base*>(data))->value()),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::SC_UINT:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64((static_cast<const sc_dt::sc_uint_base*>(data))->value()),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::SC_UINT_BASE:
		(*outstr) << 'b' <<
		integer2string(sc_dt::uint64((static_cast<const sc_dt::sc_uint_base*>(data))->value()),
				typeinfo.length);
		(*outstr) << ' ';
		return;
	case sca_type_explorer_base::SC_LOGIC:
		(*outstr) << (*(static_cast<const sc_dt::sc_logic*>(data)));
		return;
	case sca_type_explorer_base::SC_BV:
	case sca_type_explorer_base::SC_BV_BASE:
	{
		(*outstr) << 'b';
		std::string str=static_cast<const sc_dt::sc_bv_base*>(data)->to_string(sc_dt::SC_BIN_US,false);
		std::string::size_type idx=str.find('1');
		if(idx==std::string::npos) //remove leading 0
		{
			(*outstr) << '0';
		}
		else
		{
			(*outstr) << str.substr(idx);
		}
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SC_LV:
	case sca_type_explorer_base::SC_LV_BASE:
	{
		// - Remove multiple leading 0,z,x, and replace by only one
		// - For example,
		//    b000z100    -> b0z100
		//    b00000xxx   -> b0xxx
		//    b000        -> b0
		//    bzzzzz1     -> bz1
		//    bxxxz10     -> bxz10
		// - For leading 0's followed by 1, remove all leading 0's
		//    b0000010101 -> b10101
		(*outstr) << 'b';
		std::string str=static_cast<const sc_dt::sc_lv_base*>(data)->to_string(sc_dt::SC_BIN_US,false);
		char first=str[0];
		if(first=='1')
		{
			(*outstr) << str;
		}
		else
		{
			std::string::size_type idx=str.find_first_not_of(first);
			if(idx==std::string::npos) //remove leading 0
			{
				(*outstr) << first; //all bits the same
			}
			else
			{
				if(first=='0')(*outstr) << str.substr(idx);   //remove all leading 0
				else          (*outstr) << str.substr(idx-1); //leave one remaining z or x
			}
		}
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SC_BIGINT:
	case sca_type_explorer_base::SC_SIGNED:
	{
		(*outstr) << 'b';
		const sc_dt::sc_signed* val=static_cast<const sc_dt::sc_signed*>(data);
		bool leading_zero=true;
		for(long i=typeinfo.length-1;i>0;i--)
		{
			if(leading_zero && !(*val)[i]) continue;

			leading_zero=false;
			(*outstr) << (unsigned int)((*val)[i]);
		}
		(*outstr) << (unsigned int)((*val)[0]);
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SC_BIGUINT:
	case sca_type_explorer_base::SC_UNSIGNED:
	{
		(*outstr) << 'b';
		const sc_dt::sc_unsigned* val=static_cast<const sc_dt::sc_unsigned*>(data);
		bool leading_zero=true;
		for(long i=typeinfo.length-1;i>0;i--)
		{
			if(leading_zero && !(*val)[i]) continue;

			leading_zero=false;
			(*outstr) << (unsigned int)((*val)[i]);
		}
		(*outstr) << (unsigned int)((*val)[0]);
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SC_FIXED:
	case sca_type_explorer_base::SC_FIX:
	{
		(*outstr) << 'r';
		(*outstr) << static_cast<const sc_dt::sc_fix*>(data)->to_string(sc_dt::SC_DEC);
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SC_UFIXED:
	case sca_type_explorer_base::SC_UFIX:
	{
		(*outstr) << 'r';
		(*outstr) << static_cast<const sc_dt::sc_ufix*>(data)->to_string(sc_dt::SC_DEC);
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SC_FIXED_FAST:
	case sca_type_explorer_base::SC_FIX_FAST:
	{
		(*outstr) << 'r';
		(*outstr) << static_cast<const sc_dt::sc_fix_fast*>(data)->to_string(sc_dt::SC_DEC);
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SC_UFIXED_FAST:
	case sca_type_explorer_base::SC_UFIX_FAST:
	{
		(*outstr) << 'r';
		(*outstr) << static_cast<const sc_dt::sc_ufix_fast*>(data)->to_string(sc_dt::SC_DEC);
		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::STRING:
	{
		(*outstr) << 'b';
		std::string str=*static_cast<const std::string*>(data);

		bool leading_zero=true;
		for(unsigned int i=0;i<str.size();i++)
		{
			if(i>=NUM_VCD_STRING_CHAR) break; //we limit to 16 characters
			char ch=str[i];

			for(int j=7;j>=0;j--)
			{
				bool bit=((unsigned char)ch)&(1<<j);

				if(leading_zero && !bit) continue;

				leading_zero=false;
				(*outstr) << (unsigned int)(bit);
			}
		}


		(*outstr) << ' ';
		return;
	}
	case sca_type_explorer_base::SCA_COMPLEX:
		//we write tha absolute value
		(*outstr) << 'r' << std::setprecision(16) << std::abs(*(static_cast<const sca_util::sca_complex*>(data)));
		(*outstr) << ' ';
		return;
    default:
	{
		return;
	}



	}
}

//////////////////////////////////////////////////////////////////////////

void sca_vcd_trace::write_header()
{
	std::ostream& str(*outstr);

	str.precision(16); //IEEE1364-2001(E) 18.2.1 p.355
	str << std::showpoint;

	str << "$date" << std::endl;

	std::time_t raw_time;
	std::tm* timeinfo;

	std::time(&raw_time);
	timeinfo = std::localtime(&raw_time);
	str << "    " << std::asctime(timeinfo); //asctime adds a /n

	str << "$end" << std::endl<< std::endl;

	str << "$version" << std::endl;
	str << "    " << sca_core::sca_version() << std::endl;
	str << "$end" << std::endl<< std::endl;

	str << "$timescale" << std::endl;
	str << "      " << sc_core::sc_get_time_resolution() << std::endl;
	str << "$end" << std::endl<< std::endl;

	str << "$scope module SystemC_AMS $end" << std::endl<< std::endl;

	for (std::vector<sca_trace_object_data>::iterator it = traces.begin(); it
			!= traces.end(); it++)
	{
		if(it->get_type_info().type_id != sca_type_explorer_base::UNKNOWN)
		{
			name_map.push_back(create_shortcut((unsigned long)(name_map.size())));
			str << "$var ";
			if(write_type_def(it->get_type_info()))
			{
				std::ostringstream str;
				str << "Not yet implemented data type for VCD Tracing of signal: "
			        << (*it).name;
				SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

				name_map[name_map.size() - 1]=UNKNOWN_TYPE_TOKEN; //token for unknown signal
			}

			str << name_map[name_map.size() - 1];
			str << "  " << (*it).name << " $end" << std::endl;
		}
		else
		{
			name_map.push_back(UNKNOWN_TYPE_TOKEN); //token for unknown signal
		}
	}

	str << std::endl << "$upscope $end" << std::endl << std::endl;
	str << "$enddefinitions $end" << std::endl << std::endl << std::endl;

}

//////////////////////////////////////////////////////////////////////////////

void sca_vcd_trace::write_waves()
{
	bool ready_flag = false;
	sca_core::sca_time ctime;
	sca_trace_buffer::value_buffer* values;

	while (!ready_flag)
	{
		ready_flag = buffer->read_next_stamp(ctime, values);

		//no point available
		if (values == NULL)
			break;

		(*outstr) << '#';
		(*outstr) << ctime.value() << std::endl;

		for (int i = 0; i < values->size(); i++)
		{
			if(name_map[i]!=UNKNOWN_TYPE_TOKEN)
			{
				if ((*values)[i] != NULL)
				{
					write_value((*values)[i]);
					(*outstr) << name_map[i];
					(*outstr) << std::endl;
				}
			}

		}
		(*outstr) << std::endl;
	}
}

void sca_vcd_trace::write_ac_domain_init()
{
	if (!warned_ac)
	{
		std::ostringstream str;
		str << "AC-Tracing not supported for vcd file format";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

		warned_ac = true;
	}
}

void sca_vcd_trace::write_ac_noise_domain_init(
		sca_util::sca_vector<std::string>& src_name)
{
	if (!warned_ac)
	{
		std::ostringstream str;
		str << "AC-Tracing not supported for vcd file format";
		SC_REPORT_WARNING("SystemC-AMS",str.str().c_str());

		warned_ac = true;
	}
}

void sca_vcd_trace::write_ac_domain_stamp(double w, std::vector<
		sca_util::sca_complex>& tr_vec)
{
	//do nothing
}

void sca_vcd_trace::write_ac_noise_domain_stamp(double w,
		sca_util::sca_matrix<sca_util::sca_complex>& tr_matrix)
{
	//do nothing
}


void sca_vcd_trace::close()
{
	close_trace();

	if(outstr)
	{
		(*outstr) << std::endl << "#"
				  << sc_core::sc_time_stamp().value()
				  << std::endl;
	}

	close_file();
}

} // namespace sca_implementation
} // namespace sca_util

