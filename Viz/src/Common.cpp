//
//  Common.cpp
//  Visualization
//
//  Created by Tim Murray-Browne on 08/06/2013.
//
//

#include "Common.h"
#include "cinder/Font.h"
#include "cinder/gl/TextureFont.h"

float mx(0);
float my(0);




void tmb::drawString(std::string const& text, ci::Vec2f const& position, bool isCentered, ci::ColorA const& color)
{
	using namespace ci;
	static ci::Font mainFont = ci::Font("Arial", 48.);
	static ci::gl::TextureFontRef font = ci::gl::TextureFont::create(mainFont);

	gl::pushModelView();
	gl::translate(position);
	float scale = 1.;
	scale *= gl::getViewport().getWidth() / 2000.f;
	gl::scale(scale, scale);
	if (isCentered)
	{
		gl::drawStringCentered(text, position, color);
	}
	else
	{
		gl::color(color);
		font->drawString(text, position);
	}
	gl::popModelView();
}

std::ostream& tmb::operator<<(std::ostream& outstream, tmb::Quad quad)
{
	return outstream << "Quad TL"<<quad.tl<<", TR"<<quad.tr<<", BR"<<quad.br<<", BL"<<quad.bl;
}