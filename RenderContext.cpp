#include "RenderContext.h"

#include <inline_c.h>

#include <cassert>

Graphics::RenderBuffer::RenderBuffer(std::size_t ot_length, std::size_t buffer_length)
	: _ot_length(ot_length), _buffer_length(buffer_length) {
	// Initializing the OT in a constructor is unsafe, since ClearOTagR()
	// requires DMA to be enabled and may fail if called before ResetGraph() or
	// ResetCallback() (which can easily happen as constructors can run before
	// main()). Thus, this constructor is only going to allocate the buffers and
	// clearing is deferred to RenderContext::setup().
	_ot = new uint32_t[ot_length];
	_buffer = new uint8_t[buffer_length];

	assert(_ot && _buffer);

	//std::PRINTF("Allocated buffer, ot=0x%08x, buffer=0x%08x\n", ot, buffer);
}

Graphics::RenderBuffer::~RenderBuffer(void) {
	delete[] _ot;
	delete[] _buffer;

	//std::PRINTF("Freed buffer, ot=0x%08x, buffer=0x%08x\n", ot, buffer);
}

void Graphics::RenderBuffer::setup(int x, int y, int w, int h, int r, int g, int b, bool inter) {
	// Set the framebuffer's VRAM coordinates.
	SetDefDrawEnv(&_draw_env, x, y, w, h);
	SetDefDispEnv(&_disp_env, x, y, w, h);

	// Set the default background color and enable auto-clearing.
	setRGB0(&_draw_env, r, g, b);
	_draw_env.isbg = 1;
	_disp_env.isinter = inter;
}



Graphics::RenderContext::RenderContext(std::size_t ot_length, std::size_t buffer_length)
	: _buffers{
			RenderBuffer(ot_length, buffer_length),
			RenderBuffer(ot_length, buffer_length)
	} {}

void Graphics::RenderContext::setup(int w, int h, int r, int g, int b, bool inter) {
	// Place the two framebuffers vertically in VRAM.
	_buffers[0].setup(0, 0, w, h, r, g, b, inter);
	_buffers[1].setup(0, 0, w, h, r, g, b, inter);

	gte_SetGeomOffset(w >> 1, h >> 1);
	gte_SetGeomScreen(w >> 1);

	// Initialize the first buffer and clear its OT so that it can be used for
	// drawing.
	_active_buffer = 0;
	_next_packet = _draw_buffer().buffer_start();
	_draw_buffer().clear_ot();

	// Turn on the video output.
	SetDispMask(1);
}

void Graphics::RenderContext::flip(void) {
	// Wait for the GPU to finish drawing, then wait for vblank in order to
	// prevent screen tearing.
	DrawSync(0);
	VSync(0);

	// Display the framebuffer the GPU has just finished drawing and start
	// rendering the display list that was filled up in the main loop.
	_disp_buffer().display();
	_draw_buffer().draw();

	// Switch over to the next buffer, clear it and reset the packet allocation
	// pointer.
	_active_buffer ^= 1;
	_next_packet = _draw_buffer().buffer_start();
	_draw_buffer().clear_ot();
}