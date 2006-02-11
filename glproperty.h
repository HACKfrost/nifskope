/***** BEGIN LICENSE BLOCK *****

BSD License

Copyright (c) 2005, NIF File Format Library and Tools
All rights reserved.

Redistribution and use in source and binary forms, with or without
modification, are permitted provided that the following conditions
are met:
1. Redistributions of source code must retain the above copyright
   notice, this list of conditions and the following disclaimer.
2. Redistributions in binary form must reproduce the above copyright
   notice, this list of conditions and the following disclaimer in the
   documentation and/or other materials provided with the distribution.
3. The name of the NIF File Format Library and Tools projectmay not be
   used to endorse or promote products derived from this software
   without specific prior written permission.

THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
(INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

***** END LICENCE BLOCK *****/

#ifndef GLPROPERTY_H
#define GLPROPERTY_H

#include <QtOpenGL>

#include "glcontrolable.h"

class Property : public Controllable
{
protected:
	enum Type
	{
		Alpha, ZBuffer, Material, Texturing, Specular, Wireframe
	};
	
	Type typ;
	
	Property( Type t, Scene * scene, const QModelIndex & index ) : Controllable( scene, index ), typ( t ), ref( 0 ) {}
	
	template <typename T> static Type getType();
	
	int ref;
	
	friend class PropertyList;
	
public:

	static Property * create( Scene * scene, const NifModel * nif, const QModelIndex & index );

	template <typename T> T * cast()
	{
		if ( typ == getType<T>() )
			return static_cast<T*>( this );
		else
			return 0;
	}
};

class PropertyList
{
public:
	PropertyList();
	PropertyList( const PropertyList & other );
	~PropertyList();
	
	void add( Property * );
	void del( Property * );
	
	Property * get( const QModelIndex & idx ) const;
	template <typename T> T * get() const;
	
	void validate();
	
	void clear();
	
	PropertyList & operator=( const PropertyList & other );
	
	const QList<Property*> & list() const { return properties; }
	
protected:
	QList<Property*> properties;
};

template <typename T> inline T * PropertyList::get() const
{
	foreach ( Property * p, properties )
	{
		T * t = p->cast<T>();
		if ( t )	return t;
	}
	return 0;
}

class AlphaProperty : public Property
{
public:
	AlphaProperty( Scene * scene, const QModelIndex & index ) : Property( Alpha, scene, index ) {}
	
	void update( const NifModel * nif, const QModelIndex & block );
	
	bool blend() const { return alphaBlend; }
	bool test() const { return alphaTest; }
	
	friend void glProperty( AlphaProperty * );

protected:
	bool alphaBlend, alphaTest;
	GLenum alphaSrc, alphaDst, alphaFunc;
	GLfloat alphaThreshold;	
};

template <> inline Property::Type Property::getType<AlphaProperty>()
{
	return Property::Alpha;
}

class ZBufferProperty : public Property
{
public:
	ZBufferProperty( Scene * scene, const QModelIndex & index ) : Property( ZBuffer, scene, index ) {}
	
	void update( const NifModel * nif, const QModelIndex & block );
	
	bool test() const { return depthTest; }
	bool mask() const { return depthMask; }
	
	friend void glProperty( ZBufferProperty * );
	
protected:
	bool depthTest;
	bool depthMask;
};

template <> inline Property::Type Property::getType<ZBufferProperty>()
{
	return Property::ZBuffer;
}

class TexturingProperty : public Property
{
public:
	TexturingProperty( Scene * scene, const QModelIndex & index ) : Property( Texturing, scene, index ) {}
	
	void update( const NifModel * nif, const QModelIndex & block );
	
	friend void glProperty( TexturingProperty * );
	
	int baseSet() const { return baseTexSet; }

protected:
	QPersistentModelIndex iBaseTex;
	GLenum texFilter;
	GLint texWrapS, texWrapT;
	int baseTexSet;

	void setController( const NifModel * nif, const QModelIndex & controller );
	
	friend class TexFlipController;
};

template <> inline Property::Type Property::getType<TexturingProperty>()
{
	return Property::Texturing;
}

class MaterialProperty : public Property
{
public:
	MaterialProperty( Scene * scene, const QModelIndex & index ) : Property( Material, scene, index ) {}
	
	void update( const NifModel * nif, const QModelIndex & block );
	
	friend void glProperty( MaterialProperty * );
	
	GLfloat alphaValue() const { return alpha; }
	
protected:
	Color4 ambient, diffuse, specular, emissive;
	GLfloat shininess, alpha;

	void setController( const NifModel * nif, const QModelIndex & controller );
	
	friend class AlphaController;
};

template <> inline Property::Type Property::getType<MaterialProperty>()
{
	return Property::Material;
}

class SpecularProperty : public Property
{
public:
	SpecularProperty( Scene * scene, const QModelIndex & index ) : Property( Specular, scene, index ) {}
	
	void update( const NifModel * nif, const QModelIndex & index );
	
	friend void glProperty( SpecularProperty * );
	
protected:
	bool spec;
};

template <> inline Property::Type Property::getType<SpecularProperty>()
{
	return Property::Specular;
}

class WireframeProperty : public Property
{
public:
	WireframeProperty( Scene * scene, const QModelIndex & index ) : Property( Wireframe, scene, index ) {}
	
	void update( const NifModel * nif, const QModelIndex & index );
	
	friend void glProperty( WireframeProperty * );
	
protected:
	bool wire;
};

template <> inline Property::Type Property::getType<WireframeProperty>()
{
	return Property::Wireframe;
}

#endif
