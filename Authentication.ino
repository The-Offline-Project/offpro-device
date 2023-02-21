#include "Graphics.h"
#include "ScreenBuffer.h"
#include "arduino.h"
#include <string.h>


#include "Point.h"

Graphics* Graphics::_instance = 0;

Graphics::Graphics(ScreenBuffer* screenBuffer, Font* font) : _screenBuffer(screenBuffer), _font(font)
{
	_instance = this;
}

Graphics::~Graphics(void)
{
	_instance = 0;
}

Graphics* Graphics::GetInstance()
{
	return _instance;
}

void Graphics::DrawRectangle(Vector2 pos, int width, int height, bool fill)
{
	// Round vector to point
	Point posPt(pos);

	int bufferWidth = _screenBuffer->GetWidth();
	int bufferHeight = _screenBuffer->GetHeight();

	// Early clip
	if (posPt.X > bufferWidth || posPt.Y > bufferHeight)
		return;
	
	int xMax = height + posPt.X;
	int yMax = width + posPt.Y;

	for (int row=posPt.X; row<xMax; row++)
	{
		for (int col=posPt.Y; col<yMax; col++)
		{
			_screenBuffer->Write(col, row, 1);
		}
	}
}

void Graphics::DrawLine(Vector2 v1, Vector2 v2)
{
	// Get direction from v1 to v2
	Vector2 dir = Vector2::Normalize(v2 - v1);
	float distance = Vector2::Distance(v1, v2);

	for (int i=0; i<distance; i++)
	{
		Point pixel(v1 + dir * i);
		_screenBuffer->Write(pixel.X, pixel.Y, 1);
	}
}

void Graphics::DrawString(const String& str, Point pos, int spacing)
{
	Point offset = pos;

	for (int i=0; i<str.length(); ++i)
	{
		char c = str[i];

		// Draw the char
		FontChar* fontChar = _font->Chars[(int)c];
		DrawFontChar(fontChar, offset);

		// Apply spacing
		offset.X += spacing + fontChar->Width;
	}
}

void Graphics::DrawChar(char c, Point pos)
{
	FontChar* fontChar = _font->Chars[(int)c];
	DrawFontChar(fontChar, pos);
}

void Graphics::DrawFontChar(FontChar* fontChar, Point pos)
{
	int bufferWidth = _screenBuffer->GetWidth();
	int bufferHeight = _screenBuffer->GetHeight();

	// Early clip
	if (pos.X > bufferWidth || pos.Y > bufferHeight)
		return;
	if (pos.X < -fontChar->Width || pos.Y < -fontChar->Height)
		return;

	// Draw each pixel of the char
	for (int x=0; x<fontChar->Width; ++x)
	{
		int xPixel = pos.X + x;
		for (int y=0; y<fontChar->Height; ++y)
		{
			int yPixel = pos.Y + y + fontChar->OffsetY;

			// Render pixel
			if (fontChar->GetValue(x, y) > 0)
				_screenBuffer->Write(xPixel, yPixel, 1);
		}
	}
}