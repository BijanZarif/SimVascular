# Copyright (c) 2014-2015 The Regents of the University of California.
# All Rights Reserved.
#
# Permission is hereby granted, free of charge, to any person obtaining
# a copy of this software and associated documentation files (the
# "Software"), to deal in the Software without restriction, including
# without limitation the rights to use, copy, modify, merge, publish,
# distribute, sublicense, and/or sell copies of the Software, and to
# permit persons to whom the Software is furnished to do so, subject
# to the following conditions:
#
# The above copyright notice and this permission notice shall be included
# in all copies or substantial portions of the Software.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS
# IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED
# TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A
# PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER
# OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
# EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
# PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
# PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
# LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
# NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
# SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

if(NOT TEMP_DIR)
set(TEMP_DIR ${SV_BINARY_DIR}/tmp)
file(MAKE_DIRECTORY ${TEMP_DIR})
endif()
mark_as_superbuild(TEMP_DIR)

get_filename_component(SV_SOURCE_HOME ${SV_SOURCE_DIR}/../ ABSOLUTE)
dev_message("SimVascular Source Home: ${SV_SOURCE_HOME}")
if(NOT SV_BINARY_HOME)
	set(SV_BINARY_HOME ${SV_BINARY_DIR})
endif()

set(SV_HOME ${SV_BINARY_HOME})
set(SV_DISTRIBUTION_DIR ${SV_SOURCE_HOME}/Distribution)
set(SV_BINARY_DISTRIBUTION_DIR ${SV_BINARY_HOME}/Distribution)

mark_as_superbuild(VARS
	SV_SOURCE_HOME:PATH
	SV_BINARY_HOME:PATH
	SV_HOME:PATH
	SV_SOURCE_TCL_DIR:PATH
	SV_BINARY_TCL_DIR:PATH
	SV_TCL:PATH
	SV_DISTRIBUTION_DIR:PATH
	)
