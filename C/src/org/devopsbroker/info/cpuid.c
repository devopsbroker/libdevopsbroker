/*
 * cpuid.c - DevOpsBroker C source file for the org.devopsbroker.info.CPUID struct
 *
 * Copyright (C) 2019 Edward Smith <edwardsmith@devopsbroker.org>
 *
 * This program is free software: you can redistribute it and/or modify it under
 * the terms of the GNU General Public License as published by the Free Software
 * Foundation, either version 3 of the License, or (at your option) any later
 * version.
 *
 * This program is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU General Public License for more
 * details.
 *
 * You should have received a copy of the GNU General Public License along with
 * this program.  If not, see <http://www.gnu.org/licenses/>.
 * -----------------------------------------------------------------------------
 * Developed on Ubuntu 18.04.2 LTS running kernel.osrelease = 4.18.0-15
 *
 * -----------------------------------------------------------------------------
 */

// ════════════════════════════ Feature Test Macros ═══════════════════════════

#define _DEFAULT_SOURCE

// ═════════════════════════════════ Includes ═════════════════════════════════

#include "cpuid.h"

#include "../io/shell.h"

// ═══════════════════════════════ Preprocessor ═══════════════════════════════


// ═════════════════════════════════ Typedefs ═════════════════════════════════


// ═════════════════════════════ Global Variables ═════════════════════════════


// ════════════════════════════ Function Prototypes ═══════════════════════════


// ═════════════════════════ Function Implementations ═════════════════════════

void f618482d_getCoreTopology(CPUID *cpuid) {
	int numThreadsPerCore;

	Shell lscpu;
	f6843e7e_openShellForRead(&lscpu, "/usr/bin/lscpu | /usr/bin/awk '/(Thread... per core:|Core... per socket:)/{ print $4 }'");

	numThreadsPerCore = f6843e7e_readInt(&lscpu);
	cpuid->numPhysicalCores = f6843e7e_readInt(&lscpu);

	f6843e7e_closeShell(&lscpu);

	cpuid->numLogicalProcs = numThreadsPerCore * cpuid->numPhysicalCores;
}
