/* -*- c++ -*- */
/*
 * Copyright 2004,2005,2012 Free Software Foundation, Inc.
 *
 * This file is part of GNU Radio
 *
 * GNU Radio is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3, or (at your option)
 * any later version.
 *
 * GNU Radio is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with GNU Radio; see the file COPYING.  If not, write to
 * the Free Software Foundation, Inc., 51 Franklin Street,
 * Boston, MA 02110-1301, USA.
 */

#ifndef INCLUDED_GR_SIMPLE_FRAMER_SYNC_H
#define INCLUDED_GR_SIMPLE_FRAMER_SYNC_H

/*!
 * \brief Here are a couple of maximum length sequences (m-sequences)
 * that were generated by the the "mseq" matlab/octave code downloaded
 * from: <a href="http://www.mathworks.com/matlabcentral/fileexchange/990">http://www.mathworks.com/matlabcentral/fileexchange/990</a>
 *
 * <pre>
 * 31-bit m-sequence:
 *	0110100100001010111011000111110
 *	0x690AEC76 (padded on right with a zero)
 *
 * 63-bit m-sequence:
 *	101011001101110110100100111000101111001010001100001000001111110
 *      0xACDDA4E2F28C20FC (padded on right with a zero)
 * </pre>
 */

static const unsigned long long GRSF_SYNC = 0xacdda4e2f28c20fcULL;

static const int GRSF_BITS_PER_BYTE = 8;
static const int GRSF_SYNC_OVERHEAD = sizeof(GRSF_SYNC);
static const int GRSF_PAYLOAD_OVERHEAD = 1;		  	// 1 byte seqno
static const int GRSF_TAIL_PAD = 1;				// one byte trailing padding
static const int GRSF_OVERHEAD = GRSF_SYNC_OVERHEAD + GRSF_PAYLOAD_OVERHEAD + GRSF_TAIL_PAD;


#endif /* INCLUDED_GR_SIMPLE_FRAMER_SYNC_H */
