/*
 * DTSlamShaders.h
 *
 *  Created on: 24.1.2014
 *      Author: Dan
 */

#ifndef DTSlamShaders_H_
#define DTSlamShaders_H_

#include "ColorShader.h"
#include "TextShader.h"
#include "TextureShader.h"
#include "TextureWarpShader.h"
#include "TextRenderer.h"
#include "StaticColors.h"

namespace planecalib
{

class DTSlamShaders {
public:
	DTSlamShaders();

	bool init()
	{
		bool res;
		res = mColor.init();
		res &= mTexture.init();
		res &= mTextureWarp.init();
		res &= mText.init();
		res &= mTextRenderer.init(&mText);
		return res;
	}

	void free()
	{
		mColor.free();
		mText.free();
		mTexture.free();
		mTextureWarp.free();
	}

	ColorShader &getColor() {return mColor;}
	TextureShader &getTexture() {return mTexture;}
	TextureWarpShader &getTextureWarp() {return mTextureWarp;}
	TextRenderer &getText() {return mTextRenderer;}

protected:
	ColorShader mColor;
	TextureShader mTexture;
	TextureWarpShader mTextureWarp;
	TextShader mText;
    TextRenderer mTextRenderer;
};

}

#endif /* DTSlamShaders_H_ */
