/*
* This file is part of the BSGS distribution (https://github.com/JeanLucPons/Kangaroo).
* Copyright (c) 2020 Jean Luc PONS.
*
* This program is free software: you can redistribute it and/or modify
* it under the terms of the GNU General Public License as published by
* the Free Software Foundation, version 3.
*
* This program is distributed in the hope that it will be useful, but
* WITHOUT ANY WARRANTY; without even the implied warranty of
* MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
* General Public License for more details.
*
* You should have received a copy of the GNU General Public License
* along with this program. If not, see <http://www.gnu.org/licenses/>.
*/

#ifndef CONSTANTSH
#define CONSTANTSH

// Release number
#define RELEASE "2.2"

// Use symmetry
//#define USE_SYMMETRY

// Travelled-distance field width.
//
// Default (off): the original layout -- 128 bits per distance, of which
// b127=sign, b126=kangaroo type and b125..b0 the magnitude. That caps the
// search interval at 125 bits, and exceeding it used to corrupt keys
// silently; it now aborts.
//
// Uncomment, or build with -DWIDE_DIST, for a 256-bit field (254-bit
// magnitude, intervals up to 253 bits). Needed for puzzle 130 and above.
// Costs: DP tables +50% RAM, device kangaroo memory +20%, DP packets +40%,
// kangaroo transfers +100%, and roughly 4-7% throughput. Work files and the
// client/server protocol are NOT interchangeable between the two builds --
// the format magics differ so a mismatch is refused, not misparsed.
//#define WIDE_DIST

#ifdef WIDE_DIST
#define DIST_WORDS 4
#else
#define DIST_WORDS 2
#endif

// Number of random jumps
// Max 512 for the GPU
#define NB_JUMP 32

// GPU group size
#define GPU_GRP_SIZE 128

// GPU number of run per kernel call
#define NB_RUN 64

// Kangaroo type
#define TAME 0  // Tame kangaroo
#define WILD 1  // Wild kangaroo

// SendDP Period in sec
#define SEND_PERIOD 2.0

// Timeout before closing connection idle client in sec
#define CLIENT_TIMEOUT 3600.0

// Number of merge partition
#define MERGE_PART 256

#endif //CONSTANTSH
