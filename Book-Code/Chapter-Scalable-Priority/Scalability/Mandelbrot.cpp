/*
Copyright (c) 2015 James Dean Mathias

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "Mandelbrot.hpp"

#include "IRange.hpp"
#include "MandelImageTask.hpp"
#include "ThreadPool.hpp"

const double Mandelbrot::MOVEMENT_RATE = 0.004;

// ------------------------------------------------------------------
//
// @details Initialize the Mandelbrot region, the mandelbrot rendering
// bitmap, and kick off the generation of prime numbers.
//
// ------------------------------------------------------------------
Mandelbrot::Mandelbrot(HWND hwnd, uint16_t sizeX, uint16_t sizeY) :
	m_hwnd(hwnd),
	m_sizeX(sizeX),
	m_sizeY(sizeY),
	m_updateRequired(true),
	m_inUpdate(false),
	m_mandelLeft(-2.0),
	m_mandelRight(1.0),
	m_mandelTop(-1.5),
	m_mandelBottom(1.5)
{
	prepareColors();
	prepareBitmap();
	//
	// Allocate memory for the intermediate image
	m_image = std::unique_ptr<uint16_t[]>(new uint16_t[sizeX * sizeY]);
}

// ------------------------------------------------------------------
//
// @details Called by the main application event loop to allow this class
// to perform any rendering or image computation tasks, as needed.
//
// ------------------------------------------------------------------
void Mandelbrot::update()
{
	if (m_updateRequired && !m_inUpdate)
	{
		m_inUpdate = true;
		m_updateRequired = false;
		startNewImage();
	}

	//
	// Render the current mandelbrot set
	HDC hdc = GetDC(m_hwnd);
	SelectObject(m_hdcBMP, m_handleBMPWrite);
	BitBlt(hdc, 0, 0, m_sizeX - 1, m_sizeY - 1, m_hdcBMP, 0, 0, SRCCOPY);
	ReleaseDC(m_hwnd, hdc);
}

// ------------------------------------------------------------------
//
// @details This method creates the master Mandelbrot image generation task.
// That task then generates all the sub-image tasks that perform the
// actual computational work.
//
// ------------------------------------------------------------------
void Mandelbrot::startNewImage()
{
	auto task = std::shared_ptr<MandelImageTask>(new MandelImageTask(
		m_mandelLeft, m_mandelRight,
		m_mandelTop, m_mandelBottom,
		MAX_ITERATIONS,
		m_sizeX, m_sizeY,
		m_pixels, static_cast<uint16_t>(m_bmp.bmWidthBytes),
		[this]() { m_inUpdate = false; },
		Task::Priority::One));

	ThreadPool::instance()->enqueueTask(task);
}

// ------------------------------------------------------------------
//
// @details Builds the color (gradient) array used for smooth coloring the Mandelbrot
//
// ------------------------------------------------------------------
void Mandelbrot::prepareColors()
{
	for (auto position : IRange<uint16_t>(0, 767))
	{
		m_colors[position].r = 0;
		m_colors[position].g = 0;
		m_colors[position].b = 0;
		if (position >= 512)
		{
			m_colors[position].r = position - 512;
			m_colors[position].g = 255 - m_colors[position].r;
		}
		else if (position >= 256)
		{
			m_colors[position].g = position - 256;
			m_colors[position].b = 255 - m_colors[position].g;
		}
		else
		{
			m_colors[position].b = static_cast<uint8_t>(position);
		}
	}
}

// ------------------------------------------------------------------
//
// @details This is used to copy the raw mandel computed data into
// the bitmap used for rendering.  Nothing fancy here, just a simple
// grey-scale image.
//
// ------------------------------------------------------------------
void Mandelbrot::copyToPixels()
{
	//
	// Doing a little optimization through the use of pointers to copy
	// the data into the destination bitmap.
	uint16_t* source = m_image.get();
	uint8_t* destination = m_pixels;
	for (auto row : IRange<decltype(m_sizeY)>(0, m_sizeY - 1))
	{
		uint8_t* destination = m_pixels + row * m_bmp.bmWidthBytes;
		for (auto column : IRange<decltype(m_sizeX)>(0, m_sizeX - 1))
		{
			uint16_t color = *(source++);
			*(destination++) = m_colors[color].r;
			*(destination++) = m_colors[color].g;
			*(destination++) = m_colors[color].b;
			*(destination++) = 0;
		}
	}
}

// ------------------------------------------------------------------
//
// @details Moves the Mandelbrot view left
//
// ------------------------------------------------------------------
void Mandelbrot::moveLeft()
{ 
	double distance = (m_mandelRight - m_mandelLeft) * MOVEMENT_RATE;
	m_mandelLeft -= distance;
	m_mandelRight -= distance;
	m_updateRequired = true;
}

// ------------------------------------------------------------------
//
// @details Moves the Mandelbrot view right
//
// ------------------------------------------------------------------
void Mandelbrot::moveRight()
{
	double distance = (m_mandelRight - m_mandelLeft) * MOVEMENT_RATE;
	m_mandelLeft += distance;
	m_mandelRight += distance;
	m_updateRequired = true;
}

// ------------------------------------------------------------------
//
// @details Moves the Mandelbrot view up
//
// ------------------------------------------------------------------
void Mandelbrot::moveUp()
{
	double distance = (m_mandelBottom - m_mandelTop) * MOVEMENT_RATE;
	m_mandelTop += distance;
	m_mandelBottom += distance;
	m_updateRequired = true;
}

// ------------------------------------------------------------------
//
// @details Moves the Mandelbrot view down
//
// ------------------------------------------------------------------
void Mandelbrot::moveDown()
{
	double distance = (m_mandelBottom - m_mandelTop) * MOVEMENT_RATE;
	m_mandelTop -= distance;
	m_mandelBottom -= distance;
	m_updateRequired = true;
}

// ------------------------------------------------------------------
//
// @details Moves the Mandelbrot view in
//
// ------------------------------------------------------------------
void Mandelbrot::zoomIn()
{
	double distanceX = (m_mandelRight - m_mandelLeft) * MOVEMENT_RATE;
	double distanceY = (m_mandelBottom - m_mandelTop) * MOVEMENT_RATE;

	m_mandelLeft += distanceX;
	m_mandelRight -= distanceX;

	m_mandelTop += distanceY;
	m_mandelBottom -= distanceY;

	m_updateRequired = true;
}

// ------------------------------------------------------------------
//
// @details Moves the Mandlebrot view out
//
// ------------------------------------------------------------------
void Mandelbrot::zoomOut()
{
	double distanceX = (m_mandelRight - m_mandelLeft) * MOVEMENT_RATE;
	double distanceY = (m_mandelBottom - m_mandelTop) * MOVEMENT_RATE;

	m_mandelLeft -= distanceX;
	m_mandelRight += distanceX;

	m_mandelTop -= distanceY;
	m_mandelBottom += distanceY;

	m_updateRequired = true;
}

// ------------------------------------------------------------------
//
// @details This builds a bitmap that is compatible with the current
// display, according to the size requested by the application.
//
// ------------------------------------------------------------------
void Mandelbrot::prepareBitmap()
{
	HDC hdc = GetDC(m_hwnd);
	m_handleBMP = CreateCompatibleBitmap(hdc, m_sizeX, m_sizeY);
	ReleaseDC(m_hwnd, hdc);

	GetObject(m_handleBMP, sizeof(BITMAP), &m_bmp);
	m_hdcBMP = CreateCompatibleDC(NULL);

	BITMAPINFO bmiMandel;
	bmiMandel.bmiHeader.biSize = sizeof(BITMAPINFOHEADER);
	bmiMandel.bmiHeader.biWidth = m_sizeX;
	bmiMandel.bmiHeader.biHeight = m_sizeY;
	bmiMandel.bmiHeader.biPlanes = 1;
	bmiMandel.bmiHeader.biBitCount = m_bmp.bmBitsPixel;
	bmiMandel.bmiHeader.biCompression = BI_RGB;
	bmiMandel.bmiHeader.biSizeImage = 0;
	bmiMandel.bmiHeader.biXPelsPerMeter = 0;
	bmiMandel.bmiHeader.biYPelsPerMeter = 0;
	bmiMandel.bmiHeader.biClrUsed = 0;
	bmiMandel.bmiHeader.biClrImportant = 0;

	m_handleBMPWrite = CreateDIBSection(
		m_hdcBMP, &bmiMandel,
		DIB_RGB_COLORS,
		reinterpret_cast<void **>(&m_pixels), NULL, 0);
}
