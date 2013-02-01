// Copyright (C) 2003 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/


#include "Globals.h"
#include "GLUtil.h"
#include "StreamBuffer.h"

namespace OGL
{

static const u32 SYNC_POINTS = 16;

StreamBuffer::StreamBuffer(u32 type, size_t size, StreamType uploadType)
: m_uploadtype(uploadType), m_buffertype(type), m_size(size), m_iterator(0), m_used_iterator(0), m_free_iterator(0)
{
	glGenBuffers(1, &m_buffer);
	
	if(m_uploadtype == STREAM_DETECT)
		m_uploadtype = MAP_AND_SYNC;
	
	Init();
}

StreamBuffer::~StreamBuffer()
{
	Shutdown();
	glDeleteBuffers(1, &m_buffer);
}

#define SLOT(x) (x)*SYNC_POINTS/m_size

void StreamBuffer::Alloc ( size_t size, u32 stride )
{
	size_t m_iterator_aligned = m_iterator;
	if(m_iterator_aligned && stride) {
		m_iterator_aligned--;
		m_iterator_aligned = m_iterator_aligned - (m_iterator_aligned % stride) + stride;
	}
	size_t iter_end = m_iterator_aligned + size;
	
	switch(m_uploadtype) {
	case MAP_AND_ORPHAN:
		if(iter_end >= m_size) {
			glBufferData(m_buffertype, m_size, NULL, GL_STREAM_DRAW);
			m_iterator_aligned = 0;
		}
		break;
	case MAP_AND_SYNC:
		
		// insert waiting slots for used memory
		for(u32 i=SLOT(m_used_iterator); i<SLOT(m_iterator); i++)
		{
			fences[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		}
		m_used_iterator = m_iterator;
		
		// wait for new slots to end of buffer
		for(u32 i=SLOT(m_free_iterator)+1; i<=SLOT(iter_end) && i < SYNC_POINTS; i++)
		{
			glClientWaitSync(fences[i], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
			glDeleteSync(fences[i]);
		}
		m_free_iterator = iter_end;
		
		// if buffer is full
		if(iter_end >= m_size) {
			
			// insert waiting slots in unused space at the end of the buffer
			for(u32 i=SLOT(m_used_iterator); i < SYNC_POINTS; i++)
				fences[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
			
			// move to the start
			m_used_iterator = m_iterator_aligned = m_iterator = 0; // offset 0 is always aligned
			iter_end = size;
			
			// wait for space at the start
			for(u32 i=0; i<=SLOT(iter_end); i++)
			{
				glClientWaitSync(fences[i], GL_SYNC_FLUSH_COMMANDS_BIT, GL_TIMEOUT_IGNORED);
				glDeleteSync(fences[i]);
			}
			m_free_iterator = iter_end;
		}
		
		break;
	case BUFFERSUBDATA:
		m_iterator_aligned = 0;
		break;
	}
	m_iterator = m_iterator_aligned;
}

size_t StreamBuffer::Upload ( u8* data, size_t size )
{
	switch(m_uploadtype) {
	case MAP_AND_SYNC:
	case MAP_AND_ORPHAN:
		pointer = (u8*)glMapBufferRange(m_buffertype, m_iterator, size, GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
		if(pointer) {
			memcpy(pointer, data, size);
			glUnmapBuffer(m_buffertype);
		} else {
			ERROR_LOG(VIDEO, "buffer mapping failed");
		}
		break;
	case BUFFERSUBDATA:
		glBufferSubData(m_buffertype, m_iterator, size, data);
		break;
	}
	size_t ret = m_iterator;
	m_iterator += size;
	return ret;
}

void StreamBuffer::Init()
{
	switch(m_uploadtype) {
	case MAP_AND_SYNC:
		fences = new GLsync[SYNC_POINTS];
		for(u32 i=0; i<SYNC_POINTS; i++)
			fences[i] = glFenceSync(GL_SYNC_GPU_COMMANDS_COMPLETE, 0);
		
	case MAP_AND_ORPHAN:
	case BUFFERSUBDATA:
		glBindBuffer(m_buffertype, m_buffer);
		glBufferData(m_buffertype, m_size, NULL, GL_STREAM_DRAW);
		break;
	}
}

void StreamBuffer::Shutdown()
{
	switch(m_uploadtype) {
	case MAP_AND_SYNC:
		for(u32 i=0; i<SYNC_POINTS; i++)
			glDeleteSync(fences[i]);
		delete [] fences;
		break;
		
	case MAP_AND_ORPHAN:
	case BUFFERSUBDATA:
		break;
	}
}

}
